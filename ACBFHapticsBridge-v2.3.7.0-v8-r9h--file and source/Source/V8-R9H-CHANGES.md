# v8-r9h changes

v8-r9h is the internal runtime for Nexus release v2.3.7.0.

The sole runtime behavior change from v8-r9g is multi-profile verification of the game's `XInputSetState` IAT slot. The observer now accepts the verified Steam/voices38, Ubisoft Connect, or Ubisoft+ slot only when that slot exactly equals the resolved XInput API address before patching.

All v8-r9g gameplay/haptic behavior is retained.
