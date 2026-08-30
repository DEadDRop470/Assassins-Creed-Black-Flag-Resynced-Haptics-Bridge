# Build verification — v2.3.7.0 / v8-r9h

- Runtime: `v8-r9h-v2.3.7.0-multiversion`
- ASI: `ACBFHapticsBridge-v8-r9h.asi`
- SHA-256: `869361c87e445601d3d1ff5807922f4d0c60cb37f51a7bf37bdd05103514f914`
- ASI size: 197,120 bytes.
- Deterministic source baseline: the uploaded v2.3.6 source rebuilt byte-identically to the uploaded v2.3.6 ASI (`4cf92a8e...c4d0`) before the r9h patch was applied.
- Static PE audit: PASS.
- Timestamp: 0.
- ASLR: enabled.
- NX: enabled.
- High-entropy VA: enabled.
- RWX sections: none.
- COFF symbols: stripped.
- Import directory: null-only.

Verified target fingerprints:

- Steam TU 1.0.6: `8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140`
- voices38: `19920f34bb2fac814023ee3c27f0ceca1872e80a2aafb37c092c419fff77cc0d`
- Ubisoft Connect: `e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c`
- Ubisoft+: `b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75`

Supplied Ubisoft executable static verification:

- Quad flush RVA `0x04F78610`: exact signature PASS on Ubisoft Connect and Ubisoft+.
- Wwise PostEvent RVA `0x0091DEA0`: exact signature PASS on Ubisoft Connect and Ubisoft+.
- Gameplay state slots: 21/21 exact on Ubisoft Connect and 21/21 exact on Ubisoft+.
- XInputSetState IAT: Ubisoft Connect `0x1C5769E0`; Ubisoft+ `0x1EC409E0`.
- Existing Steam/voices38 observer candidate `0x1D0119E0` retained.

Behavior/performance sanity checks: v8-r9g tuning and routing are intentionally unchanged; only the verified native-XInput observer profile selection was extended.

Runtime assets expected unchanged from the uploaded v2.3.6 distribution:

- `version.dll` SHA-256 `6203c5a0ba1f8c5c77c7c12c33be178a5057063ef1da265eb0a546495c6e6a4e`
- `ShadowsHapticsPack-v5.hfx` SHA-256 `9e994203bbafbf0a318f9056066a54b02adad43c0311f77372ea71dec406565f`
- `ACBFHapticsBridge-v8.ini`: all runtime values retained from the uploaded v2.3.6 distribution, including `MasterGain=0.90`; only version comments changed.

Runtime gameplay on Ubisoft Connect/Ubisoft+ cannot be performed without access to those game installations, so those two profiles are verified statically at the exact code/state/import targets used by the ASI.
