# ACBFHapticsBridge v2.3.7.3 / v8-r9k

This revision deliberately returns to **v2.3.7.0 / v8-r9h as the behavioral baseline**.

## Retained exactly from r9h
- Steam/voices38 haptic output remains the native Black Flag Wwise Quad Audio Haptics injection path.
- Conventional rumble routing remains the r9h implementation; detected DualSense + successful GameInput rumble is not newly mirrored through XInput.
- Gameplay state/effect logic, controller-edge logic, deferred-effect logic, HFX mixing, gains, timing, movement, parkour, water, combat and menus are retained.
- `MasterGain=0.90` and all existing INI values are unchanged.

## Ubisoft-only fixes
- Dynamic PE import-table resolution of `XInputSetState`, with the known r9h IAT RVAs retained as fail-closed fallbacks.
- Runtime profile classification from the verified XInput IAT layout. The direct haptic fallback is eligible only for Ubisoft Connect / Ubisoft+ profiles.
- Physical DualSense identity is cached only for the Ubisoft fallback so a temporary XInput backend handoff does not make the fallback forget the controller.
- Meaningful native Quad haptic signal is detected from the original unmodified Ubisoft buffer. Silent Quad flush calls do not count as useful native haptics.
- If meaningful native Quad haptics are absent while bridge HFX work is pending, Ubisoft profiles may open a 48 kHz / 4-channel DualSense endpoint and send waveform data on actuator lanes 3/4.
- While that fallback is active, the Quad sink is still observed for real Ubisoft native haptics, but bridge HFX are not injected into both paths simultaneously.
- As soon as real Ubisoft native haptics appear, the direct fallback stops and native-first r9h behavior resumes.

## New INI keys
```ini
UbisoftDirectDualSenseFallback=1
UbisoftDirectDualSenseFallbackDelayMs=1000
```
These keys have no effect on Steam/voices38.
