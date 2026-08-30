# v8-r9g changes

Runtime label: `v8-r9g-500hz-input-perf-cleanup`

This revision is a performance-focused continuation of v8-r9f. It does not intentionally retune the established haptic/rumble strengths.

Key changes:

- `PollMs=2` target (500 Hz bridge-side input polling).
- GameInput identity metadata refresh: 125 ms.
- GameInput status refresh: 500 ms.
- Raw controller-button count cached per device.
- Semantic menu navigation skips redundant stick/D-pad heuristic scanning.
- Underwater L3 gate checks water-state flags only on L3 rising edge.
- Ordinary QuadAudio sink rebound no longer triggers recovery/cancel/log I/O.
- Combat/menu native gain uses one shared amplification pass after native detection.
- Conventional motor writes limited to 4 ms steady-state cadence; transitions remain immediate.
- F12 suspends controller polling for A/B diagnostics.
- F10 adds a separate `PERF` line with worker/GameInput/rumble/audio timing telemetry.

All r9f/r9d/r9c/r9b/r9 gameplay behavior remains in place unless explicitly listed above.
