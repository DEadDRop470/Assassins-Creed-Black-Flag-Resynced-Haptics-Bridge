# Source reconstruction

`RECONSTRUCTED-R7.cpp` is the readable source-level reconstruction of the r7-specific changes.

`r7_kick_stub.S` preserves the exact 28-byte kick-gate machine-code sequence represented as assembly source.

The `Reconstruction/` directory contains the implementation target and binary-patch documentation.

The `Tools/` directory contains the binary audit script used to verify the expected r7/v2.1 ASI.
