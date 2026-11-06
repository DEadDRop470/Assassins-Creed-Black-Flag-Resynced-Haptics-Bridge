#!/usr/bin/env python3
"""Verify the Windows DLL format and release identifier of the shipping ASI."""
import struct,sys
from pathlib import Path
def check(condition,label,failures):
    print('PASS' if condition else 'FAIL',label)
    if not condition:failures.append(label)
def audit_asi(path: Path, failures: list[str]) -> None:
    data = path.read_bytes()
    check(data[:2] == b"MZ", "ASI: MZ header", failures)
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    check(data[pe_offset:pe_offset + 4] == b"PE\0\0", "ASI: PE signature", failures)
    machine, sections, timestamp, _, _, optional_size, characteristics = struct.unpack_from(
        "<HHIIIHH", data, pe_offset + 4
    )
    check(machine == 0x8664 and sections > 0, "ASI: AMD64 PE", failures)
    check(timestamp == 0, "ASI: deterministic timestamp", failures)
    check(bool(characteristics & 0x2000), "ASI: DLL characteristic", failures)
    optional = pe_offset + 24
    check(struct.unpack_from("<H", data, optional)[0] == 0x20B, "ASI: PE32+", failures)
    dll_characteristics = struct.unpack_from("<H", data, optional + 0x46)[0]
    check(bool(dll_characteristics & 0x20), "ASI: high entropy VA", failures)
    check(bool(dll_characteristics & 0x40), "ASI: ASLR", failures)
    check(bool(dll_characteristics & 0x100), "ASI: NX", failures)
    rwx = []
    section_table = optional + optional_size
    for index in range(sections):
        offset = section_table + index * 40
        characteristics = struct.unpack_from("<I", data, offset + 36)[0]
        if characteristics & 0x20000000 and characteristics & 0x80000000:
            rwx.append(index)
    check(not rwx, "ASI: no RWX sections", failures)
    check(b"v8-r9q-v2.3.7.9-auto-compat" in data,
          "ASI: release runtime string", failures)

failures=[]
audit_asi(Path(sys.argv[1]),failures)
print('FINAL','FAIL' if failures else 'PASS','PE audit')
sys.exit(bool(failures))
