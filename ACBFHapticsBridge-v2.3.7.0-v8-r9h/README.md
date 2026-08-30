# ACBFHapticsBridge v2.3.7.0 / v8-r9h

Internal runtime: `v8-r9h-v2.3.7.0-multiversion`

v2.3.7.0 is a compatibility-only update over v2.3.6 / v8-r9g. Haptic tuning, semantic Wwise routing, gameplay-state hooks, timings, controller polling, native haptic injection, menu behavior, effect logic and conventional-rumble tuning are unchanged.

The only runtime compatibility change is the native XInput observer. Black Flag keeps the same verified haptic/gameplay code in the tested executables, but the `XInputSetState` import slot moves between distributions. v8-r9h verifies and supports the known slots for Steam TU 1.0.6 / voices38, Ubisoft Connect and Ubisoft+ before patching. Unknown or mismatched slots remain fail-closed and untouched.

Verified executable fingerprints:

- Steam TU 1.0.6: `8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140`
- voices38 profile: `19920f34bb2fac814023ee3c27f0ceca1872e80a2aafb37c092c419fff77cc0d`
- Ubisoft Connect: `e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c`
- Ubisoft+: `b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75`

`PollMs=2` remains the bridge sampling cadence, not a controller overclock. F10 writes status/performance diagnostics, F11 toggles the complete bridge, and F12 suspends controller polling/raw-input processing for A/B diagnostics while semantic/state/native routing remains available.

The release includes the exact source used to compile the shipped ASI under `Source/`.
