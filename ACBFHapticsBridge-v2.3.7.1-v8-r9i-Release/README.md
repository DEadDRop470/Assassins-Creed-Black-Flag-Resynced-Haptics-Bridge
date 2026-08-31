# ACBFHapticsBridge v2.3.7.1 / v8-r9i

Internal runtime: `v8-r9i-v2.3.7.1-ubisoft-output-hotfix`

v2.3.7.1 is an **output-path compatibility hotfix** over v2.3.7.0. Existing gameplay routing and haptic-effect tuning are retained; the supplied INI remains at `MasterGain=0.90`.

## What changed

- Black Flag's native Wwise / Quad Audio Haptics output remains the preferred DualSense path.
- If the verified native Quad flush is dormant at runtime, r9i can render the existing bridge waveform directly to a DualSense-compatible **48 kHz / 4-channel** controller audio endpoint, using channels 3/4 for the left/right actuators.
- Native Quad output can retake ownership without double-mixing the takeover block.
- `XInputSetState` observation is resolved from the live PE import table first, with verified profile RVAs as a fail-closed fallback.
- GameInput v0 rumble calls are logged as issued calls; when an XInput route is actually connected, r9i mirrors the motor state there and measures API success/failure.
- F10 adds an `R9I OUTPUT` line with native/direct haptic ownership and GameInput/XInput output diagnostics.

## Executable compatibility status

- **Steam TU 1.0.6 — r9i runtime confirmed PASS (2026-08-31).** SHA-256 `8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140`. Live r9i testing loaded all 21 gameplay-state hooks, resolved the XInput observer by named import at `0x1D0119E0`, and successfully activated the 48 kHz / 4-channel direct DualSense path during F9 testing.
- **Ubisoft Connect — static target validation PASS.** SHA-256 `e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c`.
- **Ubisoft+ — static target validation PASS.** SHA-256 `b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75`.

The two Ubisoft executables retain the verified Quad-flush signature, Wwise PostEvent signature, and all 21 gameplay-state callback slots used by the bridge. Runtime gameplay on those installations still needs a returned r9i log before being labeled runtime-confirmed.

## Release files

- `ACBFHapticsBridge-v8-r9i.asi` — current ASI binary.
- `ACBFHapticsBridge-v8.ini` — required runtime configuration filename; intentionally not renamed because the ASI opens this exact path.
- `ShadowsHapticsPack-v5.hfx` — current HFX package; unchanged from v2.3.7.0.
- `version.dll` — bundled third-party ASI loader; unchanged from v2.3.7.0 and governed by its own terms.
- `ACBFHapticsBridge-v2.3.7.1-NONCOMMERCIAL-LICENSE.txt` — release license/third-party notices.
- `CHANGELOG-v2.3.7.0-to-v2.3.7.1.md` — public update notes.
- `PACKAGE-CHECKSUMS.sha256` — SHA-256 manifest for every packaged file except the manifest itself.
- `Source/` — exact source and build/reconstruction material used for the release.

## Build

- ASI SHA-256: `8a68e6bcec34943f6b1afcf0ce9a063a592b36ecf2bfb3643b30d3abb7c36987`
- Exact source used to build it: `Source/src/main.cpp`
- Source SHA-256: `982d03fa68e20b12cf160e3a198f24579b8698ff168b552fda7e3256c33c583e`
- INI SHA-256: `d1fd7efc9744d836183004d4a6ff3ea0a53416012651f6455361e969b6642a46`

See `BUILD-VERIFICATION.md`, `V8-R9I-CHANGES.md`, `UBISOFT-CROSSREFERENCE.md`, and `Source/` for the complete verification/build material.
