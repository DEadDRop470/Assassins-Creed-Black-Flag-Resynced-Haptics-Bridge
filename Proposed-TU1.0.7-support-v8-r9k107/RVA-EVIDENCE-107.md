# ACBFHapticsBridge — Steam TU 1.0.7 retarget evidence (v8-r9k107)

Analysis date: 2026-09-02. All analysis was read-only against the installed
executable; no game file was modified.

## Executable identity

| | Steam TU 1.0.6 (mod baseline) | Steam TU 1.0.7 (installed) |
|---|---|---|
| SHA-256 | `8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140` | `614dab4a20a5d5c6256792e1daa6d05669c97a751079b10df1725d6965ad766d` |
| File size | 478,845,280 | 469,524,824 |
| SizeOfImage | — | 0x1D544000 (matches the startup log `gameSize`) |
| ImageBase | 0x140000000 | 0x140000000 |

The 1.0.7 exe is a genuine rebuild (VMProtect-style section layout: `.vmp0–4`,
oversized leading section). File→RVA mapping for the code region: first section
VA 0x1000, raw 0x600 (delta 0xA00).

## 1. Quad Audio Haptics flush — RELOCATED, VERIFIED (INI fix)

The exact 16-byte prologue the ASI validates before patching
(`install_flush_hook`, src/main.cpp):

```
48 89 5C 24 18 55 56 57 48 83 EC 20 0F B7 79 42
```

Scanned over the full 469 MB file (no wildcards, no masking needed — the
signature contains no relocated bytes): **exactly one match**.

- Old RVA `0x04F78610` (1.0.6) → new RVA **`0x05028310`** (1.0.7), shift +0xAFD00.
- Bytes at new RVA 0x05028310 (32 bytes, for Ghidra cross-check):
  `48 89 5C 24 18 55 56 57 48 83 EC 20 0F B7 79 42 4C 8D 44 24 40 0F B7 71 40 33 ED 48 89 6C 24 40`
  (prologue + `lea r8,[rsp+40h] / movzx esi,word [rcx+40h] / xor ebp,ebp …` —
  consistent with the sink-flush reading channel fields at rcx+0x40/0x42).
- Bytes now at the old RVA 0x04F78610: `90 F3 90 F3 …` (padding) — the function
  moved, nothing else lives there.
- INI change: `[Hook] QuadHapticsFlushRVA=0x05028310`. The ASI still verifies
  the 16 bytes at runtime before patching (fail-closed preserved).

## 2. XInputSetState IAT — RELOCATED, VERIFIED (source patch)

Walked the 1.0.7 PE import directory (data dir[1] at RVA 0x1C8379B8):
descriptor 25 = `XINPUT9_1_0.dll`, OFT 0x1C8386EC, IAT 0x1C6109D8; the
`XInputSetState` name thunk resolves to IAT slot RVA **`0x1C6109E0`**
(1.0.6 Steam value was `0x1D0119E0`).

Source patch adds `0x1C6109E0` → `R9K_PROFILE_STEAM_FAMILY` in
`r9k_profile_from_xinput_rva()` and to the `known[]` fail-closed fallback list.
The 1.0.6/Ubisoft constants are untouched. At runtime the slot is still found
dynamically by named-import walk and pointer-validated before any IAT write.

> **UPDATE (same day, after obtaining the TU 1.0.6 depot):** items 3 and 4
> below were fully resolved by byte-diffing the verified 1.0.6 executable
> (Steam depot 3751951, manifest 4397710407098141927, SHA-256 confirmed
> `8d522381…`) against the installed 1.0.7. See sections 5–6.

## 3. Wwise PostEvent (semantic router) — MOVED, NOT YET RELOCATED (superseded by §5)

- Compiled-in 1.0.6 RVA `0x0091DEA0`; bytes there in 1.0.7 are now
  `12 FF FF FF BF 13 00 00 0C 81 FA 13 00 00 0C 0F` (mid-function garbage —
  the code region shifted).
- The only recorded signature is the 6-byte prologue `41 57 41 56 41 55`
  (push r15/r14/r13), which is far too common to identify the function alone.
- The 28 semantic Wwise event IDs from the source (EVT_*) appear **zero** times
  as immediates in the 1.0.7 file — event posting is data-driven, so call-site
  anchoring is impossible.
- No longer fingerprint exists anywhere in the repo (audits recorded only the
  6 bytes; older source predates this hook).

Mitigation shipped: `PostEventRVA` is now INI-overridable ([Hook] section) and a
PostEvent install failure no longer aborts the bridge (it logs and continues
with the semantic router disabled; the site is never patched on a mismatch).
`[SemanticEvents] Enabled=0` until a verified 1.0.7 RVA exists.

## 4. 21 gameplay-state vtable hooks — MOVED, NOT YET RELOCATED

- All 21 slot RVAs / expected-function RVAs are compiled in
  (`install_gameplay_state_hooks`). On 1.0.7, 19 of the 21 old slot locations
  hold non-pointer data; 2 hold pointers to unrelated code — every pair fails
  the fail-closed `slot == base+expectedFn` check, so nothing is patched.
- Uniform-shift hypothesis tested: searched the whole `.impdata` section
  (2.4M qwords) for a single delta D where all 21 old slots+D hold valid code
  VAs. 125,209 positions pass that weak test, but the strong invariant —
  enter/exit function deltas matching 1.0.6 — never exceeds 1/7 pair
  agreements. The state vtables were re-laid-out individually; no en-bloc shift
  exists.
- The state-machine name strings (`InAir_FreeJump_Entry`, `HayStack_Entry`, …)
  exist in 1.0.7 but are not referenced by absolute pointers, so they cannot
  anchor the vtables statically.
- `[GameplayState] Enabled=0` in the INI for clean logs (they would fail closed
  anyway).

## Recovering items 3 and 4 (requires the TU 1.0.6 exe)

With the 1.0.6 executable on hand, both recover mechanically and safely:
take 32–64 bytes at each 1.0.6 function RVA, mask rel32/RIP displacement bytes,
scan 1.0.7 for a unique match → new function RVA; then find the vtable slot as
the unique data qword holding `0x140000000 + newFnRVA`. To fetch the old build
without touching the current install: Steam client → `steam://open/console` →
`download_depot 3751950 <depotID> <manifestID>` using the previous manifest ID
listed on SteamDB (app 3751950 → Depots → the ~470 MB exe depot → Manifests).
The depot lands in `steamapps/content/` and does not modify the game.

## Build (v8-r9k107)

- Source: r9k baseline + the minimal diffs above (`src/main.cpp`), built with
  `build_msys2_pe_r9k107.sh` (MSYS2 mingw64 g++ 16.2.0, same flags as
  `build_linux_pe_r9k.sh`, plus static libgcc for the `___chkstk_ms` stack
  probe the mingw target emits).
- ASI SHA-256 `6c3efa041ce6da94595342e4de6004049ebe67c87c2a0ba036d5b1f053908484`,
  187,392 bytes; two consecutive builds byte-identical.
- `audit_r9k.py`: all source/INI checks PASS; PE checks PASS (AMD64 PE32+ DLL,
  timestamp 0, ASLR/NX/High-Entropy-VA, no RWX sections, runtime string
  present). The only FAIL is the pinned upstream release SHA-256, which cannot
  match a modified build by definition.
- Unmodified companion files (hashes match upstream BUILD-VERIFICATION.md):
  `version.dll` `6203c5a0…`, `ShadowsHapticsPack-v5.hfx` `9e994203…`.

---

# Resolution via 1.0.6 depot diff (2026-09-02, later the same day)

Method: for each 1.0.6 function, enumerate every 1.0.7 occurrence of its
prologue prefix (12–16 bytes) and score the first 160 bytes with a
displacement-tolerant comparator (mismatches only allowed in runs of <=4
bytes, i.e. rel32/disp32/imm32). Accept only a clear unique winner (>=82%
match, next-best at least 8 points behind, no mismatch run > 6). Slots were
then found as the unique data qword holding ImageBase+newFn and validated by
enter/exit pair adjacency (+8) and by fuzzy-matching the code of the
neighboring vtable entries against their 1.0.6 counterparts. Scripts:
`tools/retarget_locate.py`, reproducible via
`tools/retarget_locate.py`. 1.0.6 baseline sanity passed 21/21 slots +
flush + PostEvent before any matching.

## 5. Wwise PostEvent — RESOLVED

- `0x0091DEA0` (1.0.6) → **`0x0091D950`** (1.0.7)
- 96.2% of 160 bytes identical, 2 mismatch runs (max 3 bytes, RIP-relative
  displacements); next-best candidate of 5,161 scored 23.1%.
- INI: `[Hook] PostEventRVA=0x0091D950`, `[SemanticEvents] Enabled=1`.

## 6. 21 gameplay-state hooks — RESOLVED (dual-profile in v8-r9k107)

| State | slot 1.0.6→1.0.7 | fn 1.0.6→1.0.7 | fn match | validation |
|---|---|---|---|---|
| InAir enter | 0x09151C18→0x091E0E98 | 0x015273A0→0x01521C20 | 90.0% (2nd 36.9) | pair+, prev 100%, next 95% |
| InAir exit | 0x09151C20→0x091E0EA0 | 0x017FC510→0x017FC1E0 | 93.8% (2nd 30.6) | pair-, prev 94% |
| JumpOnSpot enter | 0x09152898→0x091E1B18 | 0x05E7F6C0→0x05F1D4C0 | 96.2% (2nd 36.2) | prev 100%, next 89% |
| FreeJump enter | 0x09152848→0x091E1AC8 | 0x06B118E0→0x06BAF660 | 96.2% (2nd 90.0)¹ | prev 100%, next 92% |
| TargetedJump enter | 0x0914E4E8→0x091DD768 | 0x02750BF0→0x027D5940 | 89.4% (2nd 81.2) | prev 100%, next 88% |
| Freefall enter | 0x0914E3F8→0x091DD678 | 0x01FCD920→0x01FA4D60 | 92.5% (2nd 26.2) | prev 100% |
| FallFromLedge enter | 0x09151F38→0x091E11B8 | 0x06B01790→0x06B9F510 | 96.2% (2nd 28.8) | pair+, prev 100%, next 85% |
| FallFromLedge exit | 0x09151F40→0x091E11C0 | 0x06B01A30→0x06B9F7B0 | 87.5% (2nd 52.5) | pair-, prev 97% |
| TransitionExit enter | 0x0914E218→0x091DD498 | 0x06AFDB60→0x06B9B8E0 | 98.1% (2nd 78.1) | prev 100% |
| Climb A enter | 0x09147A38→0x091D6CB8 | 0x0152D180→0x01527A00 | 88.8% (2nd 79.4) | pair+, prev 100%, next 92% |
| Climb A exit | 0x09147A40→0x091D6CC0 | 0x0102CC10→0x0102B6F0 | 95.0% (2nd 27.5) | pair-, prev 89% |
| Climb B enter | 0x0920DC90→0x0929D130 | 0x06A98A70→0x06B367F0 | 94.4% (2nd 30.6) | pair+, prev 100%, next 84% |
| Climb B exit | 0x0920DC98→0x0929D138 | 0x06A99E00→0x06B37B80 | 90.6% (2nd 40.0) | pair-, prev 91% |
| Swimming A enter | 0x09159718→0x091E89B8 | 0x027D3CF0→0x02859AA0 | 90.0% (2nd 48.8) | pair+, prev 100%, next 100% |
| Swimming A exit | 0x09159720→0x091E89C0 | 0x018A3D00→0x018A05B0 | 98.8% (2nd 44.4)² | pair-, prev 93% |
| Swimming B enter | 0x091A3928→0x09232D28 | 0x08065C00→0x080F8A70 | 98.8% (2nd 88.8) | pair+, prev 100%, next 92% |
| Swimming B exit | 0x091A3930→0x09232D30 | 0x08066540→0x080F93B0 | 90.0% (2nd 29.4) | pair-, prev 98% |
| ShallowSwim enter | 0x0915AB98→0x091E9E38 | 0x06C9ED00→0x06D3CB40 | 100.0% (2nd 93.1)² | pair+, prev 100%, next 94% |
| ShallowSwim exit | 0x0915ABA0→0x091E9E40 | 0x06C9FAC0→0x06D3D900 | 93.1% (unique)² | pair-, prev 100% |
| HayStack enter | 0x09145208→0x091D4488 | 0x01B6C060→0x01B6C0C0 | 91.2% (2nd 41.9) | pair+, prev 100%, next 94% |
| HayStack exit | 0x09145210→0x091D4490 | 0x054F1850→0x0559F190 | 91.2% (2nd 33.1) | pair-, prev 95%, next 94% |

Every slot had exactly ONE pointer candidate in the whole 1.0.7 image.
"pair+/pair-" = the adjacent slot holds the independently-located enter/exit
partner. ¹ close 2nd is a sibling jump-state function; the slot pointer and
neighbors disambiguate. ² independently confirmed by the earlier exact-bytes
stage (the exact-bytes pre-stage).

Source: `install_gameplay_state_hooks` now dispatches per pair via
`patch_state_vtable2` (tries 1.0.6, then 1.0.7; the write still only happens
when the slot content equals ImageBase+expectedFn, so unknown future patches
keep failing closed).

## Final build (v8-r9k107, full 1.0.7 support)

- ASI SHA-256 `26d0db584e98b9e9245c800f91131eb7f0f0ed4d3644919186cb1485b9995056`
  (deterministic: two consecutive builds identical).
- audit_r9k.py: all source/INI/PE checks PASS except the pinned upstream
  release SHA (expected for a modified build).
- INI active values: QuadHapticsFlushRVA=0x05028310, PostEventRVA=0x0091D950,
  SemanticEvents Enabled=1, GameplayState Enabled=1.
