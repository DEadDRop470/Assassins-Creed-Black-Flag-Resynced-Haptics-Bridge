# ACBFHapticsBridge v8-r2 changes

## 1. Latency reduction

v8.0 deliberately waited for native Motion before many fallbacks. The runtime test showed this was perceptible as roughly 0.15–0.20 s total action-to-controller delay.

r2 changes the semantic timing policy:

- light/heavy attack: `AttackNativeProbeMs` **100 -> 32 ms**;
- confirmed no-native semantic actions: **0 ms** artificial probe;
- native feedback arriving after a fallback begins still clears plugin playback through the existing Quad native-feedback detector.

The 32 ms attack window is intentionally not zero because combat swings already have good Ubisoft native Motion and must not be doubled. It is approximately one-to-two 1024-frame/48 kHz Quad blocks plus worker scheduling, instead of the old 100 ms first-refusal delay.

## 2. Event-per-footstep locomotion

v8.0 used the real gait Events to start/maintain the old multi-second movement loop. This preserved a ~700 ms post-stop tail.

r2 instead maps:

- `0xF4919384` -> one short walk contact pulse;
- `0xC490DFC6` -> one short jog/run contact pulse.

Default source windows:

- walk: 2440 ms start, 150 ms duration;
- run: 3120 ms start, 140 ms duration.

Fast-gait event spacing still scales jog vs hard run strength. `GaitTimeoutMs=250` only expires cadence history; it no longer keeps a movement waveform alive after movement stops.

Raw stick movement is disabled for ground locomotion while semantic routing is enabled. It remains available only for unresolved continuous WATER texture.

## 3. One-shot tail caps

Default maximum playback durations after the configured source trim:

- dodge 330 ms;
- light attack 360 ms;
- heavy attack 520 ms;
- jump 260 ms;
- landing 260 ms;
- kick 320 ms;
- water lunge 330 ms;
- surface 220 ms.

The playback command/voice format now carries a maximum frame count for both native haptic injection and conventional rumble conversion.

## 4. HELM is contextual, not sticky

The v8 test showed `0x2E795D46` during a naval-damage section even though that ID had been isolated as a jump-start family in character tests. Therefore Event ID alone cannot globally force HELM -> CHARACTER.

r2 learns two Wwise object affinities at runtime:

- latest confirmed Edward/character object;
- latest helm/naval object.

While HELM:

- real walk/jog/run Events release HELM immediately;
- strong character-only actions such as dodge, sword attack, parry, pistol, sheathe/hood/kick release HELM;
- water/menu events move to their own domain;
- `PLAYER_HIT` alone does **not** release HELM;
- ambiguous jump/landing-family Events release HELM only if their `gameObject` matches the learned character object;
- long Wwise silence/resume resets domain to UNKNOWN, covering load/desync/reload transitions.

This is intentionally **not** a sticky helm state. A genuine cannon knock-off can still recover CHARACTER if the player/air event belongs to Edward, and any subsequent real Edward gait/action releases HELM immediately.

## 5. Airtime safety

Normal jump still uses `0x2E795D46` to begin semantic airtime and `0x4E92CCAC` / `0x43CD17F9` to finish it when valid. r2 adds `AirTimeoutMs=6500`. A missed landing can no longer leave a 10+ second stale airborne timer that corrupts a later landing.

## 6. Recovery split

The v8 runtime log showed many sink rebind recoveries. r2 distinguishes:

- **long silence/resume**: full semantic/menu/game domain reset to UNKNOWN;
- **sink rebound only**: output/deferred playback cleanup while preserving MENU/CHARACTER/WATER/HELM and learned Wwise object affinities.

F11 remains an explicit full reset in both directions.

## 7. Unchanged native-first policy

No plugin haptic is added for:

- native cannon/broadside Motion (`0x2FC9D3AD`, `0x774FC62C`);
- helm enter (`0xDF7BEEAE`, state anchor only);
- player hit (`0x839CE745`);
- pistol (`0x27C0E102`);
- normal/perfect parry;
- native menu tick (`0x67233A85`).

The observed Animus/controller-speaker loot cue is also **not** manufactured by this ASI and is not changed in r2.
