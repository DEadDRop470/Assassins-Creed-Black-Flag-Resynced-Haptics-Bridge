#!/usr/bin/env python3
"""Audit the verified ACBFHapticsBridge v7-r7-kick-walk4x ASI."""
from pathlib import Path
import hashlib, struct, sys

EXPECTED_SHA256 = "fb77b55728185e453fcc2046716f1d299bb78e92aededae906c9c56db58383b9"
EXPECTED_STUB = bytes.fromhex(
    "50 0f b6 05 f4 21 00 00 38 84 24 3f 01 00 00 58 "
    "72 05 e9 10 f1 ff ff e9 39 d5 ff ff"
)
EXPECTED_BRANCH = bytes.fromhex("0f 85 ab 2a 00 00")
EXPECTED_GAIN_PTR = bytes.fromhex("48 b8 54 96 01 80 01 00 00 00")


def parse_pe(data: bytes):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe:pe+4] != b"PE\0\0":
        raise ValueError("not PE")
    coff = pe + 4
    count = struct.unpack_from("<H", data, coff + 2)[0]
    opt_size = struct.unpack_from("<H", data, coff + 16)[0]
    opt = coff + 20
    image_base = struct.unpack_from("<Q", data, opt + 24)[0]
    sec = opt + opt_size
    sections = []
    for i in range(count):
        o = sec + i * 40
        name = data[o:o+8].rstrip(b"\0").decode("ascii", "replace")
        vsize, rva, raw_size, raw_ptr = struct.unpack_from("<IIII", data, o + 8)
        sections.append((name, vsize, rva, raw_size, raw_ptr, o))
    return image_base, sections


def rva_to_offset(rva, sections):
    for name, vsize, base, raw_size, raw_ptr, _ in sections:
        if base <= rva < base + max(vsize, raw_size):
            return raw_ptr + (rva - base)
    raise ValueError(f"RVA not mapped: 0x{rva:X}")


def check(label, ok):
    print(("PASS" if ok else "FAIL"), label)
    return ok


def main(path: Path):
    data = path.read_bytes()
    ok = True
    ok &= check("SHA-256", hashlib.sha256(data).hexdigest() == EXPECTED_SHA256)
    image_base, sections = parse_pe(data)
    ok &= check("image base 0x180000000", image_base == 0x180000000)

    def at(rva, n):
        off = rva_to_offset(rva, sections)
        return data[off:off+n]

    ok &= check("Square/X redirect at RVA 0x148D7", at(0x148D7, 6) == EXPECTED_BRANCH)
    ok &= check("28-byte R7 kick stub at RVA 0x17388", at(0x17388, 28) == EXPECTED_STUB)
    ok &= check("raw-X gain pointer -> LBEffectGain", at(0x164C5, 10) == EXPECTED_GAIN_PTR)
    ok &= check("default TriggerThreshold = 160", struct.unpack("<I", at(0x19584, 4))[0] == 160)
    ok &= check("LBEffectGain = 0.45", abs(struct.unpack("<f", at(0x19654, 4))[0] - 0.45) < 1e-6)

    text = next(s for s in sections if s[0] == ".text")
    ok &= check(".text VirtualSize = 0x163A4", text[1] == 0x163A4)
    ok &= check("build label present", b"v7-r7-kick-walk4x" in data)
    ok &= check("v7 INI path present", "ACBFHapticsBridge-v7.ini".encode("utf-16le") in data)
    ok &= check("v7 log path present", "ACBFHapticsBridge-v7.log".encode("utf-16le") in data)

    print("FINAL", "PASS" if ok else "FAIL")
    return 0 if ok else 1

if __name__ == "__main__":
    p = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(__file__).parent / "Base" / "ACBFHapticsBridge-v7-r7-original.asi"
    raise SystemExit(main(p))
