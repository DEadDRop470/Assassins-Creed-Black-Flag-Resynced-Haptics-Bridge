# ACBFHapticsBridge r7 implementation target

Base runtime: `v6-r6-controller-movement-tuning`.

## r7 changes

### 1. Gameplay kick chord
- Default modifier: `R2`
- Default action: `Square`
- Trigger only on the **Square press edge while R2 is already held**.
- Do not fire from holding Square first and then pressing R2.
- Do not repeatedly retrigger while Square stays down.
- Do not fire in inferred menu mode; menu input is consumed first.
- Use the existing HFX `parry` waveform.
- Give the effect its own `[Effect.Kick]` enable/timing/intensity controls.
- Native output arbitration still applies so a native game pulse suppresses the fallback instead of doubling it.
- Suggested poll/debounce guard: `CooldownMs=120`.

### 2. DualSense walking strength
r6 runtime reports `dsWalkHapticX=2.000`; r7 target is `dsWalkHapticX=4.000`.
This is a DualSense/native Wwise haptic-only multiplier. Conventional rumble walking strength stays unchanged.

### 3. Preserve r6 behavior
Keep r6 movement loop ranges, run pulse behavior, menu state recovery, configurable semantic bindings,
gameplay state hooks, native haptic arbitration, conventional rumble conversion, and latency/trim controls.
