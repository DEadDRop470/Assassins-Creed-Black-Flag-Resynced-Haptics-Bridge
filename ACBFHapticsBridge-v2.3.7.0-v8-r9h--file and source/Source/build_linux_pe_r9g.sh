#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p build
g++ -c -std=c++17 -O2 -Wall -Wextra \
  -ffreestanding -fno-builtin -fno-exceptions -fno-rtti \
  -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables \
  -fno-threadsafe-statics -fno-use-cxa-atexit -fno-pie -fno-ident \
  -fcf-protection=none -fshort-wchar -mno-red-zone -mcmodel=large \
  -o build/main.o src/main.cpp
ld -mi386pep --shared --entry DllMain --subsystem windows \
  --image-base 0x180000000 --enable-reloc-section --build-id=none \
  --no-insert-timestamp --exclude-all-symbols -s \
  -o build/ACBFHapticsBridge-v8-r9g.asi build/main.o
cp ACBFHapticsBridge-v8.ini build/ACBFHapticsBridge-v8.ini
file build/ACBFHapticsBridge-v8-r9g.asi
sha256sum build/ACBFHapticsBridge-v8-r9g.asi
