# v8-r9e changes

Internal runtime: `v8-r9e-dualsense-cleanup-long-fall-menu-tuning`  
NexusMods release line: `v2.3.6`

## DualSense bridge cleanup

r9e reduces ACBFHapticsBridge-generated feedback on DualSense without reducing Ubisoft-native Quad haptics:

- all bridge output on DualSense: `0.90x`;
- jump and ground landing: an additional `0.85x` (`0.765x` combined);
- land walk/jog/run: an additional `0.90x` (`0.81x` combined);
- water movement gets only the global `0.90x` cleanup;
- menu L1/R1: an additional `0.75x` on DualSense.

Because `RumbleOnDualSense=1` remains enabled, the same bridge/category cleanup is also applied to the bridge's parallel conventional-rumble copy on DualSense. Ubisoft-native feedback is not routed through these reductions.

## Menu shoulder tuning by controller class

The existing base menu shoulder gain remains `0.315` before controller-specific attenuation:

- XInput/Xbox-class L1/R1 menu feedback: `0.50x`;
- DualSense L1/R1 menu feedback: `0.75x`.

This does not change gameplay parry or attack strength.

## Stronger 2-3 second landing curve

The prior r9d log measured a 2656 ms landing as category 3 with gain only `0.748`, confirming long-fall detection was working but the curve was too mild.

r9e changes the base airtime curve to:

- 0.9 s -> `0.55`;
- 2.0 s -> calibrated normal `0.70`;
- 2.5 s -> `1.05`;
- 3.0 s -> `1.40`, exactly 2x the normal `0.70` base;
- >3.0 s -> base remains `1.40`.

The existing parkour-distance term can still add up to +35% on top of the airtime result. The same landing gain feeds both haptic and conventional-rumble bridge paths; XInput's 300 ms impact-duration cap and zero-reset behavior remain intact.

## Haptics vs rumble clarification

The bridge does **not** send a haptic signal to rumble only after it becomes too strong. The 48 kHz DualSense haptic stream and conventional rumble are parallel output paths. `PeakLimit` caps the haptic stream. `RumbleOnDualSense=1` controls whether the parallel conventional-rumble copy is also written to DualSense.

r9e deliberately does not add any artificial "overflow-to-rumble" threshold because that would duplicate strong impacts and can make feedback noisier.

## Retained behavior

- r9d XInput jump, landing, and light-attack motor impacts stay capped at 300 ms.
- r9d explicit 16 ms XInput motor-zero separation remains.
- r9c underwater lunge remains correlated to a real L3/SprintLunge press.
- r9 XInput land sprint remains real gait-contact step pulses only.
- DualSense walk user `1.00` still maps to the established raw `4.80` walk baseline before r9e device cleanup.
- Ubisoft-native sword/parry/pistol/kick Quad haptics remain supplemented by `CombatNativeHapticGain=1.75` when present.
- Spatial footsteps, automatic water reacquisition, and parkour-distance scaling remain.
