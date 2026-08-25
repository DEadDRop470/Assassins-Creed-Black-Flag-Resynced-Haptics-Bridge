# ACBFHapticsBridge v8 changes

## Architecture

v8 adds the exact Wwise `PostEvent` producer hook proven by StateTracer 2.3/2.4 at RVA `0x0091DEA0`. The hook signature is checked before patching. The audio-output injection remains the proven `AkQuadAudioHapticsSink` flush hook at RVA `0x04F78610`.

The PostEvent hot path only snapshots known semantic IDs into a fixed lock-free ring. The worker thread performs state decisions and schedules fallbacks.

## Native-first combat swings

Light (`0x8ABBEE18`) and heavy (`0xC21A023E`) attacks are now driven by the game's real events. The native Quad serial is snapshotted **before** Ubisoft's PostEvent call. v8 waits `AttackNativeProbeMs` (100 ms default):

- if native combat haptics appear, the Shadows fallback is suppressed;
- if no native Quad appears (e.g. swinging the sword in the air outside combat), the Shadows swing waveform plays.

No explicit guessed `combat=true/false` flag is required.

## State detection

A new semantic runtime domain is tracked: `UNKNOWN`, `MENU`, `CHARACTER`, `WATER`, `HELM`.

- HELM is anchored by the real take-wheel and cannon events and blocks character input fallbacks.
- character gait/combat/jump events recover CHARACTER after leaving the helm.
- water events recover WATER.
- menu events recover MENU.
- sink resume/rebind and F11 re-enable clear stale domain/state/playback and rebuild from subsequent real events.

The older Anvil vtable hooks remain only as guard flags for unresolved climb/water input fallbacks; they no longer own ground jump/landing effect timing while semantic routing is enabled.

## Airtime

`0x2E795D46` starts semantic airtime. `0x4E92CCAC` / `0x43CD17F9` end it. Landing gain is derived from the actual elapsed air duration using the existing short/normal/long landing curve.

This removes the old raw Cross timing from normal ground jump/landing. Cross remains only for unresolved climb-jump and swim-up fallback contexts.

## Menu state

`0x67233A85` is a real menu-navigation event that already produces native Quad: v8 preserves it and only uses it to establish/maintain MENU.

`0xEB10F213` is the controlled-test class that was audible but produced no Quad: v8 adds the missing menu haptic from that exact event.

Ordinary D-pad/stick/face-button menu haptics are no longer generated from raw navigation heuristics when semantic menu mode is enabled. The old L1/R1 native-tab inference is also disabled in semantic mode.

## Native effects intentionally untouched

- naval cannon/broadside Motion
- helm enter (state only, no requested plugin vibration)
- player-hit native haptic
- pistol native haptic
- normal/perfect parry native haptics
- native menu ticks

## Hotkeys

- `F9` — dodge preview/test waveform.
- `F10` — write full status including semantic hook, semantic domain, last semantic event, event/drop counters, semantic airtime, old state-hook diagnostics, output backend and playback counters.
- `F11` — full mod disable/enable. Re-enable performs a full state reload: semantic domain, menu state, gait, airtime, actor/state caches, deferred effects, movement and stale playback are cleared.
- `F12` — retained developer toggle for unresolved raw-input fallback mappings only. Confirmed Wwise semantic events continue to work while F12 is off.
