# ACBFHapticsBridge v2.3.7.3 / v8-r9k

Runtime: `v8-r9k-v2.3.7.3-ubisoft-isolated-fallback`

This build is intentionally based on the proven v2.3.7.0 / v8-r9h behavior.

- **Steam / voices38:** exact r9h-style native Quad haptic and conventional-rumble behavior. The new direct renderer is not eligible on these profiles.
- **Ubisoft Connect / Ubisoft+:** r9h behavior remains first choice. If meaningful native Quad haptics are absent, a guarded 48 kHz / 4-channel DualSense fallback may render HFX to actuator channels 3/4.
- Dynamic `XInputSetState` import resolution is used with verified-profile fallback.
- Existing gameplay/effect tuning is unchanged; `MasterGain=0.90` remains the shipped value.

The release includes the exact source used to build the ASI under `Source/` plus a copy of the r9h baseline source and a verifier that confirms the important r9h behavior functions remain byte-for-byte identical at source level.
