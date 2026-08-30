# ACBFHapticsBridge v2.3.6 -> v2.3.7.0

Compatibility-only release. No haptic effect, gain model, semantic event, gameplay-state, timing, controller-polling, menu, native-haptic or conventional-rumble tuning was changed.

## Changed

- Added verified native-XInput observer support for the Ubisoft Connect executable at IAT RVA `0x1C5769E0`.
- Added verified native-XInput observer support for the Ubisoft+ executable at IAT RVA `0x1EC409E0`.
- Retained the existing Steam TU 1.0.6 / voices38 observer slot at `0x1D0119E0`.
- Every candidate slot must contain the resolved `XInputSetState` export before it can be modified; otherwise the observer fails closed and leaves game imports untouched.
- Updated release/runtime identification to v2.3.7.0 / v8-r9h.
- Updated licensing/source notices for the new exact-source release.
- Release INI retains the v2.3.6 distribution tuning with `MasterGain=0.90`.

## Verified unchanged in supplied Ubisoft executables

Both Ubisoft Connect (`e553a964...601c`) and Ubisoft+ (`b7a2c382...61c75`) retain:

- Quad haptics flush signature at RVA `0x04F78610`.
- Wwise PostEvent signature at RVA `0x0091DEA0`.
- All 21/21 exact gameplay-state callback slots used by the bridge.

No executable strings or broad signature scans were added because the verified code/state targets did not move; only the XInput import layout changed.
