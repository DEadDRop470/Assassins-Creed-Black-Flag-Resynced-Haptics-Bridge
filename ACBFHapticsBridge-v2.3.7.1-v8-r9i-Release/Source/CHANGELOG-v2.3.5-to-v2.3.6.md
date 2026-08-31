# ACBFHapticsBridge v2.3.6 / v8-r9

## XInput / GameInput-class running fix

- Fixed land-running rumble becoming a continuous vibration on Xbox/XInput-class controllers.
- Land sprint no longer feeds the continuous run waveform into conventional rumble.
- Walk, jog and L3 running now follow Black Flag's real gait-contact events step by step on XInput/GameInput-class rumble.
- The motors are explicitly allowed to return to zero between contacts, preventing the "stuck vibration" feel.
- Water fast-swim/lunge keeps its continuous movement texture; this change is land-run specific.
- New F10 diagnostics: `r9=xiRunSteps/suppressedTicks/stepOnly`.

## New movement-strength baseline

The following are now the shipped v2.3.6 defaults:

- DualSense walk: +20% (`DualSenseHapticMultiplier 4.00 -> 4.80`).
- DualSense jog/fast-gait step contacts: +20% (`DualSenseJogHapticMultiplier=1.20`).
- L3 run opening response: +20% (`RunOpeningGain 1.90 -> 2.28`).
- Sustained DualSense run texture: +20% (`RunBaseGain 1.50 -> 1.80`).
- DualSense sprint contact overlay: +20% (`SprintContactOverlayGain 0.70 -> 0.84`).
- XInput land-run step contacts use `SprintStepGain=1.20`, making each real run contact 20% stronger than the jog contact baseline.

All v8-r8 spatial-footstep behavior is retained: fresh movement starts on Edward's right foot and alternates Right/Left with 100% primary-side + 25% crossfeed.

All v8-r7 automatic-water reacquisition, v8-r6 parkour/airtime logic, heavy-R1 timing, and the normalized 2.50x master / 1.80x one-shot baseline are retained.
