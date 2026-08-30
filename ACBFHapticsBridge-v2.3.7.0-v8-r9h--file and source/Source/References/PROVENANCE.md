# Source provenance

`src/main.cpp` is the exact source used to build ACBFHapticsBridge v8.

The production implementation was reconstructed forward from the full v6-r6 source retained here as `ACBFHapticsBridge-v6-r6-original-main.cpp`, while preserving the later functional tuning that mattered to the project (including the stronger DualSense walk calibration). The v8 semantic event table and routing decisions come from the controlled ACBFStateTracer 2.3/2.4 runtime investigations documented in `EVENT-MAP.md`.

The files under `References/` are reference/baseline source, not the source compiled directly into v8. The compiled source is `src/main.cpp`.

The HFX sample pack is a runtime data asset and is deliberately not described as source code. Object files and ASI binaries are build artifacts, not source.
