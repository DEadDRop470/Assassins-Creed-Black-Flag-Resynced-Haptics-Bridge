# ACBFHapticsBridge v2.3.6 — v8-r9c → v8-r9d

## XInput-only impact tuning

- Jump conventional rumble is capped to 300 ms.
- Landing conventional rumble is capped to 300 ms.
- Light-attack conventional rumble is capped to 300 ms per swing/press.
- Added an explicit short motor-zero separation after those capped impacts (`XInputActionZeroGapMs=16` by default) so adjacent actions do not merge into a continuous vibration.
- The zero gap advances timelines normally and is not a gameplay cooldown.
- DualSense 48 kHz haptics are unchanged by these XInput-only rules.

## Retained

- r9c L3-gated underwater lunge fix.
- r9 real gait-contact XInput land-running pulses.
- r9b normalized DualSense walk baseline (`1.00` = previous raw `4.80`).
- +75% Ubisoft-native sword/parry/pistol/kick haptic supplement.
- Spatial footsteps, parkour scaling, and automatic water reacquisition.
