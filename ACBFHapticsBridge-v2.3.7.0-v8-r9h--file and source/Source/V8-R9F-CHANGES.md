# v8-r9f changes

Runtime: `v8-r9f-long-fall-max-ds-locomotion-menu-tick`

- DualSense menu tick: +30% relative to r9e on bridge fallback and verified Ubisoft-native Quad tick paths.
- DualSense land walk/jog/L3 run: +15% relative to r9e.
- 3.0 s+ landing ceiling: `3.15` on both DualSense and XInput, a literal +125% over r9e's `1.40` ceiling.
- 2-3 s landing gain interpolates toward 3.15; parkour-distance scaling cannot exceed the long-fall ceiling.
- r9d XInput 300 ms action caps and zero reset retained.
- r9c L3-gated underwater lunge retained.
- r9b +75% Ubisoft-native combat haptic supplement retained.
