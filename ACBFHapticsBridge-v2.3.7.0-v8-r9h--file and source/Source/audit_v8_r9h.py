#!/usr/bin/env python3
"""Static audit helper for ACBFHapticsBridge v2.3.7.0 / v8-r9h.

Checks the built ASI plus optional ACBlackFlag.exe targets. No file is modified.
"""
from __future__ import annotations
import argparse, hashlib, mmap, struct, sys
from dataclasses import dataclass
from pathlib import Path

IMAGE_SCN_MEM_EXECUTE = 0x20000000
IMAGE_SCN_MEM_WRITE   = 0x80000000
EXPECTED_ASI_SHA256 = "869361c87e445601d3d1ff5807922f4d0c60cb37f51a7bf37bdd05103514f914"

TARGETS = {
    "8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140": ("Steam TU 1.0.6", 0x1D0119E0),
    "19920f34bb2fac814023ee3c27f0ceca1872e80a2aafb37c092c419fff77cc0d": ("voices38", 0x1D0119E0),
    "e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c": ("Ubisoft Connect", 0x1C5769E0),
    "b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75": ("Ubisoft+", 0x1EC409E0),
}
IAT_CANDIDATES = (0x1D0119E0, 0x1C5769E0, 0x1EC409E0)
HOOKS = [
    ("Wwise PostEvent", 0x0091DEA0, bytes.fromhex("41 57 41 56 41 55")),
    ("Quad haptics flush", 0x04F78610, bytes.fromhex("48 89 5C 24 18 55 56 57 48 83 EC 20 0F B7 79 42")),
]
STATE_SLOTS = [
    (0x09151C18, 0x015273A0, "InAir enter"), (0x09151C20, 0x017FC510, "InAir exit"),
    (0x09152898, 0x05E7F6C0, "JumpOnSpot enter"), (0x09152848, 0x06B118E0, "FreeJump enter"),
    (0x0914E4E8, 0x02750BF0, "TargetedJump enter"), (0x0914E3F8, 0x01FCD920, "Freefall enter"),
    (0x09151F38, 0x06B01790, "FallFromLedge enter"), (0x09151F40, 0x06B01A30, "FallFromLedge exit"),
    (0x0914E218, 0x06AFDB60, "TransitionExit enter"), (0x09147A38, 0x0152D180, "Climb A enter"),
    (0x09147A40, 0x0102CC10, "Climb A exit"), (0x0920DC90, 0x06A98A70, "Climb B enter"),
    (0x0920DC98, 0x06A99E00, "Climb B exit"), (0x09159718, 0x027D3CF0, "Swimming A enter"),
    (0x09159720, 0x018A3D00, "Swimming A exit"), (0x091A3928, 0x08065C00, "Swimming B enter"),
    (0x091A3930, 0x08066540, "Swimming B exit"), (0x0915AB98, 0x06C9ED00, "ShallowSwim enter"),
    (0x0915ABA0, 0x06C9FAC0, "ShallowSwim exit"), (0x09145208, 0x01B6C060, "HayStack enter"),
    (0x09145210, 0x054F1850, "HayStack exit"),
]

@dataclass
class Section:
    name: str; va: int; vsize: int; raw: int; rawsize: int; chars: int

class PE:
    def __init__(self, path: Path):
        self.path=path; self.f=path.open("rb"); self.mm=mmap.mmap(self.f.fileno(),0,access=mmap.ACCESS_READ)
        b=self.mm
        if len(b)<0x100 or b[:2]!=b"MZ": raise ValueError("not MZ")
        self.peoff=struct.unpack_from("<I",b,0x3C)[0]
        if b[self.peoff:self.peoff+4]!=b"PE\0\0": raise ValueError("not PE")
        fh=self.peoff+4
        self.machine,self.nsec,self.timestamp,_ps,_ns,self.szopt,self.characteristics=struct.unpack_from("<HHIIIHH",b,fh)
        self.opt=fh+20
        if struct.unpack_from("<H",b,self.opt)[0]!=0x20B: raise ValueError("not PE32+")
        self.imagebase=struct.unpack_from("<Q",b,self.opt+24)[0]
        self.sizeimage=struct.unpack_from("<I",b,self.opt+56)[0]
        self.dllchars=struct.unpack_from("<H",b,self.opt+0x46)[0]
        self.sections=[]; sec=self.opt+self.szopt
        for i in range(self.nsec):
            o=sec+i*40; name=b[o:o+8].split(b"\0",1)[0].decode("ascii","replace")
            vsize,va,rawsize,raw=struct.unpack_from("<IIII",b,o+8); chars=struct.unpack_from("<I",b,o+36)[0]
            self.sections.append(Section(name,va,vsize,raw,rawsize,chars))
    def close(self): self.mm.close(); self.f.close()
    def rva_to_off(self,rva:int):
        for s in self.sections:
            if s.va <= rva < s.va+max(s.vsize,s.rawsize):
                d=rva-s.va
                if d>=s.rawsize: return None
                return s.raw+d
        return rva if rva < min((s.va for s in self.sections),default=0x1000) else None
    def read_rva(self,rva:int,n:int):
        o=self.rva_to_off(rva)
        if o is None or o+n>len(self.mm): raise ValueError(f"RVA 0x{rva:X} not file-backed")
        return self.mm[o:o+n]
    def qword(self,rva:int): return struct.unpack("<Q",self.read_rva(rva,8))[0]
    def cstr(self,rva:int,limit=512):
        o=self.rva_to_off(rva)
        if o is None: raise ValueError("bad string RVA")
        e=self.mm.find(b"\0",o,min(len(self.mm),o+limit)); e=min(len(self.mm),o+limit) if e<0 else e
        return self.mm[o:e].decode("ascii","replace")
    def imports(self):
        irva,isize=struct.unpack_from("<II",self.mm,self.opt+0x78)
        if not irva: return
        base=self.rva_to_off(irva)
        if base is None: return
        for d in range(max(1,min(4096,(isize//20)+8 if isize else 1024))):
            o=base+d*20
            if o+20>len(self.mm): break
            oft,_ts,_fwd,name_rva,ft=struct.unpack_from("<IIIII",self.mm,o)
            if not any((oft,name_rva,ft)): break
            dll=self.cstr(name_rva); names=oft or ft
            for i in range(4096):
                thunk=struct.unpack("<Q",self.read_rva(names+i*8,8))[0]
                if not thunk: break
                name=None
                if not (thunk & (1<<63)) and oft:
                    try: name=self.cstr(thunk+2,256)
                    except Exception: pass
                yield dll,name,ft+i*8

def sha256(path:Path):
    h=hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda:f.read(8*1024*1024),b""): h.update(chunk)
    return h.hexdigest()

def audit_asi(path:Path):
    digest=sha256(path); pe=PE(path); ok=True
    try:
        print(f"ASI: {path}")
        print(f"  SHA-256: {digest} ({'PASS' if digest==EXPECTED_ASI_SHA256 else 'FAIL'})"); ok &= digest==EXPECTED_ASI_SHA256
        print(f"  size: {path.stat().st_size}")
        print(f"  timestamp: {pe.timestamp} ({'PASS' if pe.timestamp==0 else 'FAIL'})"); ok &= pe.timestamp==0
        aslr=bool(pe.dllchars&0x40); nx=bool(pe.dllchars&0x100); heva=bool(pe.dllchars&0x20)
        print(f"  ASLR/NX/HEVA: {aslr}/{nx}/{heva}"); ok &= aslr and nx
        rwx=[s.name for s in pe.sections if (s.chars&IMAGE_SCN_MEM_EXECUTE) and (s.chars&IMAGE_SCN_MEM_WRITE)]
        print(f"  RWX sections: {rwx or 'none'} ({'PASS' if not rwx else 'FAIL'})"); ok &= not rwx
        raw=path.read_bytes()
        for rva in IAT_CANDIDATES:
            present=struct.pack('<Q',rva) in raw
            print(f"  observer candidate 0x{rva:X}: {'PRESENT' if present else 'MISSING'}"); ok &= present
        print(f"  RESULT: {'PASS' if ok else 'FAIL'}")
        return ok
    finally: pe.close()

def audit_game(path:Path):
    digest=sha256(path); profile=TARGETS.get(digest); pe=PE(path); ok=True
    try:
        print(f"TARGET: {path}")
        print(f"  SHA-256: {digest}")
        print(f"  profile: {profile[0] if profile else 'UNKNOWN'}")
        if not profile: ok=False
        for name,rva,sig in HOOKS:
            try: got=pe.read_rva(rva,len(sig)); match=got==sig
            except Exception: got=b''; match=False
            print(f"  {name} 0x{rva:X}: {'PASS' if match else 'FAIL'} {got.hex(' ')}"); ok &= match
        states=0
        for slot,fn,_label in STATE_SLOTS:
            try: match=pe.qword(slot)==pe.imagebase+fn
            except Exception: match=False
            states += int(match)
        print(f"  gameplay-state slots: {states}/{len(STATE_SLOTS)} exact"); ok &= states==len(STATE_SLOTS)
        found=[]
        try:
            for dll,name,iat in pe.imports() or []:
                if dll.lower().startswith('xinput') and name=='XInputSetState': found.append((dll,iat))
        except Exception as e: print(f"  import parse warning: {e}")
        if found:
            for dll,iat in found: print(f"  XInputSetState: {dll} IAT RVA=0x{iat:X}")
        else: print("  XInputSetState: not found by name")
        expected_iat=profile[1] if profile else None
        iat_ok=bool(expected_iat is not None and any(iat==expected_iat for _,iat in found))
        print(f"  expected r9h observer slot: {('0x%X'%expected_iat) if expected_iat is not None else 'N/A'} ({'PASS' if iat_ok else 'FAIL'})")
        ok &= iat_ok
        print(f"  RESULT: {'PASS' if ok else 'FAIL'}")
        return ok
    finally: pe.close()

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('asi',type=Path)
    ap.add_argument('targets',nargs='*',type=Path)
    a=ap.parse_args(); ok=audit_asi(a.asi)
    for t in a.targets: ok=audit_game(t) and ok
    return 0 if ok else 1

if __name__=='__main__': raise SystemExit(main())
