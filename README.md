# ACBFHapticsBridge

<p align="center">
  <strong>DualSense haptics + conventional rumble for Assassin's Creed IV: Black Flag Resynced</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/RELEASE-v2.3.7.8-76C893?style=for-the-badge" alt="Release v2.3.7.8">
  <img src="https://img.shields.io/badge/BUILD-v8--r9p-7FB3FF?style=for-the-badge" alt="Build v8-r9p">
  <img src="https://img.shields.io/badge/DualSense-HAPTICS-0070D1?style=for-the-badge" alt="DualSense Haptics">
  <img src="https://img.shields.io/badge/XInput-RUMBLE-555555?style=for-the-badge" alt="XInput Rumble">
  <img src="https://img.shields.io/badge/LICENSE-NON--COMMERCIAL-E66B55?style=for-the-badge" alt="Non-Commercial License">
</p>

<p align="center">
  <strong>Runtime:</strong> <code>v8-r9p-v2.3.7.8-output-fix</code>
</p>


---

> 🟠 **IMPORTANT**  
> **License:** ACBFHapticsBridge uses the **ACBFHapticsBridge Non-Commercial License v1.1**.  
> Free personal/non-commercial use, modification and free redistribution are permitted under the license terms. **Selling, paywalling, or commercially redistributing the mod is not permitted without prior written permission.**

## 🔗 Downloads & Source

### NEXUS MODS — Main mod page / download:

[https://www.nexusmods.com/assassinscreedblackflagresynced/mods/412?tab=description](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/412?tab=description)

### GOFILE — Free mirror:

[https://gofile.io/d/whwCCJYe](https://gofile.io/d/whwCCJYe)

### GITHUB — Source, release verification & project files:

[https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge)

---

## 🎮 What this mod does

ACBFHapticsBridge adds **Assassin's Creed Shadows-derived haptic feedback** to **Assassin's Creed IV: Black Flag Resynced**.

The current **v2.3.7.8 / v8-r9p** build preserves the **v2.3.7.5 / v8-r9m** haptic waveforms, conventional-rumble conversion, gameplay mappings, gains and timings, while adding **DS4 USB / Bluetooth HID input and rumble output**, controller compatibility fixes and safer hook handling. Existing **Steam Title Updates 1.0.6 and 1.0.7** target support is retained.

On Steam, the bridge continues using Black Flag's existing **Wwise Motion / Quad Audio Haptics** path exactly as the known-good v2.3.7.0 baseline does. The Ubisoft compatibility additions are isolated to the Ubisoft Connect and Ubisoft+ executable profiles instead of changing the already-working Steam output path.

For Ubisoft Connect / Ubisoft+, Black Flag's native Quad haptic path remains the first choice. v8-r9p retains the r9k guarded **48 kHz / 4-channel direct DualSense fallback** on actuator channels **3/4** when meaningful native Quad haptics are absent while bridge HFX playback is required.

The mod supports:

**Native DualSense haptics — wired:** ✅

**DualSense through DSX + DLC / Virtual DualSense with Audio:** ✅

**Xbox 360 / XInput rumble:** ✅

**DS4 — USB / Bluetooth HID rumble + compatible emulated paths:** 🧪 New DS4 transport added; in-game hardware validation pending

**Xbox 360 emulation through DSX:** ✅

**Compatible GameInput controllers:** ✅

**Steam Input with native wired DualSense:** ✅

**DualSense:** full 48 kHz stereo waveform injection through Black Flag's native Quad Audio Haptics path on the proven Steam/r9h route. Verified Ubisoft profiles can use the guarded direct 48 kHz / 4-channel fallback only when meaningful native Quad haptics are absent.

**Conventional controllers:** the same effect timelines are converted into low/high-motor rumble for Xbox/XInput, DS4 USB/Bluetooth through the dedicated HID backend, compatible DS4 emulation and compatible GameInput controller paths.

The bridge keeps **Black Flag's own native controller feedback whenever possible**, supplements selected native effects where useful, and fills in actions where the original game provides little or no controller response.

> 🔵 **NOTE**  
> The v2.3.7.8 bridge uses **GameInput and XInput-compatible controller paths**, plus a dedicated **DS4 USB / Bluetooth HID transport**. Generic DirectInput support should not be assumed for every device.

> **DUALSENSE HAPTICS VS RUMBLE**  
> The 48 kHz DualSense haptic stream and conventional rumble are **parallel outputs**. A strong haptic effect does not automatically "overflow" into rumble. The haptic path is limited by **PeakLimit**, while conventional rumble is generated separately when enabled.  
> On Steam, v8-r9p preserves the v2.3.7.3 / r9k output behavior and the proven v2.3.7.0 / r9h routing baseline. The direct DualSense fallback remains ineligible on Steam.

---

## ✅ Confirmed / Supported Game Versions

**Steam — Title Update 1.0.7** ✅ Supported / executable target verified

### Verified ACBlackFlag.exe SHA-256:

```text
614dab4a20a5d5c6256792e1daa6d05669c97a751079b10df1725d6965ad766d
```

**Steam — Title Update 1.0.6** ✅ Supported

### Confirmed ACBlackFlag.exe SHA-256:

```text
8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140
```

The v2.3.7.0 / v8-r9h runtime remains the confirmed working Steam haptic baseline. v2.3.7.8 / v8-r9p retains the v2.3.7.5 / r9m verified 1.0.6 / 1.0.7 target support and established Steam DualSense / XInput behavior while fixing DS4 input and rumble delivery.

Steam TU 1.0.7 moves executable-specific code/data targets used by the bridge. r9p retains the verified 1.0.6 / 1.0.7 targets instead of assuming one fixed layout, while keeping the actual gameplay feedback, native Quad haptics and conventional-rumble tuning unchanged.

**Ubisoft Connect** ✅ Executable compatibility verified

### Verified ACBlackFlag.exe SHA-256:

```text
e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c
```

**Ubisoft+** ✅ Executable compatibility verified

### Verified ACBlackFlag_Plus.exe SHA-256:

```text
b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75
```

**Epic Games:** 🧪 Not yet verified — try it out and send feedback.

The supplied **Ubisoft Connect** and **Ubisoft+** executables were statically verified against the bridge's executable-dependent hooks. Both retain the required Wwise PostEvent hook, Quad Audio Haptics flush hook and all **21/21** gameplay-state callback slots used by the bridge.

v2.3.7.8 retains dynamic **XInputSetState PE-import resolution**, with verified executable-specific locations retained only as pointer-validated fail-closed fallbacks.

The new work in v8-r9p is **DS4 input / rumble transport and controller / hook safety**. Existing **1.0.6 / 1.0.7 compatibility target discovery** remains enabled. The r9k direct DualSense fallback remains isolated to **Ubisoft Connect / Ubisoft+**.

Steam keeps the known-good v2.3.7.0 / r9h output behavior.

For Ubisoft profiles, r9p retains the r9k behavior that:

- Preserves physical DualSense identity if the active controller backend temporarily changes.

- Distinguishes meaningful native Quad haptic data from silent Quad flush calls.

- Uses Black Flag's native Quad haptics whenever they are genuinely available.

- Allows the 48 kHz / 4-channel direct DualSense fallback only when Ubisoft-native haptics are absent.

- Stops the fallback as soon as genuine Ubisoft-native haptics return.

- Does not inject the same bridge HFX through both native and direct waveform paths simultaneously.

Runtime feedback for the Ubisoft builds is still welcome. If you use an unverified build such as **Epic Games**, please provide:

- your **ACBlackFlag.exe SHA-256 hash**

- the generated **ACBFHapticsBridge-v8.log**

- press **F10 while in menus**

- press **F10 once again during gameplay**

- then close the game and provide the log

Thanks!

---

## 📦 MOD REQUIREMENTS / INSTALLATION

A compatible ASI loader is required.

You can use:

- **version.dll** from **Ultimate ASI Loader**

- or the compatible **version.dll** supplied by **ACBlackFlagFix** on NexusMods

> 🟠 **IMPORTANT**  
> **version.dll is INCLUDED in the v2.3.7.8 release.**  
> It remains third-party software and is governed by its original author's license/redistribution terms.

### Installation

1. Download the normal **v2.3.7.8 release**, not the source-only package.

2. Place these files in the Black Flag game folder:

```text
ACBFHapticsBridge-v8-r9p.asi
ACBFHapticsBridge-v8.ini
ShadowsHapticsPack-v5.hfx
version.dll
```

3. They must be beside:

```text
ACBlackFlag.exe
```

4. Launch the game.

5. Press **F9** if you want to quickly test whether the bridge is producing feedback.

6. Press **F10** if you need a diagnostic snapshot in the log.

**Updating from an older version:** Close the game, remove the previous HapticsBridge ASI and keep only **ACBFHapticsBridge-v8-r9p.asi** installed. Keep your existing INI, HFX pack and compatible loader; no gain or timing changes are required.

---

## ☕ DONATING

> **Not obligated to, but appreciated nonetheless! :D**

If you liked this mod and wanna buy me a coffee, show your appreciation / leave a tip, you can donate via these wallets.

Multicoin is available; **BSC Network / ETH Network** is preferred for things such as BNB, USDC, or whatever you would like.

### It  helps pay bills, motivates me :D, and encourages me to pursue my endeavours in modding/programming.

**MetaMask / Multicoin:** 0xFfd40Cf24B1484f8567181E1c550eA494967f36f

**Solana:** DYgtA98yJ7yMf7rg8Mx65jiybeL4FyqpKGoED3VW2MuG

**TRON:** TL2oF5L8UNVLS58BmmsynndjY3nZAkTnid

**Bitcoin — Native SegWit:** bc1qjl3kv3s7l2wrmx68vhdmuqnfcj6hl6650krufx

> PayPal profile donate is not working for my country, and I can't leave the direct link here because that would mean doxxing myself :(

---

## 😄 AAAANDDDDDDDDDD THIS IS MY FIRST MOD!!

Glad it works for me at least!! :D

Please report any bugs or weird stuff so I can work on them if I can.

---

## ✨ What the ASI mod does — v2.3.7.8

ACBFHapticsBridge currently adds contextual feedback for:

**DualSense:** Full 48 kHz stereo haptics through Black Flag's native Wwise Quad Audio Haptics path on the proven Steam/r9h route, with a guarded Ubisoft-only fallback where required

**Conventional rumble:** The existing two-motor conversion for Xbox/XInput and compatible GameInput controllers, with dedicated DS4 USB/Bluetooth HID delivery and compatible emulated paths

**Movement:** Event-synchronized walking, jogging, spatial footsteps, L3 run opening/sustain and sprint contacts

**Parkour:** Jump takeoff, airtime-scaled landing, horizontal-travel boost, long-fall maximum impacts and climb catches

**Dodging:** Ground dodge / roll and directional dodge handling

**Water:** Automatic water-entry recovery, dive/splash, surface, swim-up and L3-gated underwater lunge / fast-swim behavior

**Combat:** Parry, light attacks, two-stage heavy attack, kick and native-combat haptic supplementation

**Character / equipment:** Hood action and sheathe / unsheathe feedback

**Menus:** Menu-open feedback, navigation ticks, reduced shoulder feedback and native menu-tick supplementation on DualSense

**Runtime protection:** Native-feedback arbitration to reduce doubled effects

**Recovery:** Player/menu/state reacquisition and optimized Wwise haptic-sink handling

**Performance:** 500 Hz bridge-side input worker, cached GameInput metadata, lower menu/input overhead and runtime performance diagnostics

---

## 🆕 v2.3.7.8 / v8-r9p — DS4 Input / Rumble Transport Fix

The existing haptic-to-rumble conversion is retained. This update fixes the DS4 controller path that could generate effects while GameInput delivered no vibration and no XInput motor-output device was available.

### 🎮 DS4 compatibility + motor delivery

- Added dedicated **DS4 USB / Bluetooth HID rumble output** for the existing low/high-motor envelopes.

- DS4 buttons, sticks, triggers and touchpad click feed the **existing controller mappings and menu handling** when GameInput has no normalized gamepad state.

- DS4 avoids the unverified GameInput raw-button layout and shared-vtable rumble hook. Controller / hook safety guards are retained.

- Compatible GameInput acquisition and confirmed-event / deferred effect scheduling remain available when normalized input is missing.

- Asynchronous DS4 writes preserve motor stops during effect completion, reconnects and device handoffs.

### ✅ Existing haptics / rumble tuning retained

- Native / virtual DualSense and Xbox 360 / XInput retain their established output routes.

- The r9m waveform pack, rumble conversion, gameplay mappings, gains, trims and timings are unchanged.

- **MasterGain=0.90**, the INI and the existing per-effect controls remain unchanged.

- No new gameplay effects or tuning controls are added.

- Steam **1.0.6 / 1.0.7** support and the guarded **Ubisoft Connect / Ubisoft+** DualSense fallback are retained.

### 🔎 Diagnostics + validation

- F10 and automatic reports now distinguish **DS4 HID submitted / completed writes**, input reports and actual transport errors from GameInput dispatch counts.

- **48 complete audio / motor-stream comparisons match r9m exactly** across Xbox, DualSense and DS4 render cases.

- USB / Bluetooth report, CRC, stop, failure, reconnect, cancellation and hook-safety regression tests passed.

**Runtime status:** The Windows build and production-code test harnesses pass. Physical in-game DS4 USB / Bluetooth validation is still pending; press **F9**, then **F10**, and provide the complete log if feedback is absent.

## 📜 v2.3.7.5 / v8-r9m — Steam 1.0.7 Startup Compatibility Fix

v2.3.7.5 fixes startup compatibility bugs in v2.3.7.4 / r9l. Memory scans skip inaccessible pages, verified 1.07 native addresses are tried directly, and the executable layout is retained before hook installation so the correct 21 gameplay-state targets are selected. Original hook pointers are prepared before entry-point patches are published. Static and guard-page regression checks passed; Windows game/controller validation is pending.

The gameplay, DualSense haptics, conventional rumble, effect mappings, timings, gains and Ubisoft fallback behavior from r9k are retained.

### 🎮 Steam 1.0.6 + 1.0.7 compatibility

- Added verified target discovery for both **Steam Title Update 1.0.6** and **Steam Title Update 1.0.7**.

- Added the verified TU 1.0.7 executable profile:

```text
614dab4a20a5d5c6256792e1daa6d05669c97a751079b10df1725d6965ad766d
```

- Handles the executable-layout changes between 1.0.6 and 1.0.7 without changing the actual haptic/rumble behavior.

- r9m uses discovered verified targets for the executable-dependent native Quad haptics, Wwise routing, gameplay-state and XInput-related paths instead of assuming a single Steam layout.

- The existing TU 1.0.6 profile remains supported.

### ✅ Existing output behavior retained

- v2.3.7.3 / r9k gameplay and effect behavior is retained.

- Steam still uses Black Flag's native Wwise Quad Audio Haptics path.

- Conventional rumble behavior remains unchanged from r9k.

- **MasterGain=0.90** remains unchanged.

- No movement, combat, parkour, water, menu or effect-tuning changes were added in v2.3.7.5.

- No new v2.3.7.5 INI tuning values are required.

- The r9k Ubisoft Connect / Ubisoft+ guarded 48 kHz / 4-channel DualSense fallback remains unchanged and remains isolated to verified Ubisoft profiles.

- Dynamic **XInputSetState** import resolution remains enabled with verified-profile fail-closed fallbacks.

### 🔎 Hook compatibility

The v2.3.7.5 runtime identifies verified profiles for **Steam TU 1.0.6, Steam TU 1.0.7, Ubisoft Connect and Ubisoft+**.

The available r9m runtime log confirms the Steam 1.0.6 path installs the native Quad Audio Haptics injector, the exact Wwise PostEvent router and all **21/21 gameplay-state hooks** with zero hook failures. TU 1.0.7 is included as a separately verified executable target in the r9m compatibility table.

> **IN SHORT**  
> v2.3.7.5 keeps the v2.3.7.3 haptics/rumble behavior and adds Steam **1.0.7 compatibility** without dropping **1.0.6, Ubisoft Connect or Ubisoft+** support.

## 📜 v2.3.7.3 / v8-r9k — Ubisoft-Isolated Compatibility Update

v2.3.7.3 deliberately returns to **v2.3.7.0 / v8-r9h** as the behavioral baseline.

The working Steam DualSense haptic path, conventional-rumble behavior, gameplay detection, HFX mixing and effect tuning from r9h are retained.

The compatibility work added afterward is now isolated to Ubisoft Connect / Ubisoft+.

### 🎮 Steam behavior restored to the proven baseline

- Steam uses the same Black Flag native Wwise Quad Audio Haptics injection path as v2.3.7.0 / r9h.

- The direct 48 kHz DualSense renderer is not eligible on Steam.

- The conventional-rumble implementation is restored to the r9h behavior.

- A detected DualSense successfully handled through GameInput is not newly mirrored through XInput.

- The r9i/r9j global native/direct output switching is not used on Steam.

### 🎧 Ubisoft-only DualSense fallback

- Ubisoft Connect / Ubisoft+ keep the r9h native Quad Audio Haptics route as first choice.

- The direct DualSense fallback is eligible only on verified Ubisoft profiles.

- The fallback requires meaningful bridge HFX work and absence of meaningful native Quad haptics.

- Direct output uses 48 kHz / 4-channel audio with DualSense actuator channels 3/4.

- The native Quad stream continues to be observed while the fallback is active.

- Real Ubisoft-native haptics immediately retake priority.

- Bridge HFX are not injected through both native and direct waveform paths simultaneously.

### 🔎 Ubisoft executable / XInput compatibility

- Dynamic PE import-table resolution of **XInputSetState**.

- Known verified IAT locations remain pointer-validated fail-closed fallbacks.

- Runtime executable-profile classification determines whether the Ubisoft-only direct fallback can be used.

- Physical DualSense identity is retained for the Ubisoft fallback if the input backend temporarily changes.

### ✅ r9h baseline preserved

The following important functions were verified source-identical against the actual v2.3.7.0 / r9h baseline:

- Conventional-rumble output

- Native Quad HFX injection

- Controller-edge processing

- Gameplay-state processing

- Deferred-effect processing

### 🎛️ Gameplay and tuning unchanged

- **MasterGain=0.90**

- All existing r9h INI values remain unchanged.

- All gameplay-state hooks remain unchanged.

- All Wwise semantic mappings remain unchanged.

- Movement and spatial footsteps remain unchanged.

- Jump and landing behavior remain unchanged.

- Parkour scaling remains unchanged.

- Water behavior remains unchanged.

- Combat behavior remains unchanged.

- Menu feedback remains unchanged.

- ShadowsHapticsPack-v5.hfx remains unchanged.

- version.dll remains unchanged.

Only two new configuration values are added:

```ini
UbisoftDirectDualSenseFallback=1
UbisoftDirectDualSenseFallbackDelayMs=1000
```

These settings have no effect on the Steam output path.

> **IN SHORT**  
> v2.3.7.3 keeps the proven v2.3.7.0 Steam haptic behavior and adds the newer compatibility work only where it is needed: Ubisoft Connect / Ubisoft+.

### 🚶 Spatial movement + footsteps

- Walking and jogging are synchronized to Black Flag's real gait events rather than only a stick-driven loop.

- Added true left/right spatial footsteps.

- A fresh gait cycle begins on Edward's **right foot** and alternates right/left.

- Default spatial balance is 100% on the active side with 25% crossfeed to the opposite side.

- Spatial gait applies to DualSense stereo haptics and conventional rumble conversion.

- L3 running retains the tuned 1.5-second stronger opening and sustained run texture.

- Real sprint contacts layer on top of the continuous L3 run texture.

### 🎮 XInput / Xbox rumble fixes

- Fixed land-running becoming continuous/stuck vibration on XInput-class controllers.

- XInput land sprint follows **real gait-contact events step-by-step**.

- The continuous land-run motor bed is suppressed; water fast-swim/lunge can still remain continuous where appropriate.

- XInput jump impact is capped to **300 ms**.

- XInput landing impact is capped to **300 ms**.

- XInput light-attack vibration is capped to **300 ms per swing**.

- Added a **16 ms explicit zero-motor barrier** after those short XInput impacts so rapid actions do not blend into one long vibration.

- XInput menu L1/R1 feedback reduced by 50%.

### 🧗 Jump, parkour + long-fall impacts

- Jump/landing detection remains based primarily on Black Flag's real Jump/InAir gameplay states.

- Landing strength scales with measured airtime.

- Parkour movement distance can add extra landing/catch intensity using integrated left-stick travel as a proxy.

- Improved climb-to-fall carry and airborne climb-catch impacts.

- Short and medium falls remain progressively scaled instead of all using the same impact.

- Falls of **3 seconds or longer** reach the current maximum base impact of **3.15**.

- The 3-second+ maximum applies to both DualSense bridge feedback and XInput/GameInput-class conventional rumble.

- XInput keeps the short 300 ms landing duration; long falls increase **amplitude**, not motor duration.

### 🌊 Water + underwater lunge fixes

- Improved automatic water-entry/reacquisition when Edward enters water without a manual jump.

- Fixed underwater lunge feedback triggering simply from moving/changing direction after being stationary.

- The Wwise water-lunge event is no longer trusted by itself as proof that L3 was pressed.

- A real configured **SprintLunge / L3 rising edge** must authorize the underwater lunge.

- Direction-only copies of the event are ignored.

- Legitimate underwater lunge/fast-swim sustain keeps its 3-second safety cap.

- Surface swim-up and water exit/surface behavior remain intact.

### ⚔️ Combat + native-feedback supplementation

- Native Black Flag feedback still receives first refusal to avoid doubled effects.

- Verified native Quad Audio Haptics for selected Edward combat actions are supplemented by **+75% / 1.75x**.

- Covered combat families include sword attacks, normal/perfect parry feedback, pistol fire and kicks.

- If suitable native feedback is absent, the established bridge fallback remains available.

- Cannon/naval, player-hit, movement and water haptics are not intentionally included in the combat boost.

- Two-stage Heavy R1 timing remains tuned to a 320 ms hold threshold and 590 ms second-impact delay.

- Normal parry remains latency-trimmed and native-first.

### 🎛️ DualSense tuning

- Bridge-generated DualSense output has a global 0.90 cleanup multiplier to reduce noise.

- Jump/landing receives an additional 0.85 multiplier for cleaner ordinary impacts.

- Land walk/jog/L3-run tuning uses **LandMovementHapticGain=1.035**.

- DualSense menu L1/R1 feedback reduced by 25%.

- DualSense menu navigation tick increased by 30%.

- The +30% menu tick applies to bridge fallback ticks and verified Ubisoft-native menu Quad ticks.

- Normalized DualSense walk control: user value **1.00** represents the established tuned baseline that previously used raw 4.80.

### 📋 Menu + input responsiveness

- Exact Black Flag Wwise menu events are preferred for normal menu navigation.

- When semantic menu routing is active, the bridge no longer continuously rescans analog-stick/D-pad navigation heuristics.

- The underwater L3 correlation check is evaluated on an actual L3 edge instead of continuously while the left stick moves.

- Menu/category haptic sink swaps are handled as lightweight sink handoffs instead of repeated full playback resets.

- Custom L1/R1 menu tuning remains active.

### ⚡ Performance cleanup — retained from v8-r9g

- Bridge-side controller/input worker target changed from approximately 125 Hz to **500 Hz / 2 ms**.

- This does **not** overclock the controller hardware. A 250 Hz USB source still provides a fresh report about every 4 ms; the 500 Hz bridge worker simply samples it promptly.

- GameInput device identity/metadata is cached instead of queried every 2 ms.

- Device identity refresh interval: 125 ms.

- Device status refresh interval: 500 ms.

- DualSense raw controller-button layout/count is cached per device; DS4 uses its dedicated HID input path without GameInput raw-button queries.

- Conventional steady-state rumble writes are capped at **250 Hz / 4 ms**, while start/stop/zero transitions remain immediate.

- Native haptic detection and native combat/menu gain processing share one buffer pass.

- Normal haptic-sink pointer changes no longer cause expensive full recovery resets.

- Added detailed performance telemetry to F10 for worker gaps, GameInput time, rumble-write time, audio-hook time and sink rebinds.

- F12 suspends bridge controller-input inference for A/B diagnostics while semantic/state/native routing continues. DS4 HID I/O remains serviced so pending writes and motor stops can complete.

### 🔄 Recovery + reliability

- Improved state reacquisition after reloads, menu transitions and long Wwise silence.

- Ordinary sink changes preserve valid semantic/gameplay state instead of unnecessarily clearing everything.

- Genuine long silence/load events still trigger a clean recovery.

- Expanded diagnostics make controller backend, semantic domain, state hooks, water, landing, native arbitration and output activity easier to verify.

---

## ⌨️ Keyboard / ASI Hotkeys

**F9:** Plays the Dodge test effect through the active DualSense haptic / conventional rumble output paths. Useful for checking whether the mod is producing feedback.

**F10:** Writes a detailed runtime snapshot to **ACBFHapticsBridge-v8.log**, including controller/backend information, semantic/gameplay state, water/landing diagnostics, native arbitration, effect activity, compatibility diagnostics and **DS4 HID input / submitted / completed / error counters**.

**F11:** Enables / disables the complete bridge. Disabling immediately stops ASI-added playback and clears bridge state. Re-enabling performs a clean runtime/state reacquisition.

**F12:** Suspends / resumes **bridge controller-input inference** for diagnostics. Confirmed Wwise semantic events, gameplay-state hooks and native routing remain active. DS4 HID I/O remains serviced to complete pending writes and motor stops. This is useful for A/B testing input/performance behavior without unloading the ASI.

---

## 🎮 Default Controller Bindings

**Jump: Cross** | Raw/fallback correlation for climb jump, swim-up and jump-state diagnostics; actual ground jump/landing is state-driven

**DodgeDive: Circle** | Water/air dive fallback; normal ground dodge prefers the exact game event

**Parry: L1** | Normal-parry fallback when suitable native feedback is missing

**Attack: R1** | Hybrid raw correlation for combo gaps/heavy hold; known attacks prefer exact game events

**SprintLunge: L3** | L3 land sprint and required authorization for underwater lunge / fast-swim

**SheatheHood: DPadLeft** | Fallback/correlation; known sheathe/unsheathe and Hood events are semantic/event-driven

**Kick: R2 + Square** | Fallback combination; Black Flag's actual kick event is preferred when available

The **left stick** remains important for:

- the game's own walking/running direction and speed

- movement-strength classification

- directional-dodge context

- climbing-jump direction/context

- parkour horizontal-travel estimation used by landing/catch scaling

The game itself also uses the left stick for menu navigation, but when **SemanticEvents/MenuState** is enabled the bridge follows Black Flag's actual menu Wwise events instead of continuously treating stick direction as a separate menu-feedback trigger.

Landing, haystack entry, water entry and water surface effects are primarily triggered by detected gameplay states/events rather than a remappable raw button.

---

## 🔧 Changing Controller Inputs

Open:

```text
ACBFHapticsBridge-v8.ini
```

beside:

```text
ACBlackFlag.exe
```

Use the **[InputBindings]** section for the controller-button fallback/correlation mappings.

Accepted binding names include Cross/A, Circle/B, Square/X, Triangle/Y, L1/LB, R1/RB, L2/LT, R2/RT, L3/LS, R3/RS and the D-Pad directions.

Write **Cross** for the PlayStation X/cross button. The value **X** means the Xbox X / PlayStation Square button.

Touchpad, PS/Guide, paddles, keyboard keys, stick directions, Options/Start and Share/Create/View are not general action-binding values. Menu opening/navigation remains handled separately by **[Menus]**.

Restart Black Flag after changing the INI.

---

## ⚙️ Per-Effect Configuration

Automatic effects remain independently configurable where applicable.

Users can adjust:

- enable / disable state

- intensity

- trigger timing

- waveform start trim

- cooldowns

- movement loop timing

This allows the bridge to be tuned **without recompiling the ASI**.

### Intensity

```ini
IntensityMultiplier=1.00
```

Changes both DualSense bridge haptics and conventional rumble for that effect.

Examples:

**0.50** = half strength

**1.25** = 25% stronger

**2.00** = double strength

### Timing

**TriggerOffsetMs=-20:** Makes the effect earlier, but never before the triggering event/input/state was detected

**TriggerOffsetMs=20:** Makes it later

**StartTrimOffsetMs=20:** Skips more of the waveform beginning, producing a faster impact

**StartTrimOffsetMs=-20:** Restores more of the original lead-in

These settings **never change game or animation speed**.

### 🧪 Configuration Examples

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

No new v2.3.7.8 effect-tuning controls were added; the following v2.3.7.3 / r9k settings remain unchanged.

## 🧩 Special v2.3.7.3 Effect Settings

### DualSense walk normalization

```ini
[Effect.Walk]
DualSenseHapticNormalizedBaseline=1
DualSenseHapticMultiplier=1.00
```

**1.00** = the established tuned DualSense walk baseline retained from v2.3.7.0.

**0.50** = half that tuned walk level.

**2.00** = double that tuned walk level.

**4.80** = maximum user scale for this normalized control, still subject to **PeakLimit**.

Old INIs without **DualSenseHapticNormalizedBaseline** use legacy/raw semantics for compatibility.

### DualSense jog/run tuning

```ini
[Effect.Run]
DualSenseJogHapticMultiplier=1.20
OpeningDurationMs=1500

[Movement]
RunBaseGain=1.80
RunOpeningGain=2.28
RunOpeningDurationMs=1500
```

### Spatial footsteps

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

### Long-fall scaling

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

The current release ramps toward **3.15** and treats falls of roughly **3 seconds or longer** as maximum base-impact landings.

### XInput-only short impact limits

```ini
[Rumble]
LandRunStepOnly=1
SprintStepGain=1.20
XInputJumpLandingMaxDurationMs=300
XInputLightAttackMaxDurationMs=300
XInputActionZeroGapMs=16
```

These duration/zero-gap controls are for conventional XInput/Xbox-class rumble. They do **not** shorten DualSense 48 kHz haptic playback.

### DualSense bridge cleanup

```ini
[DualSense]
BridgeHapticGain=0.90
JumpLandingHapticGain=0.85
LandMovementHapticGain=1.035
```

These affect **bridge-generated** DualSense feedback, not Ubisoft-native Quad haptics.

### Menu tuning

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

**Enabled=0** disables plugin-added menu feedback while menu-domain detection still protects gameplay feedback from leaking into detected menus.

### Underwater L3 gate

```ini
[SemanticEvents]
WaterLungeSustainMs=3000
WaterLungeInputGateMs=600
```

The Wwise water-lunge event can confirm a lunge only after a real configured SprintLunge/L3 rising edge inside this authorization window.

### Native combat supplement

```ini
[NativeArbitration]
Enabled=1
CombatNativeBoost=1
CombatNativeHapticGain=1.75
CombatNativeBoostWindowMs=700
```

**1.75** means the selected verified Ubisoft-native combat Quad haptic stream is supplemented by +75% while the action window is active.

---

## 🌐 Global Settings

### Output type

```ini
[Output]
NativeHaptics=1
ConventionalRumble=1
PreferGameInput=1
RumbleOnDualSense=1
UbisoftDirectDualSenseFallback=1
UbisoftDirectDualSenseFallbackDelayMs=1000
```

**NativeHaptics=0:** Disables full DualSense waveform injection.

**ConventionalRumble=0:** Disables ordinary low/high-motor vibration generated by the bridge.

**RumbleOnDualSense=0:** Leaves the bridge's full DualSense haptic layer without its parallel conventional-rumble copy on detected DualSense devices.

**UbisoftDirectDualSenseFallback=1:** Allows the guarded 48 kHz / 4-channel DualSense fallback on verified Ubisoft Connect / Ubisoft+ executable profiles when meaningful native Quad haptics are absent. It does not enable the direct renderer on Steam.

**UbisoftDirectDualSenseFallbackDelayMs=1000:** Grace interval before the Ubisoft-only direct fallback may take over when native Quad haptics remain absent.

Keep the normal output options enabled for the normal hybrid configuration.

### Overall intensity

```ini
[Playback]
NormalizedBaseline=1
MasterGain=0.90
MovementGain=1.00
OneShotGain=1.00
PeakLimit=2.00
```

With **NormalizedBaseline=1**:

- **MasterGain=0.90** = the supplied v2.3.7.8 master setting, unchanged from v2.3.7.3 and equal to 90% of the normalized tuned master baseline

- **OneShotGain=1.00** = the tuned current one-shot baseline

- **MovementGain** changes continuous movement textures

- **PeakLimit** is the final haptic output limiter; 0 disables limiting

### Conventional rumble

```ini
[Rumble]
Gain=1.00
MovementGain=2.00
LowMotorScale=3.00
HighMotorScale=1.60
Deadzone=0.008
```

These control conversion of the captured waveform timeline into conventional controller motors.

### Movement thresholds

```ini
[Movement]
WalkThreshold=11000
RunThreshold=24500
SprintToggle=1
```

Lower thresholds make the bridge classify movement strength with less stick deflection.

**SprintToggle=1:** Press the configured SprintLunge input once for the run latch.

**SprintToggle=0:** Hold the configured SprintLunge input.

### Performance / controller polling

```ini
[General]
PollMs=2
GameInputDeviceRefreshMs=125
GameInputStatusRefreshMs=500
RumbleWriteIntervalMs=4
```

**PollMs=2** means a 500 Hz bridge-side worker target. It does not change the physical polling/report rate of the controller itself.

The metadata/status refresh intervals are intentionally slower because VID/PID/device-layout information does not need to be queried 500 times per second.

Steady-state conventional rumble is capped to a 4 ms / 250 Hz write cadence while important start/stop/zero changes remain immediate.

---

## ⚠️ Important INI Warning

Use:

```ini
[InputBindings]
```

for controller-button fallback/correlation mappings.

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

Changing them selects which captured HFX waveform an action/fallback uses.

Normally, leave these at their supplied values unless you know exactly what you are changing:

- **[Buttons]**

- **[Hook]**

- advanced **[GameplayState]** addresses / hook-related values

- verified semantic event IDs

---

## 🔐 v2.3.7.8 DS4 Compatibility Build — Build / Regression Verified

**Release:** **v2.3.7.8**

**Internal build:** **ACBFHapticsBridge-v8-r9p**

### Runtime label:

```text
v8-r9p-v2.3.7.8-output-fix
```

### ASI:

```text
ACBFHapticsBridge-v8-r9p.asi
```

### ASI SHA-256:

```text
5efa68c7a00312f1537c9bd882390907f484f3129c6f7ea6b6792dfa206f440c
```

### Release ZIP SHA-256:

```text
0c30f3eb588e5c64960fbcff2c9cb28a3ae560404cd5e971d69d784122f4b095
```

### Steam TU 1.0.7 ACBlackFlag.exe SHA-256:

```text
614dab4a20a5d5c6256792e1daa6d05669c97a751079b10df1725d6965ad766d
```

### Steam TU 1.0.6 ACBlackFlag.exe SHA-256:

```text
8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140
```

### HFX SHA-256:

```text
9e994203bbafbf0a318f9056066a54b02adad43c0311f77372ea71dec406565f
```

### version.dll SHA-256:

```text
6203c5a0ba1f8c5c77c7c12c33be178a5057063ef1da265eb0a546495c6e6a4e
```

### Supported executable profiles:

- **Steam — Title Update 1.0.7:** verified executable target support retained from r9m

- **Steam — Title Update 1.0.6:** prior runtime-confirmed native Quad/Wwise path and all 21 gameplay-state targets retained

- **Ubisoft Connect:** executable compatibility verified; r9k Ubisoft-only output fallback behavior retained

- **Ubisoft+:** executable compatibility verified; r9k Ubisoft-only output fallback behavior retained

v2.3.7.8 / r9p is a **DS4 controller transport / safety update** over the r9m behavior and compatibility targets. The existing HFX rendering, gameplay mappings, timings, gains and INI tuning are retained.

The available r9m runtime log confirms on the tested Steam 1.0.6 executable:

- native Quad Audio Haptics flush injector installed

- exact Wwise PostEvent router installed

- **21/21 gameplay-state hooks installed, 0 failed**

- dynamic named-import **XInputSetState** resolution active

The TU 1.0.7 executable is separately included in the r9m verified-target table with SHA-256 `614dab4a20a5d5c6256792e1daa6d05669c97a751079b10df1725d6965ad766d`.

**Build / regression status:** Two clean r9p builds are byte-identical. All **48 rendered audio / motor-stream comparisons match r9m**, and DS4 protocol / lifecycle and hook-safety tests pass. Existing verified Steam / Ubisoft targets are retained; physical in-game r9p DS4 feedback validation remains pending.

> 🔵 **NOTE**  
> Release-specific ASI / ZIP hashes below identify the delivered **v2.3.7.8 / r9p** files. The HFX and loader hashes remain unchanged.

---

## 📂 Source Notice — v2.3.7.8

The current **v2.3.7.8 / r9p** release and source kit contain the **actual buildable source for the current ASI**. The r9m haptic / rumble tuning is retained; the new **ds4_hid.h** supplies DS4 input / motor transport without changing the waveform renderer or existing effect mappings.

The supplied source layout is:

```text
Source/
├─ src/
│  ├─ main.cpp
│  └─ ds4_hid.h
├─ build_linux_pe_r9p.sh
├─ verify_release_r9p.sh
├─ audit_r9p.py
├─ audit_preservation_r9p.py
├─ verify_r9h_baseline.py
├─ validate_hfx.py
├─ test_ds4.cpp
├─ test_ds4_hid.cpp
├─ test_effect_fingerprint.cpp
├─ test_output_pipeline.cpp
├─ test_hooks.cpp
├─ test_startup.cpp
├─ test_legacy_reading.cpp
├─ References/
├─ ACBFHapticsBridge-v8.ini
├─ LICENSE.txt
├─ v2.3.7.5-to-v2.3.7.8-runtime.patch
└─ v2.3.7.7-to-v2.3.7.8-runtime.patch
```

The package also includes **BUILD-VERIFICATION.md**, **VALIDATION.txt**, **FIX-REPORT.md**, **INSTALL-AND-TEST.txt** and **PACKAGE-CHECKSUMS.sha256**. The source-only kit uses the unchanged **ShadowsHapticsPack-v5.hfx** beside Source for waveform integration and r9m comparison tests.

---

## 📜 License — Non-Commercial

> 🟠 **IMPORTANT**  
> **ACBFHapticsBridge v2.3.7.8 is NOT released under the MIT License.**  
> It is distributed under the **ACBFHapticsBridge Non-Commercial License v1.1**.

The full release-specific license is included with the source and inside the downloadable release archive. ACBFHapticsBridge remains under the **ACBFHapticsBridge Non-Commercial License v1.1**.

### What the license allows

**Download and use the mod personally:** ✅

**Use the mod for non-commercial purposes:** ✅

**Study the source/build material:** ✅

**Modify the ACBFHapticsBridge material:** ✅

**Make a non-commercial derivative:** ✅

**Redistribute the original or a modified version for free:** ✅

**Fork the source for non-commercial development:** ✅

**Keep reasonable attribution and the license with redistributed copies:** **Required**

### What is NOT allowed without my prior written permission

**Sell ACBFHapticsBridge:** ❌

**Charge for a download or copy:** ❌

**Put the mod behind a paywall:** ❌

**Require Patreon/subscription/payment to download the mod:** ❌

**Bundle it inside a paid mod pack or paid software package:** ❌

**Sell a modified or substantially identical derivative:** ❌

**Charge for activation, access, installation, or use of the mod itself:** ❌

**Remove the copyright/license notice and present the project as your own:** ❌

**Commercially sublicense or exploit the ASI/source/tools:** ❌

Changing the filename, repacking the files, making small modifications, or creating a substantially identical derivative **does not turn it into something that can be sold**.

Commercial permission can only be granted separately by the ACBFHapticsBridge copyright holder in writing.

### Donations

Voluntary donations to support **my own development work** are fine.

The mod itself is intended to remain freely accessible. A third-party redistributor may not make access to ACBFHapticsBridge dependent on a purchase, subscription, Patreon membership, donation, or other payment without written permission.

### Ubisoft / Assassin's Creed material

The ACBFHapticsBridge Non-Commercial License only covers material for which the ACBFHapticsBridge contributors hold the applicable rights.

It does **not** grant ownership of or commercial rights to:

- Assassin's Creed IV: Black Flag

- Assassin's Creed Shadows

- Ubisoft trademarks, game code, artwork, audio, haptic design, assets, or other intellectual property

- Anvil / AnvilNext material

- Wwise or other third-party technology

- Ubisoft executables, DLLs, data files, or proprietary game files

- unrelated third-party components

Nothing in this repository or release should be interpreted as granting permission to commercially exploit Ubisoft or other third-party intellectual property.

### Shadows HFX capture notice

The release contains:

```text
ShadowsHapticsPack-v5.hfx
```

This file contains **captured and/or processed haptic waveform data derived from controller/haptic output produced by Assassin's Creed Shadows**.

The HFX pack is **not being claimed as wholly original Ubisoft-independent content**, and it is **not licensed as original ACBFHapticsBridge code** under the project license.

The fact that I captured, processed, edited, reconstructed, packaged, or mapped the haptic data does **not** mean this license grants ownership of Ubisoft's underlying game content or haptic design.

No commercial rights to Ubisoft material are granted by this project.

Any redistribution or use of the HFX pack remains subject to applicable copyright, platform rules, permissions, and third-party terms.

### version.dll and other third-party files

The current v2.3.7.8 package includes:

```text
version.dll
```

That file remains governed by **its original author's own license and redistribution terms**.

Bundling a third-party ASI loader with ACBFHapticsBridge does not place that loader under the ACBFHapticsBridge Non-Commercial License.

### Attribution / modified versions

If you redistribute a permitted non-commercial modification:

- keep the ACBFHapticsBridge copyright and license notice

- include the license with the files

- clearly state that your version is modified

- do not present an unofficial derivative as an official ACBFHapticsBridge release

- do not imply endorsement by the original project

- do not claim ownership of Ubisoft or other third-party material

> 🔴 **WARNING**  
> **Selling this mod, paywalling it, or commercially redistributing ACBFHapticsBridge material is not permitted under the project license without prior written permission.**

---

## 🔮 Future Updates

Future updates will include — hopefully — better haptics and rumble effects captured from **Assassin's Creed Shadows**, along with improved state/context detection as I continue working on the bridge.

This is my first mod, so if you find bugs, weird behavior, controller-specific problems, missed effects, or anything else useful, please report it.

<p align="center"><strong>Not perfect but perfectible. DEadDRop out!</strong></p>
