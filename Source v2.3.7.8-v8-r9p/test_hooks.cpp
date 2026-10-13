// Guard-page-backed tests for the shipping pointer patch and state transactions.
#include "src/main.cpp"
extern "C" void* mmap(void*, unsigned long, int, int, int, long);
extern "C" int mprotect(void*, unsigned long, int);
extern "C" int puts(const char*);
static u64 pages[128]{}; static u32 pageCount = 0;
static bool use107 = false; static u64 failAt = 0, raceAt = 0;
static u32 writes = 0, protectCalls = 0, restoreFailures = 0;
static bool badWritable = false, simulateRestoreFailure = false;
static bool observePublication = false;
static u32 publicationFailures = 0, xinputCalls = 0;
static usize WINABI mock_query(LPCVOID p, CompatMemoryInfo* m, usize n) {
    if (n != sizeof(*m)) return 0;
    u64 a=(u64)p, base=(u64)g_gameBase;
    *m={}; m->BaseAddress=(void*)(a&~0xFFFull); m->RegionSize=4096; m->State=MEM_COMMIT;
    m->Protect=0x20;
    if (a>=base && a<base+g_gameSize) {
        m->Protect=1;
        for (u32 i=0;i<pageCount;++i) if (((a-base) & ~0xFFFull)==pages[i]) m->Protect=0x20;
    }
    return sizeof(*m);
}
static BOOL WINABI mock_protect(void* p, usize n, DWORD mode, DWORD* old) {
    ++protectCalls; *old=0x20;
    if (n!=8) return 0;
    if (mode==PAGE_EXECUTE_READWRITE || mode==PAGE_READWRITE) {
        ++writes; if(mode!=PAGE_EXECUTE_READWRITE) badWritable=true;
        if ((u64)p==failAt) return 0;
        if ((u64)p==raceAt) { *(u64*)p=0x55555555; raceAt=0; }
    } else if (simulateRestoreFailure && restoreFailures++==0) return 0;
    if (observePublication && mode == 0x20 && *(u64*)p == (u64)&game_xinput_set_state_observer) {
        if (!g_gameXInputSetStateOriginal) ++publicationFailures;
        XINPUT_VIBRATION v{};
        if (game_xinput_set_state_observer(0, &v) != 0) ++publicationFailures;
    }
    return 1;
}
static DWORD WINABI mock_xinput(DWORD, XINPUT_VIBRATION*) { ++xinputCalls; return 0; }
static bool expect(bool b,const char* label) {
    puts(b ? label : "FAIL assertion"); if(!b)puts(label); return b;
}
static void add_page(u64 rva) {
    u64 page=rva&~0xFFFull;
    for(u32 i=0;i<pageCount;++i)if(pages[i]==page)return;
    pages[pageCount++]=page; mprotect(g_gameBase+page,4096,7);
}
static void prepare(bool v107) {
    use107=v107; failAt=raceAt=0; writes=protectCalls=0; badWritable=false;
    simulateRestoreFailure=false; restoreFailures=0;
    atomic_store_u32(&g_stateHooksInstalled,0); atomic_store_u32(&g_stateHookFailures,0);
    atomic_store_u32(&g_stateHookRollbacks,0); atomic_store_u32(&g_stateHookRollbackFailures,0);
    atomic_store_u32(&g_pointerProtectionFailures,0);
    g_compatLayoutKnown=true; g_compat107Layout=v107; g_cfg.gameplayStateHooks=1;
    for(const StateHookTarget& t:kStateHookTargets) {
        u64 slot=v107?t.slot107:t.slot106, fn=v107?t.function107:t.function106;
        add_page(slot); add_page(fn); *(u64*)(g_gameBase+slot)=(u64)g_gameBase+fn;
    }
}
static u32 residue() {
    u32 n=0;
    for(const StateHookTarget& t:kStateHookTargets)
        if(*(u64*)(g_gameBase+(use107?t.slot107:t.slot106))==(u64)t.replacement)++n;
    return n;
}
static bool originals_match() {
    for(const StateHookTarget& t:kStateHookTargets)
        if(*(u64*)(g_gameBase+(use107?t.slot107:t.slot106)) !=
            (u64)g_gameBase+(use107?t.function107:t.function106))return false;
    return true;
}
int main() {
    g_gameSize=0x1DE27000;
    g_gameBase=(u8*)mmap(nullptr,g_gameSize,0,0x22,-1,0);
    if((u64)g_gameBase==~0ull)return 2;
    g_api.VirtualQuery=&mock_query; g_api.VirtualProtect=&mock_protect;
    bool ok=true;
    for(u32 version=0;version<2;++version) {
        prepare(version!=0); install_gameplay_state_hooks();
        ok &= expect(residue()==21 && atomic_load_u32(&g_stateHooksInstalled)==21 && !badWritable,
            version?"PASS all 21 verified 1.07 state hooks installed atomically":"PASS all 21 verified 1.06 state hooks installed atomically");
        prepare(version!=0);
        const auto& last=kStateHookTargets[20];
        *(u64*)(g_gameBase+(use107?last.slot107:last.slot106))=0x1234;
        install_gameplay_state_hooks();
        ok &= expect(residue()==0 && writes==0 && atomic_load_u32(&g_stateHookFailures)==1,
            version?"PASS last-slot 1.07 mismatch preflights entire table before mutation":"PASS last-slot 1.06 mismatch preflights entire table before mutation");
        prepare(version!=0); const auto& failure=kStateHookTargets[10];
        failAt=(u64)g_gameBase+(use107?failure.slot107:failure.slot106);
        install_gameplay_state_hooks();
        ok &= expect(residue()==0 && originals_match() && atomic_load_u32(&g_stateHooksInstalled)==0 &&
            atomic_load_u32(&g_stateHookRollbacks)==1 && !atomic_load_u32(&g_stateHookRollbackFailures),
            version?"PASS 1.07 mid-install protection failure rolls back prior ten slots":"PASS 1.06 mid-install protection failure rolls back prior ten slots");
    }
    prepare(false); g_compatLayoutKnown=false; install_gameplay_state_hooks();
    ok &= expect(residue()==0 && writes==0,"PASS unknown layout cannot patch fixed state addresses");
    prepare(false); g_cfg.gameplayStateHooks=0; install_gameplay_state_hooks();
    ok &= expect(residue()==0 && writes==0,"PASS disabled state hooks leave all slots untouched");
    u64 slot=0x1111;
    ok &= expect(!compat_swap_pointer(&slot,0x2222,0x3333) && slot==0x1111,
        "PASS compare-exchange does not overwrite mismatched pointer");
    raceAt=(u64)&slot;
    ok &= expect(!compat_swap_pointer(&slot,0x1111,0x3333) && slot==0x55555555,
        "PASS competing pointer update survives atomic publication rejection");
    slot=0x1111; simulateRestoreFailure=true; restoreFailures=0;
    ok &= expect(!compat_swap_pointer(&slot,0x1111,0x3333) && slot==0x1111 && restoreFailures==2 &&
        atomic_load_u32(&g_pointerProtectionFailures)==1,
        "PASS failed protection restore reverses publication and retries protection");
    simulateRestoreFailure=false;
    ok &= expect(!compat_swap_pointer((u64*)((u8*)&slot+1),0,1),
        "PASS unaligned pointer slot is rejected");
    ok &= expect(!compat_memory_range(0x1000,8,false),"PASS inaccessible image page is rejected");
    // Malformed PE optional header must not walk across a guard page.
    add_page(0); *(u16*)g_gameBase=0x5A4D; *(u32*)(g_gameBase+0x3C)=0xFF0;
    g_xinputSetState=&mock_xinput;
    ok &= expect(!r9k_find_xinput_setstate_iat(nullptr,nullptr),
        "PASS malformed import header rejected before reading inaccessible page");
    // Minimal valid PE imports at the retained, verified 1.06 IAT location.
    memset(g_gameBase, 0, 4096);
    *(u16*)g_gameBase = 0x5A4D; *(u32*)(g_gameBase+0x3C) = 0x100;
    *(u32*)(g_gameBase+0x100) = 0x00004550;
    u8* opt = g_gameBase+0x100+24; *(u16*)opt = 0x20B;
    *(u32*)(opt+0x78) = 0x200; *(u32*)(opt+0x7C) = 40;
    *(u32*)(g_gameBase+0x200) = 0x300;
    *(u32*)(g_gameBase+0x20C) = 0x400;
    *(u32*)(g_gameBase+0x210) = 0x1D0119E0;
    *(u64*)(g_gameBase+0x300) = 0x500;
    memcpy(g_gameBase+0x400, "xinput9_1_0.dll", 15);
    memcpy(g_gameBase+0x502, "XInputSetState", 15);
    add_page(0x1D0119E0);
    u64* iat = (u64*)(g_gameBase+0x1D0119E0); *iat = (u64)&mock_xinput;
    g_cfg.nativeArbitration = 1; g_gameXInputSetStateOriginal = nullptr;
    atomic_store_u32(&g_gameXInputObserverInstalled,0);
    atomic_store_u32(&g_gameXInputObserverFailures,0);
    observePublication = true; install_game_xinput_observer(); observePublication = false;
    ok &= expect(*iat == (u64)&game_xinput_set_state_observer &&
        g_gameXInputSetStateOriginal == &mock_xinput &&
        atomic_load_u32(&g_gameXInputObserverInstalled) == 1 &&
        atomic_load_u32(&g_r9kGameProfile) == R9K_PROFILE_STEAM_FAMILY &&
        atomic_load_u32(&g_r9kXInputResolveMethod) == 1,
        "PASS named-import XInput observer resolves verified profile and installs atomically");
    ok &= expect(publicationFailures == 0 && xinputCalls == 1,
        "PASS game can call XInput hook immediately after publication and reach saved original");
    install_game_xinput_observer();
    ok &= expect(*iat == (u64)&game_xinput_set_state_observer &&
        g_gameXInputSetStateOriginal == &mock_xinput &&
        atomic_load_u32(&g_gameXInputObserverFailures) == 1,
        "PASS repeat XInput installation rejects recursion without replacing saved original");
    puts(ok?"FINAL PASS hook transaction harness":"FINAL FAIL hook transaction harness");
    return ok?0:1;
}
