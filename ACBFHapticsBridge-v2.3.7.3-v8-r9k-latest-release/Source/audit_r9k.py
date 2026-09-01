#!/usr/bin/env python3
"""Audit ACBFHapticsBridge v2.3.7.3 / v8-r9k source/INI/PE."""
from __future__ import annotations
import argparse, hashlib, struct, sys
from pathlib import Path
RUNTIME=b"v8-r9k-v2.3.7.3-ubisoft-isolated-fallback"
EXPECTED_SHA="99e7862e3dfc8efbb0508394719649de937d2a618b2ed3a27832e445642c761e"

def sha256(p):
 h=hashlib.sha256()
 with open(p,'rb') as f:
  for c in iter(lambda:f.read(1024*1024),b''): h.update(c)
 return h.hexdigest()

def source(p):
 s=Path(p).read_text(); checks={
  'r9k runtime': 'v8-r9k-v2.3.7.3-ubisoft-isolated-fallback' in s,
  'Ubisoft-only profile gate': 'r9k_ubisoft_profile()' in s and 'R9K_PROFILE_UBISOFT_CONNECT' in s and 'R9K_PROFILE_UBISOFT_PLUS' in s,
  'Steam profile excluded from direct gate': 'R9K_PROFILE_STEAM_FAMILY' in s,
  'dynamic XInput resolver': 'r9k_find_xinput_setstate_iat' in s,
  'physical DS cache isolated': 'g_r9kPhysicalDualSense' in s,
  'meaningful native signal timestamp': 'g_r9kLastNativeHapticSignalTick' in s,
  'direct renderer 48k/4ch': 'nChannels=4' in s and 'nSamplesPerSec=48000' in s,
  'actuator lanes 3/4': 'i*4u+2u' in s and 'i*4u+3u' in s,
  'native observer while direct': 'r9k_observe_native_sink_only' in s,
  'baseline rumble mirror comment retained': 'Mirror\n    // the same motor state through XInput for every non-DualSense controller.' in s,
  '21-state installer retained': 'install_gameplay_state_hooks();' in s,
 }
 for k,v in checks.items(): print(('PASS' if v else 'FAIL'),'source',k)
 return all(checks.values())

def ini(p):
 s=Path(p).read_text(); checks={
  'MasterGain=0.90':'MasterGain=0.90' in s,
  'NativeHaptics=1':'NativeHaptics=1' in s,
  'ConventionalRumble=1':'ConventionalRumble=1' in s,
  'RumbleOnDualSense=1':'RumbleOnDualSense=1' in s,
  'Ubisoft fallback=1':'UbisoftDirectDualSenseFallback=1' in s,
  'Ubisoft delay=1000':'UbisoftDirectDualSenseFallbackDelayMs=1000' in s,
 }
 for k,v in checks.items(): print(('PASS' if v else 'FAIL'),'ini',k)
 return all(checks.values())

def pe(p):
 data=Path(p).read_bytes(); ok=True
 def chk(v,n):
  nonlocal ok; print(('PASS' if v else 'FAIL'),'PE',n); ok &= bool(v)
 chk(sha256(p)==EXPECTED_SHA,'expected deterministic SHA-256')
 chk(len(data)>0x100 and data[:2]==b'MZ','MZ');
 if not ok: return False
 po=struct.unpack_from('<I',data,0x3c)[0]; chk(data[po:po+4]==b'PE\0\0','PE signature')
 machine,nsec,timestamp,_,_,optsz,chars=struct.unpack_from('<HHIIIHH',data,po+4)
 chk(machine==0x8664,'AMD64'); chk(bool(chars&0x2000),'DLL'); chk(timestamp==0,'timestamp=0')
 opt=po+24; chk(struct.unpack_from('<H',data,opt)[0]==0x20b,'PE32+')
 dc=struct.unpack_from('<H',data,opt+70)[0]; chk(bool(dc&0x20),'HIGH_ENTROPY_VA'); chk(bool(dc&0x40),'ASLR'); chk(bool(dc&0x100),'NX')
 sec=opt+optsz; rwx=[]
 for i in range(nsec):
  o=sec+i*40; name=data[o:o+8].split(b'\0',1)[0].decode('ascii','replace'); c=struct.unpack_from('<I',data,o+36)[0]
  if c&0x20000000 and c&0x80000000:rwx.append(name)
 chk(not rwx,'no RWX sections')
 chk(RUNTIME in data,'runtime string present')
 return ok

def main():
 ap=argparse.ArgumentParser(); ap.add_argument('--asi',type=Path); ap.add_argument('--source',type=Path); ap.add_argument('--ini',type=Path); a=ap.parse_args(); ok=True
 if a.source: ok &= source(a.source)
 if a.ini: ok &= ini(a.ini)
 if a.asi: ok &= pe(a.asi)
 print('FINAL', 'PASS' if ok else 'FAIL'); return 0 if ok else 1
if __name__=='__main__': raise SystemExit(main())
