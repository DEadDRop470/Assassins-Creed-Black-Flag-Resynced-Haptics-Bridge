# ACBFHapticsBridge v2.3.7.0 → v2.3.7.1 / v8-r9i

## Output compatibility hotfix

v2.3.7.1 focuses on controller-output compatibility while preserving the gameplay, effect, timing, gain, menu, movement, combat, parkour and water tuning shipped in v2.3.7.0.

### DualSense haptics

- Added a guarded **direct DualSense haptics fallback** for cases where Black Flag's native Quad/Wwise haptic flush is present but remains dormant at runtime.
- The fallback opens a compatible **48 kHz / 4-channel** DualSense audio endpoint and sends the bridge's existing left/right haptic waveform to actuator channels **3/4** while channels 1/2 remain silent.
- Black Flag's native Quad haptics path remains preferred whenever it is active.
- Added a race-safe ownership handoff so native Quad output can resume without double-mixing the transition block.
- Added `DirectDualSenseFallback=1` and `DirectDualSenseFallbackDelayMs=1000` to the release INI.

### XInput / conventional rumble

- Replaced fixed-profile-only `XInputSetState` observation with **dynamic PE import-table resolution** first, followed by verified profile-specific fallback slots only when pointer validation succeeds.
- Added XInput route probing and mirroring when a connected controller route is available.
- Corrected GameInput rumble telemetry: a void GameInput call is now recorded as **issued**, not treated as proof that physical vibration succeeded.
- Added separate GameInput/XInput attempt, success and failure diagnostics.

### Diagnostics and compatibility

- Added `R9I OUTPUT` diagnostics to expose native/direct DualSense ownership and GameInput/XInput routing status.
- Steam TU 1.0.6 r9i runtime test: **PASS**. The bridge loaded cleanly, all 21 gameplay-state hooks installed, the XInput observer resolved by named import, and the direct DualSense 48 kHz/4-channel path activated successfully during F9 testing.
- Ubisoft Connect and Ubisoft+ executable targets remain statically validated for the Quad haptics flush, Wwise PostEvent route and all 21 gameplay-state callbacks. r9i removes the old dependency on one fixed XInput IAT location.

### Unchanged from v2.3.7.0

- No intentional gameplay-event remapping.
- No movement/combat/menu/water/parkour retuning.
- No HFX pack change.
- No loader change.
- All existing INI values are unchanged.
- `MasterGain=0.90` remains the shipped default.

## Release identification

- Nexus/project version: **v2.3.7.1**
- Internal runtime: **v8-r9i**
- ASI: `ACBFHapticsBridge-v8-r9i.asi`
- Runtime label: `v8-r9i-v2.3.7.1-ubisoft-output-hotfix`
