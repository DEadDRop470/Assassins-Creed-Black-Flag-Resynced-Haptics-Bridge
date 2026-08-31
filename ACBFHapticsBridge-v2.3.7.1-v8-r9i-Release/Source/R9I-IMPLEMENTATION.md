# v8-r9i implementation notes

## Failure being fixed

The problematic Ubisoft+ run proves that semantic/gameplay detection is alive
while output is not. It reports a DualSense, live controller packets, scheduled
bridge effects and GameInput rumble writes, while `flushCalls`, `injectedBlocks`,
`injectedFrames`, sink pointer and XInput-rumble successes remain zero.

## Direct DualSense fallback

`direct_dualsense.cpp.inc` dynamically resolves `winmm.dll` WaveOut exports at
runtime, preserving the freestanding/no-static-import style of the bridge. It
looks only at controller-audio endpoints whose display name contains
`DualSense` or `Wireless Controller` and validates them by successfully opening
**48,000 Hz / 4-channel / 16-bit PCM extensible** format.

The queued format is:

- channels 1/2: zero normal audio;
- channel 3: existing bridge left haptic waveform;
- channel 4: existing bridge right haptic waveform;
- 480 frames per buffer (10 ms);
- three buffers (30 ms bounded queue).

The renderer consumes the **same existing HFX voices and movement mixer**. It
does not create a second gameplay router or duplicate effect timings.

### Ownership and takeover

`g_hapticOutputOwner` is:

- `0`: no current plugin haptic backend;
- `1`: Black Flag native Quad/Wwise flush;
- `2`: direct DualSense WaveOut fallback.

The fallback can start only when all of these are true:

- bridge enabled;
- `NativeHaptics=1`;
- active GameInput device is classified as DualSense;
- `DirectDualSenseFallback=1`;
- native Quad flush has been silent for the configured delay; and
- actual haptic work is pending.

A native flush atomically sets owner 1. If the previous owner was 2, the first
native flush calls the original Ubisoft function but does not also inject the
plugin waveform into that same block. This removes the dangerous handoff race
between the native audio thread and direct worker. The worker then resets and
closes WaveOut; subsequent native flushes use normal bridge injection again.

The direct renderer keeps a separate cancellation-generation cursor from the
native audio thread.

## Conventional rumble correction

The previous GameInput writer returned `true` after issuing
`SetRumbleState`, even though GameInput v0's method itself returns `void`.
That made a DualSense GameInput call look successful and prevented the existing
XInput fallback from running.

r9i records separately:

- GameInput attempts;
- GameInput calls issued;
- XInput attempts;
- XInput API successes;
- XInput failures;
- XInput route probes / current route availability.

For an active DualSense, an actually connected XInput route is now mirrored.
This targets DSX/Steam Input/ViGEm-style paths without pretending that a void
GameInput call proves physical output.

## Dynamic XInput observer

The observer now parses the loaded ACBlackFlag PE import table for an XInput DLL
and `XInputSetState`. It accepts a named import first and resolved-pointer match
second. Only if import parsing cannot produce a verified slot does it try the
three already audited profile RVAs:

- Steam/legacy profile: `0x1D0119E0`;
- Ubisoft Connect: `0x1C5769E0`;
- Ubisoft+: `0x1EC409E0`.

Every fallback slot still has to equal the resolved `XInputSetState` function
pointer before being modified.

## Diagnostics

F10 keeps the existing status and adds an `R9I OUTPUT` line showing native vs
direct ownership, direct endpoint/open/write counters, nonzero block/frame
counts, peak, GameInput attempts, XInput attempts/successes/failures, and route
probe state.
