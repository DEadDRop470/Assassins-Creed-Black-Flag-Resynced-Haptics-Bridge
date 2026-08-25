# ACBFHapticsBridge v8-r4 focused test checklist

Use only the r4 ASI. Keep `SemanticEvents=1`. F10 can be used as a breadcrumb between test blocks.

## 1. Jump / landing — gameplay-state airtime

- standing jump x3
- forward jump ~1 m x3
- longer/running jump x3
- verify every valid jump gets takeoff and landing feedback
- landing strength should track airtime and no stale 6+ second semantic-air timer should appear

## 2. Normal/perfect parry

- press L1/parry in empty air/out of combat: fallback should feel much earlier than r3
- normal combat parry: Ubisoft native feedback should win; no obvious duplicate
- perfect parry: native feedback only

## 3. Light R1 combo

- perform two 4-hit/rapid R1 sequences out of combat
- every accepted sword swing/tap should now produce feedback
- first mapped event remains semantic; later missing stages may report as r4 raw light fallbacks in F10
- check that holding R1 for heavy does not incorrectly create an extra light swing

## 4. Heavy attack

- two or three heavy attacks out of combat
- first phase should be clearly present
- second phase should arrive much earlier than r3
- in combat, native Motion should suppress/replace plugin copies where Ubisoft provides it

## 5. Sheathe / unsheathe

- sheathe x3 and unsheathe x3
- first phase should remain immediate
- strong second phase should be about 30% earlier than the old capture spacing

## 6. Land L3 run

- jog without L3: keep the event-pulsed jog baseline
- while moving fast, press L3
- first ~1.5 s should be noticeably stronger
- after 1.5 s it should settle but remain ~50% stronger than jog
- release the movement stick: continuous run should stop immediately
- XInput/DSX Xbox mode should now use continuous run rumble by default

## 7. Water state

Surface:
- enter normal surface swimming
- press L3 while moving: fast-swim texture should sustain continuously
- stop/release movement: it should stop

Underwater:
- dive so the underwater state/event is observed
- press L3 / perform lunge
- expect immediate lunge impact plus sustained fast-swim texture
- sustained underwater texture must hard-stop after ~3 seconds
- a new lunge can start another 3-second window
- surface again: underwater latch must clear

## 8. Native-only regression

Confirm unchanged Ubisoft feedback for:
- player hit
- pistol
- perfect parry
- cannon/broadside
- native haptic menu navigation

Take helm should still add no plugin vibration.

## 9. Hotkeys / diagnostics

- F9 dodge preview still works
- F10 status is safe to tap repeatedly
- F11 OFF -> no plugin playback; F11 ON -> clean state reacquisition
- F12 only toggles unresolved raw fallback layer

Useful r4 F10 tail:
`r4=run/under/lungeRemain/lightRaw/heavyRaw/parryFB/heavy12/sheath2=...`
