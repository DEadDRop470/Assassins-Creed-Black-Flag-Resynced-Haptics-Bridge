# v8-r9k implementation notes

r9k is not a continuation of r9j's global output arbitration. It is rebuilt from r9h and isolates new output behavior to verified Ubisoft profiles.

The Steam/voices38 route never calls the direct WaveOut renderer. `write_rumble_output`, `inject_sink_block`, `process_controller_edges`, `process_gameplay_state_signals`, and `process_deferred_effects` are source-identical to the r9h baseline.

For Ubisoft Connect/Ubisoft+, the XInput IAT profile enables a fallback service. Native Quad remains first choice. The fallback starts only when HFX work is pending, a DualSense is known, and meaningful original Ubisoft haptic signal has not been detected for the configured grace period. While direct output is active, native buffers are observed without consuming bridge voices twice. Genuine native haptic signal immediately returns priority to the native path.
