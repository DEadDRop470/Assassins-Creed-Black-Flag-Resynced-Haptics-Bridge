# Ubisoft executable cross-reference

Verified XInputSetState IAT profiles retained from r9h:
- Steam TU 1.0.6 / voices38 family: `0x1D0119E0` — direct fallback disabled.
- Ubisoft Connect: `0x1C5769E0` — direct fallback eligible.
- Ubisoft+: `0x1EC409E0` — direct fallback eligible.

r9k first resolves `XInputSetState` dynamically from the PE import table. The known RVAs above are used only as pointer-validated fail-closed fallbacks and for profile classification.

The existing Quad flush, Wwise PostEvent and 21 gameplay-state hook targets are inherited from the r9h baseline.
