// Real HFX -> production semantic scheduler -> native sink and motor mixer.
#include "src/main.cpp"
extern "C" int puts(const char*);
extern "C" int open(const char*,int,...);
extern "C" long read(int,void*,unsigned long);
extern "C" long lseek(int,long,int);
extern "C" int close(int);
extern "C" void* malloc(unsigned long);
static int fd=-1;static const char* packPath=nullptr;
static u64 ds4PositiveReports=0,ds4Reports=0;
static BOOL WINABI reset_motor_event(HANDLE) {return 1;}
static BOOL WINABI motor_write(HANDLE h,const void* bytes,DWORD n,DWORD* got,void*) {
    if(h!=(HANDLE)2 || n!=32)return 0;
    const u8* p=(const u8*)bytes;
    if(p[0]!=5 || p[1]!=1)return 0;
    ++ds4Reports;if(p[4] || p[5])++ds4PositiveReports;*got=n;return 1;
}
static HANDLE WINABI file_open(const WCHAR*,DWORD access,DWORD,void*,DWORD,DWORD,HANDLE) {
    if(access!=GENERIC_READ)return invalid_handle_value();
    fd=open(packPath,0);return fd<0?invalid_handle_value():(HANDLE)1;
}
static BOOL WINABI file_size(HANDLE,LARGE_INTEGER* size) {
    size->QuadPart=lseek(fd,0,2);lseek(fd,0,0);return size->QuadPart>0;
}
static BOOL WINABI file_read(HANDLE,void* dst,DWORD n,DWORD* got,void*) {
    *got=0;while(*got<n){long k=read(fd,(u8*)dst+*got,n-*got);if(k<=0)return 0;*got+=(DWORD)k;}return 1;
}
static BOOL WINABI file_close(HANDLE) {close(fd);fd=-1;return 1;}
static void* WINABI allocation(void*,usize n,DWORD,DWORD) {return malloc((unsigned long)n);}
static bool expect(bool b,const char* label) {puts(b?label:"FAIL assertion");if(!b)puts(label);return b;}
static u32 pending() {u32 n=0;for(auto& d:g_deferredEffects)if(d.active)++n;return n;}
static void reset_pipeline(u64 now) {
    clear_deferred_effects();clear_playback();clear_rumble_playback();
    atomic_store_u32(&g_commandWrite,0);atomic_store_u32(&g_commandRead,0);
    atomic_store_u32(&g_rumbleCommandWrite,0);atomic_store_u32(&g_rumbleCommandRead,0);
    atomic_store_u32(&g_cancelGeneration,0);g_audioCancelSeen=g_rumbleCancelSeen=0;
    atomic_store_u64(&g_nativeFeedbackSerial,0);atomic_store_u64(&g_nativeHapticUntil,0);atomic_store_u64(&g_nativeRumbleUntil,0);
    atomic_store_u32(&g_enabled,1);atomic_store_u32(&g_inputEnabled,1);g_cfg.nativeArbitration=1;
    atomic_store_u32(&g_v8RuntimeMode,V8_MODE_UNKNOWN);atomic_store_u64(&g_v8CharacterObject,0);
    atomic_store_u64(&g_workerNowTick,now);
    atomic_store_u32(&g_movementMode,0);g_r9dXInputForceZeroUntil=0;
    g_rumbleOutputWasActive=false;g_lastRumblePhysicalWriteTick=0;
}
static bool render_until(u64 start,u64 end,bool& audio,bool& motors) {
    u64 before=ds4PositiveReports;
    u8 sink[0x48]{};float samples[192]{};
    *(float**)(sink+0x30)=samples;*(u16*)(sink+0x40)=96;
    for(u64 now=start;now<=end;now+=2) {
        atomic_store_u64(&g_workerNowTick,now);
        reset_missing_controller_input(false,now);
        process_deferred_effects(now);
        memset(samples,0,sizeof(samples));*(u16*)(sink+0x42)=0;
        flush_hook(sink);
        for(float f:samples)if(absf(f)>.000001f)audio=true;
        RumbleSample r=render_rumble(96,now);
        if(r.low>0 || r.high>0)motors=true;
        write_rumble_output(r);
    }
    return audio && motors && ds4PositiveReports>before;
}
int main(int argc,char** argv) {
    if(argc!=2) return 2;
    packPath=argv[1];
    g_api.CreateFileW=&file_open;g_api.GetFileSizeEx=&file_size;g_api.ReadFile=&file_read;
    g_api.CloseHandle=&file_close;g_api.VirtualAlloc=&allocation;
    bool ok=expect(load_haptics_pack() && g_effectCount==14,"PASS actual 14-effect HFX pack loaded by production parser");
    resolve_mappings();
    g_api.WriteFile=&motor_write;g_ds4Api.resetEvent=&reset_motor_event;
    g_ds4Active=&g_ds4Hid[0];g_ds4Active->file=(HANDLE)2;g_ds4Active->outputBytes=32;
    g_ds4Active->write.event=(HANDLE)3;
    atomic_store_u32(&g_inputBackend,3);atomic_store_u32(&g_activeVendor,0x054C);
    atomic_store_u32(&g_activeProduct,0x05C4);atomic_store_u32(&g_activeDualSense,0);
    reset_pipeline(1000);
    // A confirmed light attack uses the production 32 ms native-first probe.
    V8SemanticEvent event{};event.id=EVT_LIGHT_ATTACK;event.tick=1000;event.gameObject=0x1234;
    v8_handle_semantic_event(event);
    ok &= expect(pending()>0,"PASS confirmed game event schedules native-first deferred effect");
    atomic_store_u32(&g_menuMode,1);
    for(u64 now=1000;now<1032;now+=2){reset_missing_controller_input(false,now);process_deferred_effects(now);}
    ok &= expect(pending()>0 && atomic_load_u32(&g_menuMode)==1,
        "PASS absent input preserves deferred game effect and semantic menu ownership");
    bool audio=false,motors=false;
    ok &= expect(render_until(1032,1450,audio,motors),
        "PASS actual game-event waveform reaches native Quad and completed DS4 motor reports without input reads");
    reset_pipeline(2000);g_cfg.fxDodge.triggerOffsetMs=40;
    ok &= expect(schedule_configured_effect(g_map.dodge,1.0f,g_cfg.fxDodge,108,2000,0,~0ull,
        g_cfg.dodgeStartTrimMs,g_cfg.semanticDodgeDurationMs) && pending()>0,
        "PASS delayed F9 preview uses production scheduling");
    reset_missing_controller_input(true,2001);
    ok &= expect(pending()>0,"PASS actual input-loss transition preserves delayed F9 preview");
    audio=motors=false;
    ok &= expect(render_until(2002,2400,audio,motors),
        "PASS delayed F9 waveform reaches native audio and completed DS4 reports after missing-input transition");
    reset_pipeline(3000);g_cfg.fxDodge.triggerOffsetMs=0;
    schedule_configured_effect(g_map.dodge,1.0f,g_cfg.fxDodge,108,3000,0,~0ull,
        g_cfg.dodgeStartTrimMs,g_cfg.semanticDodgeDurationMs);
    audio=motors=false;
    ok &= expect(render_until(3000,3300,audio,motors),
        "PASS immediate F9 waveform reaches completed DS4 motor reports without normalized input");
    ok &= expect(ds4Reports>ds4PositiveReports && atomic_load_u64(&g_ds4HidCompleted)==ds4Reports,
        "PASS complete real-HFX pipeline produces motor starts, explicit zeros and transport completion counts");
    puts(ok?"FINAL PASS end-to-end waveform/output regression harness":"FINAL FAIL waveform/output regression harness");
    return ok?0:1;
}
