# Source delta audit — v8-r9h -> v8-r9i

Baseline source SHA-256: `43e8d279b14f9eb9e0e3b9b66ddea50fde4d7ffcdc0650394a81fa56840a6e7d`

r9i source SHA-256: `982d03fa68e20b12cf160e3a198f24579b8698ff168b552fda7e3256c33c583e`

Raw source diff: 526 inserted lines / 68 removed lines across 24 hunks. The large insertion is the direct DualSense WaveOut backend.

Reviewed change areas:

1. release/runtime label and two new output config fields;
2. INI load/status for those fields;
3. direct DualSense 48 kHz/4-channel renderer and output-ownership state;
4. native Quad flush handoff guard;
5. dynamic loaded-PE `XInputSetState` IAT resolution and verified profile fallback;
6. GameInput/XInput rumble attempts/route/success telemetry and mirrored route;
7. F10 `R9I OUTPUT` diagnostics;
8. worker-loop direct-output service and shutdown.

No r9i diff hunk changes the existing effect mapping table, gameplay-state slot table, gait/parkour/water/combat/menu tuning constants, waveform trims, effect durations, or existing INI values. Parsed INI comparison reports exactly two added keys and zero modified/removed existing keys.

The r9i patcher was fail-closed and applied to the exact uploaded r9h `Source/src/main.cpp`, not to a reconstructed approximation.
