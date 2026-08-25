# ACBFHapticsBridge v8-r4 — hybrid semantic/state haptics

v8-r4 combines exact Black Flag Wwise gameplay events with the game's own gameplay-state callbacks. Native Ubisoft feedback stays authoritative where it already exists; Shadows-derived fallback waveforms are added only for missing feedback or for explicitly unresolved combo/input gaps.

## Main architecture

- Exact Wwise `PostEvent` hook: semantic menu, gait, dodge, attacks, equipment, water, helm/naval anchors.
- Black Flag gameplay-state hooks: authoritative jump/InAir/landing tracking and water/climb context.
- Native Quad observer: lets Ubisoft combat/cannon/pistol/damage feedback win instead of doubling it.
- Dual output: 48 kHz stereo DualSense haptics plus conventional low/high-motor rumble derived from the same fallback timeline.

## r4 highlights

- normal parry fallback probe reduced from 60 ms to **24 ms**
- gameplay-state airtime retained/restored as the authoritative jump/landing system
- hybrid per-R1 fallback fills unmapped light-combo #2/#3/#4 gaps
- heavy attack split into a stronger early phase + earlier second phase
- sheathe/unsheathe strong second phase moved about **30% earlier**
- L3 land run: **1.5 s high opening**, then continuous **1.50x** sustained run, ~50% stronger than jog
- XInput-class running defaults to continuous rumble instead of the old 300 ms pulse approximation
- surface fast swim can sustain; underwater lunge/sprint is capped at **3 seconds**
- HELM stays non-sticky: verified Edward InAir/Jump/Swimming states or real character events can release wheel mode
- internal command ring enlarged 64 -> 256 after r3 logs showed haptic command drops

## Install

Put these files beside `ACBlackFlag.exe` / in the same mod loader location used by the previous bridge build:

- `ACBFHapticsBridge-v8-r4.asi`
- `ACBFHapticsBridge-v8.ini`
- `ShadowsHapticsPack-v5.hfx`

Do not load an older ACBFHapticsBridge ASI at the same time.

## Hotkeys

- **F9**: dodge preview
- **F10**: status dump to `ACBFHapticsBridge-v8.log`
- **F11**: disable/enable the complete bridge; re-enable clears and reacquires runtime state
- **F12**: toggle unresolved raw-input fallbacks; confirmed semantic/state routing remains active

## Important current limitation

The first light swing is mapped to the exact Wwise event `0x8ABBEE18`. The r3 log exposed extra combo-associated event families, but their #2/#3/#4 stage identities are not proven. r4 therefore fills missing later swings with a guarded **R1 edge fallback**. This is intentionally labeled hybrid/input-assisted rather than falsely calling those stages fully semantic.

See `V8-R4-CHANGES.md`, `EVENT-MAP.md`, and `TEST-CHECKLIST.md` for details.
