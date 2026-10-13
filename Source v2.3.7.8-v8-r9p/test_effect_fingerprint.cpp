// Compare complete rendered sample/motor streams against the exact r9m source.
#ifdef TEST_R9M_REFERENCE
#include "References/v2.3.7.5-r9m-baseline-main.cpp"
#else
#include "src/main.cpp"
#endif
extern "C" int printf(const char*,...);
extern "C" int open(const char*,int,...);
extern "C" long read(int,void*,unsigned long);
extern "C" long lseek(int,long,int);
extern "C" int close(int);
extern "C" void* malloc(unsigned long);
static const char* path=nullptr;static int fd=-1;
static HANDLE WINABI pack_open(const WCHAR*,DWORD access,DWORD,void*,DWORD,DWORD,HANDLE) {
    if(access!=GENERIC_READ)return invalid_handle_value();
    fd=open(path,0);return fd<0?invalid_handle_value():(HANDLE)1;
}
static BOOL WINABI pack_size(HANDLE,LARGE_INTEGER* out) {
    out->QuadPart=lseek(fd,0,2);lseek(fd,0,0);return out->QuadPart>0;
}
static BOOL WINABI pack_read(HANDLE,void* p,DWORD n,DWORD* got,void*) {
    *got=0;while(*got<n){long k=read(fd,(u8*)p+*got,n-*got);if(k<=0)return 0;*got+=(DWORD)k;}return 1;
}
static BOOL WINABI pack_close(HANDLE) {close(fd);return 1;}
static void* WINABI allocate(void*,usize n,DWORD,DWORD) {return malloc((unsigned long)n);}
static void hash_bytes(u64& hash,const void* p,u32 n) {
    const u8* bytes=(const u8*)p;
    for(u32 i=0;i<n;++i){hash^=bytes[i];hash*=1099511628211ull;}
}
static void reset_render(u64 now) {
    clear_playback();clear_rumble_playback();clear_deferred_effects();
    atomic_store_u32(&g_commandWrite,0);atomic_store_u32(&g_commandRead,0);
    atomic_store_u32(&g_rumbleCommandWrite,0);atomic_store_u32(&g_rumbleCommandRead,0);
    atomic_store_u32(&g_cancelGeneration,0);g_audioCancelSeen=g_rumbleCancelSeen=0;
    atomic_store_u64(&g_nativeHapticUntil,0);atomic_store_u64(&g_nativeRumbleUntil,0);
    atomic_store_u64(&g_workerNowTick,now);atomic_store_u32(&g_movementMode,0);
    atomic_store_u32(&g_enabled,1);g_cfg.masterGain=.90f;
    atomic_store_u32(&g_r9LandSprintRumbleStepOnlyActive,0);
}
static u64 render_case(u32 effect,bool movement,u64 now) {
    reset_render(now);u64 hash=14695981039346656037ull;
    if(movement) atomic_store_u32(&g_movementMode,effect==0?1:2);
    else if(!queue_effect((i32)effect,.75f,108u))return 0;
    u32 blocks=movement?750u:(u32)((g_effects[effect].frames+95)/96)+4u;
    u8 sink[0x48]{};float samples[192]{};
    *(float**)(sink+0x30)=samples;*(u16*)(sink+0x40)=96;
    for(u32 block=0;block<blocks;++block) {
        u64 tick=now+(u64)block*2;atomic_store_u64(&g_workerNowTick,tick);
        memset(samples,0,sizeof(samples));*(u16*)(sink+0x42)=0;
        inject_sink_block(sink);RumbleSample motors=render_rumble(96,tick);
        hash_bytes(hash,samples,sizeof(samples));
        hash_bytes(hash,&motors.low,sizeof(float));hash_bytes(hash,&motors.high,sizeof(float));
        u8 active=motors.timelineActive?1:0;hash_bytes(hash,&active,1);
    }
    return hash;
}
int main(int argc,char** argv) {
    if(argc!=2)return 2;
    path=argv[1];
    g_api.CreateFileW=&pack_open;g_api.GetFileSizeEx=&pack_size;g_api.ReadFile=&pack_read;
    g_api.CloseHandle=&pack_close;g_api.VirtualAlloc=&allocate;
    if(!load_haptics_pack())return 3;
    resolve_mappings();
    for(u32 route=0;route<3;++route) {
        atomic_store_u32(&g_activeVendor,route==0?0x045Eu:0x054Cu);
        atomic_store_u32(&g_activeProduct,route==0?0x028Eu:(route==1?0x0CE6u:0x05C4u));
        atomic_store_u32(&g_activeDualSense,route==1?1:0);
        u32 backend=route==0?2:1;
#ifndef TEST_R9M_REFERENCE
        if(route==2)backend=3; // new transport must not change native Sony rendering
#endif
        atomic_store_u32(&g_inputBackend,backend);
        for(u32 i=0;i<g_effectCount;++i)
            printf("route=%u effect=%s fingerprint=%016llx\n",route,g_effects[i].name,render_case(i,false,10000));
        for(u32 mode=0;mode<2;++mode)
            printf("route=%u movement=%u fingerprint=%016llx\n",route,mode,render_case(mode,true,50000));
    }
    return 0;
}
