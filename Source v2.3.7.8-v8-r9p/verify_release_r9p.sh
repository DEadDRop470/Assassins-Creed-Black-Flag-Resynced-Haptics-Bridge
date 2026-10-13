#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p build
bash build_linux_pe_r9p.sh
cp build/ACBFHapticsBridge-v8-r9p.asi build/repro-first.asi
bash build_linux_pe_r9p.sh
cmp build/repro-first.asi build/ACBFHapticsBridge-v8-r9p.asi
flags=(-std=c++17 -O2 -Wall -Wextra -Werror -ffreestanding -fno-builtin -fshort-wchar -mno-red-zone -fno-exceptions -fno-rtti)
for harness in test_ds4 test_ds4_hid test_hooks test_startup test_legacy_reading test_output_pipeline; do
  g++ "${flags[@]}" "$harness.cpp" -o "build/$harness"
  if [[ "$harness" != test_output_pipeline ]]; then "build/$harness"; fi
done
if [[ -f ../ShadowsHapticsPack-v5.hfx ]]; then
  build/test_output_pipeline ../ShadowsHapticsPack-v5.hfx
  python3 validate_hfx.py ../ShadowsHapticsPack-v5.hfx
  g++ "${flags[@]}" test_effect_fingerprint.cpp -o build/test_effect_fingerprint
  g++ "${flags[@]}" -DTEST_R9M_REFERENCE test_effect_fingerprint.cpp -o build/test_effect_fingerprint_r9m
  build/test_effect_fingerprint ../ShadowsHapticsPack-v5.hfx > build/r9p-effects.txt
  build/test_effect_fingerprint_r9m ../ShadowsHapticsPack-v5.hfx > build/r9m-effects.txt
  cmp build/r9m-effects.txt build/r9p-effects.txt
  printf '%s\n' 'PASS 48 complete native-audio/motor-stream fingerprints match exact r9m source (14 waveforms + walk/run, each across Xbox, DualSense, DS4).'
else
  printf '%s\n' 'Real-HFX integration requires the unchanged ShadowsHapticsPack-v5.hfx alongside Source.'
fi
g++ "${flags[@]}" -DTEST_R9M_REFERENCE test_legacy_reading.cpp -o build/test_legacy_r9m
build/test_legacy_r9m
g++ "${flags[@]}" -DTEST_R9N_REFERENCE test_legacy_reading.cpp -o build/test_legacy_r9n
if build/test_legacy_r9n; then
  printf '%s\n' 'FAIL: r9n regression fixture unexpectedly acquired the combined-only stream'
  exit 1
else
  result=$?
  if [[ "$result" != 1 ]]; then exit "$result"; fi
  printf '%s\n' 'PASS negative control: exact r9n source rejects a stream accepted by exact r9m and r9p'
fi
python3 audit_r9p.py --source src/main.cpp --ini ACBFHapticsBridge-v8.ini --asi build/ACBFHapticsBridge-v8-r9p.asi
python3 audit_preservation_r9p.py
python3 verify_r9h_baseline.py
if [[ -n "$(nm -u build/main.o)" ]]; then
  printf '%s\n' 'FAIL: unresolved object symbols'; exit 1
fi
printf '%s\n' 'PASS reproducible Windows PE build, controller and full-output harnesses, negative controls, source/INI preservation, no unresolved symbols'
