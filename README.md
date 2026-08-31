<div align="center">

# ACBFHapticsBridge

### DualSense haptics + conventional rumble for Assassin's Creed IV: Black Flag Resynced

![Version](https://img.shields.io/badge/VERSION-2.3.7.1-76c893?style=for-the-badge)
![Build](https://img.shields.io/badge/BUILD-v8--r9i-7fb3ff?style=for-the-badge)
![License](https://img.shields.io/badge/LICENSE-NON--COMMERCIAL-e66b55?style=for-the-badge)

**Runtime:** `v8-r9i-v2.3.7.1-ubisoft-output-hotfix`

</div>

> [!IMPORTANT]
> **License:** ACBFHapticsBridge uses the **ACBFHapticsBridge Non-Commercial License v1.1**.  
> Free personal/non-commercial use, modification and free redistribution are permitted under the license terms.  
> **Selling, paywalling, or commercially redistributing the mod is not permitted without prior written permission.**

---

## 🔗 Downloads & Source

### NEXUS MODS — Main mod page / download

https://www.nexusmods.com/assassinscreedblackflagresynced/mods/412?tab=description

### GOFILE — Free mirror

https://gofile.io/d/whwCCJYe

### GITHUB — Source, release verification & project files

https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge

---

# 🎮 What this mod does

**ACBFHapticsBridge** adds **Assassin's Creed Shadows-derived haptic feedback** to **Assassin's Creed IV: Black Flag Resynced**.

The current **v2.3.7.1** runtime combines Black Flag's real Wwise gameplay events, gameplay-state callbacks, GameInput/XInput controller state, native-feedback arbitration, the game's existing **Wwise Motion / Quad Audio Haptics** path and a guarded **48 kHz / 4-channel direct DualSense fallback** for cases where the native Quad haptic sink remains dormant.

### Supported controller/output paths

| Controller / Output Path | Status |
|---|:---:|
| Native DualSense haptics — wired | ✅ |
| DualSense through DSX + DLC / Virtual DualSense with Audio | ✅ |
| Xbox 360 / XInput rumble | ✅ |
| DS4 / compatible emulated rumble paths | ✅ |
| Xbox 360 emulation through DSX | ✅ |
| Compatible GameInput controllers | ✅ |
| Steam Input with native wired DualSense | ✅ |

### DualSense

Full **48 kHz waveform haptics** through Black Flag's native Quad Audio Haptics path when available, with a guarded direct **48 kHz / 4-channel DualSense fallback** using actuator channels **3/4** when the native sink remains dormant.

### Conventional controllers

The same effect timelines are converted into low/high-motor rumble for Xbox/XInput, DS4/emulated and compatible GameInput controller paths.

The bridge keeps Black Flag's own native controller feedback whenever possible, supplements selected native effects where useful, and fills in actions where the original game provides little or no controller response.

> [!NOTE]
> The verified v2.3.7.1 bridge primarily uses **GameInput and XInput-compatible controller paths**. Generic DirectInput support should not be assumed for every device.

### DualSense Haptics vs Rumble

The **48 kHz DualSense haptic stream** and **conventional rumble** are parallel outputs.

A strong haptic effect does not automatically "overflow" into rumble.

The haptic path is limited by `PeakLimit`, while conventional rumble is generated separately when enabled.

---

# ✅ Confirmed Game Versions

| Version | Status |
|---|---|
| **Steam — Title Update 1.0.6** | ✅ v2.3.7.1 runtime confirmed |
| **Ubisoft Connect** | ✅ Executable compatibility verified |
| **Ubisoft+** | ✅ Executable compatibility verified |
| **Epic Games** | 🧪 Not yet verified |

### Steam — Title Update 1.0.6

**Confirmed `ACBlackFlag.exe` SHA-256:**

```text
8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140
```

### Ubisoft Connect

**Verified `ACBlackFlag.exe` SHA-256:**

```text
e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c
```

### Ubisoft+

**Verified `ACBlackFlag_Plus.exe` SHA-256:**

```text
b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75
```

The supplied **Ubisoft Connect** and **Ubisoft+** executables were statically verified against the bridge's executable-dependent hooks.

Both retain the required:

- Wwise `PostEvent` hook
- Quad Audio Haptics flush hook
- **21/21 gameplay-state callback targets**

v2.3.7.1 resolves **`XInputSetState` dynamically from the loaded executable's PE import table first**.

The verified executable-specific locations remain only as fail-closed, pointer-validated fallbacks.

This removes the previous dependency on one fixed XInput import-slot location across Steam, Ubisoft Connect and Ubisoft+.

### Steam v2.3.7.1 runtime validation

The Steam runtime test successfully confirmed:

- **21/21 gameplay-state hooks installed**
- **0 gameplay-state hook failures**
- dynamic `XInputSetState` named-import resolution
- DualSense **48 kHz / 4-channel** endpoint detection
- direct DualSense haptic activation
- actuator output through channels **3/4**
- normal gameplay-state/event operation after fallback activation

Runtime feedback for the Ubisoft builds is still welcome.

For an unverified executable such as Epic Games, please provide:

1. your `ACBlackFlag.exe` SHA-256
2. the generated `ACBFHapticsBridge-v8.log`
3. press **F10 while in menus**
4. press **F10 once during gameplay**
5. close the game and provide the complete log

Thanks!

---

# 📦 MOD REQUIREMENTS / INSTALLATION

A compatible **ASI loader** is required.

You can use:

- `version.dll` from **Ultimate ASI Loader**
- or the compatible `version.dll` supplied by **ACBlackFlagFix** on NexusMods

> [!IMPORTANT]
> **`version.dll` is INCLUDED in the v2.3.7.1 release.**
>
> It remains third-party software and is governed by its original author's license/redistribution terms.

## Installation

1. Download the normal **v2.3.7.1 release**, not the source-only package.

2. Place these files in the Black Flag game folder:

```text
ACBFHapticsBridge-v8-r9i.asi
ACBFHapticsBridge-v8.ini
ShadowsHapticsPack-v5.hfx
version.dll
```

3. They must be beside:

```text
ACBlackFlag.exe
```

4. Launch the game.

5. Press **F9** to quickly test whether the bridge is producing feedback.

6. Press **F10** if you need a diagnostic snapshot in the log.

---

# ☕ DONATING

Not obligated to, but appreciated nonetheless! :D

If you liked this mod and wanna buy me a coffee, show your appreciation / leave a tip, you can donate via these wallets.

Multicoin is available; **BSC Network / ETH Network is preferred** for things such as BNB, USDC, or whatever you would like.

It helps pay bills, motivates me :D, and encourages me to pursue my endeavours in modding/programming.

| Network | Address |
|---|---|
| **MetaMask / Multicoin** | `0xFfd40Cf24B1484f8567181E1c550eA494967f36f` |
| **Solana** | `DYgtA98yJ7yMf7rg8Mx65jiybeL4FyqpKGoED3VW2MuG` |
| **TRON** | `TL2oF5L8UNVLS58BmmsynndjY3nZAkTnid` |
| **Bitcoin — Native SegWit** | `bc1qjl3kv3s7l2wrmx68vhdmuqnfcj6hl6650krufx` |

PayPal profile donate is not working for my country, and I can't leave the direct link here because that would mean doxxing myself :(

---

# 😄 AAAANDDDDDDDDDD THIS IS MY FIRST MOD!!

Glad it works for me at least!! :D

Please report any bugs or weird stuff so I can work on them if I can.

---

# ✨ What the ASI Mod Does — v2.3.7.1

ACBFHapticsBridge currently adds contextual feedback for:

| Category | Feedback |
|---|---|
| **DualSense** | Full 48 kHz native Quad haptics + guarded direct 48 kHz / 4-channel fallback |
| **Conventional rumble** | Two-motor conversion for Xbox/XInput, DS4/emulated and compatible GameInput controllers |
| **Movement** | Event-synchronized walking, jogging, spatial footsteps, L3 run opening/sustain and sprint contacts |
| **Parkour** | Jump takeoff, airtime-scaled landing, horizontal-travel boost, long-fall impacts and climb catches |
| **Dodging** | Ground dodge / roll and directional dodge handling |
| **Water** | Water entry, dive/splash, surface, swim-up and L3-gated underwater lunge / fast-swim |
| **Combat** | Parry, light attacks, two-stage heavy attack, kick and native-combat supplementation |
| **Character / Equipment** | Hood action and sheathe / unsheathe feedback |
| **Menus** | Menu-open feedback, navigation ticks, reduced shoulder feedback and native menu-tick supplementation |
| **Runtime protection** | Native-feedback arbitration to reduce doubled effects |
| **Recovery** | Player/menu/state reacquisition and optimized Wwise haptic-sink handling |
| **Performance** | 500 Hz bridge input worker, cached GameInput metadata and runtime telemetry |

---

# 🆕 v2.3.7.1 / v8-r9i — Changelog

v2.3.7.1 is an **output/backend compatibility hotfix** over the tuned **v2.3.7.0 / v8-r9h** runtime.

Gameplay-event mappings, gain models, Wwise semantic events, gameplay-state behavior, timing, movement, combat, water, menu and parkour tuning remain unchanged.

## 🎮 DualSense Output Fallback

- Added a guarded **direct DualSense haptics fallback** for cases where Black Flag's native Quad/Wwise haptic flush is present but remains dormant at runtime.
- Opens a compatible **48 kHz / 4-channel DualSense audio endpoint**.
- Sends bridge haptic waveform output to actuator channels **3/4**.
- Front channels remain silent.
- Black Flag's native Quad path remains the first choice whenever active.
- Added race-safe ownership handoff between native and direct output.
- Added:

```ini
DirectDualSenseFallback=1
DirectDualSenseFallbackDelayMs=1000
```

## 🎮 XInput / Conventional-Rumble Routing

- Replaced fixed-profile-only XInput observation with **dynamic PE import-table resolution** for `XInputSetState`.
- Verified executable-specific IAT locations remain only as fail-closed fallbacks.
- Added connected XInput route probing and mirroring.
- Corrected GameInput rumble telemetry.
- A void GameInput write is now treated as **issued**, not proof that physical vibration succeeded.
- Added separate GameInput/XInput attempt, success and failure diagnostics.

## 🔎 Runtime Validation

- **Steam TU 1.0.6 runtime test: PASS**
- **21/21 gameplay-state hooks installed**
- **0 failures**
- `XInputSetState` resolved through named PE import
- Direct DualSense fallback successfully activated
- DualSense endpoint confirmed at **48 kHz / 4 channels**
- Haptic actuators confirmed on channels **3/4**
- Ubisoft Connect and Ubisoft+ retain the verified Quad/Wwise/state-hook targets

## 🎛️ Retained v2.3.7.0 Tuning

- No intentional gameplay-event remapping
- No movement retuning
- No combat retuning
- No menu retuning
- No water retuning
- No parkour retuning
- No HFX pack change
- No ASI loader change
- Existing INI tuning retained
- **`MasterGain=0.90` remains the shipped default**

> [!NOTE]
> **In short:** v2.3.7.1 keeps the complete v2.3.7.0 gameplay/haptics behavior and tuning while improving DualSense output fallback and XInput/GameInput routing for Steam, Ubisoft Connect and Ubisoft+ executable layouts.

---

# 🎛️ Retained v2.3.6 / v8-r9g Feature and Tuning Baseline

The sections below describe the movement, combat, water, menu, DualSense, XInput and performance behavior inherited unchanged by v2.3.7.1.

---

## 🚶 Spatial Movement + Footsteps

Walking and jogging are synchronized to Black Flag's real gait events rather than only a stick-driven loop.

Added true left/right spatial footsteps.

A fresh gait cycle begins on Edward's right foot and alternates right/left.

Default spatial balance is **100% on the active side with 25% crossfeed** to the opposite side.

Spatial gait applies to DualSense stereo haptics and conventional rumble conversion.

L3 running retains the tuned **1.5-second stronger opening** and sustained run texture.

Real sprint contacts layer on top of the continuous L3 run texture.

---

## 🎮 XInput / Xbox Rumble Fixes

Fixed land-running becoming continuous/stuck vibration on XInput-class controllers.

XInput land sprint now follows real gait-contact events step-by-step.

The continuous land-run motor bed is suppressed; water fast-swim/lunge can still remain continuous where appropriate.

- XInput jump impact: **300 ms maximum**
- XInput landing impact: **300 ms maximum**
- XInput light-attack vibration: **300 ms maximum per swing**
- Explicit zero-motor barrier: **16 ms**
- XInput menu L1/R1 feedback: **-50%**

---

## 🧗 Jump, Parkour + Long-Fall Impacts

Jump/landing detection remains based primarily on Black Flag's real `Jump/InAir` gameplay states.

Landing strength scales with measured airtime.

Parkour movement distance can add extra landing/catch intensity using integrated left-stick travel as a proxy.

Improved climb-to-fall carry and airborne climb-catch impacts.

Short and medium falls remain progressively scaled.

Falls of approximately **3 seconds or longer** reach the current maximum base impact of **3.15**.

This applies to:

- DualSense bridge feedback
- XInput/GameInput conventional rumble

XInput retains the short 300 ms landing duration; long falls increase amplitude rather than motor duration.

---

## 🌊 Water + Underwater Lunge Fixes

Improved automatic water-entry/reacquisition when Edward enters water without a manual jump.

Fixed underwater-lunge feedback triggering simply from movement or direction changes after being stationary.

The Wwise water-lunge event is no longer trusted by itself as proof that L3 was pressed.

A real configured **SprintLunge / L3 rising edge** must authorize the underwater lunge.

Direction-only copies of the event are ignored.

Legitimate underwater lunge / fast-swim sustain retains its **3-second safety cap**.

Surface swim-up and water exit/surface behavior remain intact.

---

## ⚔️ Combat + Native-Feedback Supplementation

Native Black Flag feedback receives first refusal to avoid doubled effects.

Verified native Quad Audio Haptics for selected Edward combat actions are supplemented by **+75% / 1.75x**.

Covered combat families include:

- sword attacks
- normal/perfect parry feedback
- pistol fire
- kicks

If suitable native feedback is absent, the bridge fallback remains available.

Cannon/naval, player-hit, movement and water haptics are not intentionally included in the combat boost.

Two-stage Heavy R1 timing remains:

```text
Hold threshold: 320 ms
Second impact: 590 ms
```

Normal parry remains latency-trimmed and native-first.

---

## 🎛️ DualSense Tuning

Bridge-generated DualSense output uses:

```ini
BridgeHapticGain=0.90
JumpLandingHapticGain=0.85
LandMovementHapticGain=1.035
```

Other retained tuning:

- DualSense menu L1/R1: **-25%**
- DualSense menu navigation tick: **+30%**
- Native Ubisoft menu Quad tick supplementation retained
- Normalized walk control `1.00` represents the tuned baseline previously represented by raw `4.80`

---

## 📋 Menu + Input Responsiveness

Exact Black Flag Wwise menu events are preferred for normal navigation.

When semantic menu routing is active, the bridge no longer continuously rescans analog-stick/D-pad navigation heuristics.

Underwater L3 correlation is evaluated on the actual L3 edge instead of continuously while moving the left stick.

Menu/category haptic sink swaps are handled as lightweight sink handoffs instead of full playback resets.

Custom L1/R1 menu tuning remains active.

---

## ⚡ Performance Cleanup — Retained from v8-r9g

Bridge-side controller/input worker target:

```text
500 Hz / 2 ms
```

This **does not overclock the controller hardware**.

A 250 Hz USB controller still provides a fresh report roughly every 4 ms; the bridge simply checks frequently enough to process it promptly.

GameInput metadata is cached instead of queried every 2 ms.

```text
Device identity refresh: 125 ms
Device status refresh:   500 ms
Rumble write interval:   4 ms / 250 Hz
```

Start/stop/zero transitions remain immediate.

Native haptic detection and native combat/menu gain processing share one buffer pass.

Normal haptic-sink changes no longer cause unnecessary full recovery resets.

F10 provides performance telemetry for:

- worker gaps
- GameInput time
- rumble-write time
- audio-hook time
- sink rebinds

F12 can suspend controller polling/raw-input processing for A/B diagnostics while semantic/state/native routing continues.

---

## 🔄 Recovery + Reliability

Improved state reacquisition after:

- reloads
- menu transitions
- long Wwise silence

Ordinary sink changes preserve valid semantic/gameplay state instead of clearing everything.

Genuine long silence/load events still trigger clean recovery.

Expanded diagnostics expose controller backend, semantic domain, state hooks, water, landing, native arbitration and output activity.

---

# ⌨️ Keyboard / ASI Hotkeys

| Key | Function |
|---|---|
| **F9** | Plays the Dodge test effect through active DualSense haptic / conventional-rumble outputs |
| **F10** | Writes a detailed runtime snapshot to `ACBFHapticsBridge-v8.log`, including v8-r9i output routing |
| **F11** | Enables/disables the complete bridge |
| **F12** | Suspends/resumes bridge controller polling and raw-input processing for diagnostics |

---

# 🎮 Default Controller Bindings

| Setting | Default | Purpose |
|---|---|---|
| `Jump` | `Cross` | Jump/climb/swim-up fallback correlation |
| `DodgeDive` | `Circle` | Water/air dive fallback |
| `Parry` | `L1` | Normal-parry fallback |
| `Attack` | `R1` | Combo/heavy-hold correlation |
| `SprintLunge` | `L3` | Land sprint + underwater-lunge authorization |
| `SheatheHood` | `DPadLeft` | Sheathe/unsheathe + Hood fallback |
| Kick | `R2 + Square` | Fallback combination |

The left stick remains important for:

- walking/running direction and speed
- movement-strength classification
- directional-dodge context
- climbing-jump direction/context
- parkour horizontal-travel estimation

Landing, haystack entry, water entry and water surface effects are primarily triggered by detected gameplay states/events rather than remappable raw buttons.

---

# 🔧 Changing Controller Inputs

Open:

```text
ACBFHapticsBridge-v8.ini
```

beside:

```text
ACBlackFlag.exe
```

Find:

```ini
[InputBindings]
Jump=Cross
DodgeDive=Circle
Parry=L1
Attack=R1
SprintLunge=L3
SheatheHood=DPadLeft
```

For example, if Jump is remapped to Triangle:

```ini
[InputBindings]
Jump=Triangle
DodgeDive=Circle
Parry=L1
Attack=R1
SprintLunge=L3
SheatheHood=DPadLeft
```

> [!IMPORTANT]
> **v2.3.7.1 prefers exact Black Flag Wwise events and gameplay-state callbacks whenever available.**
>
> `[InputBindings]` controls unresolved/raw fallbacks and state correlation. It does not replace the semantic event system.

> [!IMPORTANT]
> Restart the game after changing the INI. Settings are loaded during startup.

---

# 🕹️ Available Input Names

| PlayStation-style name | Internal/Xbox position |
|---|---|
| `Cross` | A |
| `Circle` | B |
| `Square` | X |
| `Triangle` | Y |
| `L1` | LB |
| `R1` | RB |
| `L2` | LT |
| `R2` | RT |
| `L3` | LS |
| `R3` | RS |
| `DPadUp` | D-pad Up |
| `DPadDown` | D-pad Down |
| `DPadLeft` | D-pad Left |
| `DPadRight` | D-pad Right |
| `None / Disabled / Off` | No direct raw binding |

Values are case-insensitive. Spaces, underscores and hyphens are ignored.

> [!WARNING]
> Use `Cross` for the PlayStation Cross/X button.
>
> Writing `X` means the Xbox **X position**, corresponding to PlayStation **Square**.

These cannot be used as normal gameplay action bindings:

- Touchpad
- PS / Guide
- controller paddles
- keyboard keys
- analog-stick directions
- Options / Start
- Share / Create / View

Press F10 after launching.

The log should contain something similar to:

```text
INPUTCFG jump=Triangle dodgeDive=Circle parry=L1 attack=R1 sprintLunge=L3 sheatheHood=DPadLeft invalidFallbacks=0
```

---

# 🎚️ Changing Individual Effects

Example:

```ini
[Effect.Jump]
Enabled=1
IntensityMultiplier=1.00
TriggerOffsetMs=0
StartTrimOffsetMs=0
CooldownMs=300
```

### Common Settings

```text
Enabled=1                 Effect enabled
Enabled=0                 Effect disabled
IntensityMultiplier=0.50  Half strength
IntensityMultiplier=1.00  Current calibrated default
IntensityMultiplier=1.25  25% stronger
IntensityMultiplier=2.00  Double strength
```

`IntensityMultiplier` changes both bridge-generated DualSense haptics and conventional rumble for that effect.

### Timing

```text
TriggerOffsetMs=-20      Earlier, but never before detection
TriggerOffsetMs=20       Later
StartTrimOffsetMs=20     Skip more waveform beginning
StartTrimOffsetMs=-20    Restore more original lead-in
```

These settings never change game or animation speed.

---

# 🧪 Configuration Examples

### Disable walking feedback

```ini
[Effect.Walk]
Enabled=0
```

### Reduce parry strength by 30%

```ini
[Effect.Parry]
Enabled=1
IntensityMultiplier=0.70
```

### Make ground dodge start faster

```ini
[Effect.GroundDodge]
TriggerOffsetMs=-20
StartTrimOffsetMs=25
```

### Disable second heavy-attack impact

```ini
[Effect.HeavyAttackSecond]
Enabled=0
```

### Make menu navigation softer

```ini
[Effect.MenuTick]
IntensityMultiplier=0.60
```

---

# 🧩 Special v2.3.7.1 Effect Settings

## DualSense Walk Normalization

```ini
[Effect.Walk]
DualSenseHapticNormalizedBaseline=1
DualSenseHapticMultiplier=1.00
```

```text
1.00 = established tuned baseline
0.50 = half the tuned level
2.00 = double the tuned level
4.80 = maximum normalized user scale, still subject to PeakLimit
```

---

## DualSense Jog / Run

```ini
[Effect.Run]
DualSenseJogHapticMultiplier=1.20
OpeningDurationMs=1500

[Movement]
RunBaseGain=1.80
RunOpeningGain=2.28
RunOpeningDurationMs=1500
```

---

## Spatial Footsteps

```ini
[Footsteps]
SpatialFootsteps=1
RightFootRightGain=1.00
RightFootLeftGain=0.25
LeftFootLeftGain=1.00
LeftFootRightGain=0.25
ResetAfterMs=750
SprintContactOverlayGain=0.84
```

---

## Long-Fall Scaling

```ini
[GameplayState]
LandingShortAirMs=900
LandingLongAirMs=2000
LandingMaxAirMs=3000
LandingMinimumGain=0.30
LandingShortGain=0.55
LandingGain=0.70
LandingLongGain=3.15
```

---

## XInput Short-Impact Limits

```ini
[Rumble]
LandRunStepOnly=1
SprintStepGain=1.20
XInputJumpLandingMaxDurationMs=300
XInputLightAttackMaxDurationMs=300
XInputActionZeroGapMs=16
```

These controls affect conventional XInput/Xbox-class rumble and **do not shorten DualSense 48 kHz haptic playback**.

---

## DualSense Bridge Cleanup

```ini
[DualSense]
BridgeHapticGain=0.90
JumpLandingHapticGain=0.85
LandMovementHapticGain=1.035
```

These affect bridge-generated feedback, not Ubisoft-native Quad haptics.

---

## Menu Tuning

```ini
[Menus]
Enabled=1
TickGain=0.35
OpenGain=0.28
ShoulderGain=0.315
XInputShoulderGainMultiplier=0.50
DualSenseShoulderGainMultiplier=0.75
DualSenseTickGainMultiplier=1.30
DualSenseTickNativeWindowMs=250
```

---

## Underwater L3 Gate

```ini
[SemanticEvents]
WaterLungeSustainMs=3000
WaterLungeInputGateMs=600
```

A real configured `SprintLunge/L3` rising edge must authorize the Wwise water-lunge event.

---

## Native Combat Supplement

```ini
[NativeArbitration]
Enabled=1
CombatNativeBoost=1
CombatNativeHapticGain=1.75
CombatNativeBoostWindowMs=700
```

`1.75` means selected verified Ubisoft-native combat Quad haptics are supplemented by **+75%** during the action window.

---

# 🌐 Global Settings

## Output Type

```ini
[Output]
NativeHaptics=1
ConventionalRumble=1
PreferGameInput=1
RumbleOnDualSense=1
DirectDualSenseFallback=1
DirectDualSenseFallbackDelayMs=1000
```

### `NativeHaptics=0`

Disables full DualSense waveform injection.

### `ConventionalRumble=0`

Disables ordinary low/high-motor vibration generated by the bridge.

### `RumbleOnDualSense=0`

Leaves the bridge's full DualSense haptic layer without its parallel conventional-rumble copy on detected DualSense devices.

### `DirectDualSenseFallback=1`

Allows **v8-r9i** to open a compatible DualSense **48 kHz / 4-channel** audio endpoint when Black Flag's native Quad haptic sink remains dormant.

### `DirectDualSenseFallbackDelayMs=1000`

Waits **1000 ms** for the native Quad path before allowing the direct DualSense fallback to take ownership.

Keep both `NativeHaptics` and `ConventionalRumble` enabled for the normal hybrid configuration.

---

## Overall Intensity

```ini
[Playback]
NormalizedBaseline=1
MasterGain=0.90
MovementGain=1.00
OneShotGain=1.00
PeakLimit=2.00
```

With `NormalizedBaseline=1`:

- **`MasterGain=0.90`** = supplied v2.3.7.1 master setting, equal to 90% of the normalized tuned master baseline
- **`OneShotGain=1.00`** = tuned current one-shot baseline
- `MovementGain` = continuous movement-texture multiplier
- `PeakLimit` = final haptic limiter; `0` disables limiting

---

## Conventional Rumble

```ini
[Rumble]
Gain=1.00
MovementGain=2.00
LowMotorScale=3.00
HighMotorScale=1.60
Deadzone=0.008
```

---

## Movement Thresholds

```ini
[Movement]
WalkThreshold=11000
RunThreshold=24500
SprintToggle=1
```

`SprintToggle=1` = press L3 once for run latch.

`SprintToggle=0` = hold configured SprintLunge input.

---

## Performance / Controller Polling

```ini
[General]
PollMs=2
GameInputDeviceRefreshMs=125
GameInputStatusRefreshMs=500
RumbleWriteIntervalMs=4
```

`PollMs=2` means a **500 Hz bridge-side worker target**.

It does not change the controller hardware's physical polling rate.

---

# ⚠️ Important INI Warning

Use:

```ini
[InputBindings]
```

for controller-button fallback/correlation mappings.

The lower:

```ini
[Buttons]
```

section contains waveform selections such as:

```ini
BEffect=dodge
LBEffect=parry
RBEffect=light_attack_single
XEffect=parry
```

Those **are not controller-remapping fields**.

Normally leave these at their supplied values unless you know exactly what you are changing:

```text
[Buttons]
[Hook]
advanced [GameplayState] addresses / hook-related values
verified semantic event IDs
```

---

# 🔐 Verified v2.3.7.1 Build

| Item | Value |
|---|---|
| **Nexus release** | `v2.3.7.1` |
| **Internal build** | `ACBFHapticsBridge-v8-r9i` |
| **Runtime label** | `v8-r9i-v2.3.7.1-ubisoft-output-hotfix` |
| **ASI** | `ACBFHapticsBridge-v8-r9i.asi` |

### ASI SHA-256

```text
8a68e6bcec34943f6b1afcf0ce9a063a592b36ecf2bfb3643b30d3abb7c36987
```

### Supported Executable Profiles

| Executable | Verification |
|---|---|
| Steam — Title Update 1.0.6 | ✅ Runtime confirmed |
| Ubisoft Connect | ✅ Executable compatibility verified |
| Ubisoft+ | ✅ Executable compatibility verified |

The current v2.3.7.1 release includes the **actual C++ source used to build the released ASI** under the separate `Source/` folder.

Release verification includes:

- deterministic clean build scripts
- PE / hook audit tooling
- HFX validation tooling
- semantic event map
- test checklist
- source checksums
- package checksums
- build-verification documentation
- exact release-specific license information

The final packaged source was rebuilt and verified to reproduce the shipped ASI **byte-for-byte**.

The PE audit verifies the ASI as:

- x86-64
- deterministic timestamp
- ASLR enabled
- NX enabled
- high-entropy VA
- no RWX sections
- no real static imports

---

# 📂 Source Notice — v2.3.7.1

Unlike the old v2.1 reconstruction package, the current v2.3.7.1 release contains the **actual buildable source used for the current ASI**.

```text
Source/
├─ src/
│  └─ main.cpp
├─ build_linux_pe_r9i.sh
├─ audit_v8_r9i.py
├─ validate_hfx.py
├─ ACBFHapticsBridge-v8.ini
├─ SOURCE-CHECKSUMS.sha256
├─ BUILD-VERIFICATION.md
├─ EVENT-MAP.md
├─ TEST-CHECKLIST.md
├─ R9I-IMPLEMENTATION.md
├─ UBISOFT-CROSSREFERENCE.md
├─ SOURCE-DELTA-AUDIT-v8-r9i.md
├─ V8-R9I-CHANGES.md
├─ CHANGELOG-v2.3.7.0-to-v2.3.7.1.md
├─ ACBFHapticsBridge-v2.3.7.1-NONCOMMERCIAL-LICENSE.txt
├─ LICENSE.txt
└─ release/build provenance and historical change notes
```

---

# 📜 License — Non-Commercial

> [!IMPORTANT]
> **ACBFHapticsBridge v2.3.7.1 is NOT released under the MIT License.**
>
> It is distributed under the **ACBFHapticsBridge Non-Commercial License v1.1**.

The full release-specific license is included as:

```text
ACBFHapticsBridge-v2.3.7.1-NONCOMMERCIAL-LICENSE.txt
```

## ✅ What the License Allows

- Download and use the mod personally
- Non-commercial use
- Study the source/build material
- Modify ACBFHapticsBridge material
- Create non-commercial derivatives
- Redistribute original or modified versions for free
- Fork the source for non-commercial development
- Keep the copyright/license notice with redistributed copies

## ❌ What Is Not Allowed Without Prior Written Permission

- Selling ACBFHapticsBridge
- Charging for downloads or copies
- Paywalling the mod
- Requiring Patreon/subscription/payment for access
- Bundling it inside paid mod packs or paid software
- Selling substantially identical derivatives
- Charging for activation/access/installation/use of the mod itself
- Removing copyright/license notices and presenting the project as your own
- Commercial sublicensing or exploitation of the ASI/source/tools

Changing the filename, repacking files, making small modifications, or creating a substantially identical derivative does not make the project commercially redistributable.

Commercial permission can only be granted separately by the ACBFHapticsBridge copyright holder in writing.

---

## Donations

Voluntary donations to support my own development work are fine.

The mod itself is intended to remain freely accessible.

A third-party redistributor may not make access dependent on a purchase, subscription, Patreon membership, donation, or other payment without written permission.

---

## Ubisoft / Assassin's Creed Material

The ACBFHapticsBridge Non-Commercial License covers only material for which ACBFHapticsBridge contributors hold applicable rights.

It does **not** grant ownership of or commercial rights to:

- Assassin's Creed IV: Black Flag
- Assassin's Creed Shadows
- Ubisoft trademarks
- Ubisoft game code
- artwork
- audio
- haptic design
- game assets
- Anvil / AnvilNext material
- Wwise or other third-party technology
- Ubisoft executables, DLLs or proprietary files
- unrelated third-party components

Nothing in this repository or release should be interpreted as granting permission to commercially exploit Ubisoft or other third-party intellectual property.

---

## Shadows HFX Capture Notice

The release contains:

```text
ShadowsHapticsPack-v5.hfx
```

This file contains captured and/or processed haptic waveform data derived from controller/haptic output produced by **Assassin's Creed Shadows**.

The HFX pack is not being claimed as wholly original Ubisoft-independent content and is not licensed as original ACBFHapticsBridge code under the project license.

Capturing, processing, editing, reconstructing, packaging or mapping the haptic data does not mean this license grants ownership of Ubisoft's underlying game content or haptic design.

No commercial rights to Ubisoft material are granted.

Any redistribution or use of the HFX pack remains subject to applicable copyright, platform rules, permissions and third-party terms.

---

## version.dll and Other Third-Party Files

The current v2.3.7.1 package includes:

```text
version.dll
```

That file remains governed by its original author's license and redistribution terms.

Bundling a third-party ASI loader with ACBFHapticsBridge does not place that loader under the ACBFHapticsBridge Non-Commercial License.

---

## Attribution / Modified Versions

If you redistribute a permitted non-commercial modification:

- keep the ACBFHapticsBridge copyright and license notice
- include the license
- clearly identify your version as modified
- do not present an unofficial derivative as an official ACBFHapticsBridge release
- do not imply endorsement by the original project
- do not claim ownership of Ubisoft or other third-party material

> [!WARNING]
> **Selling this mod, paywalling it, or commercially redistributing ACBFHapticsBridge material is not permitted under the project license without prior written permission.**

---

# 🔮 Future Updates

Future updates will include — hopefully — better haptics and rumble effects captured from Assassin's Creed Shadows, along with improved state/context detection as I continue working on the bridge.

This is my first mod, so if you find bugs, weird behavior, controller-specific problems, missed effects, or anything else useful, please report it.

---

<div align="center">

### Not perfect but perfectible. DEadDRop out!

</div>
