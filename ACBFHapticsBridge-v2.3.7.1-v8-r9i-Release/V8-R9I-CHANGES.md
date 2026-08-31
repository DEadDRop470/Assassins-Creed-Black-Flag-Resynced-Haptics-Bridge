# ACBFHapticsBridge v2.3.7.1 / v8-r9i changes

v2.3.7.1 is an output-path compatibility hotfix over v2.3.7.0. Gameplay and haptic-effect tuning are intentionally retained.

- Added guarded direct DualSense 48 kHz / four-channel haptic rendering when Black Flag's native Quad/Wwise flush path is dormant at runtime.
- Native Quad output remains preferred and can retake ownership safely.
- Added a race-safe native/direct handoff that avoids double-mixing during takeover.
- Corrected conventional-rumble success semantics: GameInput v0 void calls are tracked as issued calls, not assumed physical success.
- Added measured XInput route probing and DualSense XInput mirroring when a route is connected.
- Added GameInput/XInput attempts, successes/failures and route diagnostics.
- Replaced fixed-profile-only XInput observer selection with dynamic PE import resolution plus verified Steam/Ubisoft profile fallback RVAs.
- Added compact `R9I OUTPUT` F10 diagnostics.
- Added `DirectDualSenseFallback` and `DirectDualSenseFallbackDelayMs` output settings.
- Kept `MasterGain=0.90` and all existing effect/gameplay tuning unchanged.
- Steam TU 1.0.6 live r9i test on 2026-08-31: PASS; 21/21 state hooks installed, named-import XInput resolution succeeded, and direct 48 kHz / 4-channel DualSense output activated successfully during F9 testing.
