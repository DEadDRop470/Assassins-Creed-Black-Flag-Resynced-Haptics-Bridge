# Build verification — v2.3.7.1 / v8-r9i

- Runtime: `v8-r9i-v2.3.7.1-ubisoft-output-hotfix`
- ASI: `ACBFHapticsBridge-v8-r9i.asi`
- SHA-256: `8a68e6bcec34943f6b1afcf0ce9a063a592b36ecf2bfb3643b30d3abb7c36987`
- ASI size: 206,848 bytes
- Source `src/main.cpp` SHA-256: `982d03fa68e20b12cf160e3a198f24579b8698ff168b552fda7e3256c33c583e`
- Release INI SHA-256: `d1fd7efc9744d836183004d4a6ff3ea0a53416012651f6455361e969b6642a46`
- PE audit: PASS
- Deterministic COFF timestamp: 0
- ASLR / dynamic base: enabled
- NX: enabled
- High-entropy VA: enabled
- RWX sections: none
- Static imports: none; GNU PE ld's null-only import-directory placeholder is accepted as empty, matching the byte-proven r9h linker layout

## Deterministic compiler-path proof

Before r9i was built, the uploaded v2.3.7.0 / v8-r9h source was rebuilt with its packaged build script. The rebuilt r9h ASI matched the shipped r9h ASI byte-for-byte at SHA-256:

`869361c87e445601d3d1ff5807922f4d0c60cb37f51a7bf37bdd05103514f914`

The r9i source was built through the same compiler/linker route. Rebuilding from the final packaged `Source/src/main.cpp` again reproduced the shipped r9i ASI byte-for-byte at:

`8a68e6bcec34943f6b1afcf0ce9a063a592b36ecf2bfb3643b30d3abb7c36987`

## Source/INI scope

The r9i source delta is confined to release/config metadata, direct DualSense output, native/direct ownership handoff, dynamic XInput-IAT resolution, GameInput/XInput rumble routing/diagnostics, status output, and worker service/shutdown integration. No gameplay/effect retuning was intentionally introduced.

Parsed INI comparison against r9h found:

- added `[Output] DirectDualSenseFallback=1`
- added `[Output] DirectDualSenseFallbackDelayMs=1000`
- no removed keys
- no changed existing values
- `MasterGain=0.90` unchanged

See `SOURCE-DELTA-AUDIT-v8-r9i.md` for the source-diff summary.

## Steam r9i runtime verification — PASS, 2026-08-31

The returned Steam TU 1.0.6 log confirms:

- all 14 HFX effects load;
- GameInput v0 initializes;
- the native Quad injector installs at RVA `0x04F78610`;
- Wwise PostEvent installs at RVA `0x0091DEA0`;
- gameplay-state hooks install `21/21`, failures `0`;
- the r9i XInput observer resolves by named PE import at IAT RVA `0x1D0119E0`;
- F9 reaches the new output-fallback path;
- a DualSense endpoint successfully opens at 48 kHz / 4 channels / 16 bit with actuator channels 3/4;
- semantic/state events continue afterward;
- user reported the Steam version's haptics/vibration behavior seemed good.

## Executable profile evidence

- Steam TU 1.0.6: **r9i runtime-confirmed PASS**, hash `8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140`.
- Ubisoft Connect: static core target validation PASS, hash `e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c`, verified XInputSetState IAT `0x1C5769E0`.
- Ubisoft+: static core target validation PASS, hash `b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75`, verified XInputSetState IAT `0x1EC409E0`.

For both Ubisoft executables, the Quad Audio Haptics flush, Wwise PostEvent producer, and 21/21 gameplay-state slots match the bridge's validated targets. r9i resolves the XInput observer dynamically first and adds the direct DualSense fallback for the observed case where a compatible runtime does not execute the native Quad flush.

Live Ubisoft Connect / Ubisoft+ r9i logs remain the final runtime confirmation for those editions.
