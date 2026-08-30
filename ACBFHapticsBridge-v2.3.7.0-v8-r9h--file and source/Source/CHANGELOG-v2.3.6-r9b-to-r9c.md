# ACBFHapticsBridge v2.3.6 — v8-r9b → v8-r9c

## Fixed

- Fixed underwater lunge feedback incorrectly starting from ordinary directional movement after being stationary.
- `0x7ED9038C` is no longer trusted as an L3 press. A real configured SprintLunge/L3 rising edge must authorize the event.
- Added a short event-before-input tolerance so legitimate L3 lunges are not lost because of worker/poll ordering.

## XInput-only tuning

- Added `XInputJumpLandingMaxDurationMs=1500`.
- Added `XInputLightAttackMaxDurationMs=500`.
- These caps affect conventional XInput/GameInput Xbox-class rumble only; DualSense 48 kHz haptics are unchanged.

## Retained

- r9 real gait-contact land-running pulses for XInput.
- r9b normalized DualSense walk baseline (`1.00` = previous `4.80`).
- +75% Ubisoft-native combat haptic supplement for sword/parry/pistol/kick.
- Spatial footsteps, parkour scaling and automatic water reacquisition.
