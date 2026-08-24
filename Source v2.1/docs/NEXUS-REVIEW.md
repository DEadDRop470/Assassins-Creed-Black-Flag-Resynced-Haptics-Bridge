# NexusMods review notes

## Review target

This repository corresponds to the NexusMods **v2.1** release whose internal development package was named:

`ACBFHapticsBridge-v7-r7-kick-walk4x-test`

Expected released ASI SHA-256:

`fb77b55728185e453fcc2046716f1d299bb78e92aededae906c9c56db58383b9`

## Why the source is described as a reconstruction

The r7 build was created by narrowly patching the previously compiled r6 runtime. The original full C++/Visual Studio project for the r6 runtime was not present in the material used for the r7 patch.

The repository therefore avoids claiming a false one-to-one original source tree. Instead:

1. `Source/RECONSTRUCTED-R7.cpp` describes the r7-specific behavior at source level.
2. `Source/r7_kick_stub.S` represents the exact injected 28-byte machine-code sequence.
3. `Source/Reconstruction/R7-BINARY-PATCH.md` documents the concrete binary patch points.
4. `Source/Tools/audit_r7.py` verifies the released ASI's hash, PE layout, branch bytes, exact kick stub, trigger threshold, gain pointer, build label, and runtime file paths.

## r7-specific behavior

- R2 must already be held when Square is pressed for the kick feedback path to activate.
- The kick path uses the existing calibrated parry effect/gain.
- Native DualSense walking output is multiplied by 4.00x relative to the original level used by the earlier runtime.
- Conventional rumble walking output is not multiplied by that 4.00x native-only factor.

## Excluded material

The review repository intentionally excludes:

- Assassin's Creed IV game executables;
- proprietary Ubisoft game data/assets;
- runtime logs containing transient memory addresses;
- later r8-r21 experimental builds and diagnostics;
- crash dumps and local test files;
- editor/build-cache files;
- credentials, account information, and machine-specific local paths.

## Reconstruction limitation

This is **not a complete buildable Visual Studio project for the entire ASI**. The full original r6 C++ project was not available when r7 was produced. The repository is intended to make the r7-specific source reconstruction, exact injected code, configuration, and binary-verification evidence reviewable rather than to misrepresent reconstructed material as original full source.
