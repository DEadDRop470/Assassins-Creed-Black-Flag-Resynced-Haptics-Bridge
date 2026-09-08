#!/usr/bin/env python3
"""Static audit for ACBFHapticsBridge v2.3.7.4 / v8-r9l."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

OLD_SHAS = {
    "8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140",
    "19920f34bb2fac814023ee3c27f0ceca1872e80a2aafb37c092c419fff77cc0d",
}
NEW_SHA = "614dab4a20a5d5c6256792e1daa6d05669c97a751079b10df1725d6965ad766d"
FLUSH_SIGNATURE = bytes.fromhex(
    "48 89 5c 24 18 55 56 57 48 83 ec 20 0f b7 79 42"
)
POST_PREFIX = bytes.fromhex(
    "41 57 41 56 41 55 41 54 56 57 55 53 48 83 ec 58"
    " 45 31 f6 80 3d"
)
POST_MIDDLE = bytes.fromhex(
    "89 cd 44 8b 94 24 d0 00 00 00 4c 8b 9c 24 c0 00 00 00 4c 8b 2d"
)
POST_SUFFIX = bytes.fromhex("49 8d 85 10 01 00 00")

STATE_TARGETS = [
    ("InAir enter", 0x09151C18, 0x015273A0, 0x091E0E98, 0x01521C20),
    ("InAir exit", 0x09151C20, 0x017FC510, 0x091E0EA0, 0x017FC1E0),
    ("JumpOnSpot enter", 0x09152898, 0x05E7F6C0, 0x091E1B18, 0x05F1D4C0),
    ("FreeJump enter", 0x09152848, 0x06B118E0, 0x091E1AC8, 0x06BAF660),
    ("TargetedJump enter", 0x0914E4E8, 0x02750BF0, 0x091DD768, 0x027D5940),
    ("Freefall enter", 0x0914E3F8, 0x01FCD920, 0x091DD678, 0x01FA4D60),
    ("FallFromLedge enter", 0x09151F38, 0x06B01790, 0x091E11B8, 0x06B9F510),
    ("FallFromLedge exit", 0x09151F40, 0x06B01A30, 0x091E11C0, 0x06B9F7B0),
    ("TransitionExit", 0x0914E218, 0x06AFDB60, 0x091DD498, 0x06B9B8E0),
    ("Climb A enter", 0x09147A38, 0x0152D180, 0x091D6CB8, 0x01527A00),
    ("Climb A exit", 0x09147A40, 0x0102CC10, 0x091D6CC0, 0x0102B6F0),
    ("Climb B enter", 0x0920DC90, 0x06A98A70, 0x0929D130, 0x06B367F0),
    ("Climb B exit", 0x0920DC98, 0x06A99E00, 0x0929D138, 0x06B37B80),
    ("Swimming A enter", 0x09159718, 0x027D3CF0, 0x091E89B8, 0x02859AA0),
    ("Swimming A exit", 0x09159720, 0x018A3D00, 0x091E89C0, 0x018A05B0),
    ("Swimming B enter", 0x091A3928, 0x08065C00, 0x09232D28, 0x080F8A70),
    ("Swimming B exit", 0x091A3930, 0x08066540, 0x09232D30, 0x080F93B0),
    ("ShallowSwim enter", 0x0915AB98, 0x06C9ED00, 0x091E9E38, 0x06D3CB40),
    ("ShallowSwim exit", 0x0915ABA0, 0x06C9FAC0, 0x091E9E40, 0x06D3D900),
    ("HayStack enter", 0x09145208, 0x01B6C060, 0x091D4488, 0x01B6C0C0),
    ("HayStack exit", 0x09145210, 0x054F1850, 0x091D4490, 0x0559F190),
]


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(4 * 1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


class PE:
    def __init__(self, path: Path):
        self.path = path
        self.data = path.read_bytes()
        self.pe_offset = struct.unpack_from("<I", self.data, 0x3C)[0]
        if self.data[self.pe_offset:self.pe_offset + 4] != b"PE\0\0":
            raise ValueError(f"{path}: invalid PE signature")
        self.sections = []
        self.section_count = struct.unpack_from("<H", self.data, self.pe_offset + 6)[0]
        optional_size = struct.unpack_from("<H", self.data, self.pe_offset + 20)[0]
        self.optional = self.pe_offset + 24
        self.image_base = struct.unpack_from("<Q", self.data, self.optional + 0x18)[0]
        self.image_size = struct.unpack_from("<I", self.data, self.optional + 0x38)[0]
        section_table = self.optional + optional_size
        for index in range(self.section_count):
            offset = section_table + index * 40
            virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from(
                "<IIII", self.data, offset + 8
            )
            self.sections.append((virtual_address, virtual_size, raw_offset, raw_size))

    def rva_to_offset(self, rva: int) -> int:
        candidates = [
            (va, raw + rva - va)
            for va, virtual_size, raw, raw_size in self.sections
            if va <= rva < va + max(virtual_size, raw_size)
        ]
        if not candidates:
            raise ValueError(f"{self.path}: RVA 0x{rva:X} is not file-backed")
        return max(candidates)[1]

    def offset_to_rva(self, offset: int) -> int | None:
        candidates = [
            (va, va + offset - raw)
            for va, virtual_size, raw, raw_size in self.sections
            if raw <= offset < raw + raw_size
        ]
        return max(candidates)[1] if candidates else None

    def qword(self, rva: int) -> int:
        return struct.unpack_from("<Q", self.data, self.rva_to_offset(rva))[0]

    def dword(self, rva: int) -> int:
        return struct.unpack_from("<I", self.data, self.rva_to_offset(rva))[0]

    def c_string(self, rva: int) -> str:
        offset = self.rva_to_offset(rva)
        end = self.data.find(b"\0", offset)
        if end < 0:
            end = len(self.data)
        return self.data[offset:end].decode("ascii", "replace")

    def directory(self, index: int) -> tuple[int, int]:
        return struct.unpack_from("<II", self.data, self.optional + 0x70 + index * 8)

    def imports(self):
        import_rva, import_size = self.directory(1)
        if not import_rva:
            return
        maximum = import_size // 20 + 1 if import_size else 4096
        for descriptor_index in range(maximum):
            rva = import_rva + descriptor_index * 20
            oft, timestamp, forwarder, name_rva, first_thunk = struct.unpack_from(
                "<IIIII", self.data, self.rva_to_offset(rva)
            )
            if not (oft or timestamp or forwarder or name_rva or first_thunk):
                break
            if not name_rva or not first_thunk:
                continue
            dll = self.c_string(name_rva)
            for index in range(4096):
                thunk_rva = (oft or first_thunk) + index * 8
                thunk = self.qword(thunk_rva)
                if not thunk:
                    break
                slot_rva = first_thunk + index * 8
                if thunk & 0x8000000000000000:
                    yield dll, int(thunk & 0xFFFF), slot_rva
                else:
                    yield dll, self.c_string(int(thunk) + 2), slot_rva


def check(condition: bool, label: str, failures: list[str]) -> None:
    print(("PASS" if condition else "FAIL"), label)
    if not condition:
        failures.append(label)


def exact_rvas(pe: PE, signature: bytes) -> list[int]:
    result = []
    offset = 0
    while True:
        offset = pe.data.find(signature, offset)
        if offset < 0:
            return result
        rva = pe.offset_to_rva(offset)
        if rva is not None:
            result.append(rva)
        offset += 1


def post_anchor_rvas(pe: PE) -> list[int]:
    result = []
    offset = 0
    while True:
        offset = pe.data.find(POST_PREFIX, offset)
        if offset < 0:
            return result
        tail = pe.data[offset:offset + 64]
        if (
            len(tail) == 64
            and tail[25] == 0
            and tail[26:28] == b"\x0F\x84"
            and tail[32:53] == POST_MIDDLE
            and tail[57:64] == POST_SUFFIX
        ):
            rva = pe.offset_to_rva(offset)
            if rva is not None:
                result.append(rva)
        offset += 1


def audit_game(path: Path, expected_sha: set[str], flush_rva: int,
                post_rva: int, use_new_targets: bool,
                failures: list[str]) -> None:
    pe = PE(path)
    digest = sha256(path)
    check(digest in expected_sha, f"{path.name}: expected SHA-256 {digest}", failures)
    flushes = exact_rvas(pe, FLUSH_SIGNATURE)
    check(flushes == [flush_rva], f"{path.name}: unique flush RVA 0x{flush_rva:08X}", failures)
    posts = post_anchor_rvas(pe)
    check(posts == [post_rva], f"{path.name}: unique PostEvent RVA 0x{post_rva:08X}", failures)

    for label, old_slot, old_fn, new_slot, new_fn in STATE_TARGETS:
        slot, fn = (new_slot, new_fn) if use_new_targets else (old_slot, old_fn)
        valid = pe.qword(slot) == pe.image_base + fn
        check(valid, f"{path.name}: {label} slot 0x{slot:08X} -> 0x{fn:08X}", failures)

    imported = list(pe.imports())
    named = {(dll.lower(), name.lower()): slot for dll, name, slot in imported
             if isinstance(name, str)}
    game_input_ordinals = [slot for dll, name, slot in imported
                           if dll.lower() == "gameinput.dll" and name == 1]
    check(("xinput9_1_0.dll", "xinputgetstate") in named,
          f"{path.name}: XInputGetState named import", failures)
    check(("xinput9_1_0.dll", "xinputsetstate") in named,
          f"{path.name}: XInputSetState named import", failures)
    expected_iat = 0x1C6109E0 if use_new_targets else 0x1D0119E0
    check(named.get(("xinput9_1_0.dll", "xinputsetstate")) == expected_iat,
          f"{path.name}: XInputSetState IAT RVA 0x{expected_iat:08X}", failures)
    check(bool(game_input_ordinals), f"{path.name}: GameInput ordinal 1 import", failures)


def audit_source(path: Path, failures: list[str]) -> None:
    source = path.read_text()
    checks = {
        "v8-r9l runtime": "v8-r9l-v2.3.7.4-1.06-1.07-compat" in source,
        "unique flush scanner": "find_unique_exact_rva" in source,
        "unique PostEvent scanner": "find_unique_v8_post_event_rva" in source,
        "1.07 state table": "0x091E0E98ull,0x01521C20ull" in source,
        "1.07 XInput profile": "R9K_PROFILE_STEAM_TU_107" in source,
        "GameInput rumble routine retained": "static bool set_gameinput_rumble" in source,
        "XInput rumble routine retained": "static bool set_xinput_rumble" in source,
        "rumble output routine retained": "static void write_rumble_output" in source,
        "native sink routine retained": "static bool inject_sink_block" in source,
    }
    for label, condition in checks.items():
        check(condition, f"source: {label}", failures)


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
    check(b"v8-r9l-v2.3.7.4-1.06-1.07-compat" in data,
          "ASI: release runtime string", failures)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=Path)
    parser.add_argument("--ini", type=Path)
    parser.add_argument("--asi", type=Path)
    parser.add_argument("--game106", type=Path)
    parser.add_argument("--game107", type=Path)
    args = parser.parse_args()
    failures: list[str] = []
    if args.source:
        audit_source(args.source, failures)
    if args.ini:
        ini = args.ini.read_text()
        for key in ("NativeHaptics=1", "ConventionalRumble=1", "PreferGameInput=1",
                    "RumbleOnDualSense=1", "MasterGain=0.90"):
            check(key in ini, f"INI: {key}", failures)
    if args.asi:
        audit_asi(args.asi, failures)
    if args.game106:
        audit_game(args.game106, OLD_SHAS, 0x04F78610, 0x0091DEA0, False, failures)
    if args.game107:
        audit_game(args.game107, {NEW_SHA}, 0x05028310, 0x0091D950, True, failures)
    print("FINAL", "PASS" if not failures else "FAIL")
    if failures:
        print("Failures:")
        for failure in failures:
            print(" -", failure)
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
