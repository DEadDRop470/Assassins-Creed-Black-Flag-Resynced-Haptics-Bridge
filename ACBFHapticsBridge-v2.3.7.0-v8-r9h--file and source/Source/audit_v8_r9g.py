#!/usr/bin/env python3
"""Static audit helper for ACBFHapticsBridge v8-r9g.

Checks the built ASI and, when supplied, compatible ACBlackFlag.exe targets.
Uses only the Python standard library.
"""
from __future__ import annotations
import argparse, hashlib, struct, sys
from pathlib import Path

IMAGE_SCN_MEM_EXECUTE = 0x20000000
IMAGE_SCN_MEM_READ    = 0x40000000
IMAGE_SCN_MEM_WRITE   = 0x80000000

TARGETS = {
    "official": {
        "sha256": "8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140",
    },
    "voices38": {
        "sha256": "19920f34bb2fac814023ee3c27f0ceca1872e80a2aafb37c092c419fff77cc0d",
    },
}
HOOKS = [
    ("PostEvent", 0x0091DEA0, bytes.fromhex("41 57 41 56 41 55")),
    ("QuadFlush", 0x04F78610, bytes.fromhex("48 89 5C 24 18 55 56 57 48 83 EC 20 0F B7 79 42")),
]

def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def parse_pe(path: Path):
    b = path.read_bytes()
    if b[:2] != b"MZ": raise ValueError("not MZ")
    pe = struct.unpack_from("<I", b, 0x3C)[0]
    if b[pe:pe+4] != b"PE\0\0": raise ValueError("not PE")
    coff = pe + 4
    machine, nsec, timestamp, _symptr, _nsyms, optsz, chars = struct.unpack_from("<HHIIIHH", b, coff)
    opt = coff + 20
    magic = struct.unpack_from("<H", b, opt)[0]
    if magic != 0x20B: raise ValueError(f"not PE32+ (magic 0x{magic:X})")
    dllchars = struct.unpack_from("<H", b, opt + 0x46)[0]
    nrva = struct.unpack_from("<I", b, opt + 0x6C)[0]
    dd = opt + 0x70
    import_rva = import_size = 0
    if nrva > 1:
        import_rva, import_size = struct.unpack_from("<II", b, dd + 8)
    sec_off = opt + optsz
    sections=[]
    for i in range(nsec):
        o=sec_off+i*40
        name=b[o:o+8].split(b"\0",1)[0].decode("ascii","replace")
        vsize, va, rawsz, rawptr = struct.unpack_from("<IIII", b, o+8)
        schars=struct.unpack_from("<I", b, o+36)[0]
        sections.append((name,vsize,va,rawsz,rawptr,schars))
    return b, {"machine":machine,"nsec":nsec,"timestamp":timestamp,"chars":chars,"dllchars":dllchars,
               "import_rva":import_rva,"import_size":import_size,"sections":sections}


def rva_to_raw(info, rva: int):
    for name,vsize,va,rawsz,rawptr,chars in info["sections"]:
        span=max(vsize,rawsz)
        if va <= rva < va+span:
            delta=rva-va
            if delta >= rawsz: return None
            return rawptr+delta
    return None


def audit_asi(path: Path) -> bool:
    b, info = parse_pe(path)
    ok=True
    print(f"ASI: {path}")
    print(f"  SHA-256: {sha256(path)}")
    print(f"  sections: {info['nsec']}")
    print(f"  timestamp: {info['timestamp']} ({'OK' if info['timestamp']==0 else 'FAIL'})")
    if info['timestamp'] != 0: ok=False
    d=info['dllchars']
    print(f"  DllCharacteristics: 0x{d:04X} (ASLR={'yes' if d&0x40 else 'no'}, NX={'yes' if d&0x100 else 'no'}, HEVA={'yes' if d&0x20 else 'no'})")
    if not (d & 0x40 and d & 0x100): ok=False
    rwx=[]
    for name,_,_,_,_,c in info['sections']:
        if (c & IMAGE_SCN_MEM_EXECUTE) and (c & IMAGE_SCN_MEM_WRITE): rwx.append(name)
    print(f"  RWX sections: {rwx or 'none'} ({'OK' if not rwx else 'FAIL'})")
    if rwx: ok=False
    # The freestanding linker emits a 24-byte .idata containing only a null descriptor.
    imp_raw = rva_to_raw(info, info['import_rva']) if info['import_rva'] else None
    import_null = bool(imp_raw is not None and info['import_size'] <= 0x18 and not any(b[imp_raw:imp_raw+info['import_size']]))
    print(f"  import directory: RVA=0x{info['import_rva']:X} size=0x{info['import_size']:X}; null-only={import_null} ({'OK' if import_null else 'CHECK'})")
    # Symbol-stripped characteristic is 0x8 in COFF Characteristics.
    stripped=bool(info['chars'] & 0x0008)
    print(f"  COFF symbols stripped: {stripped} ({'OK' if stripped else 'CHECK'})")
    print(f"  RESULT: {'PASS' if ok else 'FAIL'}")
    return ok


def audit_target(path: Path) -> bool:
    digest=sha256(path)
    profile=next((k for k,v in TARGETS.items() if v['sha256']==digest), None)
    b, info=parse_pe(path)
    ok=profile is not None
    print(f"TARGET: {path}")
    print(f"  SHA-256: {digest}")
    print(f"  profile: {profile or 'UNKNOWN'}")
    for name,rva,sig in HOOKS:
        off=rva_to_raw(info,rva)
        got=b[off:off+len(sig)] if off is not None else b""
        match=got==sig
        print(f"  {name}: RVA=0x{rva:X} raw={('0x%X'%off) if off is not None else 'N/A'} match={match} bytes={got.hex(' ').upper()}")
        ok &= match
    print(f"  RESULT: {'PASS' if ok else 'FAIL'}")
    return ok


def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("asi", type=Path)
    ap.add_argument("targets", nargs="*", type=Path)
    a=ap.parse_args()
    ok=audit_asi(a.asi)
    for t in a.targets:
        ok=audit_target(t) and ok
    return 0 if ok else 1

if __name__ == "__main__":
    raise SystemExit(main())
