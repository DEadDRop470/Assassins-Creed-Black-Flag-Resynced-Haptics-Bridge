# v8-r8 changes

Internal runtime: `v8-r8-spatial-footsteps`  
NexusMods release: `v2.3.5`

The r8 change is focused on gait spatialization.

Black Flag's real gait-contact event stream is now phase-tracked as alternating feet. A new movement cycle starts on RIGHT, then alternates LEFT/RIGHT for each accepted contact. Default haptic/motor balance is 1.00 on the contacting side and 0.25 on the opposite side.

For normal walk/jog, the existing event-driven contact pulse is spatialized directly. For L3 sprint, the continuous run texture remains active and the real contact event adds a 0.70-strength spatial overlay.

Phase is reset after 750 ms without a valid contact, on gait-type changes, and on major domain/recovery transitions.

No gameplay memory-write or animation-speed modification is introduced.
