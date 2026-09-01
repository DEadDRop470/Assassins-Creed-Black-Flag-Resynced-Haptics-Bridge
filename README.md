<p align="center">
  <strong>ACBFHapticsBridge</strong>
</p>
<p align="center">
  <strong>DualSense haptics + conventional rumble for Assassin's Creed IV: Black Flag Resynced</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/RELEASE-v2.3.7.3-76C893?style=for-the-badge" alt="Release v2.3.7.3">
  <img src="https://img.shields.io/badge/BUILD-v8--r9k-7FB3FF?style=for-the-badge" alt="Build v8-r9k">
  <img src="https://img.shields.io/badge/DualSense-HAPTICS-0070D1?style=for-the-badge" alt="DualSense Haptics">
  <img src="https://img.shields.io/badge/XInput-RUMBLE-555555?style=for-the-badge" alt="XInput Rumble">
  <img src="https://img.shields.io/badge/LICENSE-NON--COMMERCIAL-E66B55?style=for-the-badge" alt="Non-Commercial License">
</p>

<p align="center">
  <strong>Runtime:</strong> <code>v8-r9k-v2.3.7.3-ubisoft-isolated-fallback</code>
</p>

🟠 IMPORTANT
License: ACBFHapticsBridge uses the ACBFHapticsBridge Non-Commercial License v1.1.
Free personal/non-commercial use, modification and free redistribution are permitted under the license terms. Selling, paywalling, or commercially redistributing the mod is not permitted without prior written permission.

🔗 Downloads & Source

Nexus Mods — Main mod page / download: https://www.nexusmods.com/assassinscreedblackflagresynced/mods/412?tab=description

GOFILE — Free mirror: https://gofile.io/d/whwCCJYe

GitHub — Source, release verification & project files: https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge

🎮 What this mod does

ACBFHapticsBridge adds Assassin's Creed Shadows-derived haptic feedback to Assassin's Creed IV: Black Flag Resynced.

The current v2.3.7.3 / v8-r9k build deliberately uses the proven v2.3.7.0 / v8-r9h behavior as its gameplay, Steam DualSense haptic and conventional-rumble baseline.

On Steam, the bridge continues using Black Flag's existing Wwise Motion / Quad Audio Haptics path exactly as the known-good v2.3.7.0 baseline does. The Ubisoft compatibility additions are isolated to the Ubisoft Connect and Ubisoft+ executable profiles instead of changing the already-working Steam output path.

For Ubisoft Connect / Ubisoft+, Black Flag's native Quad haptic path remains the first choice. If meaningful native Quad haptics are absent while bridge HFX playback is required, v8-r9k can use a guarded 48 kHz / 4-channel direct DualSense fallback on actuator channels 3/4.

The mod supports:

✅ Native DualSense haptics — wired

✅ DualSense through DSX + DLC / Virtual DualSense with Audio

✅ Xbox 360 / XInput rumble

✅ DS4 / compatible emulated rumble paths

✅ Xbox 360 emulation through DSX

✅ Compatible GameInput controllers

✅ Steam Input with native wired DualSense

DualSense: full 48 kHz stereo waveform injection through Black Flag's native Quad Audio Haptics path on the proven Steam/r9h route. Verified Ubisoft profiles can use the guarded direct 48 kHz / 4-channel fallback only when meaningful native Quad haptics are absent.

Conventional controllers: the same effect timelines are converted into low/high-motor rumble for Xbox/XInput, DS4/emulated and compatible GameInput controller paths.

The bridge keeps Black Flag's own native controller feedback whenever possible, supplements selected native effects where useful, and fills in actions where the original game provides little or no controller response.

🔵 NOTE
The verified v2.3.7.3 bridge primarily uses GameInput and XInput-compatible controller paths. Generic DirectInput support should not be assumed for every device.

🔵 NOTE
DUALSENSE HAPTICS VS RUMBLE: The 48 kHz DualSense haptic stream and conventional rumble are parallel outputs. A strong haptic effect does not automatically "overflow" into rumble. The haptic path is limited by PeakLimit, while conventional rumble is generated separately when enabled. On Steam, v8-r9k preserves the proven v2.3.7.0 / r9h routing behavior.

✅ Confirmed / Supported Game Versions

Version

Status

Steam — Title Update 1.0.6

✅ Supported — v2.3.7.0 / r9h is the confirmed working behavioral baseline used by r9k

Ubisoft Connect

✅ Executable compatibility verified

Ubisoft+

✅ Executable compatibility verified

Epic Games

🧪 Not yet verified

Steam — Title Update 1.0.6

ACBlackFlag.exe SHA-256

8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140

Ubisoft Connect

ACBlackFlag.exe SHA-256

e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c

Ubisoft+

ACBlackFlag_Plus.exe SHA-256

b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75

voices38 verified profile

19920f34bb2fac814023ee3c27f0ceca1872e80a2aafb37c092c419fff77cc0d

The supplied Ubisoft Connect and Ubisoft+ executables were statically verified against the bridge's executable-dependent hooks. Both retain the required Wwise PostEvent hook, Quad Audio Haptics flush hook and all 21/21 gameplay-state callback slots used by the bridge.

v2.3.7.3 retains dynamic XInputSetState PE-import resolution, with verified executable-specific locations retained only as pointer-validated fail-closed fallbacks.

The important difference in v8-r9k is that the newer output compatibility logic is isolated to Ubisoft Connect / Ubisoft+.

Steam keeps the known-good v2.3.7.0 / r9h output behavior.

For Ubisoft profiles, r9k also:

preserves physical DualSense identity if the active controller backend temporarily changes;

distinguishes meaningful native Quad haptic data from silent Quad flush calls;

uses Black Flag's native Quad haptics whenever they are genuinely available;

allows the 48 kHz / 4-channel direct DualSense fallback only when Ubisoft-native haptics are absent;

stops the fallback as soon as genuine Ubisoft-native haptics return;

does not inject the same bridge HFX through both native and direct waveform paths simultaneously.

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

🟠 IMPORTANT
version.dll is INCLUDED in the v2.3.7.3 release.
It remains third-party software and is governed by its original author's license/redistribution terms.

Installation

Download the normal v2.3.7.3 release, not the source-only package.

Place these files in the Black Flag game folder:

ACBFHapticsBridge-v8-r9k.asi
ACBFHapticsBridge-v8.ini
ShadowsHapticsPack-v5.hfx
version.dll

They must be beside:

ACBlackFlag.exe

Launch the game.

Press F9 if you want to quickly test whether the bridge is producing feedback.

Press F10 if you need a diagnostic snapshot in the log.

☕ DONATING

Not obligated to, but appreciated nonetheless! 

If you liked this mod and wanna buy me a coffee, show your appreciation / leave a tip, you can donate via these wallets.

Multicoin is available; BSC Network / ETH Network is preferred for things such as BNB, USDC, or whatever you would like.

It helps pay bills, motivates me , and encourages me to pursue my endeavours in modding/programming.

MetaMask / Multicoin: 0xFfd40Cf24B1484f8567181E1c550eA494967f36f

Solana: DYgtA98yJ7yMf7rg8Mx65jiybeL4FyqpKGoED3VW2MuG

TRON: TL2oF5L8UNVLS58BmmsynndjY3nZAkTnid

Bitcoin — Native SegWit: bc1qjl3kv3s7l2wrmx68vhdmuqnfcj6hl6650krufx

PayPal profile donate is not working for my country, and I can't leave the direct link here because that would mean doxxing myself :(

😄 AAAANDDDDDDDDDD THIS IS MY FIRST MOD!!

Glad it works for me at least!! 

Please report any bugs or weird stuff so I can work on them if I can.

✨ What the ASI mod does — v2.3.7.3

ACBFHapticsBridge currently adds contextual feedback for:

DualSense: Full 48 kHz stereo haptics through Black Flag's native Wwise Quad Audio Haptics path on the proven Steam/r9h route, with a guarded Ubisoft-only fallback where required

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

🆕 v2.3.7.3 / v8-r9k — Ubisoft-Isolated Compatibility Update

v2.3.7.3 deliberately returns to v2.3.7.0 / v8-r9h as the behavioral baseline.

The working Steam DualSense haptic path, conventional-rumble behavior, gameplay detection, HFX mixing and effect tuning from r9h are retained.

The compatibility work added afterward is now isolated to Ubisoft Connect / Ubisoft+.

🎮 Steam behavior restored to the proven baseline

Steam uses the same Black Flag native Wwise Quad Audio Haptics injection path as v2.3.7.0 / r9h.

The direct 48 kHz DualSense renderer is not eligible on Steam.

The conventional-rumble implementation is restored to the r9h behavior.

A detected DualSense successfully handled through GameInput is not newly mirrored through XInput.

The r9i/r9j global native/direct output switching is not used on Steam.

🎧 Ubisoft-only DualSense fallback

Ubisoft Connect / Ubisoft+ keep the r9h native Quad Audio Haptics route as first choice.

The direct DualSense fallback is eligible only on verified Ubisoft profiles.

The fallback requires meaningful bridge HFX work and absence of meaningful native Quad haptics.

Direct output uses 48 kHz / 4-channel audio with DualSense actuator channels 3/4.

The native Quad stream continues to be observed while the fallback is active.

Real Ubisoft-native haptics immediately retake priority.

Bridge HFX are not injected through both native and direct waveform paths simultaneously.

🔎 Ubisoft executable / XInput compatibility

Dynamic PE import-table resolution of XInputSetState.

Known verified IAT locations remain pointer-validated fail-closed fallbacks.

Runtime executable-profile classification determines whether the Ubisoft-only direct fallback can be used.

Physical DualSense identity is retained for the Ubisoft fallback if the input backend temporarily changes.

✅ r9h baseline preserved

The following important functions were verified source-identical against the actual v2.3.7.0 / r9h baseline:

conventional-rumble output

native Quad HFX injection

controller-edge processing

gameplay-state processing

deferred-effect processing

🎛️ Gameplay and tuning unchanged

MasterGain=0.90

all existing r9h INI values remain unchanged

all gameplay-state hooks remain unchanged

all Wwise semantic mappings remain unchanged

movement and spatial footsteps remain unchanged

jump and landing behavior remain unchanged

parkour scaling remains unchanged

water behavior remains unchanged

combat behavior remains unchanged

menu feedback remains unchanged

ShadowsHapticsPack-v5.hfx remains unchanged

version.dll remains unchanged

Only two new configuration values are added:

UbisoftDirectDualSenseFallback=1
UbisoftDirectDualSenseFallbackDelayMs=1000

These settings have no effect on the Steam output path.

🔵 NOTE
In short: v2.3.7.3 keeps the proven v2.3.7.0 Steam haptic behavior and adds the newer compatibility work only where it is needed: Ubisoft Connect / Ubisoft+.

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

XInput land sprint follows real gait-contact events step-by-step.

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

Falls of 3 seconds or longer reach the current maximum base impact of 3.15.

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

Land walk/jog/L3-run tuning uses LandMovementHapticGain=1.035.

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

⌨️ Keyboard / ASI Hotkeys

F9: Plays the Dodge test effect through the active DualSense haptic / conventional rumble output paths. Useful for checking whether the mod is producing feedback.

F10: Writes a detailed runtime snapshot to ACBFHapticsBridge-v8.log, including controller/backend information, semantic/gameplay state, water/landing diagnostics, native arbitration, effect activity and v8-r9k diagnostics.

F11: Enables / disables the complete bridge. Disabling immediately stops ASI-added playback and clears bridge state. Re-enabling performs a clean runtime/state reacquisition.

F12: Suspends / resumes bridge controller polling and raw-input processing for diagnostics. Confirmed Wwise semantic events, gameplay-state hooks and native routing remain active. This is useful for A/B testing input/performance behavior without unloading the ASI.

🎮 Default Controller Bindings

Jump: Cross | Raw/fallback correlation for climb jump, swim-up and jump-state diagnostics; actual ground jump/landing is state-driven

DodgeDive: Circle | Water/air dive fallback; normal ground dodge prefers the exact game event

Parry: L1 | Normal-parry fallback when suitable native feedback is missing

Attack: R1 | Hybrid raw correlation for combo gaps/heavy hold; known attacks prefer exact game events

SprintLunge: L3 | L3 land sprint and required authorization for underwater lunge / fast-swim

SheatheHood: DPadLeft | Fallback/correlation; known sheathe/unsheathe and Hood events are semantic/event-driven

Kick: R2 + Square | Fallback combination; Black Flag's actual kick event is preferred when available

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

Use the [InputBindings] section for the controller-button fallback/correlation mappings.

Accepted binding names include Cross/A, Circle/B, Square/X, Triangle/Y, L1/LB, R1/RB, L2/LT, R2/RT, L3/LS, R3/RS and the D-Pad directions.

Write Cross for the PlayStation X/cross button. The value X means the Xbox X / PlayStation Square button.

Touchpad, PS/Guide, paddles, keyboard keys, stick directions, Options/Start and Share/Create/View are not general action-binding values. Menu opening/navigation remains handled separately by [Menus].

Restart Black Flag after changing the INI.

⚙️ Per-Effect Configuration

Automatic effects remain independently configurable where applicable.

Users can adjust:

enable / disable state

intensity

trigger timing

waveform start trim

cooldowns

movement loop timing

This allows the bridge to be tuned without recompiling the ASI.

Intensity

IntensityMultiplier=1.00

Changes both DualSense bridge haptics and conventional rumble for that effect.

Examples:

0.50 = half strength

1.25 = 25% stronger

2.00 = double strength

Timing

TriggerOffsetMs=-20: Makes the effect earlier, but never before the triggering event/input/state was detected.

TriggerOffsetMs=20: Makes it later.

StartTrimOffsetMs=20: Skips more of the waveform beginning, producing a faster impact.

StartTrimOffsetMs=-20: Restores more of the original lead-in.

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

🧩 Special v2.3.7.3 Effect Settings

DualSense walk normalization

[Effect.Walk]
DualSenseHapticNormalizedBaseline=1
DualSenseHapticMultiplier=1.00

1.00 = established tuned DualSense walk baseline retained from v2.3.7.0

0.50 = half that tuned walk level

2.00 = double that tuned walk level

4.80 = maximum user scale for this normalized control, still subject to PeakLimit

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
UbisoftDirectDualSenseFallback=1
UbisoftDirectDualSenseFallbackDelayMs=1000

NativeHaptics=0: Disables full DualSense waveform injection.

ConventionalRumble=0: Disables ordinary low/high-motor vibration generated by the bridge.

RumbleOnDualSense=0: Leaves the bridge's full DualSense haptic layer without its parallel conventional-rumble copy on detected DualSense devices.

UbisoftDirectDualSenseFallback=1: Allows the guarded 48 kHz / 4-channel DualSense fallback on verified Ubisoft Connect / Ubisoft+ executable profiles when meaningful native Quad haptics are absent. It does not enable the direct renderer on Steam.

UbisoftDirectDualSenseFallbackDelayMs=1000: Grace interval before the Ubisoft-only direct fallback may take over when native Quad haptics remain absent.

Overall intensity

[Playback]
NormalizedBaseline=1
MasterGain=0.90
MovementGain=1.00
OneShotGain=1.00
PeakLimit=2.00

With NormalizedBaseline=1:

MasterGain=0.90 = supplied v2.3.7.3 master setting, equal to 90% of the normalized tuned master baseline

OneShotGain=1.00 = tuned current one-shot baseline

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

🔐 Verified v2.3.7.3 Build

Item

Value

Release

v2.3.7.3

Internal build

ACBFHapticsBridge-v8-r9k

Runtime label

v8-r9k-v2.3.7.3-ubisoft-isolated-fallback

ASI

ACBFHapticsBridge-v8-r9k.asi

ASI SHA-256

99e7862e3dfc8efbb0508394719649de937d2a618b2ed3a27832e445642c761e

Release ZIP SHA-256

3e43483b9605bd560b1f75bc943bf4fd75038a05c8bd5195f721659c52afad69

HFX SHA-256

9e994203bbafbf0a318f9056066a54b02adad43c0311f77372ea71dec406565f

version.dll SHA-256

6203c5a0ba1f8c5c77c7c12c33be178a5057063ef1da265eb0a546495c6e6a4e

Two clean consecutive r9k builds produced the same ASI byte-for-byte.

Build verification confirms:

AMD64 PE32+ DLL

deterministic COFF timestamp

ASLR

NX

High Entropy VA

no RWX sections

no real static imports

14-effect / 48 kHz stereo HFX pack

unchanged version.dll

unchanged r9h gameplay/tuning configuration

source-identical verification of the critical r9h baseline functions

🔵 NOTE
v2.3.7.0 / r9h remains the confirmed working Steam behavioral baseline. v2.3.7.3 / r9k is deliberately built around that baseline while isolating the newer compatibility work to Ubisoft profiles.

📂 Source Notice — v2.3.7.3

The current v2.3.7.3 release contains the actual buildable source used for the current ASI.

A simplified layout is:

Source/
├─ src/
│  └─ main.cpp
├─ build_linux_pe_r9k.sh
├─ audit_r9k.py
├─ verify_r9h_baseline.py
├─ validate_hfx.py
├─ ACBFHapticsBridge-v8.ini
├─ SOURCE-CHECKSUMS.sha256
├─ BUILD-VERIFICATION.md
├─ R9K-IMPLEMENTATION.md
├─ SOURCE-DELTA-AUDIT-v8-r9k.md
├─ UBISOFT-CROSSREFERENCE.md
├─ TEST-CHECKLIST.md
├─ CHANGELOG-v2.3.7.0-to-v2.3.7.3.md
├─ ACBFHapticsBridge-v2.3.7.3-NONCOMMERCIAL-LICENSE.txt
├─ LICENSE.txt
└─ release/build provenance and historical change notes

📜 License — Non-Commercial

🟠 IMPORTANT
ACBFHapticsBridge v2.3.7.3 is NOT released under the MIT License.
It is distributed under the ACBFHapticsBridge Non-Commercial License v1.1.

The full release-specific license is included with the source and inside the downloadable release archive as:

ACBFHapticsBridge-v2.3.7.3-NONCOMMERCIAL-LICENSE.txt

What the license allows

✅ Download and use the mod personally

✅ Use the mod for non-commercial purposes

✅ Study the source/build material

✅ Modify the ACBFHapticsBridge material

✅ Make a non-commercial derivative

✅ Redistribute the original or a modified version for free

✅ Fork the source for non-commercial development

Required: keep reasonable attribution and the license with redistributed copies

What is NOT allowed without prior written permission

❌ Sell ACBFHapticsBridge

❌ Charge for a download or copy

❌ Put the mod behind a paywall

❌ Require Patreon/subscription/payment to download the mod

❌ Bundle it inside a paid mod pack or paid software package

❌ Sell a modified or substantially identical derivative

❌ Charge for activation, access, installation, or use of the mod itself

❌ Remove the copyright/license notice and present the project as your own

❌ Commercially sublicense or exploit the ASI/source/tools

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

The current v2.3.7.3 package includes:

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

🔴 WARNING
Selling this mod, paywalling it, or commercially redistributing ACBFHapticsBridge material is not permitted under the project license without prior written permission.

🔮 Future Updates

Future updates will include — hopefully — better haptics and rumble effects captured from Assassin's Creed Shadows, along with improved state/context detection as I continue working on the bridge.

This is my first mod, so if you find bugs, weird behavior, controller-specific problems, missed effects, or anything else useful, please report it.

<p align="center"><strong>Not perfect but perfectible. DEadDRop out!</strong></p>
