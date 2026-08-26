# v8-r9d changes

Internal runtime: `v8-r9d-xinput-300ms-zero-reset-l3-gated-water-lunge`  
NexusMods release line: `v2.3.6`

## XInput-only 300 ms action impacts

The r9c mixed-controller validation showed that the XInput jump/landing cap of 1500 ms could not shorten the mapped `dodge` source because that source is only about 788 ms long. It also confirmed rapid real parkour transitions can place a new jump very shortly after a landing, which can make conventional motors feel continuous even though the effects are separate.

r9d therefore changes only the conventional XInput/GameInput Xbox-class motor path:

- jump rumble: maximum 300 ms;
- landing rumble: maximum 300 ms;
- light-attack rumble: maximum 300 ms per swing/press.

These limits apply to the canonical jump/landing sources and the raw, hybrid, and semantic light-attack paths. They do **not** change DualSense 48 kHz haptic playback, water/haystack effects, or animation/game timing.

## Explicit XInput motor-zero separation

When a capped jump, landing, or light-attack motor voice finishes, r9d schedules a short explicit zero-motor barrier (`XInputActionZeroGapMs=16` by default). During that tiny gap, other rumble timelines continue advancing, but the conventional motor output is forced to zero. This provides a real separation between adjacent impacts without adding a gameplay cooldown or suppressing valid rapid parkour transitions.

F10 adds `r9d=zeroBarrier/zeroTicks/gapMs`.

## Retained r9c/r9b/r9 behavior

- Underwater `0x7ED9038C` cannot trigger lunge feedback without a correlated SprintLunge/L3 press.
- XInput land sprint remains real gait-contact step pulses only.
- DualSense walk user `1.00` = effective legacy/raw `4.80`.
- Ubisoft-native sword/parry/pistol/kick Quad haptics are supplemented by `CombatNativeHapticGain=1.75` while native-first arbitration remains intact.
- Right-foot-first 100/25 spatial gait contacts remain.
- Automatic water reacquisition and parkour scaling remain unchanged.
