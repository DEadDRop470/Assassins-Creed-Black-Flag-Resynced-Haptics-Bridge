# Source layout — v2.3.7.1 / v8-r9i

`src/main.cpp` is the exact source for the shipped r9i ASI.

Current build/verification entry points:

- `build_linux_pe_r9i.sh` — deterministic r9i PE build.
- `build_linux_pe.sh` — convenience wrapper for the current r9i build.
- `audit_r9i.py` — current source/INI/PE sanity audit.
- `validate_hfx.py` — HFX package validator.
- `BUILD-VERIFICATION.md` — release hash/provenance, PE audit and Steam runtime verification.
- `SOURCE-DELTA-AUDIT-v8-r9i.md` — r9h → r9i scope check.
- `V8-R9I-CHANGES.md` and `R9I-IMPLEMENTATION.md` — implementation notes.
- `UBISOFT-CROSSREFERENCE.md` — Steam / Ubisoft Connect / Ubisoft+ executable/output cross-reference.
- `SOURCE-CHECKSUMS.sha256` — SHA-256 manifest for the Source tree except the manifest itself.

Older revision build scripts, audits and changelogs are retained as historical development/provenance material; their historical filenames are intentional and they are not the current build entry point.
