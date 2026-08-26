# v8-r9g gameplay test checklist

## DualSense / GameInput

- Confirm F10 shows `pollMs/hz=2/500` in the `PERF` line.
- Walk, jog and L3 run still follow the established spatial/event cadence.
- Jump and landing remain unchanged from r9f except for the existing long-fall scaling.
- A 3.0 s+ fall still reaches the r9f max-impact behavior.
- Menu stick/D-pad/box navigation still produces the expected native/bridge tick without delayed selection.
- Menu L1/R1 retains the existing DualSense reduced strength.
- Touchpad menu opening still enters menu state correctly.
- Underwater direction changes do not trigger lunge; L3 does.
- Swim-up, water entry/surface and normal water movement remain functional.

## XInput/Xbox-class

- Walk/jog/L3 run remain real gait-contact pulses; no continuous stuck land-run rumble.
- Jump, landing and light attack remain 300 ms max with explicit zero separation.
- Menu L1/R1 remains at the existing 50% attenuation.
- 3.0 s+ falls still use the max 3.15 landing gain while the XInput action duration remains 300 ms.

## Performance A/B

- Reproduce the menus/categories where r9f could hitch.
- Press F10 after several minutes and inspect `PERF`:
  - `workerGapUs=max/gt16/gt25/gt50`
  - `giUs=max/gt4/gt8/gt16`
  - `rumbleUs=max/gt4/gt8/gt16`
  - `audioUs=max/gt1/gt2/gt5`
  - `giCache=id/status/rawCount`
  - `rumbleCadenceSkip`
  - `sinkRebind`
- Use F12 to suspend controller polling completely. If a hitch still occurs while F12 is off, GameInput polling is not the direct cause.
- Re-enable F12 and verify there are no false button edges or stuck movement states.

Assistant-side verification is static/build-level only; final gameplay validation must be performed in Black Flag.
