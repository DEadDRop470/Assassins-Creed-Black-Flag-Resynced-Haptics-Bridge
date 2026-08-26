# Build verification — v2.3.6 / v8-r9g

- Runtime: `v8-r9g-500hz-input-perf-cleanup`
- ASI: `ACBFHapticsBridge-v8-r9g.asi`
- SHA-256: `4cf92a8e7b72766903ca5ac294c7cac0b84521b7a9d910af10520aa5ec25c4d0`
- ASI size: 196,608 bytes.
- Target: ACBlackFlag.exe Steam TU 1.0.6, SHA-256 `8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140`
- Deterministic rebuild: PASS; two clean builds were byte-identical.
- Static PE audit: PASS.
- Timestamp: 0.
- ASLR: enabled.
- NX: enabled.
- High-entropy VA: enabled.
- RWX sections: none.
- COFF symbols: stripped.
- Import directory: null-only.

Behavior/performance sanity checks (static/code-path):

- Worker/input poll default is `PollMs=2` (configured target 500 Hz).
- GameInput device identity metadata is refreshed at 125 ms, status at 500 ms, and raw controller-button count is cached per device.
- Normal GameInput state sampling remains every worker poll; DualSense raw button state remains available for touchpad detection.
- Exact semantic menu mode returns through a fast path before analog-stick/D-pad/face-button navigation heuristics; L1/R1 menu fallback remains active.
- r9c underwater lunge water-state correlation is evaluated only on a real L3 rising edge; direction-only underwater semantic events remain rejected.
- Ordinary QuadAudio sink pointer rebound only changes the selected sink and counter; it no longer increments cancel generation or worker recovery serial. Long silence/resume still performs full recovery.
- Native detection still reads Ubisoft's unmodified buffer before any gain. Combat/menu native amplification now shares one gain pass after detection.
- Conventional rumble steady-state writes are limited by `RumbleWriteIntervalMs=4`; start and stop transitions remain immediate. Input polling is still 2 ms.
- F12 bypasses controller polling/raw-input processing while semantic/state/native routing remains active for diagnostics.
- QPC-based F10 `PERF` diagnostics cover worker gaps, GameInput calls, rumble writes and bridge audio-hook time.
- r9f 3.0 s+ long-fall max gain 3.15 remains unchanged for both DualSense and XInput.
- r9d XInput 300 ms jump/landing/light-attack caps and explicit 16 ms zero barrier remain.
- r9 real gait-contact XInput land running, spatial footsteps, r9b native combat supplement, DualSense tuning and menu gains remain.

Runtime assets expected unchanged:

- `version.dll` SHA-256 `6203c5a0ba1f8c5c77c7c12c33be178a5057063ef1da265eb0a546495c6e6a4e`
- `ShadowsHapticsPack-v5.hfx` SHA-256 `9e994203bbafbf0a318f9056066a54b02adad43c0311f77372ea71dec406565f` (14 effects, 48 kHz stereo)

Source-package rebuild: PASS; the assembled source package reproduced the shipped ASI byte-for-byte and passed the same static PE audit.
