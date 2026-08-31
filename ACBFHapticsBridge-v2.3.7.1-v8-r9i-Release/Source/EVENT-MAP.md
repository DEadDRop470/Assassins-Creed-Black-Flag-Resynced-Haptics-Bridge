# Event map — v8-r9i / v2.3.7.1

The gameplay/event map is retained unchanged from v8-r9h/v2.3.7.0. r9i changes output/backend compatibility only.

Key retained semantic IDs:

- `0xF4919384` — walk gait contact
- `0xC490DFC6` — fast gait contact
- `0x7ED9038C` — underwater semantic event that is **not sufficient proof of L3 input**; a correlated SprintLunge/L3 rising edge is required before it can own lunge feedback

Combat native-boost family remains the verified light/heavy sword, normal/perfect parry, pistol, and kick event set in `Source/src/main.cpp`.

Gameplay-state hooks remain authoritative for Jump/InAir/landing, climb, swimming, shallow-water, and haystack transitions.

r9i does not remap gameplay events. It retains r9h/r9g routing, r9f long-fall tuning, r9d's 300 ms XInput jump/landing/light-attack caps, and r9c's L3-gated water-lunge logic. v2.3.7.1 changes output ownership, DualSense fallback, XInput resolution/routing and diagnostics only.
