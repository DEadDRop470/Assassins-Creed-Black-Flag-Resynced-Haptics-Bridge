# v8-r9 changes

Internal runtime: `v8-r9-xinput-step-run-tuned-movement`  
NexusMods release: `v2.3.6`

## Conventional rumble land-run routing

v8-r8 kept the native continuous L3 run texture active for conventional rumble while also adding real spatial gait-contact pulses. On Xbox/XInput-class devices the continuous motor bed dominated those pulses and felt like vibration was stuck on.

v8-r9 separates the two paths:

- Native DualSense haptics: continuous L3 run texture remains, with spatial step overlays.
- Xbox/XInput/GameInput-class conventional rumble: land sprint uses real gait-contact pulses only.
- Water fast-swim/lunge remains continuous.

The XInput-class check covers direct XInput and GameInput devices exposing the Microsoft/Xbox vendor path. `LandRunStepOnly=1` is the new default.

## Movement tuning

- Walk native DualSense gain: 4.80 (+20%).
- Jog native DualSense pulse gain: 1.20 (+20%).
- L3 opening run gain: 2.28 (+20%).
- Sustained run gain: 1.80 (+20%).
- Sprint spatial contact overlay: 0.84 (+20%).
- XInput run contact multiplier: 1.20.

## Diagnostics

F10 adds `r9=xiRunSteps/suppressedTicks/stepOnly` so a test log can prove that real run contacts were emitted and the old continuous conventional-rumble path was being suppressed.
