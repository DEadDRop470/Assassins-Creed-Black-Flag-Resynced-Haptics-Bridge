# v8-r9c changes

Internal runtime: `v8-r9c-l3-gated-water-lunge-native-combat-boost`  
NexusMods release line: `v2.3.6`

## Underwater lunge input gate

The r9b log proved that Wwise event `0x7ED9038C` can occur while underwater when a direction is chosen after being stationary, without an L3 press. r9c therefore no longer treats that semantic event as an input edge.

- A real configured `SprintLunge`/L3 rising edge arms one short lunge token.
- `0x7ED9038C` may start/confirm the lunge only while that token is fresh.
- The token is consumed after one accepted lunge.
- Direction-only `0x7ED9038C` copies keep WATER-domain evidence but do not start the lunge effect or the 3-second lunge timer.
- A 96 ms inverse-order tolerance covers Wwise-event-before-controller-poll ordering.
- Default authorization window: `WaterLungeInputGateMs=600`.

F10 adds `r9c=waterL3Arms/semAccept/semReject/pending/gateMs`.

## XInput-only action duration caps

New `[Rumble]` settings:

- `XInputJumpLandingMaxDurationMs=1500`
- `XInputLightAttackMaxDurationMs=500`

These are **maximum conventional-rumble voice lengths** for XInput/GameInput Xbox-class output only. They do not alter the native DualSense 48 kHz haptic timeline. If the selected source excerpt is already shorter than the configured maximum, it ends naturally at the source boundary.

The jump/landing cap applies to canonical land-jump, ground/fallback landing and climb-catch landing sources. Haystack and water effects retain their established timing. Light-attack capping applies to the raw, hybrid and semantic light-attack paths.

F10 adds `xiCaps=jumpLand/light/ms` so XInput runs can verify that the cap path is being used.

## Retained r9b/r9 behavior

- DualSense walk user `1.00` = effective legacy/raw `4.80`.
- Ubisoft-native sword/parry/pistol/kick Quad haptics are supplemented by `CombatNativeHapticGain=1.75` while native-first arbitration remains intact.
- XInput land sprint remains real gait-contact step pulses only.
- Right-foot-first 100/25 spatial gait contacts remain.
- Water reacquisition and parkour scaling remain unchanged.
