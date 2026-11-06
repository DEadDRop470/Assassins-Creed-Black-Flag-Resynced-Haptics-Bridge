#!/usr/bin/env python3
"""Compare every parsed baseline function, allowing only reviewed routing/hook edits."""
from pathlib import Path
import hashlib
import re
import sys
HERE = Path(__file__).resolve().parent
BASE = HERE/'References/v2.3.7.8-r9p-baseline-main.cpp'
NEW = HERE/'src/main.cpp'
ALLOWED = {
    'load_config', 'compat_scan', 'preflight_gameplay_state_hooks',
    'install_gameplay_state_hooks', 'r9k_profile_from_xinput_rva',
    'r9k_profile_name', 'r9k_resolve_and_classify_xinput', 'worker_thread',
}

def bodies(source):
    functions = {}
    occurrences = {}
    pattern = re.compile(r'(?m)^(?:static |extern "C" )?(?:const )?[A-Za-z_][\w:<>\s*&]*?\s+([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{')
    for match in pattern.finditer(source):
        name=match.group(1)
        n=occurrences.get(name, 0); occurrences[name]=n+1
        start=match.start(); i=match.end()-1; depth=0
        string=char=line=block=escape=False
        while i<len(source):
            c=source[i]; nxt=source[i+1] if i+1<len(source) else ''
            if line:
                if c=='\n': line=False
            elif block:
                if c=='*' and nxt=='/': block=False; i+=1
            elif string or char:
                if escape: escape=False
                elif c=='\\': escape=True
                elif string and c=='"': string=False
                elif char and c=="'": char=False
            else:
                if c=='/' and nxt=='/': line=True; i+=1
                elif c=='/' and nxt=='*': block=True; i+=1
                elif c=='"': string=True
                elif c=="'": char=True
                elif c=='{': depth+=1
                elif c=='}':
                    depth-=1
                    if depth==0:
                        functions[(name,n)]=source[start:i+1]; break
            i+=1
        else: raise ValueError(name)
    return functions

old=bodies(BASE.read_text()); new=bodies(NEW.read_text())
errors=[]; unchanged=0; changed=[]
for key, body in old.items():
    if key not in new: errors.append(f'missing function {key}'); continue
    if body==new[key]: unchanged+=1
    elif key[0] in ALLOWED: changed.append(key[0])
    else: errors.append(f'unreviewed function changed {key}')
print(f'R9P BASELINE FUNCTIONS: {len(old)}; byte-identical bodies: {unchanged}')
print('REVIEWED CHANGED FUNCTIONS:', ', '.join(changed))
a=BASE.read_text(); b=NEW.read_text()
start='struct Config {';end='static Config g_cfg;'
x=a[a.index(start):a.index(end)+len(end)]
y=b[b.index(start):b.index(end)+len(end)]
y='\n'.join(line for line in y.split('\n') if not any(name in line for name in ('u32 autoDetectTargets =','u32 forceAutoScan =')))
if x!=y:errors.append('existing effect configuration/defaults changed')
else:print('PASS exact previous effect defaults; only compatibility defaults added')
start='static const StateHookTarget kStateHookTargets[] = {'
x=a[a.index(start):a.index('\n};',a.index(start))+3]
y=b[b.index(start):b.index('\n};',b.index(start))+3]
if x!=y:errors.append('legacy 1.06/1.07 state mappings changed')
else:print('PASS exact previous 1.06/1.07 state mappings')
def ini(path):
    section=''; values={}
    for line in path.read_text().splitlines():
        line=line.strip()
        if not line or line.startswith(';'):continue
        if line.startswith('[') and line.endswith(']'):section=line[1:-1]
        elif '=' in line:
            key,value=line.split('=',1);values[(section,key.strip())]=value.strip()
    return values
oldIni=ini(HERE/'References/v2.3.7.5-r9m-baseline.ini');newIni=ini(HERE/'ACBFHapticsBridge-v8.ini')
if any(newIni.get(key)!=value for key,value in oldIni.items()):errors.append('existing INI values changed')
if set(newIni)-set(oldIni)!={('Compatibility','AutoDetectTargets'),('Compatibility','ForceAutoScan')}:errors.append('unexpected added INI values')
if newIni.get(('Compatibility','AutoDetectTargets'))!='1' or newIni.get(('Compatibility','ForceAutoScan'))!='0':errors.append('unexpected compatibility defaults')
if (HERE/'ACBFHapticsBridge-v8.ini').read_bytes()!=(HERE.parent/'ACBFHapticsBridge-v8.ini').read_bytes():errors.append('runtime/source INI templates differ')
else:print('PASS every previous INI value retained; automatic fallback=1, force=0')
expected={
    HERE/'src/ds4_hid.h':'ead1b0ebd65f6ae4f9ead770ea1631899bfdf69ffff4abe093d49644cf5cf78c',
    HERE.parent/'ShadowsHapticsPack-v5.hfx':'9e994203bbafbf0a318f9056066a54b02adad43c0311f77372ea71dec406565f',
    HERE.parent/'version.dll':'6203c5a0ba1f8c5c77c7c12c33be178a5057063ef1da265eb0a546495c6e6a4e',
    BASE:'ad71508f0630c6e301b4a845f1a38d926924cc392a46fd09ba2f4be5bf835f92',
}
for path,digest in expected.items():
    if hashlib.sha256(path.read_bytes()).hexdigest()!=digest:errors.append(f'preserved file changed: {path.name}')
    else:print('PASS preserved bytes:',path.name)
for error in errors:print('FAIL',error)
print('FINAL','FAIL' if errors else 'PASS','r9p preservation audit')
sys.exit(bool(errors))
