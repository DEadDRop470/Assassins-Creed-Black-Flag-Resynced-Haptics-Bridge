# ACBFHapticsBridge v2.3.6 — v8-r9f → v8-r9g

## Performance/input cleanup

- Changed the bridge worker/input polling target from 8 ms (~125 Hz) to 2 ms (~500 Hz).
- Kept controller report rate separate from bridge polling: a 250 Hz USB device still supplies fresh reports about every 4 ms; the 500 Hz worker only samples/consumes them promptly.
- GameInput device identity/VID/PID/observer work is now cached and refreshed every 125 ms instead of every poll.
- GameInput device status is refreshed every 500 ms instead of every poll.
- Raw GameInput controller-button count is cached per device; only button state is read continuously where touchpad detection is needed.
- Exact semantic menu mode now bypasses analog-stick/D-pad/face-button navigation heuristics. Ubisoft Wwise menu events remain authoritative for ordinary menu navigation; L1/R1 custom menu feedback remains input-driven.
- The r9c underwater lunge state check now runs only on an actual L3 rising edge instead of every controller sample. L3 gating behavior is unchanged.
- Quad Audio Haptics sink pointer rebounds are now cheap ownership handoffs. Ordinary menu/category sink swaps no longer cancel playback, reset input-edge history, or cause a synchronous recovery log write. Long Wwise silence/resume still performs full recovery.
- Native combat/menu gain processing now shares one amplification pass after detection instead of separate combat and menu passes.
- Conventional rumble writes are limited to a 4 ms / 250 Hz steady-state cadence while start/stop/zero transitions remain immediate. This does not reduce the 500 Hz input polling target.
- F12 now actually suspends controller polling/raw-input processing for performance A/B tests while semantic/state/native routing can continue.
- Added QPC-based F10 `PERF` diagnostics for worker stalls, GameInput call time, rumble write time, audio-hook time, metadata-cache refreshes and sink rebinds.

## Retained behavior

- v8-r9f long-fall ceiling: 3.0 s+ landing gain 3.15 for DualSense and XInput.
- DualSense global cleanup, jump/landing cleanup, locomotion tuning, +30% menu tick and menu L1/R1 reductions.
- r9d XInput 300 ms jump/landing/light-attack impacts and explicit zero-motor barrier.
- r9c L3-only underwater lunge authorization.
- r9 real gait-contact XInput land running and spatial footsteps.
- r9b +75% Ubisoft-native sword/parry/pistol/kick supplement.

Gameplay timing/feel still requires in-game validation; the build was statically audited and rebuilt deterministically from the packaged source.
