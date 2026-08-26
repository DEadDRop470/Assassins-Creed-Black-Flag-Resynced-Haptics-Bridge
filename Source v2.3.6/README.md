# ACBFHapticsBridge v2.3.6 / v8-r9g

Internal runtime: `v8-r9g-500hz-input-perf-cleanup`

v8-r9g keeps the established v2.3.6 haptic/rumble tuning and focuses on controller/menu performance. The bridge now targets a 2 ms / 500 Hz input loop while moving expensive GameInput metadata work out of the hot path, avoiding redundant semantic-menu stick heuristics, making QuadAudio sink swaps cheap, and reducing unnecessary conventional-rumble API writes.

Important distinction: `PollMs=2` is the bridge's sampling cadence, not a controller overclock. A controller/transport that reports at 250 Hz still produces new packets around every 4 ms; the bridge simply checks often enough to consume the next report promptly.

F10 writes normal status plus a `PERF` line. Useful fields include worker gap maxima/counts, GameInput call duration, rumble-write duration, bridge audio-hook duration, GameInput cache refresh counts, rumble cadence skips and sink rebinds.

F11 toggles the complete bridge. F12 now suspends controller polling/raw-input processing so menu/input stutter can be compared with polling fully removed while semantic/state/native routing remains available.

The release includes the exact source used to compile the shipped ASI under `Source/`.
