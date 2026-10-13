#!/usr/bin/env python3
"""Compare every parsed baseline function, allowing only reviewed routing/hook edits."""
from pathlib import Path
import hashlib
import re
import sys
HERE = Path(__file__).resolve().parent
BASE = HERE/'References/v2.3.7.5-r9m-baseline-main.cpp'
NEW = HERE/'src/main.cpp'
ALLOWED = {
    'compat_detect_layout', 'patch_state_vtable', 'install_gameplay_state_hooks',
    'r9k_game_rva_valid', 'r9k_find_xinput_setstate_iat', 'install_game_xinput_observer',
    'gameinput_rumble_observer', 'install_gameinput_rumble_observer',
    'update_gameinput_device', 'poll_gameinput', 'set_gameinput_rumble', 'worker_thread',
    'write_rumble_output',
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
print(f'BASELINE FUNCTIONS: {len(old)}; byte-identical bodies: {unchanged}')
print('REVIEWED CHANGED FUNCTIONS:', ', '.join(changed))
print('ADDED FUNCTIONS:', ', '.join(sorted(k[0] for k in new.keys()-old.keys())))
for start, end in (('struct Config {', 'static Config g_cfg;'),):
    a=BASE.read_text(); b=NEW.read_text()
    a=a[a.index(start):a.index(end)+len(end)]
    b=b[b.index(start):b.index(end)+len(end)]
    if a != b: errors.append('baseline effect configuration/defaults changed')
    else: print('PASS exact r9m effect configuration/defaults')
for filename in ('ACBFHapticsBridge-v8.ini',):
    baseline=HERE/'References'/('v2.3.7.5-r9m-baseline.ini')
    current=HERE/filename
    if baseline.read_bytes()!=current.read_bytes(): errors.append('baseline INI bytes changed')
    else: print('PASS exact baseline INI bytes',hashlib.sha256(current.read_bytes()).hexdigest())
for e in errors: print('FAIL',e)
print('FINAL', 'PASS' if not errors else 'FAIL')
sys.exit(bool(errors))
