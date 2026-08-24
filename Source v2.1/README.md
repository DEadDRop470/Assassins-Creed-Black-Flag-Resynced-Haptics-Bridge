# ACBFHapticsBridge

Source reconstruction and verification material for the **ACBFHapticsBridge v2.1** NexusMods release.

## Release mapping

- NexusMods release: **v2.1**
- Internal build identifier: `ACBFHapticsBridge-v7-r7-kick-walk4x-test`
- Runtime build label: `v7-r7-kick-walk4x`
- Expected ASI SHA-256: `fb77b55728185e453fcc2046716f1d299bb78e92aededae906c9c56db58383b9`
- Target game build used for verification: Assassin's Creed IV: Black Flag TU 1.0.6 (Steam/Ubisoft)

## What this repository contains

```text
ACBFHapticsBridge/
├─ README.md
├─ LICENSE
├─ CHANGELOG.md
├─ .gitignore
├─ Source/
│  ├─ RECONSTRUCTED-R7.cpp
│  ├─ r7_kick_stub.S
│  ├─ Reconstruction/
│  │  ├─ IMPLEMENTATION.md
│  │  └─ R7-BINARY-PATCH.md
│  └─ Tools/
│     └─ audit_r7.py
├─ Release/
│  ├─ ACBFHapticsBridge-v7.ini
│  └─ README.md
└─ docs/
   ├─ NEXUS-REVIEW.md
   └─ PRIVACY-REVIEW.md
```

## Source provenance

The original r7 package was produced as a narrow binary patch on top of the compiled r6 runtime. The complete original C++/Visual Studio source for that earlier compiled runtime was not present in the source material used to produce r7.

For that reason, this repository deliberately does **not** claim that `RECONSTRUCTED-R7.cpp` is the lost original full source of the complete ASI. Instead it provides:

- a source-level reconstruction of the behavior specifically added or changed by r7;
- the exact 28-byte injected R2+Square kick-gate stub as assembly source;
- documented patch locations and behavior;
- an audit script that verifies the known r7 binary against the expected hash and patch bytes;
- the matching r7 configuration file.

This distinction is intentional so reviewers can see exactly what is reconstructed, what is byte-exact, and how the released binary can be verified.

## Main r7 / v2.1 changes

### R2 + Square kick feedback

The r7 patch gates the Square/X raw input path so it activates only when R2 is already held above the configured trigger threshold. The effect uses the existing parry waveform/gain path and avoids Square-only false positives.

### DualSense walking feedback

The native 48 kHz DualSense walking output multiplier was raised from r6's `2.00x` value to `4.00x`. Conventional rumble walking strength is intentionally unchanged.

## Binary verification

The v2.1 ASI intended for review must hash to:

```text
fb77b55728185e453fcc2046716f1d299bb78e92aededae906c9c56db58383b9
```

If you place the released binary at:

```text
Release/ACBFHapticsBridge-v7.asi
```

run:

```powershell
python Source/Tools/audit_r7.py Release/ACBFHapticsBridge-v7.asi
```

A correct r7/v2.1 binary should end with `FINAL PASS`.

## Repository scope

This repository is intentionally limited to the v2.1/r7 reconstruction and its review material. Later r8-r21 experimental diagnostics, runtime logs, user-specific test files, game executables, and unrelated development artifacts are not included.

No Ubisoft game executable or proprietary game asset is included in this repository.
