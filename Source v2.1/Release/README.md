# Release reference

This folder contains the matching r7 configuration used by the v2.1 reconstruction.

For NexusMods binary-to-source review, copy the exact v2.1 release ASI here as:

`ACBFHapticsBridge-v7.asi`

Before committing it, verify that its SHA-256 is:

`fb77b55728185e453fcc2046716f1d299bb78e92aededae906c9c56db58383b9`

Then run from the repository root:

`python Source/Tools/audit_r7.py Release/ACBFHapticsBridge-v7.asi`

Do not substitute a later r8+ development ASI: later builds may share the same filename but have a different hash and behavior.
