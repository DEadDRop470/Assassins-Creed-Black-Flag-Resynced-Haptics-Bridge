ACBFHapticsBridge 
DualSense haptics + conventional rumble for Assassin's Creed IV: Black Flag Resynced 
VERSION 2.3.7.0 | v8-r9h | NON-COMMERCIAL LICENSE Runtime: v8-r9h-v2.3.7.0-multiversion



IMPORTANT
License: ACBFHapticsBridge uses the ACBFHapticsBridge Non-Commercial License v1.0.
Free personal/non-commercial use, modification and free redistribution are permitted under the license terms. Selling, paywalling, or commercially redistributing the mod is not permitted without prior written permission.

🔗 Downloads & Source
NEXUS MODS — Main mod page / download:
https://www.nexusmods.com/assassinscreedblackflagresynced/mods/412?tab=description
GOFILE — Free mirror:
https://gofile.io/d/whwCCJYe
GITHUB — Source, release verification & project files:
https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge

🎮 What this mod does
ACBFHapticsBridge adds Assassin's Creed Shadows-derived haptic feedback to Assassin's Creed IV: Black Flag Resynced.
The current v2.3.7 runtime combines Black Flag's real Wwise gameplay events, gameplay-state callbacks, GameInput/XInput controller state, native-feedback arbitration and the game's existing Wwise Motion / Quad Audio Haptics path.
The mod supports:
Native DualSense haptics — wired: ✅
DualSense through DSX + DLC / Virtual DualSense with Audio: ✅
Xbox 360 / XInput rumble: ✅
DS4 / compatible emulated rumble paths: ✅
Xbox 360 emulation through DSX: ✅
Compatible GameInput controllers: ✅
Steam Input with native wired DualSense: ✅
DualSense: full 48 kHz stereo waveform injection through Black Flag's native Quad Audio Haptics path.
Conventional controllers: the same effect timelines are converted into low/high-motor rumble for Xbox/XInput, DS4/emulated and compatible GameInput controller paths.
The bridge keeps Black Flag's own native controller feedback whenever possible, supplements selected native effects where useful, and fills in actions where the original game provides little or no controller response.
NOTE
The verified v2.3.7.0 bridge primarily uses GameInput and XInput-compatible controller paths. Generic DirectInput support should not be assumed for every device.
DUALSENSE HAPTICS VS RUMBLE
The 48 kHz DualSense haptic stream and conventional rumble are parallel outputs. A strong haptic effect does not automatically "overflow" into rumble. The haptic path is limited by PeakLimit, while conventional rumble is generated separately when enabled.

✅ Confirmed game versions
Steam — Title Update 1.0.6: ✅ Confirmed
Confirmed ACBlackFlag.exe SHA-256:
8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140
Ubisoft Connect: ✅ Supported in v2.3.7.0
Verified ACBlackFlag.exe SHA-256:
e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c
Ubisoft+: ✅ Supported in v2.3.7.0
Verified ACBlackFlag_Plus.exe SHA-256:
b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75
Epic Games: 🧪 (Not yet verified — try it out and send feedback.)
The supplied Ubisoft Connect and Ubisoft+ executables were statically verified against the bridge's executable-dependent hooks. Both retain the required Wwise PostEvent hook, Quad Audio Haptics flush hook and all 21 gameplay-state callback slots used by the bridge. v2.3.7.0 adds the correct native-XInput observer profile for each executable.
Runtime feedback for the Ubisoft builds is still welcome. If you use an unverified build such as Epic Games, please provide:
your ACBlackFlag.exe SHA-256 hash
the generated ACBFHapticsBridge-v8.log
press F10 while in menus
press F10 once again during gameplay
then close the game and provide the log
Thanks!

📦 MOD REQUIREMENTS / INSTALLATION
A compatible ASI loader is required.
You can use:
version.dll from Ultimate ASI Loader
or the compatible version.dll supplied by ACBlackFlagFix on NexusMods
IMPORTANT
version.dll is INCLUDED in the v2.3.7.0 release.
It remains third-party software and is governed by its original author's license/redistribution terms.
Installation
1. Download the normal v2.3.7.0 release, not the source-only package.
2. Place these files in the Black Flag game folder:
ACBFHapticsBridge-v8-r9h.asi
ACBFHapticsBridge-v8.ini
ShadowsHapticsPack-v5.hfx
version.dll
3. They must be beside:
ACBlackFlag.exe
4. Launch the game.
5. Press F9 if you want to quickly test whether the bridge is producing feedback.
6. Press F10 if you need a diagnostic snapshot in the log.

☕ DONATING
Not obligated to, but appreciated nonetheless! :D
If you liked this mod and wanna buy me a coffee, show your appreciation / leave a tip, you can donate via these wallets.
Multicoin is available; BSC Network / ETH Network is preferred for things such as BNB, USDC, or whatever you would like.

It  helps pay bills, motivates me :D, and encourages me to pursue my endeavours in modding/programming.

MetaMask / Multicoin:                     0xFfd40Cf24B1484f8567181E1c550eA494967f36f
Solana:                                          DYgtA98yJ7yMf7rg8Mx65jiybeL4FyqpKGoED3VW2MuG
TRON:                                                   TL2oF5L8UNVLS58BmmsynndjY3nZAkTnid
Bitcoin — Native SegWit:                    bc1qjl3kv3s7l2wrmx68vhdmuqnfcj6hl6650krufx
PayPal profile donate is not working for my country, and I can't leave the direct link here because that would mean doxxing myself :(

😄 AAAANDDDDDDDDDD THIS IS MY FIRST MOD!!
Glad it works for me at least!! :D
Please report any bugs or weird stuff so I can work on them if I can.

✨ What the ASI mod does — v2.3.7.0
ACBFHapticsBridge currently adds contextual feedback for:
DualSense: Full 48 kHz stereo haptics through Black Flag's native Wwise Quad Audio Haptics path
Conventional rumble: Two-motor rumble conversion for Xbox/XInput, DS4/emulated and compatible GameInput controllers
Movement: Event-synchronized walking, jogging, spatial footsteps, L3 run opening/sustain and sprint contacts
Parkour: Jump takeoff, airtime-scaled landing, horizontal-travel boost, long-fall maximum impacts and climb catches
Dodging: Ground dodge / roll and directional dodge handling
Water: Automatic water-entry recovery, dive/splash, surface, swim-up and L3-gated underwater lunge / fast-swim behavior
Combat: Parry, light attacks, two-stage heavy attack, kick and native-combat haptic supplementation
Character / equipment: Hood action and sheathe / unsheathe feedback
Menus: Menu-open feedback, navigation ticks, reduced shoulder feedback and native menu-tick supplementation on DualSense
Runtime protection: Native-feedback arbitration to reduce doubled effects
Recovery: Player/menu/state reacquisition and optimized Wwise haptic-sink handling
Performance: 500 Hz bridge-side input worker, cached GameInput metadata, lower menu/input overhead and runtime performance diagnostics

🆕 v2.3.7.0 / v8-r9h — Changelog
v2.3.7.0 is a compatibility-only update to the tuned v2.3.6 / v8-r9g runtime. Haptic effects, gain models, Wwise semantic events, gameplay-state behavior, timing, controller polling, menu behavior, native-haptic processing and conventional-rumble tuning are otherwise unchanged.
🌐 Multi-version executable support
Added verified native-XInput observer support for the Ubisoft Connect executable.
Added verified native-XInput observer support for the Ubisoft+ executable.
The observer selects the correct verified
XInputSetState
import slot for the detected executable layout.
A candidate slot is modified only when it exactly contains the resolved
XInputSetState
API pointer; otherwise the observer fails closed and leaves the game's import untouched.
The supplied Ubisoft Connect and Ubisoft+ executables retain the existing Wwise PostEvent hook, Quad Audio Haptics flush hook and all 21/21 gameplay-state callback targets used by the bridge.
No new executable strings, broad signature scans, haptic mappings, effect timings or gameplay tuning were added.
The supplied INI keeps the established tuning and ships with MasterGain=0.90.
🎛️ Retained v2.3.6 / v8-r9g feature and tuning baseline
The detailed sections below describe the movement, combat, water, menu, DualSense, XInput and performance behavior inherited unchanged by v2.3.7.0.

🚶 Spatial movement + footsteps
Walking and jogging are synchronized to Black Flag's real gait events rather than only a stick-driven loop.
Added true left/right spatial footsteps.
A fresh gait cycle begins on Edward's right foot and alternates right/left.
Default spatial balance is 100% on the active side with 25% crossfeed to the opposite side.
Spatial gait applies to DualSense stereo haptics and conventional rumble conversion.
L3 running retains the tuned 1.5-second stronger opening and sustained run texture.
Real sprint contacts layer on top of the continuous L3 run texture.

🎮 XInput / Xbox rumble fixes
Fixed land-running becoming continuous/stuck vibration on XInput-class controllers.
XInput land sprint now follows real gait-contact events step-by-step.
The continuous land-run motor bed is suppressed; water fast-swim/lunge can still remain continuous where appropriate.
XInput jump impact is capped to 300 ms.
XInput landing impact is capped to 300 ms.
XInput light-attack vibration is capped to 300 ms per swing.
Added a 16 ms explicit zero-motor barrier after those short XInput impacts so rapid actions do not blend into one long vibration.
XInput menu L1/R1 feedback reduced by 50%.

🧗 Jump, parkour + long-fall impacts
Jump/landing detection remains based primarily on Black Flag's real Jump/InAir gameplay states.
Landing strength scales with measured airtime.
Parkour movement distance can add extra landing/catch intensity using integrated left-stick travel as a proxy.
Improved climb-to-fall carry and airborne climb-catch impacts.
Short and medium falls remain progressively scaled instead of all using the same impact.
Falls of 3 seconds or longer now reach the current maximum base impact of 3.15.
The 3-second+ maximum applies to both DualSense bridge feedback and XInput/GameInput-class conventional rumble.
XInput keeps the short 300 ms landing duration; long falls increase amplitude, not motor duration.

🌊 Water + underwater lunge fixes
Improved automatic water-entry/reacquisition when Edward enters water without a manual jump.
Fixed underwater lunge feedback triggering simply from moving/changing direction after being stationary.
The Wwise water-lunge event is no longer trusted by itself as proof that L3 was pressed.
A real configured SprintLunge / L3 rising edge must authorize the underwater lunge.
Direction-only copies of the event are ignored.
Legitimate underwater lunge/fast-swim sustain keeps its 3-second safety cap.
Surface swim-up and water exit/surface behavior remain intact.

⚔️ Combat + native-feedback supplementation
Native Black Flag feedback still receives first refusal to avoid doubled effects.
Verified native Quad Audio Haptics for selected Edward combat actions are supplemented by +75% / 1.75x.
Covered combat families include sword attacks, normal/perfect parry feedback, pistol fire and kicks.
If suitable native feedback is absent, the established bridge fallback remains available.
Cannon/naval, player-hit, movement and water haptics are not intentionally included in the combat boost.
Two-stage Heavy R1 timing remains tuned to a 320 ms hold threshold and 590 ms second-impact delay.
Normal parry remains latency-trimmed and native-first.

🎛️ DualSense tuning
Bridge-generated DualSense output has a global 0.90 cleanup multiplier to reduce noise.
Jump/landing receives an additional 0.85 multiplier for cleaner ordinary impacts.
Land walk/jog/L3-run tuning was raised again relative to the intermediate release and currently uses LandMovementHapticGain=1.035.
DualSense menu L1/R1 feedback reduced by 25%.
DualSense menu navigation tick increased by 30%.
The +30% menu tick applies to bridge fallback ticks and verified Ubisoft-native menu Quad ticks.
Normalized DualSense walk control: user value 1.00 represents the established tuned baseline that previously used raw 4.80.

📋 Menu + input responsiveness
Exact Black Flag Wwise menu events are preferred for normal menu navigation.
When semantic menu routing is active, the bridge no longer continuously rescans analog-stick/D-pad navigation heuristics.
The underwater L3 correlation check is evaluated on an actual L3 edge instead of continuously while the left stick moves.
Menu/category haptic sink swaps are handled as lightweight sink handoffs instead of repeated full playback resets.
Custom L1/R1 menu tuning remains active.

⚡ Performance cleanup — retained from v8-r9g
Bridge-side controller/input worker target changed from approximately 125 Hz to 500 Hz / 2 ms.
This does not overclock the controller hardware. A 250 Hz USB source still provides a fresh report about every 4 ms; the 500 Hz bridge worker simply samples it promptly.
GameInput device identity/metadata is cached instead of queried every 2 ms.
Device identity refresh interval: 125 ms.
Device status refresh interval: 500 ms.
Raw controller-button layout/count is cached per device.
Conventional steady-state rumble writes are capped at 250 Hz / 4 ms, while start/stop/zero transitions remain immediate.
Native haptic detection and native combat/menu gain processing share one buffer pass.
Normal haptic-sink pointer changes no longer cause expensive full recovery resets.
Added detailed performance telemetry to F10 for worker gaps, GameInput time, rumble-write time, audio-hook time and sink rebinds.
F12 can fully suspend bridge controller polling/raw-input processing for A/B diagnostics while semantic/state/native routing continues.

🔄 Recovery + reliability
Improved state reacquisition after reloads, menu transitions and long Wwise silence.
Ordinary sink changes preserve valid semantic/gameplay state instead of unnecessarily clearing everything.
Genuine long silence/load events still trigger a clean recovery.
Expanded diagnostics make controller backend, semantic domain, state hooks, water, landing, native arbitration and output activity easier to verify.

IN SHORT
v2.3.7.0 keeps the complete v2.3.6 hybrid Wwise + gameplay-state haptics behavior and tuning, while adding verified Ubisoft Connect / Ubisoft+ native-XInput observer compatibility. No haptic effect or gameplay tuning was intentionally changed.

⌨️ Keyboard / ASI Hotkeys
F9: Plays the Dodge test effect through the active DualSense haptic / conventional rumble output paths. Useful for checking whether the mod is producing feedback.
F10: Writes a detailed runtime snapshot to ACBFHapticsBridge-v8.log, including controller/backend information, semantic/gameplay state, water/landing diagnostics, native arbitration, effect activity and v8-r9h performance telemetry.
F11: Enables / disables the complete bridge. Disabling immediately stops ASI-added playback and clears bridge state. Re-enabling performs a clean runtime/state reacquisition.
F12: Suspends / resumes bridge controller polling and raw-input processing for diagnostics. Confirmed Wwise semantic events, gameplay-state hooks and native routing remain active. This is useful for A/B testing input/performance behavior without unloading the ASI.

🎮 Default Controller Bindings
Jump:Cross | Raw/fallback correlation for climb jump, swim-up and jump-state diagnostics; actual ground jump/landing is state-driven
DodgeDive:Circle | Water/air dive fallback; normal ground dodge prefers the exact game event
Parry:L1 | Normal-parry fallback when suitable native feedback is missing
Attack:R1 | Hybrid raw correlation for combo gaps/heavy hold; known attacks prefer exact game events
SprintLunge:L3 | L3 land sprint and required authorization for underwater lunge / fast-swim
SheatheHood:DPadLeft | Fallback/correlation; known sheathe/unsheathe and Hood events are semantic/event-driven
Kick:R2 + Square | Fallback combination; Black Flag's actual kick event is preferred when available
The left stick remains important for:
the game's own walking/running direction and speed
movement-strength classification
directional-dodge context
climbing-jump direction/context
parkour horizontal-travel estimation used by landing/catch scaling
The game itself also uses the left stick for menu navigation, but when SemanticEvents/MenuState is enabled the bridge follows Black Flag's actual menu Wwise events instead of continuously treating stick direction as a separate menu-feedback trigger.
Landing, haystack entry, water entry and water surface effects are primarily triggered by detected gameplay states/events rather than a remappable raw button.

🔧 Changing Controller Inputs
Open:
ACBFHapticsBridge-v8.ini
beside:
ACBlackFlag.exe
Find:
[InputBindings]
Jump=Cross
DodgeDive=Circle
Parry=L1
Attack=R1
SprintLunge=L3
SheatheHood=DPadLeft
Change the value after = so the bridge's raw-input fallback/correlation path matches the corresponding in-game controller assignment.
For example, if you remap Jump to Triangle in Black Flag:
[InputBindings]
Jump=Triangle
DodgeDive=Circle
Parry=L1
Attack=R1
SprintLunge=L3
SheatheHood=DPadLeft
IMPORTANT FOR v2.3.7.0
The bridge now prefers exact Black Flag Wwise events and gameplay-state callbacks whenever they are available. [InputBindings] therefore controls unresolved/raw fallbacks and state correlation; it does not replace the game's semantic event system.
IMPORTANT
Restart the game after changing the INI. Settings are loaded during startup.

🕹️ Available Input Names
Cross:A | Bottom face button
Circle:B | Right face button
Square:X | Left face button
Triangle:Y | Top face button
L1:LB | Left shoulder
R1:RB | Right shoulder
L2:LT | Left trigger
R2:RT | Right trigger
L3:LS | Left-stick click
R3:RS | Right-stick click
DPadUp: Same | D-pad Up
DPadDown: Same | D-pad Down
DPadLeft: Same | D-pad Left
DPadRight: Same | D-pad Right
None / Disabled / Off: No direct raw-input binding
Values are case-insensitive. Spaces, underscores and hyphens are ignored.
WARNING
Use Cross for the PlayStation cross/X button. Writing X means the Xbox X position, which corresponds to PlayStation Square.
The following cannot be used as normal gameplay action bindings:
Touchpad
PS / Guide button
controller paddles
keyboard keys
analog-stick directions
Options / Start
Share / Create / View
Menu controls are handled separately.
Do not give multiple action entries the same physical button unless you intentionally want both raw fallback actions to share that input.
Press F10 after launching. The log should contain something similar to:
INPUTCFG jump=Triangle dodgeDive=Circle parry=L1 attack=R1 sprintLunge=L3 sheatheHood=DPadLeft invalidFallbacks=0
If a name is invalid, that action falls back to its tested default and invalidFallbacks increases.

🎚️ Changing Individual Effects
Every automatic effect has its own section where applicable.
Example:
[Effect.Jump]
Enabled=1
IntensityMultiplier=1.00
TriggerOffsetMs=0
StartTrimOffsetMs=0
CooldownMs=300
Common settings
Enabled=1: Effect enabled
Enabled=0: Effect disabled
IntensityMultiplier=0.50: Half strength
IntensityMultiplier=1.00: Current calibrated default
IntensityMultiplier=1.25: 25% stronger
IntensityMultiplier=2.00: Double strength
TriggerOffsetMs: Adds/removes delay after the input/state/event is detected
StartTrimOffsetMs: Changes where playback begins inside the captured waveform
IntensityMultiplier changes both bridge-generated DualSense haptics and conventional rumble for that effect.
Timing
TriggerOffsetMs=-20: Makes the effect earlier, but never before the triggering event/input/state was detected
TriggerOffsetMs=20: Makes it later
StartTrimOffsetMs=20: Skips more of the waveform beginning, producing a faster impact
StartTrimOffsetMs=-20: Restores more of the original lead-in
These settings never change game or animation speed.

🧪 Configuration Examples
Disable walking feedback
[Effect.Walk]
Enabled=0
Reduce parry strength by 30%
[Effect.Parry]
Enabled=1
IntensityMultiplier=0.70
Make the ground-dodge impact start faster
[Effect.GroundDodge]
TriggerOffsetMs=-20
StartTrimOffsetMs=25
Disable only the second heavy-attack impact
[Effect.HeavyAttackSecond]
Enabled=0
Make menu navigation softer
[Effect.MenuTick]
IntensityMultiplier=0.60

🧩 Special v2.3.7.0 Effect Settings
DualSense walk normalization
[Effect.Walk]
DualSenseHapticNormalizedBaseline=1
DualSenseHapticMultiplier=1.00
1.00 = the established tuned DualSense walk baseline retained from v2.3.6.
0.50 = half that tuned walk level.
2.00 = double that tuned walk level.
4.80 = maximum user scale for this normalized control, still subject to PeakLimit.
Old INIs without DualSenseHapticNormalizedBaseline use legacy/raw semantics for compatibility.

DualSense jog/run tuning
[Effect.Run]
DualSenseJogHapticMultiplier=1.20
OpeningDurationMs=1500

[Movement]
RunBaseGain=1.80
RunOpeningGain=2.28
RunOpeningDurationMs=1500

Spatial footsteps
[Footsteps]
SpatialFootsteps=1
RightFootRightGain=1.00
RightFootLeftGain=0.25
LeftFootLeftGain=1.00
LeftFootRightGain=0.25
ResetAfterMs=750
SprintContactOverlayGain=0.84

Long-fall scaling
[GameplayState]
LandingShortAirMs=900
LandingLongAirMs=2000
LandingMaxAirMs=3000
LandingMinimumGain=0.30
LandingShortGain=0.55
LandingGain=0.70
LandingLongGain=3.15
The current release ramps toward 3.15 and treats falls of roughly 3 seconds or longer as maximum base-impact landings.

XInput-only short impact limits
[Rumble]
LandRunStepOnly=1
SprintStepGain=1.20
XInputJumpLandingMaxDurationMs=300
XInputLightAttackMaxDurationMs=300
XInputActionZeroGapMs=16
These duration/zero-gap controls are for conventional XInput/Xbox-class rumble. They do not shorten DualSense 48 kHz haptic playback.

DualSense bridge cleanup
[DualSense]
BridgeHapticGain=0.90
JumpLandingHapticGain=0.85
LandMovementHapticGain=1.035
These affect bridge-generated DualSense feedback, not Ubisoft-native Quad haptics.

Menu tuning
[Menus]
Enabled=1
TickGain=0.35
OpenGain=0.28
ShoulderGain=0.315
XInputShoulderGainMultiplier=0.50
DualSenseShoulderGainMultiplier=0.75
DualSenseTickGainMultiplier=1.30
DualSenseTickNativeWindowMs=250
Enabled=0 disables plugin-added menu feedback while menu-domain detection still protects gameplay feedback from leaking into detected menus.

Underwater L3 gate
[SemanticEvents]
WaterLungeSustainMs=3000
WaterLungeInputGateMs=600
The Wwise water-lunge event can confirm a lunge only after a real configured SprintLunge/L3 rising edge inside this authorization window.

Native combat supplement
[NativeArbitration]
Enabled=1
CombatNativeBoost=1
CombatNativeHapticGain=1.75
CombatNativeBoostWindowMs=700
1.75 means the selected verified Ubisoft-native combat Quad haptic stream is supplemented by +75% while the action window is active.

🌐 Global Settings
Output type
[Output]
NativeHaptics=1
ConventionalRumble=1
PreferGameInput=1
RumbleOnDualSense=1
NativeHaptics=0: Disables full DualSense waveform injection.
ConventionalRumble=0: Disables ordinary low/high-motor vibration generated by the bridge.
RumbleOnDualSense=0: Leaves the bridge's full DualSense haptic layer without its parallel conventional-rumble copy on detected DualSense devices.
Keep both enabled for the normal hybrid configuration.

Overall intensity
[Playback]
NormalizedBaseline=1
MasterGain=0.90
MovementGain=1.00
OneShotGain=1.00
PeakLimit=2.00
With NormalizedBaseline=1:
MasterGain=0.90 = the supplied v2.3.7.0 master setting, equal to 90% of the normalized tuned master baseline
OneShotGain=1.00 = the tuned current one-shot baseline (internally equivalent to the older raw 1.80 level)
MovementGain changes continuous movement textures
PeakLimit is the final haptic output limiter; 0 disables limiting

Conventional rumble
[Rumble]
Gain=1.00
MovementGain=2.00
LowMotorScale=3.00
HighMotorScale=1.60
Deadzone=0.008
These control conversion of the captured waveform timeline into conventional controller motors.

Movement thresholds
[Movement]
WalkThreshold=11000
RunThreshold=24500
SprintToggle=1
Lower thresholds make the bridge classify movement strength with less stick deflection.
SprintToggle=1: Press the configured SprintLunge input once for the run latch.
SprintToggle=0: Hold the configured SprintLunge input.

Performance / controller polling
[General]
PollMs=2
GameInputDeviceRefreshMs=125
GameInputStatusRefreshMs=500
RumbleWriteIntervalMs=4
PollMs=2 means a 500 Hz bridge-side worker target. It does not change the physical polling/report rate of the controller itself.
The metadata/status refresh intervals are intentionally slower because VID/PID/device-layout information does not need to be queried 500 times per second.
Steady-state conventional rumble is capped to a 4 ms / 250 Hz write cadence while important start/stop/zero changes remain immediate.

⚠️ Important INI Warning
Use:
[InputBindings]
for controller-button fallback/correlation mappings.
The lower:
[Buttons]
section contains advanced waveform names, for example:
BEffect=dodge
LBEffect=parry
RBEffect=light_attack_single
XEffect=parry
Those are not controller-remapping fields.
Changing them selects which captured HFX waveform an action/fallback uses.
Normally, leave these at their supplied values unless you know exactly what you are changing:
[Buttons]
[Hook]
advanced [GameplayState] addresses / hook-related values
verified semantic event IDs

🔐 Verified v2.3.7.0 Build
Nexus release:v2.3.7.0
Internal build:ACBFHapticsBridge-v8-r9h
Runtime label:v8-r9h-v2.3.7.0-multiversion
ASI:ACBFHapticsBridge-v8-r9h.asi
SHA-256:
869361c87e445601d3d1ff5807922f4d0c60cb37f51a7bf37bdd05103514f914
Supported executable profiles:
Steam — Title Update 1.0.6: runtime confirmed
Ubisoft Connect: executable compatibility verified
Ubisoft+: executable compatibility verified
The current v2.3.7.0 release includes the actual C++ source used to build the released ASI under the separate Source/ folder.
The current build/release verification includes:
deterministic clean build scripts
PE / hook audit tooling
HFX validation tooling
semantic event map
test checklist
source checksums
build-verification documentation
exact release-specific license information
The final packaged source was rebuilt and verified to reproduce the shipped ASI byte-for-byte.
The PE audit verifies the current ASI as x86-64 with deterministic timestamping, ASLR, NX, high-entropy VA, no RWX sections and stripped COFF symbols.

📂 Source Notice — v2.3.7.0
Unlike the old v2.1 reconstruction package, the current v2.3.7.0 release contains the actual buildable source used for the current ASI.
A simplified layout is:
Source/
├─ src/
│  └─ main.cpp
├─ build_linux_pe_r9h.sh
├─ audit_v8_r9h.py
├─ validate_hfx.py
├─ ACBFHapticsBridge-v8.ini
├─ SOURCE-CHECKSUMS.sha256
├─ BUILD-VERIFICATION.md
├─ EVENT-MAP.md
├─ TEST-CHECKLIST.md
├─ V8-R9H-CHANGES.md
├─ LICENSE.txt
└─ release/build provenance and historical change notes

📜 License — Non-Commercial
IMPORTANT
ACBFHapticsBridge v2.3.7.0 is NOT released under the MIT License.
It is distributed under the ACBFHapticsBridge Non-Commercial License v1.0.
The full release-specific license is included with the source and inside the downloadable release archive as:
ACBFHapticsBridge-v2.3.7.0-NONCOMMERCIAL-LICENSE.txt
What the license allows
Download and use the mod personally: ✅
Use the mod for non-commercial purposes: ✅
Study the source/build material: ✅
Modify the ACBFHapticsBridge material: ✅
Make a non-commercial derivative: ✅
Redistribute the original or a modified version for free: ✅
Fork the source for non-commercial development: ✅
Keep reasonable attribution and the license with redistributed copies:Required

What is NOT allowed without my prior written permission
Sell ACBFHapticsBridge: ❌
Charge for a download or copy: ❌
Put the mod behind a paywall: ❌
Require Patreon/subscription/payment to download the mod: ❌
Bundle it inside a paid mod pack or paid software package: ❌
Sell a modified or substantially identical derivative: ❌
Charge for activation, access, installation, or use of the mod itself: ❌
Remove the copyright/license notice and present the project as your own: ❌
Commercially sublicense or exploit the ASI/source/tools: ❌

Changing the filename, repacking the files, making small modifications, or creating a substantially identical derivative does not turn it into something that can be sold.
Commercial permission can only be granted separately by the ACBFHapticsBridge copyright holder in writing.

Donations
Voluntary donations to support my own development work are fine.
The mod itself is intended to remain freely accessible. A third-party redistributor may not make access to ACBFHapticsBridge dependent on a purchase, subscription, Patreon membership, donation, or other payment without written permission.

Ubisoft / Assassin's Creed material
The ACBFHapticsBridge Non-Commercial License only covers material for which the ACBFHapticsBridge contributors hold the applicable rights.
It does not grant ownership of or commercial rights to:
Assassin's Creed IV: Black Flag
Assassin's Creed Shadows
Ubisoft trademarks, game code, artwork, audio, haptic design, assets, or other intellectual property
Anvil / AnvilNext material
Wwise or other third-party technology
Ubisoft executables, DLLs, data files, or proprietary game files
unrelated third-party components
Nothing in this repository or release should be interpreted as granting permission to commercially exploit Ubisoft or other third-party intellectual property.

Shadows HFX capture notice
The release contains:
ShadowsHapticsPack-v5.hfx
This file contains captured and/or processed haptic waveform data derived from controller/haptic output produced by Assassin's Creed Shadows.
The HFX pack is not being claimed as wholly original Ubisoft-independent content, and it is not licensed as original ACBFHapticsBridge code under the project license.
The fact that I captured, processed, edited, reconstructed, packaged, or mapped the haptic data does not mean this license grants ownership of Ubisoft's underlying game content or haptic design.
No commercial rights to Ubisoft material are granted by this project.
Any redistribution or use of the HFX pack remains subject to applicable copyright, platform rules, permissions, and third-party terms.

version.dll and other third-party files
The current v2.3.7.0 package includes:
version.dll
That file remains governed by its original author's own license and redistribution terms.
Bundling a third-party ASI loader with ACBFHapticsBridge does not place that loader under the ACBFHapticsBridge Non-Commercial License.

Attribution / modified versions
If you redistribute a permitted non-commercial modification:
keep the ACBFHapticsBridge copyright and license notice
include the license with the files
clearly state that your version is modified
do not present an unofficial derivative as an official ACBFHapticsBridge release
do not imply endorsement by the original project
do not claim ownership of Ubisoft or other third-party material
WARNING
Selling this mod, paywalling it, or commercially redistributing ACBFHapticsBridge material is not permitted under the project license without prior written permission.

🔮 Future Updates
Future updates will include — hopefully — better haptics and rumble effects captured from Assassin's Creed Shadows, along with improved state/context detection as I continue working on the bridge.
This is my first mod, so if you find bugs, weird behavior, controller-specific problems, missed effects, or anything else useful, please report it.

Not perfect but perfectible. DEadDRop out!
