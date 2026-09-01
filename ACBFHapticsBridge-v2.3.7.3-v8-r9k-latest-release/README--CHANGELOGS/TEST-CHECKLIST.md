# v2.3.7.3 / v8-r9k test checklist

## Steam regression test
- Controller connected before launch.
- Confirm log says profile `Steam/voices38` and `directFallbackEligible=0`.
- Confirm there is no `R9K Ubisoft direct DualSense haptics ACTIVE` line.
- Verify haptics feel the same as v2.3.7.0/r9h for walk/run/jump/landing/combat/water/menu.
- Press F10 once; `R9K OUTPUT` should show Steam profile and direct inactive.

## Ubisoft Connect / Ubisoft+
- Confirm correct profile in the log and `directFallbackEligible=1`.
- If native Quad haptics work, direct fallback should remain dormant.
- If native Quad haptics are absent, trigger an HFX action and look for `R9K Ubisoft direct DualSense haptics ACTIVE ... 48000Hz/4ch/16bit actuatorChannels=3/4`.
- Press F10 after a good and bad-feeling state and preserve the complete log.
