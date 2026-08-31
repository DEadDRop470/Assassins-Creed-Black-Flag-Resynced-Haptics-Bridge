# ACBFHapticsBridge v2.3.7.1 / v8-r9i test checklist

## Build/audit — release verified

- [x] clean r9i source build completed with zero errors.
- [x] r9i PE/source/INI audit passed.
- [x] ASI contains `v8-r9i-v2.3.7.1-ubisoft-output-hotfix` and not the old r9h runtime label.
- [x] PE remains x86-64 DLL, ASLR/NX/high-entropy, no RWX section.
- [x] release INI keeps `MasterGain=0.90`.
- [x] packaged `Source/src/main.cpp` rebuild reproduces the release ASI byte-for-byte.

## Steam TU 1.0.6 — runtime PASS on 2026-08-31

Observed in the returned r9i log and user test:

- [x] ASI starts as v2.3.7.1 / v8-r9i.
- [x] 14-effect HFX pack loads successfully.
- [x] GameInput v0 initializes successfully.
- [x] XInput bridge uses `xinput9_1_0.dll`.
- [x] native Quad injector installs at RVA `0x04F78610`.
- [x] Wwise PostEvent router installs at RVA `0x0091DEA0`.
- [x] gameplay-state hooks install `21/21`, failures `0`.
- [x] XInput observer resolves by named import at Steam IAT RVA `0x1D0119E0`.
- [x] F9 triggers the r9i preview path.
- [x] direct DualSense output successfully opens a 48 kHz / 4-channel endpoint and routes actuators on channels 3/4 when the native sink is dormant.
- [x] jump/water semantic/state effects continue after output fallback activation.
- [x] user reported Steam haptics/vibration behavior seemed good.

Not directly evidenced by this returned log and therefore not marked as verified here: Xbox-only physical-rumble testing, native-Quad ownership takeover after direct fallback, and multi-controller reconnect behavior.

## Ubisoft Connect — pending live runtime test

- [ ] ASI loads and startup log reaches r9i routing line.
- [ ] F10 reports controller backend/vendor/product/DualSense correctly.
- [ ] Native Quad path: if `flushCalls` rises, owner stays `native-quad`.
- [ ] Dormant Quad path: after haptic work, direct endpoint becomes active.
- [ ] F9 produces DualSense haptics.
- [ ] walk/run/jump/landing/combat/menu effects produce haptics.
- [ ] XInput observer reports named-import/pointer-match/profile-fallback and a verified slot.
- [ ] conventional rumble is felt through the available controller route.

## Ubisoft+ — pending live runtime test

- [ ] startup identifies game image and controller without crash.
- [ ] previously observed zero-flush scenario activates `direct-dualsense` on haptic work.
- [ ] `directDS ... nonZero/frames` counters rise after F9/game effects.
- [ ] if a native Quad flush later wakes, owner changes to `native-quad` and direct endpoint closes cleanly.
- [ ] no doubled haptic burst during native/direct handoff.
- [ ] `rumble giTry/giIssued` can rise without being mislabeled as verified physical success.
- [ ] if an XInput route exists, `xiTry/xiOK` rises; failures are visible rather than hidden.

## Multi-controller safety — optional validation

- [ ] Test with only DualSense connected first.
- [ ] If a second XInput pad is also connected, verify the configured ControllerIndex represents the intended route before judging mirrored rumble.
- [ ] Disconnect/reconnect DualSense; direct endpoint closes/reopens without stuck output.
- [ ] F11 disable immediately stops direct haptics and plugin rumble.
- [ ] Exit game with no stuck vibration/audio endpoint.
