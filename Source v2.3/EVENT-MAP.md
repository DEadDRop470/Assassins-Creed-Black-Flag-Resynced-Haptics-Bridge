# ACBFHapticsBridge v8-r4 event/state map

## Production semantic events

| Event | Meaning / use | r4 output policy |
|---|---|---|
| `0x67233A85` | native menu navigation tick | Ubisoft native only |
| `0xEB10F213` | audible menu navigation with missing native haptic | immediate menu fallback |
| `0xF4919384` | slow walk gait | short per-event walk contact |
| `0xC490DFC6` | fast gait / jog-run | jog contact, or sustains L3 run mode |
| `0x4F8D126F` | dodge / roll | one short dodge effect per real roll |
| `0xD523F1E1` | normal parry action | native-first, 24 ms fallback |
| `0x6B0E9352` | perfect-parry family | native only |
| `0xC5DD775E` | perfect-parry family | native only |
| `0x4DD3C123` | perfect-parry family | native only |
| `0x8ABBEE18` | first/known light attack | native-first fallback |
| `0xC21A023E` | heavy attack | r4 two-phase fallback, native-first |
| `0x839CE745` | player hit / damage family | native only |
| `0x27C0E102` | pistol fire | native only |
| `0x7ED9038C` | water lunge | immediate lunge + 3 s underwater sustain cap |
| `0x731177C1` | surface | clear underwater latch + surface effect |
| `0x024F2AF6` | swim sprint/start family | WATER state anchor |
| `0x23B967BB` | dive candidate | underwater context anchor |
| `0xB50C8069` | kick family | fallback kick effect |
| `0xA15A3B30` | sheathe | split/advanced equipment effect |
| `0xD3B2B6DC` | unsheathe | split/advanced equipment effect |
| `0x8CB7D98C` | hood ON | hood/equipment fallback |
| `0xC88F29CD` | hood OFF | hood/equipment fallback |
| `0xDF7BEEAE` | take helm | state anchor only; no added haptic |
| `0x2FC9D3AD` | one-per-broadside cannon semantic | HELM/native anchor only |
| `0x774FC62C` | per-cannon native Motion family | completely native |

## Jump/landing

Wwise candidates `0x2E795D46`, `0x4E92CCAC`, and `0x43CD17F9` remain useful context but are **not authoritative** in r4. Black Flag's gameplay Jump/InAir state callbacks own takeoff, measured airtime, and landing because they cover forward/targeted jumps more reliably.

## Light-combo diagnostic candidates — NOT production labels yet

The r3 marked combo sessions repeatedly exposed same-object events including:

`0x68B013E7`, `0xDF786ED3`, `0x0AF7BAEE`, `0xA5EF84E3`, `0xC2D0D50F`, `0x5F259EE8`.

They are retained as research evidence only. The current log does not prove which one is swing #2, #3, or #4 across all animation branches, so r4 does not hard-map them. Later combo taps use guarded R1-edge fallback instead.
