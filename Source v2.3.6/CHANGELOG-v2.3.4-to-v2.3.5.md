# ACBFHapticsBridge v2.3.5 / v8-r8

## Changes from v2.3.4 / v8-r7

### Spatial left/right footsteps

- Black Flag's real walk / fast-gait contact events now drive left/right spatial feedback.
- A fresh walking or running animation starts with the **RIGHT foot**, based on in-game observation.
- Accepted contacts then alternate:
  **Right -> Left -> Right -> Left...**
- Default spatial balance:
  - Right foot: **100% right / 25% left**
  - Left foot: **100% left / 25% right**
- The same spatial weights are applied to:
  - native 48 kHz stereo DualSense haptics;
  - conventional rumble left/right motor outputs.
- The opposite-side 25% signal preserves body-transfer sensation instead of hard-muting one side.

### Sprint / running

- Existing L3 continuous run texture is retained.
- Real fast-gait contact events now add a lighter spatial contact overlay during L3 sprint.
- Default sprint contact overlay strength: **0.70**.
- Walk/run gait-type changes reset the next contact to RIGHT.

### Foot-phase recovery

- Foot phase resets after **750 ms** without a valid gait contact.
- WATER, MENU, HELM, reload/recovery state resets also restart the next fresh gait sequence on RIGHT.
- This limits long-term left/right desynchronization if an event is missed.

### New INI section

`[Footsteps]` exposes:

- `SpatialFootsteps=1`
- `RightFootRightGain=1.00`
- `RightFootLeftGain=0.25`
- `LeftFootLeftGain=1.00`
- `LeftFootRightGain=0.25`
- `ResetAfterMs=750`
- `SprintContactOverlayGain=0.70`

### Diagnostics

F10 now includes:

`r8=footR/footL/resets/next`

This shows right contacts, left contacts, phase resets, and which foot is expected next.

## Retained from v8-r7

- automatic walk/run-into-water reacquisition;
- right-foot-independent WATER state repair;
- parkour airtime + distance-scaled landings;
- Heavy R1 two-impact timing;
- normalized release gain baseline;
- user-tuned Parry, Heavy #2 and Sheathe/Hood offsets;
- native cannon/player-hit/pistol/perfect-parry arbitration.
