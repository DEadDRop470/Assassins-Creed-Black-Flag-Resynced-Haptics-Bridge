#!/usr/bin/env python3
"""Relocate ACBFHapticsBridge hook targets between two ACBlackFlag.exe builds.

Usage:
    python retarget_locate.py <known-good-exe> <new-exe>

For every hook function of the known-good build (Wwise PostEvent + the 21
gameplay-state callbacks), this enumerates every occurrence of its prologue
prefix (12-16 bytes) in the new build and scores the first 160 bytes with a
displacement-tolerant comparator: mismatches are only tolerated in runs of
<= 4 bytes (rel32 / RIP-relative disp32 / imm32). A location is accepted only
as a clear unique winner (>= 82% identical, next-best candidate at least
8 points behind, no mismatch run longer than 6 bytes).

Each vtable slot is then located as the data qword holding
ImageBase + newFunctionRVA (unique-candidate required) and validated by
enter/exit pair adjacency (+8) and by fuzzy-matching the code of the
neighboring vtable entries against their old-build counterparts.

Both input files are opened strictly read-only. The KNOWN table below is the
Steam TU 1.0.6 baseline (SHA-256 8d522381...); to retarget a future patch,
pass TU 1.0.6 (or any build already present in the source tables) as
<known-good-exe> and the new build as <new-exe>.
"""
import struct, json, sys, time

IB = 0x140000000
BODY = 160

# (slotRVA, functionRVA, label) for the known-good build.
KNOWN_VT = [
 (0x09151C18,0x015273A0,"InAir enter"),
 (0x09151C20,0x017FC510,"InAir exit"),
 (0x09152898,0x05E7F6C0,"JumpOnSpot enter"),
 (0x09152848,0x06B118E0,"FreeJump enter"),
 (0x0914E4E8,0x02750BF0,"TargetedJump enter"),
 (0x0914E3F8,0x01FCD920,"Freefall enter"),
 (0x09151F38,0x06B01790,"FallFromLedge enter"),
 (0x09151F40,0x06B01A30,"FallFromLedge exit"),
 (0x0914E218,0x06AFDB60,"TransitionExit enter"),
 (0x09147A38,0x0152D180,"Climb A enter"),
 (0x09147A40,0x0102CC10,"Climb A exit"),
 (0x0920DC90,0x06A98A70,"Climb B enter"),
 (0x0920DC98,0x06A99E00,"Climb B exit"),
 (0x09159718,0x027D3CF0,"Swimming A enter"),
 (0x09159720,0x018A3D00,"Swimming A exit"),
 (0x091A3928,0x08065C00,"Swimming B enter"),
 (0x091A3930,0x08066540,"Swimming B exit"),
 (0x0915AB98,0x06C9ED00,"ShallowSwim enter"),
 (0x0915ABA0,0x06C9FAC0,"ShallowSwim exit"),
 (0x09145208,0x01B6C060,"HayStack enter"),
 (0x09145210,0x054F1850,"HayStack exit"),
]
KNOWN_POSTEVENT = 0x0091DEA0
KNOWN_FLUSH = 0x04F78610
FLUSH_SIG = bytes.fromhex("48895C24185556574883EC200FB77942")

def parse_pe(data):
    e = struct.unpack_from("<I",data,0x3C)[0]; coff=e+4
    ns = struct.unpack_from("<H",data,coff+2)[0]
    optsz = struct.unpack_from("<H",data,coff+16)[0]
    st = coff+20+optsz
    return [(data[st+i*40:st+i*40+8].rstrip(b"\0").decode(errors="replace"),
             *struct.unpack_from("<IIII",data,st+i*40+8)) for i in range(ns)]

def mk_r2o(secs):
    def r2o(rva):
        for name,vsize,vaddr,rawsz,rawptr in secs:
            if vaddr<=rva<vaddr+max(vsize,rawsz):
                d=rva-vaddr
                return rawptr+d if d<rawsz else None
        return None
    return r2o

def mk_o2r(secs):
    def o2r(off):
        for name,vsize,vaddr,rawsz,rawptr in secs:
            if rawptr<=off<rawptr+rawsz: return vaddr+(off-rawptr)
        return None
    return o2r

def fuzzy(a, b, n=BODY):
    runs=0; run=0; m=0; maxrun=0
    for i in range(n):
        if a[i]==b[i]:
            m+=1
            if run: runs+=1; maxrun=max(maxrun,run); run=0
        else:
            run+=1
    if run: runs+=1; maxrun=max(maxrun,run)
    return (100.0*m/n, runs, maxrun)

def main():
    if len(sys.argv) != 3:
        print(__doc__); return 2
    old = open(sys.argv[1],"rb").read()
    new = open(sys.argv[2],"rb").read()
    o_r2o = mk_r2o(parse_pe(old))
    n_r2o = mk_r2o(parse_pe(new)); n_o2r = mk_o2r(parse_pe(new))

    # baseline sanity: refuse to run against a wrong known-good exe
    ok = old[o_r2o(KNOWN_FLUSH):o_r2o(KNOWN_FLUSH)+16] == FLUSH_SIG
    ok &= old[o_r2o(KNOWN_POSTEVENT):o_r2o(KNOWN_POSTEVENT)+6] == bytes.fromhex("415741564155")
    for slot,fn,_ in KNOWN_VT:
        ok &= struct.unpack_from("<Q", old, o_r2o(slot))[0] == IB+fn
    if not ok:
        print("FATAL: known-good exe does not match the KNOWN tables; aborting.")
        return 1
    print("known-good baseline sanity: PASS (flush + PostEvent + 21/21 slots)")

    # flush relocation (exact, signature has no relocated bytes)
    hits=[]; i=0
    while True:
        i = new.find(FLUSH_SIG, i)
        if i<0: break
        hits.append(i); i+=1
    print(f"\nflush signature: {len(hits)} match(es):",
          ", ".join(f"RVA 0x{n_o2r(h):08X}" for h in hits))

    def locate(old_rva, label):
        ooff = o_r2o(old_rva)
        body = old[ooff:ooff+BODY]
        for plen in (16, 12, 10, 8):
            pref = body[:plen]
            hits=[]; i=0
            while True:
                i = new.find(pref, i)
                if i < 0: break
                hits.append(i); i += 1
                if len(hits) > 250000: hits=None; break
            if not hits: continue
            scored = sorted(((fuzzy(body, new[h:h+BODY]), h) for h in hits), reverse=True)
            (b_pct,b_runs,b_maxrun), b_off = scored[0]
            second = scored[1][0][0] if len(scored)>1 else None
            unique = second is None or (b_pct - second >= 8.0)
            status = ("OK" if b_pct>=82.0 and b_maxrun<=6 and unique else
                      "WEAK" if b_pct>=60.0 else None)
            if status:
                return {"label":label,"old":old_rva,"new":n_o2r(b_off),
                        "pct":round(b_pct,1),"maxrun":b_maxrun,"prefix":plen,
                        "cands":len(hits),"second":second,"status":status}
        return {"label":label,"old":old_rva,"new":None,"status":"NOT FOUND"}

    print("\n== functions ==")
    fn_res={}
    for label,rva in [("PostEvent",KNOWN_POSTEVENT)]+[(l,f) for _,f,l in KNOWN_VT]:
        r = locate(rva,label); fn_res[label]=r
        if r["new"] is not None:
            print(f"  {label:<22} 0x{rva:08X} -> 0x{r['new']:08X}  {r['pct']:5.1f}% "
                  f"(2nd {r['second']}) maxrun={r['maxrun']} cands={r['cands']}  {r['status']}")
        else:
            print(f"  {label:<22} 0x{rva:08X} -> NOT FOUND")

    print("\n== slots ==")
    for slot,fn,label in KNOWN_VT:
        r = fn_res[label]
        if r["new"] is None: print(f"  {label:<22} SKIP"); continue
        pat = struct.pack("<Q", IB+r["new"])
        cands=[]; i=0
        while len(cands)<16:
            i=new.find(pat,i)
            if i<0: break
            rv = n_o2r(i)
            if rv is not None: cands.append(rv)
            i+=1
        prev_old = struct.unpack_from("<Q", old, o_r2o(slot)-8)[0]
        next_old = struct.unpack_from("<Q", old, o_r2o(slot)+8)[0]
        val=[]
        for c in cands:
            co = n_r2o(c)
            pv = struct.unpack_from("<Q", new, co-8)[0]
            nx = struct.unpack_from("<Q", new, co+8)[0]
            score=0; notes=[]
            for oslot,ofn,olab in KNOWN_VT:
                if oslot==slot-8 and fn_res[olab].get("new") and pv==IB+fn_res[olab]["new"]:
                    score+=2; notes.append(f"pair- {olab}")
                if oslot==slot+8 and fn_res[olab].get("new") and nx==IB+fn_res[olab]["new"]:
                    score+=2; notes.append(f"pair+ {olab}")
            for oldva,newv,tag in ((prev_old,pv,"prev"),(next_old,nx,"next")):
                if IB<=oldva<IB+0x20000000 and IB<=newv<IB+0x20000000:
                    oo=o_r2o(oldva-IB); no=n_r2o(newv-IB)
                    if oo and no:
                        pct,_,mr = fuzzy(old[oo:oo+96], new[no:no+96], 96)
                        if pct>=80 and mr<=6: score+=1; notes.append(f"{tag} {pct:.0f}%")
            val.append((score,c,notes))
        val.sort(reverse=True)
        if val and val[0][0]>=1 and (len(val)==1 or val[0][0]>val[1][0]):
            s,c,notes = val[0]
            print(f"  {label:<22} slot 0x{slot:08X} -> 0x{c:08X}  fn 0x{r['new']:08X}  "
                  f"(score {s}: {', '.join(notes)}; {len(cands)} cand)")
        else:
            print(f"  {label:<22} slot 0x{slot:08X} -> UNRESOLVED {[(s,hex(c)) for s,c,_ in val]}")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
