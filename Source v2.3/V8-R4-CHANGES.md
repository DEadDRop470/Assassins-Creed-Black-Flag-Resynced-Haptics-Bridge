# ACBFHapticsBridge v8-r4 changes

## Why r4 exists

v8-r3 restored the gameplay-state airtime path and the supplied test log confirmed it is the better source of truth for jumping/landing: repeated `STATE effect: confirmed jump` / `confirmed ground landing` pairs tracked standing and moving jumps, with measured airtime and no semantic-air timeout failures. r4 keeps that design and focuses on latency, combo coverage, run intensity, water-state behavior, and multi-phase waveform timing.

## 1. Gameplay-state airtime remains authoritative

- Jump/InAir gameplay-state hooks own jump start, airborne duration, and landing.
- Wwise `0x2E795D46`, `0x4E92CCAC`, and `0x43CD17F9` are context hints only.
- Forward/targeted jumps no longer depend on a particular Wwise sound event.
- Landing gain still follows measured airtime.

## 2. Normal parry fallback is 60% earlier

- `ParryProbeDelayMs`: **60 -> 24 ms**.
- Real combat parry native Quad remains first choice.
- If no native feedback appears, the Shadows parry fallback is used.
- The same 24 ms timing applies to DualSense haptics and conventional rumble because both are scheduled from the same effect timeline.
- Perfect-parry event family remains native-only.

## 3. Every quick R1 combo tap can now receive feedback

Exact `0x8ABBEE18` remains the primary mapped light-attack event. The r3 diagnostic log also showed several recurring same-object events around combo chains (`0x68B013E7`, `0xDF786ED3`, `0x0AF7BAEE`, `0xA5EF84E3`, `0xC2D0D50F`, `0x5F259EE8`), but the test does not prove a stable stage number for each ID.

r4 therefore does **not** hard-label those candidates yet. Instead it adds a hybrid safety path:

- R1 rising edge records a candidate attack press.
- If a mapped semantic light/heavy event owns that press, the raw fallback is suppressed.
- If no mapped semantic event owns it, a quick R1 release plays one light-swing fallback.
- A hold that passes the heavy threshold uses the r4 heavy two-phase fallback instead.
- Native feedback arbitration still cancels/suppresses an added copy when Ubisoft combat Motion is present.

This makes combo #2/#3/#4 usable now without pretending the diagnostic Wwise IDs are already proven animation-stage IDs.

## 4. Heavy attack rebuilt as two compact phases

The original `heavy_r2` Shadows capture contains its useful impacts later than Black Flag's animation timing. r4 no longer plays that long internal spacing as-is.

Default phase plan:

- phase 1: source 120 ms, duration 180 ms, gain 1.25, native probe 32 ms
- phase 2: source 235 ms, duration 190 ms, scheduled 100 ms after the attack (plus the same native-probe window), gain 1.00

The first phase is deliberately stronger and the second phase is pulled much earlier.

## 5. Sheathe/unsheathe second phase moved ~30% earlier

- phase 1: source 0 ms, 180 ms duration
- phase 2: source 900 ms, played at 620 ms, 360 ms duration

The waveform content is unchanged; only the timing of the strong second segment is compressed to fit Black Flag better.

Hood ON/OFF continue to share the same hood/equipment waveform and are not phase-split by this change.

## 6. L3 land run becomes continuous

Walk/jog remain driven by the real gait events:

- `0xF4919384` = slow walk family
- `0xC490DFC6` = fast gait / jog-run family

L3 now explicitly arms a continuous land-running texture while Edward is in CHARACTER mode and the stick is above the run threshold.

Defaults:

- first **1.5 s**: `RunOpeningGain=1.90`
- sustained run: `RunBaseGain=1.50`
- jog baseline: ~1.00 event-pulse scale

So sustained L3 run is about **50% stronger than jogging**, with a deliberately larger opening response.

Releasing the movement stick stops the continuous run immediately. `GaitTimeoutMs=450` is only a safety timeout for a lost event stream; it is not a post-stop tail.

XInput run pulse emulation is disabled by default in r4 so conventional rumble follows the same continuous run envelope (`XInputRunPulseEnabled=0`).

## 7. Water / underwater L3 behavior

r4 tracks WATER separately from CHARACTER and adds an underwater latch from the confirmed/candidate water events.

- surface swimming + L3: continuous fast-swim texture while state/input remains valid
- dive candidate `0x23B967BB`: marks underwater context
- water lunge `0x7ED9038C`: immediate lunge haptic + sustained fast-swim texture
- underwater lunge/sprint texture hard-caps at **3000 ms**
- surface `0x731177C1`, swim-exit, or shallow-water entry clears the underwater/lunge latch

This prevents a submerged lunge from turning into an indefinite run/swim loop.


## 7a. HELM remains non-sticky

r4 does **not** lock HELM until a guessed exit event. A verified Edward gameplay-state transition can release it:

- accepted InAir / Jump state -> CHARACTER
- accepted Swimming state -> WATER
- real Edward gait/action events -> CHARACTER
- menu events -> MENU
- long load/desync recovery -> UNKNOWN and reacquire

This preserves the case where cannon damage, a knock-off, desynchronization, or a load genuinely removes Edward from the wheel, while avoiding the r2 false HELM->CHARACTER transition from a shared Wwise jump-like sound ID.

## 8. Command ring enlarged

The r3 F10 status showed haptic queue drops while conventional-rumble queue drops remained zero. r4 increases the internal effect command ring from **64 to 256** entries to reduce losses during bursty action/state transitions.

## 9. Native effects intentionally untouched

No added plugin effect for:

- cannon/broadside native Motion
- player hit native feedback
- pistol native feedback
- perfect parry native family
- native menu tick `0x67233A85`
- helm-enter event (state anchor only)

## 10. Hotkeys

- **F9** — direct dodge preview/test
- **F10** — status report; r4 adds run/underwater/lunge/hybrid-attack/phase counters
- **F11** — complete mod OFF/ON; ON performs a full state/runtime reacquisition
- **F12** — unresolved raw-input fallback layer toggle
