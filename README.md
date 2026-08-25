# ACBFHapticsBridge

<p align="center">
  <img src="https://img.shields.io/badge/Release-v2.3-2ea44f?style=for-the-badge" alt="Release v2.1">
  <img src="https://img.shields.io/badge/Internal-v8--r4-6f42c1?style=for-the-badge" alt="Internal v7-r7-kick-walk4x">
  <img src="https://img.shields.io/badge/DualSense-Haptics-0070d1?style=for-the-badge" alt="DualSense Haptics">
  <img src="https://img.shields.io/badge/XInput-Rumble-555555?style=for-the-badge" alt="XInput Rumble">
  <img src="https://img.shields.io/badge/License-Non--Commercial-red?style=for-the-badge" alt="Non-Commercial License">
</p>

<p align="center">
  <strong>Assassin's Creed Shadows-derived haptic feedback and conventional rumble for Assassin's Creed IV: Black Flag Resynced.</strong>
</p>

---

> [!IMPORTANT]
> **License:** ACBFHapticsBridge  uses the **ACBFHapticsBridge Non-Commercial License v1.0**.  
> Free personal/non-commercial use, modification and free redistribution are permitted under the license terms. **Selling, paywalling, or commercially redistributing the mod is not permitted without prior written permission.**

## 🔗 Nexus Mods

**NEXUS MODS:**  
https://www.nexusmods.com/assassinscreedblackflagresynced/mods/412?tab=description

---

## 🎮 What this mod does

ACBFHapticsBridge adds **Assassin's Creed Shadows-derived haptic feedback** to **Assassin's Creed IV: Black Flag Resynced**.

The mod supports:

| Controller / Output | Support |
|---|:---:|
| Native DualSense haptics — wired | ✅ |
| DualSense through DSX + DLC / Virtual DualSense with Audio | ✅ |
| Xbox 360 / XInput rumble | ✅ |
| DS4 / compatible emulated rumble paths | ✅ |
| Xbox 360 emulation through DSX | ✅ |
| Compatible GameInput controllers | ✅ |
| Steam Input with native wired DualSense | ✅ |

The bridge hooks into the game executable and uses the **native Wwise Motion / Quad Audio Haptics pipeline already present in Anvil Engine** to inject or supplement controller feedback.

Because the controller side is handled through **GameInput / XInput-compatible paths**, it can also work with compatible controller-emulation setups such as **DSX** or **DS4Windows**, depending on how the virtual controller is exposed to the game.

> [!NOTE]
> I previously described this more broadly as native DInput/XInput compatibility. The verified v2.1 bridge itself primarily uses **GameInput and XInput paths**, so generic DirectInput support should not be assumed for every device.

---

## ✅ Confirmed game version

**Confirmed compatible:**

| Store / Version | Status |
|---|:---:|
| Steam — Title Update 1.0.6 | ✅ Confirmed |
| Ubisoft Connect | 🧪 Needs executable/hash/log testing |
| Epic Games | 🧪 Needs executable/hash/log testing |

Support for Ubisoft Connect / Epic versions can be added once I have the required executable information.

If you use the **Ubisoft Connect or Epic version**, please provide:

- your `ACBlackFlag.exe` **SHA-256 hash**
- the generated `ACBFHapticsBridge` log
- press **F10 while in menus**
- press **F10 once again during gameplay**
- then close the game and provide the log

Thanks!

---

# 📦 MOD REQUIREMENTS / INSTALLATION

A compatible ASI loader is required.

You can use:

- `version.dll` from **Ultimate ASI Loader**
- or the compatible `version.dll` supplied by **ACBlackFlagFix** on NexusMods

> [!IMPORTANT]
> `version.dll` is **not included in the v2.1 release for now**.

### Installation

1. Download the normal **v2.1 release**, not the source package.
2. Place:
   - `ACBFHapticsBridge-v7.asi`
   - `ACBFHapticsBridge-v7.ini`
   - the required `.hfx` haptics pack
   - `version.dll`
3. Put all of them in the same folder as:
   - `ACBlackFlag.exe`
4. Launch the game.
5. Press **F9** if you want to quickly test whether the bridge is producing feedback.

---

# ☕ DONATING

> **Not obligated to, but appreciated nonetheless! :D**

If you liked this mod and wanna buy me a coffee, show your appreciation / leave a tip, you can donate via these wallets.

Multicoin is available; **BSC Network / ETH Network** is preferred for things such as BNB, USDC, or whatever you would like.

**Not obligated to — it just helps pay bills, motivates me :D, and encourages me to pursue my endeavours.**

| Network | Address |
|---|---|
| **MetaMask / Multicoin** | `0xFfd40Cf24B1484f8567181E1c550eA494967f36f` |
| **Solana** | `DYgtA98yJ7yMf7rg8Mx65jiybeL4FyqpKGoED3VW2MuG` |
| **TRON** | `TL2oF5L8UNVLS58BmmsynndjY3nZAkTnid` |
| **Bitcoin — Native SegWit** | `bc1qjl3kv3s7l2wrmx68vhdmuqnfcj6hl6650krufx` |

> PayPal profile donate is not working for my country, and I can't leave the direct link here because that would mean doxxing myself :(

---

# 😄 AAAANDDDDDDDDDD THIS IS MY FIRST MOD!!

Glad it works for me at least!! :D

Please report any bugs or weird stuff so I can work on them if I can.

---

# ✨ What the ASI mod does — v2.3

ACBFHapticsBridge currently adds contextual feedback for:

| Area | Effects |
|---|---|
| **DualSense** | Full 48 kHz stereo haptics through Black Flag's native Wwise haptic path |
| **Conventional rumble** | Two-motor rumble conversion for Xbox/XInput, DS4/emulated and compatible GameInput controllers |
| **Movement** | Walking, running, sprint-start impact |
| **Parkour** | Jump takeoff, short/normal/long landing, haystack landing, climbing-jump correlation |
| **Dodging** | Ground dodge, directional dodge |
| **Water** | Dive/water entry, surface, swim-up, water lunge / fast-swim start |
| **Combat** | Parry, light attack, two-stage heavy attack, R2 + Square kick |
| **Character / equipment** | Hood action, sheathe / unsheathe |
| **Menus** | Menu-open feedback, navigation ticks, tab/shoulder feedback |
| **Runtime protection** | Native-feedback arbitration to reduce doubled effects |
| **Recovery** | Menu/player-state recovery and Wwise haptic-sink rebind after reloads |

The bridge keeps **Black Flag's own native controller feedback whenever possible** and fills in actions where the original game provides little or no controller response.

---

🆕 v2.3 / v8-r4 Changelog

Compared with:

v2.1 / v7-r7-kick-walk4x

v8-r4 is a major gameplay-feedback overhaul. The bridge now combines Black Flag's real Wwise gameplay events with gameplay-state callbacks, improving timing and context detection while reducing false or duplicated effects.

🧠 Hybrid Wwise + Gameplay-State Detection

exact Wwise PostEvent gameplay-event router

retains 21 gameplay-state hooks

new CHARACTER, WATER, HELM and MENU runtime contexts

real events identify movement, dodge, parry, attacks, kick, sheathe/unsheathe, hood actions, swimming, water lunge, helm/naval activity and menus

This reduces incorrect feedback from actions sharing the same input.

🧗 Jump + Landing Rebuilt

Jumping now uses Black Flag's actual Jump / InAir states as the authoritative source.

Improved detection covers standing, forward, running and parkour/targeted jumps. Landing strength scales from measured airtime for short, normal and long landings, with better protection against stale or false airborne states.

⚔️ Combat Improvements

Light attacks: better R1 combo coverage using real events plus guarded fallback taps for unmapped later swings

Heavy R1: retimed two-phase effect with a stronger first impact and much earlier second impact

Parry: fallback latency reduced from 60 ms → 24 ms; perfect parries remain native

Dodge: can follow Black Flag's real dodge event instead of only Circle/B

Kick: prefers the real kick event instead of only R2 + Square

Sheathe/unsheathe: dedicated event detection; strong second phase roughly 30% earlier

Hood ON/OFF: recognized separately

Ubisoft's own combat feedback still receives priority when available.

🚶 Movement Overhaul

Walking and jogging now use Black Flag's real gait events for tighter synchronization and less feedback after Edward stops.

L3 running is now continuous:

first 1.5 s: 1.90x opening response

sustained run: 1.50x

roughly 50% stronger than normal jogging

releasing movement stops it immediately

The old repeated ~300 ms XInput run pulses are disabled by default. Conventional rumble follows the same continuous run envelope.

🌊 Water / Underwater Improvements

Water now has its own runtime context.

surface L3 fast-swim can sustain continuous feedback

underwater lunge adds an immediate impact followed by fast-swim feedback

underwater sustain has a hard 3-second maximum per lunge

surfacing, leaving swimming or entering shallow water clears the state

This prevents stuck underwater/sprint feedback.

⛵ Helm / Naval Detection

HELM is now a real gameplay context instead of a sticky inferred flag.

Confirmed Edward activity—jumping, swimming, gait events, character actions, menus or recovery—can automatically return the bridge to CHARACTER mode.

Native naval feedback is left to Ubisoft where appropriate.

🛡️ Better Native-Feedback Arbitration

The bridge better preserves Black Flag's own feedback and suppresses unnecessary fallbacks when suitable native feedback already exists, including:

cannon / broadside Motion

player damage

pistol fire

perfect parry

native menu feedback

helm entry

This reduces doubled or overly strong effects.

📋 Menu + Recovery Improvements

Menu handling follows real menu events more closely.

Recovery now distinguishes between a normal Wwise sink rebound, which preserves gameplay context, and a genuine load/desync, which resets and reacquires CHARACTER, WATER, HELM or MENU.

F11 re-enable also performs a clean state reacquisition.

⚡ Performance + Timing

effect queue increased from 64 → 256

waveforms shortened/trimmed for faster response

improved timing for dodge, parry, attacks, jump/landing, kick, water and sheathe effects

long unused waveform tails are no longer kept unnecessarily

🎮 Output + Source Improvements

DualSense keeps full 48 kHz stereo haptics through Black Flag's native Wwise Quad Audio Haptics path.

Conventional rumble remains supported for Xbox/XInput, DS4/emulated and compatible GameInput controllers.

Unlike v2.1's reconstructed r7 patch material, v8-r4 includes the exact C++ source used to compile the released ASI, plus build verification, hook/PE auditing, HFX validation, event mapping and checksums.

⭐ In Short

v8-r4 moves ACBFHapticsBridge much closer to Black Flag's real gameplay: real Wwise event detection, hybrid gameplay-state routing, better jump/landing accuracy, improved R1 combos, faster combat feedback, event-synced movement, continuous L3 running, expanded swimming, smarter helm/menu handling, fewer duplicate effects, improved recovery and exact buildable C++ source.

---

# ⚙️ Per-Effect Configuration

Automatic effects remain independently configurable where applicable.

Users can adjust:

- enable / disable state
- intensity
- trigger timing
- waveform start trim
- cooldowns
- movement loop timing

This allows the bridge to be tuned **without recompiling the ASI**.

---

# 🎛️ R7 Effect Summary

## Movement

- Walking
- Running
- Sprint start

## Parkour

- Jump
- Short landing
- Normal landing
- Long landing
- Haystack landing
- Ground dodge
- Directional dodge

## Water

- Dive / water entry
- Surface
- Swim up
- Water lunge / fast-swim start

## Combat

- Parry
- Light attack
- Heavy attack — first impact
- Heavy attack — second impact
- Kick

## Character / Equipment

- Weapon sheathe / unsheathe
- Shared hood / action transition

## Menus

- Menu open
- Menu navigation ticks
- Menu / tab interaction

---

# ⌨️ Keyboard / ASI Hotkeys

| Key | ASI function |
|---|---|
| **F9** | Plays the Dodge test effect through DualSense haptics and conventional rumble. Useful to know if the mod works or not. |
| **F10** | Writes diagnostic information to `ACBFHapticsBridge-v7.log`, including controller backend, game state, effects, rumble activity and movement tuning. I may require this log for bugfixes. |
| **F11** | Enables / disables the entire mod. Disabling immediately stops all ASI-added haptics and rumble. |
| **F12** | Enables / disables only automatic gameplay and menu effects. The ASI stays loaded, native game feedback remains active, and F9 still works. Useful for comparisons and ship gameplay — basically disables mod-added haptics/rumble while keeping the ASI loaded for testing / Jackdaw steering. A future update may automate this with a reliable ship-state event. |

---

# 🎮 Default Controller Bindings

| INI action | Default input | What it controls |
|---|---|---|
| `Jump` | `Cross` | Ground jump, directional climbing jump, swim-up and jump/landing state correlation |
| `DodgeDive` | `Circle` | Ground dodge, directional roll, airborne dive request and water dive |
| `Parry` | `L1` | Parry fallback when the game produces no native effect |
| `Attack` | `R1` | Tap/release light attack and held two-stage heavy attack |
| `SprintLunge` | `L3` | Sprint latch, sprint-start kick and water lunge / fast swimming |
| `SheatheHood` | `DPadLeft` | Hood toggle and sheathe / unsheathe feedback |
| **Kick** | **R2 + Square** | r7 binary-gated gameplay kick |

The **left stick** remains responsible for:

- walking and running strength
- directional-dodge detection
- climbing-jump direction
- menu navigation

Landing, haystack entry, water entry and water surface effects are primarily triggered by detected character states rather than a remappable button.

---

# 🔧 Changing Controller Inputs

Open:

`ACBFHapticsBridge-v7.ini`

beside:

`ACBlackFlag.exe`

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

Change the value after `=` to match the corresponding in-game controller assignment.

For example, if you change Jump to Triangle in Black Flag:

```ini
[InputBindings]
Jump=Triangle
DodgeDive=Circle
Parry=L1
Attack=R1
SprintLunge=L3
SheatheHood=DPadLeft
```

This moves all relevant Jump processing to Triangle, including:

- immediate takeoff feedback
- jump-state correlation
- climbing-jump detection
- swim-up feedback
- guarded landing cycle

Cross will no longer trigger provisional jump feedback.

> [!IMPORTANT]
> Restart the game after changing the INI. Settings are loaded during startup.

---

# 🕹️ Available Input Names

| PlayStation value | Xbox alias | Physical position |
|---|---|---|
| `Cross` | `A` | Bottom face button |
| `Circle` | `B` | Right face button |
| `Square` | `X` | Left face button |
| `Triangle` | `Y` | Top face button |
| `L1` | `LB` | Left shoulder |
| `R1` | `RB` | Right shoulder |
| `L2` | `LT` | Left trigger |
| `R2` | `RT` | Right trigger |
| `L3` | `LS` | Left-stick click |
| `R3` | `RS` | Right-stick click |
| `DPadUp` | Same | D-pad Up |
| `DPadDown` | Same | D-pad Down |
| `DPadLeft` | Same | D-pad Left |
| `DPadRight` | Same | D-pad Right |
| `None` / `Disabled` / `Off` | — | No direct input |

Values are case-insensitive. Spaces, underscores and hyphens are ignored.

> [!WARNING]
> Use `Cross` for the PlayStation cross/X button. Writing `X` means the **Xbox X position**, which corresponds to **PlayStation Square**.

The following cannot be used as normal gameplay action bindings:

- Touchpad
- PS / Guide button
- controller paddles
- keyboard keys
- analog-stick directions
- Options / Start
- Share / Create / View

Menu controls are handled separately.

Do not give multiple action entries the same physical button unless you intentionally want both actions to trigger.

The mod does not automatically resolve duplicate assignments.

Press **F10** after launching. The log should contain something similar to:

```text
INPUTCFG jump=Triangle dodgeDive=Circle parry=L1 attack=R1 sprintLunge=L3 sheatheHood=DPadLeft invalidFallbacks=0
```

If a name is invalid, that action falls back to its tested default and `invalidFallbacks` increases.

---

# 🎚️ Changing Individual Effects

Every automatic effect has its own section where applicable.

Example:

```ini
[Effect.Jump]
Enabled=1
IntensityMultiplier=1.00
TriggerOffsetMs=0
StartTrimOffsetMs=0
CooldownMs=300
```

## Common settings

| Setting | Meaning |
|---|---|
| `Enabled=1` | Effect enabled |
| `Enabled=0` | Effect disabled |
| `IntensityMultiplier=0.50` | Half strength |
| `IntensityMultiplier=1.00` | Default strength |
| `IntensityMultiplier=1.25` | 25% stronger |
| `IntensityMultiplier=2.00` | Double strength |
| `TriggerOffsetMs` | Adds or removes delay after the input/state is detected |
| `StartTrimOffsetMs` | Changes where playback begins inside the captured waveform |

`IntensityMultiplier` changes **both DualSense haptics and conventional rumble**.

### Timing

| Example | Effect |
|---|---|
| `TriggerOffsetMs=-20` | Makes the effect earlier, but never before the input/state was detected |
| `TriggerOffsetMs=20` | Makes the effect later |
| `StartTrimOffsetMs=20` | Skips more of the waveform beginning, producing a faster impact |
| `StartTrimOffsetMs=-20` | Restores more of the original lead-in |

These settings **never change game or animation speed**.

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

### Make the ground-dodge impact start faster

```ini
[Effect.GroundDodge]
TriggerOffsetMs=-20
StartTrimOffsetMs=25
```

### Disable only the second heavy-attack impact

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

# 🧩 Special Effect Settings

Some sections include additional controls:

| Section | Additional controls |
|---|---|
| `[Effect.Walk]` | `LoopStartMs`, `LoopEndMs`, `FadeMs`, `DualSenseHapticMultiplier` |
| `[Effect.Run]` | Loop points, fade, `OpeningDurationMs` |
| `[Effect.Jump]` | `CooldownMs` |
| Landing sections | Separate short / normal / long intensity and timing |
| `[Effect.WaterDive]` | Input and game-state cooldowns |
| `[Effect.MenuTick]` | Direction-repeat delay and interval |
| `[Effect.HeavyAttackFirst]` | `HoldThresholdMs` |
| `[Effect.HeavyAttackSecond]` | `DelayAfterFirstMs` |

Example heavy-attack timing:

```ini
[Effect.HeavyAttackFirst]
HoldThresholdMs=320

[Effect.HeavyAttackSecond]
DelayAfterFirstMs=640
```

---

# 🌐 Global Settings

## Output type

```ini
[Output]
NativeHaptics=1
ConventionalRumble=1
PreferGameInput=1
RumbleOnDualSense=1
```

| Setting | Function |
|---|---|
| `NativeHaptics=0` | Disables full DualSense waveform injection |
| `ConventionalRumble=0` | Disables ordinary low/high-motor vibration |
| `RumbleOnDualSense=0` | Leaves only the full native DualSense haptic layer on detected DualSense devices |

Keep both native haptics and conventional rumble enabled for the normal hybrid configuration.

---

## Overall intensity

```ini
[Playback]
MasterGain=1.00
MovementGain=1.00
OneShotGain=1.00
```

| Setting | Function |
|---|---|
| `MasterGain` | Changes everything |
| `MovementGain` | Changes walk/run textures |
| `OneShotGain` | Changes jump, landing, dodge, attack, water and menu impacts |

---

## Conventional rumble

```ini
[Rumble]
Gain=1.00
MovementGain=2.00
LowMotorScale=3.00
HighMotorScale=1.60
Deadzone=0.008
```

These only control conversion of the Shadows waveforms into conventional controller motors.

---

## Movement

```ini
[Movement]
WalkThreshold=11000
RunThreshold=24500
SprintToggle=1
```

Lower thresholds make walking/running activate with less stick movement.

| Setting | Meaning |
|---|---|
| `SprintToggle=1` | Press SprintLunge once |
| `SprintToggle=0` | Hold the configured SprintLunge input |

---

## Menus

The verified v2.1 INI uses:

```ini
[Menus]
Enabled=1
TickGain=0.35
OpenGain=0.28
ShoulderGain=0.315
```

`Enabled=0` disables ASI-added map/inventory/menu feedback without disabling gameplay effects.

---

# ⚠️ Important INI Warning

Use:

```ini
[InputBindings]
```

for controller buttons.

The lower:

```ini
[Buttons]
```

section contains **advanced waveform names**, for example:

```ini
BEffect=dodge
LBEffect=parry
RBEffect=light_attack_single
XEffect=parry
```

Those are **not controller-remapping fields**.

Changing them selects which captured HFX waveform an action uses.

Normally, leave:

- `[Buttons]`
- `[Hook]`
- advanced `[GameplayState]` addresses

at their supplied values.

---

# 🔐 Verified v2.1 Build

| Property | Value |
|---|---|
| Nexus release | `v2.1` |
| Internal build | `ACBFHapticsBridge-v7-r7-kick-walk4x-test` |
| Runtime label | `v7-r7-kick-walk4x` |
| ASI | `ACBFHapticsBridge-v7.asi` |
| SHA-256 | `fb77b55728185e453fcc2046716f1d299bb78e92aededae906c9c56db58383b9` |
| Confirmed executable target | Steam TU 1.0.6 |

The source-review folder contains the reconstruction, exact r7 injected assembly stub, matching configuration and binary verification tooling.

---

# 📂 Source / Reconstruction Notice

> [!CAUTION]
> The r7/v2.1 source package does **not** falsely claim to be the lost original full C++ / Visual Studio project for the complete ASI.

R7 was produced as a narrow binary patch over the compiled r6 runtime.

The repository therefore provides:

- source-level reconstruction of the r7-specific behavior
- exact reconstructed 28-byte R2 + Square injected stub
- matching v2.1 INI
- binary-patch notes
- verification tooling
- verified v2.1 ASI reference binary

---

# 📁 Source v2.1 Repository Layout

```text
Source v2.1/
├─ Source/
│  ├─ RECONSTRUCTED-R7.cpp
│  ├─ r7_kick_stub.S
│  ├─ Reconstruction/
│  │  ├─ IMPLEMENTATION.md
│  │  └─ R7-BINARY-PATCH.md
│  └─ Tools/
│     └─ audit_r7.py
├─ Release/
│  ├─ ACBFHapticsBridge-v7.asi
│  ├─ ACBFHapticsBridge-v7.ini
│  └─ EXPECTED-V2.1-ASI-SHA256.txt
├─ docs/
│  ├─ NEXUS-REVIEW.md
│  └─ PRIVACY-REVIEW.md
├─ CHANGELOG.md
├─ LICENSE
├─ README.md
├─ SHA256SUMS.txt
└─ verify-v2.1.ps1
```

---

# 📜 License — Non-Commercial

> [!IMPORTANT]
> **ACBFHapticsBridge v2.1 is NOT released under the MIT License.**  
> It is distributed under the **ACBFHapticsBridge Non-Commercial License v1.0**.

The full license is included with the **source**, the **release files**, and **inside the downloadable release archive**.  
See the included `LICENSE` / `ACBFHapticsBridge-v2.1-NONCOMMERCIAL-LICENSE.txt` for the complete terms.

## What the license allows

| Use | Allowed? |
|---|:---:|
| Download and use the mod personally | ✅ |
| Use the mod for non-commercial purposes | ✅ |
| Study the source / reconstruction material | ✅ |
| Modify the ACBFHapticsBridge material | ✅ |
| Make a non-commercial derivative | ✅ |
| Redistribute the original or a modified version **for free** | ✅ |
| Fork the source for non-commercial development | ✅ |
| Keep reasonable attribution and the license with redistributed copies | **Required** |

## What is NOT allowed without my prior written permission

| Commercial use | Allowed? |
|---|:---:|
| Sell ACBFHapticsBridge | ❌ |
| Charge for a download or copy | ❌ |
| Put the mod behind a paywall | ❌ |
| Require Patreon/subscription/payment to download the mod | ❌ |
| Bundle it inside a paid mod pack or paid software package | ❌ |
| Sell a modified or substantially identical derivative | ❌ |
| Charge for activation, access, installation, or use of the mod itself | ❌ |
| Remove the copyright/license notice and present the project as your own | ❌ |
| Commercially sublicense or exploit the ASI/source/reconstruction/tools | ❌ |

Changing the filename, repacking the files, making small modifications, or creating a substantially identical derivative **does not turn it into something that can be sold**.

Commercial permission can only be granted separately by the ACBFHapticsBridge copyright holder in writing.

### Donations

Voluntary donations to support **my own development work** are fine.

The mod itself is intended to remain freely accessible. A third-party redistributor may not make access to ACBFHapticsBridge dependent on a purchase, subscription, Patreon membership, donation, or other payment without written permission.

---

## Ubisoft / Assassin's Creed material

The ACBFHapticsBridge Non-Commercial License only covers material for which the ACBFHapticsBridge contributors hold the applicable rights.

It does **not** grant ownership of or commercial rights to:

- **Assassin's Creed IV: Black Flag**
- **Assassin's Creed Shadows**
- Ubisoft trademarks, game code, artwork, audio, haptic design, assets, or other intellectual property
- Anvil / AnvilNext material
- Wwise or other third-party technology
- Ubisoft executables, DLLs, data files, or proprietary game files
- unrelated third-party components

Nothing in this repository or release should be interpreted as granting permission to commercially exploit Ubisoft or other third-party intellectual property.

---

## Shadows HFX capture notice

The release may contain:

```text
ShadowsHapticsPack-v5.hfx
```

This file contains **captured and/or processed haptic waveform data derived from controller/haptic output produced by Assassin's Creed Shadows**.

The HFX pack is **not being claimed as wholly original Ubisoft-independent content**, and it is **not licensed as original ACBFHapticsBridge code** under the project license.

The fact that I captured, processed, edited, reconstructed, packaged, or mapped the haptic data does **not** mean this license grants ownership of Ubisoft's underlying game content or haptic design.

No commercial rights to Ubisoft material are granted by this project.

Any redistribution or use of the HFX pack remains subject to applicable copyright, platform rules, permissions, and third-party terms.

---

## `version.dll` and other third-party files

If a future release includes an ASI loader such as:

```text
version.dll
```

that file remains governed by **its original author's own license and redistribution terms**.

Bundling a third-party loader with ACBFHapticsBridge does not place that loader under the ACBFHapticsBridge Non-Commercial License.

---

## Attribution / modified versions

If you redistribute a permitted non-commercial modification:

- keep the ACBFHapticsBridge copyright and license notice;
- include the license with the files;
- clearly state that your version is modified;
- do not present an unofficial derivative as an official ACBFHapticsBridge release;
- do not imply endorsement by the original project;
- do not claim ownership of Ubisoft or other third-party material.

> [!WARNING]
> **Selling this mod, paywalling it, or commercially redistributing ACBFHapticsBridge material is not permitted under the project license without prior written permission.**

---

# 🔮 Future Updates

Future updates will include — hopefully — better haptics and rumble effects captured from **Assassin's Creed Shadows**, along with improved state/context detection as I continue working on the bridge.

This is my first mod, so if you find bugs, weird behavior, controller-specific problems, missed effects, or anything else useful, please report it.

---

<p align="center">
  <strong>Not perfect but perfectible. DEadDRop out!</strong>
</p>
