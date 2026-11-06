<h1 align="center">🎮 ACBFHapticsBridge</h1>

<p align="center">
  <strong>DualSense haptics and conventional rumble</strong><br>
  for Assassin's Creed IV: Black Flag Resynced
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Version-2.3.7.9-76C893?style=for-the-badge" alt="Version 2.3.7.9">
  <img src="https://img.shields.io/badge/Title_Update-1.08-7FB3FF?style=for-the-badge" alt="Title Update 1.08 profile">
  <img src="https://img.shields.io/badge/DualSense-Haptics-0070D1?style=for-the-badge" alt="DualSense haptics">
  <img src="https://img.shields.io/badge/Controller-Rumble-8A63D2?style=for-the-badge" alt="Controller rumble">
  <img src="https://img.shields.io/badge/License-Non--Commercial-E66B55?style=for-the-badge" alt="Non-Commercial License">
</p>

<p align="center">
  By <strong>DEadDRop</strong><br>
 
</p>

<p align="center">
  <a href="https://www.nexusmods.com/assassinscreedblackflagresynced/mods/412?tab=files"><img src="https://img.shields.io/badge/Nexus_Mods-Downloads-E6832B?style=for-the-badge" alt="Nexus Mods downloads"></a>
  <a href="https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/releases"><img src="https://img.shields.io/badge/GitHub-Releases-181717?style=for-the-badge&amp;logo=github&amp;logoColor=white" alt="GitHub releases"></a>
</p>

[Features](#what-the-mod-does) · [Installation](#requirements-and-installation) · [Settings](#strength-and-output-settings) · [Compatibility](#automatic-compatibility-settings) · [Donations](#donations) · [Licence](#licence-and-attribution)

---

## What the mod does

ACBFHapticsBridge brings **Assassin's Creed Shadows-derived haptic waveforms** into Black Flag Resynced, using the game's native Wwise Quad Audio Haptics path for compatible DualSense output and converting the same effect timelines into conventional motor rumble.

The bridge follows game events, gameplay states and controller input to provide contextual feedback while preserving the game's native feedback where possible.

| Category | Feedback |
| --- | --- |
| Movement | Walking, jogging, alternating left/right footsteps, sprint opening and sustained running feedback. |
| Parkour | Jump takeoff, landing strength based on airtime and travel, long-fall impacts and climbing catches. |
| Dodging | Ground rolls and directional dodges. |
| Water | Entry, diving, surfacing, swimming upward and L3-gated underwater lunge feedback. |
| Combat | Parry, light attacks, two-stage heavy attacks, kicks and selected native-combat supplementation. |
| Equipment | Hood actions and weapon sheathing/unsheathing. |
| Menus | Opening feedback, navigation ticks and shoulder-button feedback. |
| Output control | Configurable strength, per-effect timing, native-feedback arbitration and context recovery. |

---

## Controllers and output

| Controller / setup | Output |
| --- | --- |
| DualSense over USB | 48 kHz stereo haptic waveform injection through the game's native haptic path, with optional conventional rumble alongside it. |
| DSX Virtual DualSense with Audio | Compatible virtual-audio routing, including wireless forwarding when supported and configured in DSX. |
| Xbox/XInput | Conventional low/high-motor rumble, including compatible Xbox 360 emulation. |
| DualShock 4 | Dedicated USB/Bluetooth HID input and rumble transport, plus compatible emulated-controller paths. |
| GameInput | Compatible controller input and motor-output routes. |

Haptic waveforms and conventional rumble are separate, parallel outputs. Their availability depends on the controller, connection and active input/emulation setup. Generic DirectInput support is not guaranteed for every device.

DS4 USB/Bluetooth transport has passed protocol and output-fixture tests; physical in-game validation remains pending.

---

## Game compatibility and EXE hashes

The ASI checks executable-specific profiles and can search for compatible targets automatically. These SHA-256 hashes identify the executable profiles/reference files used by the project.

<details>
<summary><strong>Show title updates and executable SHA-256 hashes</strong></summary>

**Title Update 1.08 — ACBlackFlag.exe**

```text
6f89418be0de287c7dd29893326ebfb817c3472c0106ece8a6e8f81a915c3a55
```

**Steam — Title Update 1.07**

```text
614dab4a20a5d5c6256792e1daa6d05669c97a751079b10df1725d6965ad766d
```

**Steam — Title Update 1.06**

```text
8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140
```

**Title Update 1.06-compatible executable — additional hash**

```text
19920f34bb2fac814023ee3c27f0ceca1872e80a2aafb37c092c419fff77cc0d
```

**Ubisoft Connect — ACBlackFlag.exe**

```text
e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c
```

**Ubisoft+ — ACBlackFlag_Plus.exe**

```text
b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75
```

</details>

The supplied TU 1.08 executable passes fixed-profile and automatic-discovery tests for both audio targets and all **21 gameplay-state hooks**. Physical controller output on TU 1.08 still requires an in-game test. The title-update label follows the supplied file's identification.

Epic Games and other unverified EXEs may work through automatic discovery when all required targets pass validation; support is not guaranteed solely because scanning is enabled.

---

## Requirements and installation

You need **Black Flag Resynced**, a compatible controller and a working **64-bit ASI loader**. Keep the compatible loader you already use, or install one separately.

Download the runtime release from the Nexus Mods Files tab or the GitHub Releases page. The ZIP contains exactly these four files:

```text
ACBFHapticsBridge-v8-r9q.asi
ACBFHapticsBridge-v8.ini
ShadowsHapticsPack-v5.hfx
LICENSE.txt
```

1. Close the game.
2. Remove any other HapticsBridge ASI from the game folder. Keep only one bridge ASI installed.
3. Extract the four release files beside `ACBlackFlag.exe`.
4. Keep or install a compatible x64 ASI loader separately. The runtime ZIP does not include a loader.
5. Start the game and reach gameplay. Press **F9** to preview the Dodge effect.

**Keeping your settings:** retain your customized INI and existing HFX pack when replacing the ASI. Missing compatibility keys automatically use the defaults shown below.

> **Restart required:** INI changes are loaded when the game starts.

**Uninstalling:** close the game and remove the bridge ASI. Remove its INI/HFX files if no longer needed. Keep loader files used by other mods.

---

## Hotkeys

| Key | Function |
| --- | --- |
| <kbd>F9</kbd> | Play the Dodge preview through the active output paths. |
| <kbd>F10</kbd> | Write a detailed runtime diagnostic snapshot to the log. |
| <kbd>F11</kbd> | Enable or disable the complete bridge and its added playback. |
| <kbd>F12</kbd> | Suspend or resume controller-input inference for diagnostics. Semantic events, gameplay-state hooks and native routing remain active; this is not the complete bridge-off control. |

---

## Controller bindings

| Action | Default binding |
| --- | --- |
| Jump | Cross / A |
| Dodge or dive | Circle / B |
| Parry | L1 / LB |
| Attack | R1 / RB |
| Sprint or underwater lunge | L3 / left-stick click |
| Sheathe or hood action | D-pad Left |
| Kick fallback | R2 + Square / RT + X |

Many effects follow actual game events and states. If you remap the game's controls, update the matching `[InputBindings]` entries for the remaining input fallbacks and restart the game.

---

## Strength and output settings

Open `ACBFHapticsBridge-v8.ini`. These are the main shipped strength settings:

```ini
[Playback]
MasterGain=0.90
MovementGain=1.00
OneShotGain=1.00
PeakLimit=2.00
```

| Setting | Purpose |
| --- | --- |
| MasterGain | Overall bridge feedback strength. |
| MovementGain | Movement textures such as walking, running and swimming. |
| OneShotGain | Short effects such as jumps, landings, combat and menus. |
| PeakLimit | Limits the haptic waveform's output peaks. |

**Output controls:**

```ini
[Output]
NativeHaptics=1
ConventionalRumble=1
PreferGameInput=1
RumbleOnDualSense=1
```

**NativeHaptics** controls bridge waveform injection. **ConventionalRumble** controls motor feedback. **RumbleOnDualSense** allows the parallel motor-rumble layer on DualSense.

The guarded direct DualSense fallback applies only to eligible Ubisoft Connect/Ubisoft+ profiles when required native haptic data are absent. Automatic detection does not enable that fallback for every EXE.

**Per-effect tuning:** each relevant `[Effect.*]` section can enable/disable that bridge effect, adjust its intensity and change playback timing.

Example — reduce the bridge's parry effect to 70% of its calibrated strength:

```ini
[Effect.Parry]
Enabled=1
IntensityMultiplier=0.70
```

**IntensityMultiplier=1.00** keeps that effect's calibrated strength. **Enabled=0** disables that bridge effect. These controls do not disable the game's own native feedback.

**TriggerOffsetMs** adjusts effect scheduling. **StartTrimOffsetMs** changes where playback begins within the waveform. The INI comments explain the supported values and additional movement, landing, menu and DualSense controls.

---

## Automatic compatibility settings

```ini
[Compatibility]
AutoDetectTargets=1
ForceAutoScan=0
```

| Setting | Meaning |
| --- | --- |
| AutoDetectTargets=1 | Use validated automatic discovery when no executable profile passes. |
| AutoDetectTargets=0 | Use only profiles that pass their checks. |
| ForceAutoScan=0 | Normal operation; check known profiles first. |
| ForceAutoScan=1 | Bypass profile addresses and test the complete discovery path, including on a recognized EXE. Validation is still required. |

Discovery runs at startup. It searches for the required audio functions and gameplay-state tables and checks them before installing the bridge's engine hooks. Incomplete, incompatible or ambiguous matches leave those hooks inactive and record the reason.

This helps with supported address changes after game updates. Changes to engine interfaces, code patterns or event content can still require a mod update.

---

## Testing and troubleshooting

- Check that the ASI, INI and HFX are beside the game EXE and that your ASI loader is working.
- Keep only one HapticsBridge ASI installed.
- Use F9 to check the preview, then test movement, jumping/landing, water, combat and menus.
- Use F11 for a complete bridge on/off comparison. F12 only suspends input inference.
- If feedback is absent, provide `ACBFHapticsBridge-v8.log`, your EXE's SHA-256, controller model, connection type and active emulation/input setup. F10 records a runtime snapshot when the bridge is active.

The bridge uses a **2 ms / 500 Hz worker target** with the shipped PollMs setting. This does not overclock the controller or change its physical report rate.

---

## Donations

**Not obligated to, but appreciated nonetheless! :D**

If you enjoy the mod and would like to buy me a coffee or support my development work, these are my donation wallets. Donations are voluntary; access to the mod remains free.

**MetaMask / Multicoin — Ethereum or BSC:**

```text
0xFfd40Cf24B1484f8567181E1c550eA494967f36f
```

**Solana:**

```text
DYgtA98yJ7yMf7rg8Mx65jiybeL4FyqpKGoED3VW2MuG
```

**TRON:**

```text
TL2oF5L8UNVLS58BmmsynndjY3nZAkTnid
```

**Bitcoin — Native SegWit:**

```text
bc1qjl3kv3s7l2wrmx68vhdmuqnfcj6hl6650krufx
```

---

## Licence and attribution

ACBFHapticsBridge uses the **ACBFHapticsBridge Non-Commercial License v1.1**. Read the included `LICENSE.txt` for the complete terms.

- Personal/non-commercial use, study, modification and free redistribution are permitted under the licence terms.
- Keep attribution and the licence when redistributing permitted copies. Clearly identify modified versions.
- Selling the mod, charging for access, paywalling it or bundling it into paid software/mod packs requires prior written permission from the applicable copyright holder.
- Voluntary donations do not make access to the mod dependent on payment.

The project licence covers only material for which the contributors hold the applicable rights. Ubisoft game content, trademarks, Wwise and third-party loaders remain subject to their own rights and terms.

**HFX notice:** ShadowsHapticsPack-v5.hfx contains captured/processed haptic waveform data derived from Assassin's Creed Shadows. It is not claimed as wholly original Ubisoft-independent content or relicensed as original bridge code. No ownership of Ubisoft content or commercial rights to that material are granted by this project.

The mod is provided under the warranty and liability terms in LICENSE.txt. No game executable is distributed with this release.

---

## Source and feedback

[GitHub — source and project files](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge)

Please report controller-specific problems, missed effects or unexpected behaviour with enough detail to reproduce them.

<p align="center"><strong>Not perfect but perfectible.<br>DEadDRop out!</strong></p>
