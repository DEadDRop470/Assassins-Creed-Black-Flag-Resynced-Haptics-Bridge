# Steam / Ubisoft Connect / Ubisoft+ cross-reference for v2.3.7.1

| Profile | SHA-256 | Native XInputSetState IAT | v2.3.7.1 status |
|---|---|---:|---|
| Steam TU 1.0.6 | `8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140` | `0x1D0119E0` | **r9i runtime PASS**; named-import resolution + direct DualSense fallback observed live |
| Ubisoft Connect | `e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c` | `0x1C5769E0` | Quad flush + PostEvent + 21/21 state slots statically validated; live r9i test pending |
| Ubisoft+ | `b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75` | `0x1EC409E0` | Quad flush + PostEvent + 21/21 state slots statically validated; live r9i test pending |

## Steam r9i runtime confirmation — 2026-08-31

The returned Steam log confirms:

- `Gameplay state vtable hooks installed=21 failed=0`.
- `Installed r9i XInput observer IAT RVA 0x1D0119E0 method=named-import`.
- F9 preview reached the new r9i output fallback.
- A DualSense endpoint opened successfully as 48 kHz / 4-channel / 16-bit with actuator channels 3/4.
- Gameplay-state detection continued afterward (jump, water entry/dive, swim-up and surface/exit events).

## Ubisoft executable evidence retained from v2.3.7.0 analysis

The Ubisoft+ protected image has PE `SizeOfImage=0x1FB00000`. Its Quad-haptic flush still has the same verified prologue at `RVA 0x04F78610`, and Wwise PostEvent remains at `RVA 0x0091DEA0`.

Static presence of a function is not proof that a running edition actually routes audio through it. The previously reported Ubisoft-family zero-Quad-flush behavior is why v2.3.7.1 adds an alternate output backend rather than changing the already matching hook RVA.

Because GameInput v0's rumble call is void-returning, v2.3.7.1 also stops using "call issued" as its only success signal and measures/mirrors XInput when a route exists.

## Compatibility strategy in r9i

Executable-specific gameplay/state targets are not broadened or blindly scanned. The v2.3.7.0 hook/state validation remains intact. Only the native XInput observer becomes PE-import driven, while the output layer can fall back to a controller audio endpoint if the native Quad callback is dormant at runtime.
