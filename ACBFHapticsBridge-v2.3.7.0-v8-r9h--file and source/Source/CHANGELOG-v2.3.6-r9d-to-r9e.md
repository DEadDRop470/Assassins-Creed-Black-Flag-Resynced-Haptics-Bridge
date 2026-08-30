# ACBFHapticsBridge v2.3.6 — v8-r9d -> v8-r9e

- Reduced all bridge-generated DualSense feedback by 10% (`0.90x`).
- Reduced DualSense jump/landing by an additional 15% (`0.85x`; `0.765x` combined with global cleanup).
- Reduced DualSense land walk/jog/run by an additional 10% (`0.90x`; `0.81x` combined).
- Reduced menu L1/R1 by 50% on XInput/Xbox-class controllers.
- Reduced menu L1/R1 by 25% on DualSense.
- Strengthened long-fall landing scaling: 2.0 s = `0.70`, 2.5 s = `1.05`, 3.0 s = `1.40` (2x normal base).
- Retained XInput 300 ms jump/landing/light-attack impacts and explicit motor-zero reset.
- Retained L3-gated underwater lunge, spatial gait, XInput step-running, native combat +75%, auto-water, and parkour-distance scaling.
- Clarified output routing: DualSense haptics and conventional rumble are parallel; strong haptics are limited by `PeakLimit`, not rerouted into rumble.
