# ACBFHapticsBridge v2.3.6 — v8-r9e -> v8-r9f

## DualSense tuning

- Increased DualSense menu navigation tick intensity by 30% (`DualSenseTickGainMultiplier=1.30`) on both bridge fallback ticks and the verified Ubisoft-native menu Quad tick; XInput menu ticks are unchanged.
- Increased DualSense land walk, jog, and L3-run intensity by 15% relative to r9e. The r9e land-movement factor `0.90` becomes `1.035` (`0.90 * 1.15`).
- Retained the r9e global DualSense bridge cleanup (`0.90`) and extra ordinary jump/landing factor (`0.85`).

## Long-fall impact

- Interprets “increase by 125%” literally as +125% = `2.25x` the r9e long-fall ceiling.
- `LandingLongGain` rises from `1.40` to `3.15` for both DualSense haptics and conventional XInput rumble.
- The 2.0-3.0 second airtime region ramps continuously from the normal `0.70` landing base to `3.15`.
- Every measured 3.0 second or longer ground/climb impact uses exactly `3.15` base gain.
- Parkour distance can make a 2-3 second fall approach the ceiling sooner, but it is capped and cannot exceed the configured long-fall maximum.
- XInput long-fall impact still uses the established 300 ms motor duration and explicit zero reset; only its amplitude is increased.

## Retained fixes

- XInput jump/landing/light-attack motor impacts remain capped to 300 ms with a 16 ms explicit zero-motor barrier.
- XInput L3 land-running remains real gait-contact pulses with the continuous run motor bed suppressed.
- Underwater lunge remains correlated to L3/SprintLunge input; direction-only `0x7ED9038C` is rejected.
- Ubisoft-native sword/parry/pistol/kick Quad haptics remain supplemented to 1.75x when present.
