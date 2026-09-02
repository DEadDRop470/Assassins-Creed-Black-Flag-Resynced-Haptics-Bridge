#!/usr/bin/env bash
# Windows/MSYS2 adaptation of build_linux_pe_r9k.sh (same flags, mingw64 toolchain).
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p build
/mingw64/bin/g++ -c -std=c++17 -O2 -Wall -Wextra \
  -ffreestanding -fno-builtin -fno-exceptions -fno-rtti \
  -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables \
  -fno-threadsafe-statics -fno-use-cxa-atexit -fno-pie -fno-ident \
  -fcf-protection=none -fshort-wchar -mno-red-zone -mcmodel=large \
  -o build/main.o src/main.cpp
# mingw codegen emits ___chkstk_ms stack probes for >4 KiB frames; pull just
# that routine from static libgcc (no CRT is linked).
LIBGCC="$(/mingw64/bin/g++ -print-libgcc-file-name)"
/mingw64/bin/ld --shared --entry DllMain --subsystem windows \
  --image-base 0x180000000 --enable-reloc-section --build-id=none \
  --no-insert-timestamp --exclude-all-symbols -s \
  -o build/ACBFHapticsBridge-v8-r9k107.asi build/main.o "$LIBGCC"
cp ACBFHapticsBridge-v8.ini build/ACBFHapticsBridge-v8.ini
sha256sum build/ACBFHapticsBridge-v8-r9k107.asi
