# Source delta audit: r9h -> r9k

Verified against the exact v2.3.7.0 / v8-r9h `main.cpp` retained in `Source/References/`.

Source-identical baseline functions:
- `write_rumble_output`
- `inject_sink_block`
- `process_controller_edges`
- `process_gameplay_state_signals`
- `process_deferred_effects`

INI semantic comparison: all r9h keys/values are unchanged. Exactly two keys are added under `[Output]`: `UbisoftDirectDualSenseFallback=1` and `UbisoftDirectDualSenseFallbackDelayMs=1000`.

New code is limited to executable-profile/XInput-IAT resolution, an Ubisoft-only persistent DualSense identity cache, meaningful-native-haptic timestamping, Ubisoft-only 48 kHz / 4-channel direct fallback, native-buffer observation during that fallback, diagnostics, and version labels.
