# r7 binary patch details

The uploaded r6 package contained the compiled ASI but no C++/Visual Studio source. r7 therefore applies a narrow patch to the existing r6 code path rather than replacing controller handling.

- Original Square/X press test remains `XINPUT_GAMEPAD_X (0x4000)`.
- The taken branch at VA `0x1800148D7` is redirected to a code cave at VA `0x180017388`.
- The stub reads the existing configured trigger threshold from VA `0x180019584`, compares it against the current XInput `bRightTrigger` byte at the worker's `rsp+0x137`, and only then enters the existing raw-X scheduler at VA `0x1800164AF`.
- If R2 is below threshold, execution returns to the original fallthrough at VA `0x1800148DD`.
- The raw-X scheduler's gain pointer at VA `0x1800164C5` now points to `LBEffectGain` at VA `0x180019654` instead of the shared 1.0 constant.
- `XEffect=parry` activates that existing scheduler.
- Hardcoded runtime paths were changed to `ACBFHapticsBridge-v7.ini` and `.log`.

Stub length: 28 bytes.

- `.text` PE `VirtualSize` was expanded from `0x16388` to `0x163A4` so the 28-byte stub is mapped/executable; `SizeOfRawData` remains `0x16400`, and the next section starts at RVA `0x18000`.
