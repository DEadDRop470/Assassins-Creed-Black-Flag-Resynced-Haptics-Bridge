#include "src/main.cpp"
extern "C" void* mmap(void*, unsigned long, int, int, int, long);
extern "C" int mprotect(void*, unsigned long, int);
extern "C" int puts(const char*);
static u64 codePages[3] = {0x1000, 0x3000, 0x05028000};
static usize WINABI mock_query(LPCVOID p, CompatMemoryInfo* m, usize n) {
    if (n!=sizeof(*m)) return 0;
    u64 off=(u64)p-(u64)g_gameBase;
    if (off>=g_gameSize) return 0;
    u64 page=off & ~0xFFFull;
    *m={};m->BaseAddress=g_gameBase+page;m->RegionSize=4096;m->State=MEM_COMMIT;
    m->Protect=1;
    for (u64 v: codePages) if (v==page) m->Protect=0x40;
    return sizeof(*m);
}
static bool expect(bool b,const char* s) { if (!b) puts(s);return b; }
int main() {
    g_gameSize=0x05100000;
    g_gameBase=(u8*)mmap(nullptr,g_gameSize,0,0x22,-1,0);
    if ((u64)g_gameBase==~0ull) return 2;
    for(u64 v:codePages) if(mprotect(g_gameBase+v,4096,7)) return 3;
    g_api.VirtualQuery=&mock_query;
    bool ok=true;
    memcpy(g_gameBase+0x1100,kQuadFlushSignature,16);
    ok &= expect(find_unique_exact_rva(kQuadFlushSignature,16)==0x1100,"FAIL unique readable executable match across no-access gaps");
    memcpy(g_gameBase+0x3100,kQuadFlushSignature,16);
    ok &= expect(find_unique_exact_rva(kQuadFlushSignature,16)==0,"FAIL duplicate rejection");
    memset(g_gameBase+0x3100,0,16);
    ok &= expect(!compat_memory_range(0x1FF8,16,true),"FAIL cross-page no-access rejection");
    ok &= expect(!compat_code_matches(0x2000,kQuadFlushSignature,16),"FAIL inaccessible configured RVA rejection");
    CompatMemoryInfo guard{};guard.State=MEM_COMMIT;guard.Protect=0x140;
    ok &= expect(!compat_region_allowed(guard,true),"FAIL guard rejection");
    guard.Protect=4;ok &= expect(!compat_region_allowed(guard,true),"FAIL non-executable rejection");
    u8* post=g_gameBase+0x3200;
    memcpy(post,kV8PostPrefix,21);post[25]=0;post[26]=0x0F;post[27]=0x84;
    memcpy(post+32,kV8PostMiddle,21);memcpy(post+57,kV8PostSuffix,7);
    ok &= expect(find_unique_v8_post_event_rva()==0x3200,"FAIL masked PostEvent discovery");
    memcpy(g_gameBase+QUAD_FLUSH_RVA_107,kQuadFlushSignature,16);
    compat_detect_layout();
    memset(g_gameBase+QUAD_FLUSH_RVA_107,0x90,16);
    ok &= expect(compat_is_107_layout(),"FAIL cached 1.07 layout after flush overwritten");
    compat_detect_layout();
    ok &= expect(!compat_is_107_layout(),"FAIL 1.06-compatible default");
    puts(ok?"PASS startup guard-page/scanner/layout regression tests":"FAIL");
    return ok?0:1;
}
