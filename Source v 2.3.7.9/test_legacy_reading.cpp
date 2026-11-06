// One production-ABI fixture; compile unchanged against r9m, r9n and r9o.
#if defined(TEST_R9M_REFERENCE)
#include "References/v2.3.7.5-r9m-baseline-main.cpp"
#elif defined(TEST_R9N_REFERENCE)
#include "References/v2.3.7.6-r9n-baseline-main.cpp"
#else
#include "src/main.cpp"
#endif
extern "C" int puts(const char*);
struct Pad { void** table; u32 refs; };
struct Info { u32 size; u16 vendor, product; u64 padding; };
static void* dt[11]{}; static void* rt[23]{}; static void* it[5]{};
static Pad device{dt,1}, reading{rt,1}, input{it,1};
static Info info{16,0x054C,0x0CE6,0};
static usize WINABI query(LPCVOID p,CompatMemoryInfo* m,usize n) {
    if (!p || n!=sizeof(*m)) return 0;
    *m={};m->BaseAddress=(void*)((u64)p&~0xFFFull);m->RegionSize=4096;
    m->State=MEM_COMMIT;m->Protect=0x20;return sizeof(*m);
}
static BOOL WINABI protect(void*,usize,DWORD,DWORD* old) {*old=0x20;return 1;}
static u32 FASTABI release(void* p) { return --((Pad*)p)->refs; }
static const GameInputDeviceInfoPrefixV0* FASTABI get_info(void*) {
    return (const GameInputDeviceInfoPrefixV0*)&info;
}
static u32 FASTABI status(void*) {return 1;}
static void FASTABI rumble(void*,const GameInputRumbleParamsV0*) {}
static u32 FASTABI kind(void*) {return 0x40004;}
static void FASTABI get_device(void*,void** out) {++device.refs;*out=&device;}
static u32 FASTABI count(void*) {return 15;}
static u32 FASTABI buttons(void*,u32 n,u8* out) {for(u32 i=0;i<n;++i)out[i]=0;return n;}
static u8 FASTABI state(void*,GameInputGamepadStateV0* s) {*s={};s->buttons=4;return 1;}
static i32 FASTABI current(void*,u32 k,void*,void** out) {
    *out=nullptr;
    // Deliberately provide the earlier compatible combined stream, while the
    // narrower Gamepad-only request is unsupported. No transport is assumed.
    if(k!=0x40004) return (i32)0x80004005u;
    ++reading.refs;*out=&reading;return 0;
}
int main() {
    dt[2]=(void*)&release;dt[3]=(void*)&get_info;dt[4]=(void*)&status;dt[10]=(void*)&rumble;
    rt[2]=(void*)&release;rt[3]=(void*)&kind;rt[6]=(void*)&get_device;
    rt[10]=(void*)&count;rt[11]=(void*)&buttons;rt[22]=(void*)&state;
    it[4]=(void*)&current;g_gameInput=&input;
    g_api.VirtualQuery=&query;g_api.VirtualProtect=&protect;
    atomic_store_u64(&g_workerNowTick,1000);
    XINPUT_STATE out{};bool ok=poll_gameinput(out) && out.Gamepad.wButtons==XINPUT_A;
    if(g_gameInputDevice){com_release(g_gameInputDevice);g_gameInputDevice=nullptr;}
    ok &= device.refs==1 && reading.refs==1;
    puts(ok?"PASS legacy combined-only stream acquired and normalized":"REJECTED legacy combined-only stream; no normalized controller output");
    return ok?0:1;
}
