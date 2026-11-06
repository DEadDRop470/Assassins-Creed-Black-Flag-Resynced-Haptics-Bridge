#!/usr/bin/env python3
"""Validate and list an SHFXPK1 Shadows haptics pack."""
from __future__ import annotations
import argparse
import struct
from pathlib import Path

HEADER = struct.Struct("<8sIIIII4xQ")
ENTRY = struct.Struct("<32sQQfIII16x")

def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("pack", type=Path)
    args = ap.parse_args()
    data = args.pack.read_bytes()
    if len(data) < HEADER.size:
        raise SystemExit("pack is shorter than its header")
    magic, version, rate, channels, count, header_bytes, data_offset = HEADER.unpack_from(data)
    assert magic == b"SHFXPK1\0", magic
    assert version == 1
    assert rate == 48_000 and channels == 2
    assert header_bytes >= HEADER.size
    assert header_bytes + count * ENTRY.size <= data_offset <= len(data)
    print(f"{args.pack.name}: {count} effects, {rate} Hz, {channels} channels, {len(data)} bytes")
    for index in range(count):
        pos = header_bytes + index * ENTRY.size
        raw_name, offset, frames, peak, entry_rate, entry_channels, flags = ENTRY.unpack_from(data, pos)
        name = raw_name.split(b"\0", 1)[0].decode("ascii")
        assert entry_rate == rate and entry_channels == channels
        assert offset + frames * channels * 4 <= len(data)
        print(f"{index:02d} {name:32s} frames={frames:7d} seconds={frames/rate:7.3f} peak={peak:.6f} flags={flags}")

if __name__ == "__main__":
    main()

