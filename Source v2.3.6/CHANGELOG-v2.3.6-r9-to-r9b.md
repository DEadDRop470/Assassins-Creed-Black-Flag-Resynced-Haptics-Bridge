# ACBFHapticsBridge v2.3.6 — v8-r9 -> v8-r9b rebuild

## Added

- Normalized the DualSense walk haptic control so the old r9 raw `4.80` becomes the new user-facing `1.00` baseline.
- Added an explicit compatibility marker so older r9 INIs keep legacy/raw walk semantics.
- Allowed the normalized walk user scale up to `4.80` (effective old/raw `23.04`), still subject to `PeakLimit`.
- Added a 75% (`1.75x`) supplement to Ubisoft-native Quad Audio Haptics for verified Edward sword attacks, parries, pistol fire and kick.
- Added F10 native-combat diagnostics: arm count, boosted blocks/frames, active window, gain and last armed event ID.

## Behavior preserved

- Confirmed v8-r9 XInput land-running fix is unchanged.
- Native feedback continues to win arbitration; bridge fallbacks are used only where already established and native feedback is absent.
- Cannon/player-hit/menu/movement/water native haptics are not intentionally boosted by the combat supplement.
- Spatial footsteps, water reacquisition, parkour scaling, heavy-R1 timing and all r9 movement tuning remain intact.
