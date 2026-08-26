# Event map — v8-r9g

Key retained semantic IDs:

- `0xF4919384` — walk gait contact
- `0xC490DFC6` — fast gait contact
- `0x7ED9038C` — underwater semantic event that is **not sufficient proof of L3 input**; a correlated SprintLunge/L3 rising edge is required before it can own lunge feedback

Combat native-boost family remains the verified light/heavy sword, normal/perfect parry, pistol, and kick event set in `src/main.cpp`.

Gameplay-state hooks remain authoritative for Jump/InAir/landing, climb, swimming, shallow-water, and haystack transitions.

r9g does not remap gameplay events. It retains r9f long-fall tuning, r9d's 300 ms XInput jump/landing/light-attack caps, and r9c's L3-gated water-lunge logic. The performance revision only moves redundant polling/metadata/menu/sink work out of hot paths.
