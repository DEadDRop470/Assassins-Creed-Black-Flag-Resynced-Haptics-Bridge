#!/usr/bin/env python3
from pathlib import Path
import hashlib, sys
HERE=Path(__file__).resolve().parent
BASE=HERE/'References/v2.3.7.0-r9h-baseline-main.cpp'
NEW=HERE/'src/main.cpp'

def extract(src,sig):
 a=src.index(sig); b=src.index('{',a); depth=0; i=b; ins=inc=line=block=esc=False
 while i<len(src):
  c=src[i]; n=src[i+1] if i+1<len(src) else ''
  if line:
   if c=='\n': line=False
  elif block:
   if c=='*' and n=='/': block=False; i+=1
  elif ins:
   if esc: esc=False
   elif c=='\\': esc=True
   elif c=='"': ins=False
  elif inc:
   if esc: esc=False
   elif c=='\\': esc=True
   elif c=="'": inc=False
  else:
   if c=='/' and n=='/': line=True; i+=1
   elif c=='/' and n=='*': block=True; i+=1
   elif c=='"': ins=True
   elif c=="'": inc=True
   elif c=='{': depth+=1
   elif c=='}':
    depth-=1
    if depth==0:return src[a:i+1]
  i+=1
 raise ValueError(sig)

base=BASE.read_text(); new=NEW.read_text(); ok=True
checks=[
 'static void write_rumble_output',
 'static bool inject_sink_block',
 'static void process_controller_edges',
 'static void process_gameplay_state_signals',
 'static void process_deferred_effects',
]
for sig in checks:
 a=extract(base,sig); b=extract(new,sig); same=a==b; ok &= same
 print(('PASS' if same else 'FAIL'),sig,'base',hashlib.sha256(a.encode()).hexdigest()[:16],'r9k',hashlib.sha256(b.encode()).hexdigest()[:16])
# INI semantic delta: exactly two new Ubisoft fallback keys, no changed old values.
def parse(p):
 sec=''; d={}
 for raw in p.read_text().splitlines():
  x=raw.strip()
  if not x or x.startswith(';'): continue
  if x.startswith('[') and x.endswith(']'): sec=x[1:-1]; continue
  if '=' in x:
   k,v=x.split('=',1); d[(sec,k.strip())]=v.strip()
 return d
old=parse(HERE/'References/v2.3.7.0-r9h-baseline.ini'); cur=parse(HERE/'ACBFHapticsBridge-v8.ini')
added={k:v for k,v in cur.items() if k not in old}; removed={k:v for k,v in old.items() if k not in cur}; changed={k:(old[k],cur[k]) for k in old.keys()&cur.keys() if old[k]!=cur[k]}
expected={("Output","UbisoftDirectDualSenseFallback"):'1',("Output","UbisoftDirectDualSenseFallbackDelayMs"):'1000'}
ini_ok=added==expected and not removed and not changed; ok &= ini_ok
print(('PASS' if ini_ok else 'FAIL'),'INI baseline semantic delta','added=',added,'removed=',removed,'changed=',changed)
print('FINAL','PASS' if ok else 'FAIL'); sys.exit(0 if ok else 1)
