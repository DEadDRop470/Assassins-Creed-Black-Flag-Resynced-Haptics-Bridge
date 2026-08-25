# Source layout

- `src/main.cpp` — exact C++ source compiled for ACBFHapticsBridge v8-r4.
- `build_linux_pe.sh` — deterministic freestanding PE/ASI build script used for this release.
- `audit_v8_r4.py` — static PE + official/Voices38 hook-signature audit helper.
- `validate_hfx.py` — HFX container validator/listing helper.
- `ACBFHapticsBridge-v8.ini` — shipped r4 configuration.
- `README.md`, `V8-R4-CHANGES.md`, `EVENT-MAP.md`, `TEST-CHECKLIST.md` — release documentation.
- `References/` — older reference/reconstruction material kept for provenance; it is not the v8-r4 compiled source.

Generated `build/main.o` is an intermediate binary object and is **not source**. The release/source packages omit intermediates unless specifically documented.
