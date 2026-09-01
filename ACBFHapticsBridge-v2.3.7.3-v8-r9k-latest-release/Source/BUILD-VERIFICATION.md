# Build verification — v2.3.7.3 / v8-r9k

- ASI SHA-256: `99e7862e3dfc8efbb0508394719649de937d2a618b2ed3a27832e445642c761e`
- ASI size: 208384 bytes
- Two clean consecutive builds: byte-for-byte identical.
- PE: AMD64 PE32+ DLL, deterministic COFF timestamp 0, ASLR, NX, High Entropy VA, no RWX sections.
- GNU PE linker emits a 24-byte import directory with an all-zero descriptor; there are no real static imports.
- HFX: 14 effects, 48 kHz stereo, SHA-256 `9e994203bbafbf0a318f9056066a54b02adad43c0311f77372ea71dec406565f`.
- `version.dll` unchanged from r9h: SHA-256 `6203c5a0ba1f8c5c77c7c12c33be178a5057063ef1da265eb0a546495c6e6a4e`.
- INI semantic delta from r9h: exactly two new Ubisoft-only fallback keys; all pre-existing key/value pairs unchanged. `MasterGain=0.90` retained.
- Source baseline verifier PASS: r9h `write_rumble_output`, `inject_sink_block`, `process_controller_edges`, `process_gameplay_state_signals`, and `process_deferred_effects` are source-identical.
- r9k source/INI/PE audit PASS.

Runtime note: v2.3.7.0/r9h is the confirmed working Steam baseline. r9k is built from that baseline and requires live validation after the Ubisoft-isolated changes.
