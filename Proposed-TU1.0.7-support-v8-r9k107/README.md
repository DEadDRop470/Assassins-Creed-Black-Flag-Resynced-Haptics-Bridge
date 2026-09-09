# Proposed: Steam TU 1.0.7 support (v8-r9k107)

The 2026-09-01 Steam update (TU 1.0.7, exe SHA-256 `614dab4a…`, 469,524,824
bytes) rebuilt `ACBlackFlag.exe`; every r9k hook target moved and the bridge
fails closed at startup. This proposal retargets all of them and keeps the
TU 1.0.6 values intact, so one build supports both executables.

Tested in-game on Steam TU 1.0.7 with a physical DualSense:
`flush injector at RVA 0x5028310`, `PostEvent router at RVA 0x91D950`,
`Gameplay state vtable hooks installed=21 failed=0`,
`profile=Steam/voices38 xinputIAT=0x1C6109E0 method=named-import`.

## Changes to `Source/src/main.cpp` (based on v2.3.7.3 / v8-r9k)

1. **`[Hook] PostEventRVA` INI key** (default `0x0091DEA0`, the TU 1.0.6
   value). The 6-byte prologue signature is still verified before patching.
2. **PostEvent install failure is no longer fatal.** The worker previously
   exited (`return 0`) when the signature check failed, taking down the flush
   injector and all input fallbacks with it, although the log message already
   said "semantic router disabled". It now logs and continues with the
   semantic router off; the site is never patched on a mismatch.
3. **Dual-profile gameplay-state tables.** `patch_state_vtable2` tries the
   TU 1.0.6 slot/function pair, then the TU 1.0.7 pair, via a non-invasive
   probe; the write still only happens when the slot content equals
   `ImageBase+expectedFunctionRVA`, so unknown future executables keep
   failing closed per slot.
4. **TU 1.0.7 Steam XInput profile**: IAT slot `0x1C6109E0` added to
   `r9k_profile_from_xinput_rva()` (STEAM_FAMILY) and to the `known[]`
   fail-closed fallback list. Taken from the PE import directory
   (`XINPUT9_1_0.dll` → `XInputSetState`); runtime resolution is still the
   dynamic named-import walk.
5. Flush/PostEvent install log lines now print the configured RVA instead of
   the hardcoded 1.0.6 addresses.

`Source/ACBFHapticsBridge-v8.ini` ships TU 1.0.7 values active
(`QuadHapticsFlushRVA=0x05028310`, `PostEventRVA=0x0091D950`); a TU 1.0.6
install only needs those two keys flipped back (state hooks and profile
detection adapt automatically).

## Evidence

`RVA-EVIDENCE-107.md` documents every relocation with the matched bytes:
the flush prologue matches exactly once in the 1.0.7 image; PostEvent and the
21 state functions were located by diffing the depot-verified TU 1.0.6 exe
against 1.0.7 (unique displacement-tolerant prologue match per function,
96.2% body identity for PostEvent; every vtable slot had exactly one pointer
candidate, validated by enter/exit adjacency and neighbor-entry code).

`tools/retarget_locate.py` reproduces the whole derivation from the two
executables and is reusable for future TUs
(`python retarget_locate.py <known-good-exe> <new-exe>`).

## Notes / scope

- Only the **Steam** profile was re-derived; Ubisoft Connect / Ubisoft+
  TU 1.0.7 equivalents were not (no access to those executables). They keep
  their r9k values and fail closed on 1.0.7 until re-derived.
- `Source/build_msys2_pe_r9k107.sh` is a Windows/MSYS2 build variant used for
  testing (same flags; adds static libgcc for the `___chkstk_ms` probe mingw
  emits). The canonical release should come from the existing Linux pipeline —
  the source diff is the contribution, not the binary.
- Modified version of ACBFHapticsBridge v2.3.7.3, contributed under the
  ACBFHapticsBridge Non-Commercial License v1.1.
