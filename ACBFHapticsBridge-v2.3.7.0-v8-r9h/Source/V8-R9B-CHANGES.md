# v8-r9b changes

Internal runtime: `v8-r9b-normalized-walk-native-combat-boost`  
NexusMods release line: `v2.3.6`

## DualSense walk normalization

The r9 walk haptic raw value `4.80` is now the normalized user baseline `1.00`.

- Shipped: `DualSenseHapticNormalizedBaseline=1`
- Shipped: `DualSenseHapticMultiplier=1.00`
- Effective walk multiplier at 1.00: `4.80` (identical to r9)
- User range: `0.00..4.80`
- Maximum effective legacy/raw multiplier: `23.04`
- `PeakLimit` remains the final output limiter.

Backward compatibility is deliberate: an older r9 INI without `DualSenseHapticNormalizedBaseline` is treated as legacy/raw, so its existing `DualSenseHapticMultiplier=4.80` still evaluates to `4.80`, not `23.04`.

Important: this key is the **DualSense walk** multiplier, not a global all-effect DualSense multiplier. Jog and L3 run still use their existing separate controls/baselines.

## Ubisoft-native combat haptic supplement

The bridge now arms a short, action-scoped native haptic window for the verified Edward combat family:

- light sword attack
- heavy sword attack
- normal parry
- perfect parry A/B/C
- pistol fire
- kick

When Ubisoft's native Quad Audio Haptics appear inside that window, the original buffer is detected first, then multiplied by `CombatNativeHapticGain=1.75` (+75%). The existing native-first arbitration then owns the action and suppresses/discards any bridge duplicate.

If no native haptic signal appears, no native amplification occurs and the established bridge fallback path runs normally. Perfect-parry and pistol remain native-owned as they were in r9; r9b does not invent a new fallback for actions that previously had none.

R1 and L1 controller edges also refresh the window while in CHARACTER mode, covering sword combo stages that can have native Motion even when the mapped first-stage Wwise event is absent. Menu and HELM input return before this arm path, preventing R1/L1 UI/naval input from intentionally boosting combat output.

## Retained r9 behavior

- Xbox/XInput/GameInput-class land sprint remains real gait-contact pulse only.
- Water fast-swim/lunge remains continuous.
- DualSense walk/jog/run +20% r9 movement baseline is retained.
- Right-foot-first 100/25 spatial footsteps retained.
- r7 water reacquisition and r6 parkour scaling retained.
