#!/usr/bin/env python3
"""Audit v8-r9i source/INI and, when supplied, the built PE32+ ASI."""
from __future__ import annotations
import argparse
from pathlib import Path
import struct
import hashlib
import sys

RUNTIME = b"v8-r9i-v2.3.7.1-ubisoft-output-hotfix"
OLD_RUNTIME = b"v8-r9h-v2.3.7.0-multiversion"


def sha256(p: Path) -> str:
    h = hashlib.sha256()
    with p.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def audit_source(p: Path) -> bool:
    s = p.read_text(encoding="utf-8")
    checks = {
        "r9i runtime": "v8-r9i-v2.3.7.1-ubisoft-output-hotfix" in s,
        "direct renderer": "service_direct_dualsense_haptics(now);" in s,
        "race-safe takeover": "previousOutputOwner != 2u" in s,
        "dynamic XInput IAT": "r9i_find_xinput_setstate_iat" in s,
        "measured XInput route": "xinput_rumble_route_available(now)" in s,
        "r9i diagnostics": "R9I OUTPUT owner=" in s,
        "old DS skip removed": "if (!atomic_load_u32(&g_activeDualSense) || !gameInputWrote)" not in s,
    }
    ok = all(checks.values())
    for k, v in checks.items(): print(("PASS" if v else "FAIL"), "source", k)
    return ok


def audit_ini(p: Path) -> bool:
    s = p.read_text(encoding="utf-8")
    checks = {
        "MasterGain=0.90": "MasterGain=0.90" in s,
        "DirectDualSenseFallback=1": "DirectDualSenseFallback=1" in s,
        "fallback delay=1000": "DirectDualSenseFallbackDelayMs=1000" in s,
        "native haptics enabled": "NativeHaptics=1" in s,
        "conventional rumble enabled": "ConventionalRumble=1" in s,
    }
    ok = all(checks.values())
    for k, v in checks.items(): print(("PASS" if v else "FAIL"), "INI", k)
    return ok


def audit_pe(p: Path) -> bool:
    data = p.read_bytes()
    ok = True
    def chk(cond: bool, label: str):
        nonlocal ok
        print(("PASS" if cond else "FAIL"), "PE", label)
        ok &= cond
    if len(data) < 0x100 or data[:2] != b"MZ":
        chk(False, "MZ header"); return False
    peoff = struct.unpack_from("<I", data, 0x3C)[0]
    chk(peoff + 0x108 <= len(data), "PE header in file")
    if peoff + 0x108 > len(data): return False
    chk(data[peoff:peoff+4] == b"PE\0\0", "PE signature")
    machine, nsec, timestamp, _, _, optsz, chars = struct.unpack_from("<HHIIIHH", data, peoff + 4)
    chk(machine == 0x8664, "AMD64 machine")
    chk(bool(chars & 0x2000), "DLL characteristic")
    chk(timestamp == 0, "deterministic COFF timestamp=0")
    opt = peoff + 24
    magic = struct.unpack_from("<H", data, opt)[0]
    chk(magic == 0x20B, "PE32+")
    if magic != 0x20B: return False
    dllchars = struct.unpack_from("<H", data, opt + 70)[0]
    chk(bool(dllchars & 0x20), "HIGH_ENTROPY_VA")
    chk(bool(dllchars & 0x40), "DYNAMIC_BASE / ASLR")
    chk(bool(dllchars & 0x100), "NX_COMPAT")
    # PE32+ import directory = DataDirectory[1] at optional header + 112 + 8.
    # GNU PE ld can emit a 24-byte import-directory placeholder whose first
    # IMAGE_IMPORT_DESCRIPTOR is all-zero.  The released r9h ASI has that exact
    # layout, so a non-zero directory RVA alone is not proof of static imports.
    import_rva, import_sz = struct.unpack_from("<II", data, opt + 120)
    sec_off = opt + optsz

    def rva_to_file_off(rva: int):
        for j in range(nsec):
            so = sec_off + j * 40
            if so + 40 > len(data):
                break
            vsize, va, rawsize, raw = struct.unpack_from("<IIII", data, so + 8)
            span = max(vsize, rawsize)
            if va <= rva < va + span:
                out = raw + (rva - va)
                return out if out < len(data) else None
        return rva if rva < len(data) else None

    no_real_imports = import_rva == 0 or import_sz == 0
    if not no_real_imports:
        io = rva_to_file_off(import_rva)
        # IMAGE_IMPORT_DESCRIPTOR is 20 bytes.  An all-zero first descriptor
        # terminates the table immediately and therefore represents no imports.
        no_real_imports = bool(io is not None and io + 20 <= len(data)
                               and data[io:io+20] == b"\0" * 20)
    chk(no_real_imports, "no real static imports (empty GNU ld descriptor allowed)")
    rwx = []
    for i in range(nsec):
        off = sec_off + i * 40
        if off + 40 > len(data):
            rwx.append("<truncated>"); break
        rawname = data[off:off+8].split(b"\0",1)[0]
        name = rawname.decode("ascii", "replace")
        characteristics = struct.unpack_from("<I", data, off + 36)[0]
        execute = bool(characteristics & 0x20000000)
        write = bool(characteristics & 0x80000000)
        if execute and write: rwx.append(name)
    chk(not rwx, "no RWX sections" + (f" ({rwx})" if rwx else ""))
    chk(RUNTIME in data, "r9i runtime string present")
    chk(OLD_RUNTIME not in data, "old r9h runtime string absent")
    print("INFO PE sha256", sha256(p))
    print("INFO PE bytes", len(data))
    return ok


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("asi", nargs="?", type=Path)
    ap.add_argument("source", nargs="?", type=Path)
    ap.add_argument("ini", nargs="?", type=Path)
    args = ap.parse_args()
    ok = True
    if args.source: ok &= audit_source(args.source)
    if args.ini: ok &= audit_ini(args.ini)
    if args.asi: ok &= audit_pe(args.asi)
    if not any((args.asi, args.source, args.ini)):
        print("ERROR: provide ASI and/or source/INI paths", file=sys.stderr); return 2
    print("FINAL", "PASS" if ok else "FAIL")
    return 0 if ok else 1

if __name__ == "__main__":
    raise SystemExit(main())
