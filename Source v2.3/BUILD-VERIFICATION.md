# ACBFHapticsBridge v8-r4 build verification

Build audited: `ACBFHapticsBridge-v8-r4.asi`

## Deterministic build

Two consecutive clean invocations of `build_linux_pe.sh` produced byte-identical ASI files.

- ASI SHA-256: `d8276d2a197ac122bc648d072dce47d4085d1b685989175edca66f92c1eb2f00`

## PE audit

`audit_v8_r4.py` result: PASS

- PE32+ x86-64 DLL
- 16 sections
- PE timestamp = 0
- ASLR = yes
- NX = yes
- high-entropy VA = yes
- RWX sections = none
- import directory is null-only (freestanding build)
- COFF symbols stripped

## Game signature audit

Official Steam/Ubisoft TU 1.0.6:

- SHA-256 `8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140`
- PostEvent RVA `0x0091DEA0` signature: PASS
- QuadFlush RVA `0x04F78610` signature: PASS

Voices38 profile:

- SHA-256 `19920f34bb2fac814023ee3c27f0ceca1872e80a2aafb37c092c419fff77cc0d`
- PostEvent RVA `0x0091DEA0` signature: PASS
- QuadFlush RVA `0x04F78610` signature: PASS

## Runtime HFX validation

`ShadowsHapticsPack-v5.hfx` validated successfully:

- 14 effects
- 48,000 Hz
- stereo
- 13,228,728 bytes

## Source note

`src/main.cpp` is the exact C++ source used to build this release. `References/` contains older provenance/reference material and is not the compiled r4 source. Object files and other build intermediates are not source and are not included in the clean source package.
