// Load and relocate a real PE as data. Never execute any game instruction.
#include "src/main.cpp"
extern "C" void* mmap(void*,unsigned long,int,int,int,long);
extern "C" int munmap(void*,unsigned long);
extern "C" int open(const char*,int,...);
extern "C" long lseek(int,long,int);
extern "C" int close(int);
extern "C" int printf(const char*,...);
struct Region {u64 first,last;u32 protection;};
static Region regions[100]{};static u32 regionCount{};
static u64 splitAt{},blockedFirst{},blockedLast{};
static u64 hostCallbackPage{};
static u64 failWritableRva{};
static u32 protectedWrites{},forwardedCalls{};
static usize WINABI query(LPCVOID pointer,CompatMemoryInfo* m,usize n) {
    u64 off=(u64)pointer-(u64)g_gameBase;
    if(n!=sizeof(*m))return 0;
    if(off>=g_gameSize) {
        if(((u64)pointer&~4095ull)!=hostCallbackPage)return 0;
        *m={};m->BaseAddress=(void*)hostCallbackPage;m->RegionSize=4096;
        m->State=MEM_COMMIT;m->Protect=0x20;return sizeof(*m);
    }
    u64 first=0,last=g_gameSize;u32 protection=1;
    for(u32 i=0;i<regionCount;++i) {
        const auto& r=regions[i];
        if(off>=r.first && off<r.last){first=r.first;last=r.last;protection=r.protection;break;}
        if(r.last<=off && r.last>first)first=r.last;
        if(r.first>off && r.first<last)last=r.first;
    }
    if(blockedLast) {
        if(off>=blockedFirst && off<blockedLast){first=blockedFirst;last=blockedLast;protection=1;}
        else if(off<blockedFirst && last>blockedFirst)last=blockedFirst;
        else if(off>=blockedLast && first<blockedLast)first=blockedLast;
    }
    if(splitAt>first && splitAt<last){if(off<splitAt)last=splitAt;else first=splitAt;}
    *m={};m->BaseAddress=g_gameBase+first;m->RegionSize=last-first;
    m->State=MEM_COMMIT;m->Protect=protection;return sizeof(*m);
}
static BOOL WINABI protect(void* p,usize n,DWORD mode,DWORD* old) {
    CompatMemoryInfo m{};if(n!=8 || !query(p,&m,sizeof(m)))return 0;
    *old=m.Protect;if(mode==PAGE_READWRITE || mode==PAGE_EXECUTE_READWRITE) {
        ++protectedWrites;if(failWritableRva && (u64)p-(u64)g_gameBase==failWritableRva)return 0;
    }
    return 1;
}
static DWORD WINABI xinput(DWORD,XINPUT_VIBRATION*){++forwardedCalls;return 0;}
static bool expect(bool value,const char* label) {
    printf("%s %s\n",value?"PASS":"FAIL",label);return value;
}
static bool load_image(const char* path) {
    int fd=open(path,0);if(fd<0)return false;
    long size=lseek(fd,0,2);if(size<4096){close(fd);return false;}
    u8* raw=(u8*)mmap(nullptr,(u64)size,1,2,fd,0);close(fd);
    if((u64)raw==~0ull)return false;
    u32 pe=*(u32*)(raw+0x3C);u8* nt=raw+pe;u8* opt=nt+24;
    if((u64)pe+264>(u64)size || *(u32*)nt!=0x4550 || *(u16*)opt!=0x20B)return false;
    g_gameSize=*(u32*)(opt+0x38);u64 preferred=*(u64*)(opt+0x18);
    g_gameBase=(u8*)mmap(nullptr,g_gameSize,3,0x22,-1,0);
    if((u64)g_gameBase==~0ull)return false;
    u32 headers=*(u32*)(opt+0x3C);if(headers>(u64)size || headers>g_gameSize)return false;
    memcpy(g_gameBase,raw,headers);regions[regionCount++]={0,(headers+4095ull)&~4095ull,2};
    u32 count=*(u16*)(nt+6);u8* table=opt+*(u16*)(nt+20);
    if(count>96 || (u64)(table-raw)+(u64)count*40>(u64)size)return false;
    for(u32 i=0;i<count;++i) {
        u8* s=table+i*40;u64 rva=*(u32*)(s+12),bytes=*(u32*)(s+16),offset=*(u32*)(s+20);
        u64 virtualBytes=*(u32*)(s+8);u32 flags=*(u32*)(s+36);
        if(offset+bytes>(u64)size || rva+bytes>g_gameSize)return false;
        memcpy(g_gameBase+rva,raw+offset,bytes);
        u64 span=virtualBytes>bytes?virtualBytes:bytes;
        u32 permission=(flags&0x20000000u)?0x20:((flags&0x40000000u)?2:1);
        if(span)regions[regionCount++]={rva,(rva+span+4095ull)&~4095ull,permission};
    }
    u32 reloc=*(u32*)(opt+112+5*8),relocBytes=*(u32*)(opt+116+5*8);
    munmap(raw,(u64)size);
    if((u64)reloc+relocBytes>g_gameSize)return false;
    u64 delta=(u64)g_gameBase-preferred,end=(u64)reloc+relocBytes;
    for(u64 at=reloc;at+8<=end;) {
        u8* b=g_gameBase+at;u32 page=*(u32*)b,block=*(u32*)(b+4);
        if(block<8 || at+block>end)return false;
        for(u32 i=8;i+2<=block;i+=2) {
            u16 item=*(u16*)(b+i);u32 type=item>>12;u64 rva=(u64)page+(item&4095);
            if(type==0)continue;
            if(type!=10 || rva+8>g_gameSize)return false;
            *(u64*)(g_gameBase+rva)+=delta;
        }
        at+=block;
    }
    hostCallbackPage=(u64)&xinput&~4095ull;
    g_api.VirtualQuery=&query;g_api.VirtualProtect=&protect;g_xinputSetState=&xinput;
    return true;
}
static bool equals_locations(const CompatStateLocation* expected) {
    if(!g_selectedStateLocations)return false;
    for(u32 i=0;i<21;++i)if(g_selectedStateLocations[i].slot!=expected[i].slot ||
        g_selectedStateLocations[i].function!=expected[i].function)return false;
    return true;
}
static u64 blank_data(u64 length) {
    for(u32 i=0;i<regionCount;++i) {
        const auto& r=regions[i];if(r.protection!=2 || r.last-r.first<length+8192)continue;
        for(u64 p=r.last-length-4096;p>r.first+4096;p-=8) {
            bool blank=true;for(u64 j=0;j<length;++j)if(g_gameBase[p+j]){blank=false;break;}
            if(blank)return p;
        }
    }
    return 0;
}
int main(int argc,char** argv) {
    if(argc!=3 || !load_image(argv[1]))return 2;
    bool version108=argv[2][0]=='8',ok=true;
    CompatStateLocation expected[21]{};
    for(u32 i=0;i<21;++i)expected[i]=version108?kCompat108States[i]:
        CompatStateLocation{kStateHookTargets[i].slot106,kStateHookTargets[i].function106};
    u64 flush=version108?kCompat108_flush_rva:QUAD_FLUSH_RVA_106;
    u64 post=version108?kCompat108_post_event_rva:0x0091DEA0ull;
    g_cfg.forceAutoScan=0;compat_resolve_targets();
    ok &= expect(g_compatTargetMode==(version108?COMPAT_108:COMPAT_106),"actual EXE selects its validated fixed profile");
    g_cfg.forceAutoScan=1;compat_resolve_targets();
    ok &= expect(g_compatTargetMode==COMPAT_AUTO && equals_locations(expected) &&
        g_cfg.flushRva==flush && g_v8PostEventRva==post,"actual EXE automatic scan finds both audio hooks and exact 21/21 state targets");
    install_gameplay_state_hooks();
    ok &= expect(g_stateHooksInstalled==21 && protectedWrites==21,"actual EXE discovered state set installs all 21 hooks");
    for(u32 i=0;i<21;++i)*(u64*)(g_gameBase+expected[i].slot)=(u64)g_gameBase+expected[i].function;
    atomic_store_u32(&g_stateHooksInstalled,0);failWritableRva=expected[10].slot;
    install_gameplay_state_hooks();failWritableRva=0;bool originalPointers=true;
    for(u32 i=0;i<21;++i)if(*(u64*)(g_gameBase+expected[i].slot)!=(u64)g_gameBase+expected[i].function)originalPointers=false;
    ok &= expect(originalPointers && g_stateHooksInstalled==0 && g_stateHookRollbacks==1 && !g_stateHookRollbackFailures,
        "discovered target installation rolls back prior slots after a protection failure");
    // Profile-independent table movement: no address in the profile is updated.
    u64 scratch=blank_data(512);if(!scratch)return 3;
    u64 table=expected[0].slot-56;u8 saved[72];memcpy(saved,g_gameBase+table,72);
    memcpy(g_gameBase+scratch,saved,72);*(u64*)(g_gameBase+expected[0].slot)=0;
    g_cfg.forceAutoScan=0;g_cfg.autoDetectTargets=0;compat_resolve_targets();
    ok &= expect(g_compatTargetMode==COMPAT_NONE && !g_selectedStateLocations,
        "unmatched profile remains inactive when automatic fallback is disabled");
    g_cfg.autoDetectTargets=1;
    compat_resolve_targets();
    ok &= expect(g_compatTargetMode==COMPAT_AUTO && g_selectedStateLocations &&
        g_selectedStateLocations[0].slot==scratch+56 && g_selectedStateLocations[1].slot==scratch+64,
        "relocated gameplay table is discovered without editing a profile");
    g_cfg.forceAutoScan=1;
    // A real page-protection boundary may bisect a table or a code signature.
    splitAt=scratch+64;
    ok &= expect(compat_discover_state_tables() && g_autoStateLocations[0].slot==scratch+56,
        "nine-method validation spans adjacent readable memory regions");splitAt=0;
    memcpy(g_gameBase+table,saved,72);compat_resolve_targets();
    ok &= expect(g_compatTargetMode==COMPAT_NONE && !g_selectedStateLocations && !g_compatLayoutKnown,
        "duplicate valid tables reject the whole discovered state set");
    memset(g_gameBase+scratch,0,72);
    // Keep the entry anchor intact while changing a buffer-field displacement.
    u8 audioField=g_gameBase[flush+0x66];g_gameBase[flush+0x66]^=8;compat_resolve_targets();
    ok &= expect(g_compatTargetMode==COMPAT_NONE && !g_selectedStateLocations,
        "unchanged audio entry anchor cannot approve a changed sink buffer layout");g_gameBase[flush+0x66]=audioField;
    u64 code=compat_follow_code((u64)g_gameBase+expected[0].function);u8 prologue[8];memcpy(prologue,(void*)code,8);
    memset((void*)code,0xCC,8);compat_resolve_targets();
    ok &= expect(g_compatTargetMode==COMPAT_NONE && !g_selectedStateLocations,
        "changed callback instructions reject the incompatible state set");memcpy((void*)code,prologue,8);
    u16 patternId=kCompatStateModels[0].bank[version108?1:0][7];const auto& pattern=kCompatCodePatterns[patternId];
    u32 masked=0;while(masked<pattern.length && pattern.mask[masked])++masked;
    if(masked==pattern.length)return 4;
    ((u8*)code)[masked]^=0x5A;compat_resolve_targets();
    ok &= expect(g_compatTargetMode==COMPAT_AUTO && equals_locations(expected),
        "changed relative address operand preserves opcode and layout matching");((u8*)code)[masked]^=0x5A;
    splitAt=flush+8;
    ok &= expect(find_unique_exact_rva(kQuadFlushSignature,16)==flush,
        "audio signature can cross adjacent executable memory regions");splitAt=0;
    blockedFirst=expected[0].function&~4095ull;blockedLast=blockedFirst+4096;
    ok &= expect(!compat_discover_state_tables(),"unreadable callback page is rejected");blockedFirst=blockedLast=0;
    u32 fullImageSize=g_gameSize;g_gameSize=(u32)(code-(u64)g_gameBase+4);
    ok &= expect(!compat_pattern_matches(code,patternId),"callback pattern cannot extend outside the declared image");g_gameSize=fullImageSize;
    // Move the named import to a different IAT RVA and prove the real observer forwards.
    u64 iatRva=0;u32 method=0;u64* originalIat=r9k_find_xinput_setstate_iat(&iatRva,&method);
    u8* nt=g_gameBase+*(u32*)(g_gameBase+0x3C);u8* opt=nt+24;
    u32 imports=*(u32*)(opt+0x78);bool moved=false;
    for(u64 p=imports;p<(u64)imports+4096;p+=20) {
        u32 first=*(u32*)(g_gameBase+p+16);if(!first)break;
        u32 name=*(u32*)(g_gameBase+p+12);
        if(r9k_ascii_starts_ci((const char*)(g_gameBase+name),"xinput") &&
           iatRva>=first && iatRva<first+64) {
            memcpy(g_gameBase+scratch+128,g_gameBase+first,64);
            *(u32*)(g_gameBase+p+16)=(u32)(scratch+128);iatRva=scratch+128+(iatRva-first);moved=true;break;
        }
    }
    if(!originalIat || !moved)return 5;
    *(u64*)(g_gameBase+iatRva)=(u64)&xinput;g_compatTargetMode=COMPAT_AUTO;
    g_cfg.nativeArbitration=1;install_game_xinput_observer();XINPUT_VIBRATION v{};
    DWORD result=game_xinput_set_state_observer(0,&v);
    ok &= expect(g_gameXInputObserverInstalled==1 && g_r9kGameProfile==R9K_PROFILE_AUTO_VERIFIED &&
        g_r9kXInputIatRva==iatRva && g_r9kXInputResolveMethod==1 && !r9k_ubisoft_profile() &&
        *(u64*)(g_gameBase+iatRva)==(u64)&game_xinput_set_state_observer && result==0 && forwardedCalls==1,
        "relocated named XInput import installs and forwards without enabling Ubisoft-only output");
    *(u64*)(g_gameBase+iatRva)=(u64)&xinput;g_compatTargetMode=COMPAT_NONE;
    atomic_store_u32(&g_gameXInputObserverInstalled,0);install_game_xinput_observer();
    ok &= expect(g_gameXInputObserverInstalled==0 && g_r9kGameProfile==R9K_PROFILE_UNKNOWN &&
        *(u64*)(g_gameBase+iatRva)==(u64)&xinput,"unknown import RVA is rejected when the engine is unverified");
    printf("FINAL %s actual-PE profile / automatic-discovery integration (%s)\n",ok?"PASS":"FAIL",version108?"1.08":"old reference");
    munmap(g_gameBase,g_gameSize);return ok?0:1;
}
