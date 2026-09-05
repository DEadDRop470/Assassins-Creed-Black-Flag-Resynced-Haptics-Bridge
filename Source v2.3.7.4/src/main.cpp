// ACBFHapticsBridge v8-r9l / v2.3.7.4 executable compatibility revision
// Built from the complete v6-r6 reconstruction source plus the exact Wwise
// PostEvent hook proven by ACBFStateTracer 2.3/2.4.  v8-r4 keeps confirmed
// actions from raw controller heuristics to real Black Flag gameplay/Wwise events.
// Replays exact 48 kHz stereo float32 haptic waveforms captured from
// Assassin's Creed Shadows through Assassin's Creed Black Flag Resynced's
// native Wwise AkQuadAudioHapticsSink flush path.
// The same timeline is reduced to conventional low/high-motor rumble through
// GameInput v0, with XInput as a fallback.
//
// Freestanding Windows x64 DLL/ASI: no CRT or import libraries are required.

#if !defined(_M_X64) && !defined(__x86_64__)
#error x64 build required
#endif

#if defined(_MSC_VER)
#define WINABI __stdcall
#define FASTABI __fastcall
extern "C" unsigned long long __readgsqword(unsigned long);
#pragma intrinsic(__readgsqword)
#else
#define WINABI __attribute__((ms_abi))
#define FASTABI __attribute__((ms_abi))
#endif

using u8 = unsigned char;
using i8 = signed char;
using u16 = unsigned short;
using i16 = signed short;
using u32 = unsigned int;
using i32 = signed int;
using u64 = unsigned long long;
using i64 = signed long long;
using usize = unsigned long long;
using DWORD = u32;
using WORD = u16;
using SHORT = i16;
using BOOL = i32;
using UINT = u32;
using HANDLE = void*;
using HMODULE = void*;
using LPVOID = void*;
using LPCVOID = const void*;
using WCHAR = wchar_t;
using LPTHREAD_START_ROUTINE = DWORD (WINABI*)(void*);

struct LIST_ENTRY { LIST_ENTRY* Flink; LIST_ENTRY* Blink; };
struct LARGE_INTEGER { i64 QuadPart; };
struct SYSTEMTIME {
    WORD wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds;
};

extern "C" BOOL WINABI DllMain(void*, DWORD, void*);
static void* volatile g_relocAnchor = (void*)&DllMain;

// Keep these local so the PE has no CRT dependency even when the optimizer
// lowers small aggregate copies/clears to memcpy/memset.
extern "C" void* memset(void* dst, int value, usize size) {
    u8* d = (u8*)dst;
    for (usize i = 0; i < size; ++i) d[i] = (u8)value;
    return dst;
}
extern "C" void* memcpy(void* dst, const void* src, usize size) {
    u8* d = (u8*)dst;
    const u8* s = (const u8*)src;
    for (usize i = 0; i < size; ++i) d[i] = s[i];
    return dst;
}
extern "C" int memcmp(const void* a, const void* b, usize size) {
    const u8* x = (const u8*)a;
    const u8* y = (const u8*)b;
    for (usize i = 0; i < size; ++i) {
        if (x[i] != y[i]) return x[i] < y[i] ? -1 : 1;
    }
    return 0;
}
extern "C" { int _fltused = 0; }

#if defined(_MSC_VER)
extern "C" long _InterlockedExchange(long volatile*, long);
extern "C" long _InterlockedExchangeAdd(long volatile*, long);
extern "C" long _InterlockedCompareExchange(long volatile*, long, long);
extern "C" long _InterlockedOr(long volatile*, long);
extern "C" long _InterlockedAnd(long volatile*, long);
extern "C" long long _InterlockedExchange64(long long volatile*, long long);
extern "C" long long _InterlockedExchangeAdd64(long long volatile*, long long);
extern "C" long long _InterlockedCompareExchange64(long long volatile*, long long, long long);
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedOr)
#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedExchange64)
#pragma intrinsic(_InterlockedExchangeAdd64)
#pragma intrinsic(_InterlockedCompareExchange64)
static u32 atomic_load_u32(volatile u32* p) {
    return (u32)_InterlockedCompareExchange((volatile long*)p, 0, 0);
}
static void atomic_store_u32(volatile u32* p, u32 v) {
    (void)_InterlockedExchange((volatile long*)p, (long)v);
}
static u32 atomic_exchange_u32(volatile u32* p, u32 v) {
    return (u32)_InterlockedExchange((volatile long*)p, (long)v);
}
static u32 atomic_fetch_add_u32(volatile u32* p, u32 v) {
    return (u32)_InterlockedExchangeAdd((volatile long*)p, (long)v);
}
static u32 atomic_fetch_or_u32(volatile u32* p, u32 v) {
    return (u32)_InterlockedOr((volatile long*)p, (long)v);
}
static u32 atomic_fetch_and_u32(volatile u32* p, u32 v) {
    return (u32)_InterlockedAnd((volatile long*)p, (long)v);
}
static u64 atomic_load_u64(volatile u64* p) {
    return (u64)_InterlockedCompareExchange64((volatile long long*)p, 0, 0);
}
static void atomic_store_u64(volatile u64* p, u64 v) {
    (void)_InterlockedExchange64((volatile long long*)p, (long long)v);
}
static u64 atomic_fetch_add_u64(volatile u64* p, u64 v) {
    return (u64)_InterlockedExchangeAdd64((volatile long long*)p, (long long)v);
}
static bool atomic_cas_u64(volatile u64* p, u64 expected, u64 desired) {
    return (u64)_InterlockedCompareExchange64((volatile long long*)p,
                                               (long long)desired,
                                               (long long)expected) == expected;
}
#else
static u32 atomic_load_u32(volatile u32* p) {
    return __atomic_load_n(p, __ATOMIC_ACQUIRE);
}
static void atomic_store_u32(volatile u32* p, u32 v) {
    __atomic_store_n(p, v, __ATOMIC_RELEASE);
}
static u32 atomic_exchange_u32(volatile u32* p, u32 v) {
    return __atomic_exchange_n(p, v, __ATOMIC_ACQ_REL);
}
static u32 atomic_fetch_add_u32(volatile u32* p, u32 v) {
    return __atomic_fetch_add(p, v, __ATOMIC_RELAXED);
}
static u32 atomic_fetch_or_u32(volatile u32* p, u32 v) {
    return __atomic_fetch_or(p, v, __ATOMIC_ACQ_REL);
}
static u32 atomic_fetch_and_u32(volatile u32* p, u32 v) {
    return __atomic_fetch_and(p, v, __ATOMIC_ACQ_REL);
}
static u64 atomic_load_u64(volatile u64* p) {
    return __atomic_load_n(p, __ATOMIC_ACQUIRE);
}
static void atomic_store_u64(volatile u64* p, u64 v) {
    __atomic_store_n(p, v, __ATOMIC_RELEASE);
}
static u64 atomic_fetch_add_u64(volatile u64* p, u64 v) {
    return __atomic_fetch_add(p, v, __ATOMIC_RELAXED);
}
static bool atomic_cas_u64(volatile u64* p, u64 expected, u64 desired) {
    return __atomic_compare_exchange_n(p, &expected, desired, false,
                                       __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
}
#endif


static u64 read_gs_qword(u32 offset) {
#if defined(_MSC_VER)
    return __readgsqword(offset);
#else
    u64 value;
    __asm__ volatile("movq %%gs:(%1), %0" : "=r"(value) : "r"((u64)offset));
    return value;
#endif
}

static constexpr DWORD DLL_PROCESS_ATTACH = 1;
static constexpr DWORD DLL_PROCESS_DETACH = 0;
static constexpr DWORD PAGE_READWRITE = 0x04;
static constexpr DWORD PAGE_EXECUTE_READWRITE = 0x40;
static constexpr DWORD MEM_COMMIT = 0x1000;
static constexpr DWORD MEM_RESERVE = 0x2000;
static constexpr DWORD GENERIC_READ = 0x80000000u;
static constexpr DWORD GENERIC_WRITE = 0x40000000u;
static constexpr DWORD FILE_APPEND_DATA = 0x00000004u;
static constexpr DWORD FILE_SHARE_READ = 0x00000001u;
static constexpr DWORD FILE_SHARE_WRITE = 0x00000002u;
static constexpr DWORD CREATE_ALWAYS = 2;
static constexpr DWORD OPEN_EXISTING = 3;
static constexpr DWORD OPEN_ALWAYS = 4;
static constexpr DWORD FILE_ATTRIBUTE_NORMAL = 0x00000080u;
static constexpr DWORD ERROR_SUCCESS = 0;

static constexpr i32 VK_F9 = 0x78;
static constexpr i32 VK_F10 = 0x79;
static constexpr i32 VK_F11 = 0x7A;
static constexpr i32 VK_F12 = 0x7B;

static HANDLE invalid_handle_value() { return (HANDLE)(~(usize)0); }

using PFN_VirtualAlloc = void* (WINABI*)(void*, usize, DWORD, DWORD);
using PFN_VirtualProtect = BOOL (WINABI*)(void*, usize, DWORD, DWORD*);
using PFN_FlushInstructionCache = BOOL (WINABI*)(HANDLE, LPCVOID, usize);
using PFN_CreateThread = HANDLE (WINABI*)(void*, usize, LPTHREAD_START_ROUTINE, void*, DWORD, DWORD*);
using PFN_Sleep = void (WINABI*)(DWORD);
using PFN_GetAsyncKeyState = SHORT (WINABI*)(i32);
using PFN_GetTickCount64 = u64 (WINABI*)();
using PFN_QueryPerformanceCounter = BOOL (WINABI*)(LARGE_INTEGER*);
using PFN_QueryPerformanceFrequency = BOOL (WINABI*)(LARGE_INTEGER*);
using PFN_GetCurrentThreadId = DWORD (WINABI*)();
using PFN_GetLocalTime = void (WINABI*)(SYSTEMTIME*);
using PFN_CreateFileW = HANDLE (WINABI*)(const WCHAR*, DWORD, DWORD, void*, DWORD, DWORD, HANDLE);
using PFN_ReadFile = BOOL (WINABI*)(HANDLE, void*, DWORD, DWORD*, void*);
using PFN_WriteFile = BOOL (WINABI*)(HANDLE, const void*, DWORD, DWORD*, void*);
using PFN_GetFileSizeEx = BOOL (WINABI*)(HANDLE, LARGE_INTEGER*);
using PFN_CloseHandle = BOOL (WINABI*)(HANDLE);
using PFN_DeleteFileW = BOOL (WINABI*)(const WCHAR*);
using PFN_GetPrivateProfileStringW = DWORD (WINABI*)(const WCHAR*, const WCHAR*, const WCHAR*, WCHAR*, DWORD, const WCHAR*);
using PFN_LoadLibraryW = HMODULE (WINABI*)(const WCHAR*);

struct Api {
    PFN_VirtualAlloc VirtualAlloc{};
    PFN_VirtualProtect VirtualProtect{};
    PFN_FlushInstructionCache FlushInstructionCache{};
    PFN_CreateThread CreateThread{};
    PFN_Sleep Sleep{};
    PFN_GetAsyncKeyState GetAsyncKeyState{};
    PFN_GetTickCount64 GetTickCount64{};
    PFN_QueryPerformanceCounter QueryPerformanceCounter{};
    PFN_QueryPerformanceFrequency QueryPerformanceFrequency{};
    PFN_GetCurrentThreadId GetCurrentThreadId{};
    PFN_GetLocalTime GetLocalTime{};
    PFN_CreateFileW CreateFileW{};
    PFN_ReadFile ReadFile{};
    PFN_WriteFile WriteFile{};
    PFN_GetFileSizeEx GetFileSizeEx{};
    PFN_CloseHandle CloseHandle{};
    PFN_DeleteFileW DeleteFileW{};
    PFN_GetPrivateProfileStringW GetPrivateProfileStringW{};
    PFN_LoadLibraryW LoadLibraryW{};
};
static Api g_api;

static bool atomic_max_u64(volatile u64* p, u64 value) {
    u64 current = atomic_load_u64(p);
    while (value > current) {
        if (atomic_cas_u64(p, current, value)) return true;
        current = atomic_load_u64(p);
    }
    return false;
}

static volatile u64 g_perfQpcFrequency{};
static volatile u64 g_perfWorkerMaxGapUs{};
static volatile u64 g_perfWorkerGapGt16Ms{};
static volatile u64 g_perfWorkerGapGt25Ms{};
static volatile u64 g_perfWorkerGapGt50Ms{};
static volatile u64 g_perfGameInputMaxUs{};
static volatile u64 g_perfGameInputGt4Ms{};
static volatile u64 g_perfGameInputGt8Ms{};
static volatile u64 g_perfGameInputGt16Ms{};
static volatile u64 g_perfRumbleWriteMaxUs{};
static volatile u64 g_perfRumbleWriteGt4Ms{};
static volatile u64 g_perfRumbleWriteGt8Ms{};
static volatile u64 g_perfRumbleWriteGt16Ms{};
static volatile u64 g_perfAudioHookMaxUs{};
static volatile u64 g_perfAudioHookGt1Ms{};
static volatile u64 g_perfAudioHookGt2Ms{};
static volatile u64 g_perfAudioHookGt5Ms{};
static volatile u64 g_perfDeviceIdentityRefreshes{};
static volatile u64 g_perfDeviceStatusRefreshes{};
static volatile u64 g_perfRawButtonCountQueries{};
static volatile u64 g_perfSemanticMenuStickSkips{};
static volatile u64 g_perfRumbleCadenceSkips{};

static u64 perf_counter_now() {
    if (!g_api.QueryPerformanceCounter) return 0;
    LARGE_INTEGER value{};
    return g_api.QueryPerformanceCounter(&value) ? (u64)value.QuadPart : 0;
}

static u64 perf_elapsed_us(u64 start, u64 end) {
    u64 frequency = atomic_load_u64(&g_perfQpcFrequency);
    if (!start || !end || end < start || !frequency) return 0;
    u64 delta = end - start;
    return (delta * 1000000ull) / frequency;
}

static void perf_record_worker_gap(u64 us) {
    if (!us) return;
    atomic_max_u64(&g_perfWorkerMaxGapUs, us);
    if (us > 16000ull) atomic_fetch_add_u64(&g_perfWorkerGapGt16Ms, 1);
    if (us > 25000ull) atomic_fetch_add_u64(&g_perfWorkerGapGt25Ms, 1);
    if (us > 50000ull) atomic_fetch_add_u64(&g_perfWorkerGapGt50Ms, 1);
}

static void perf_record_gameinput(u64 us) {
    if (!us) return;
    atomic_max_u64(&g_perfGameInputMaxUs, us);
    if (us > 4000ull) atomic_fetch_add_u64(&g_perfGameInputGt4Ms, 1);
    if (us > 8000ull) atomic_fetch_add_u64(&g_perfGameInputGt8Ms, 1);
    if (us > 16000ull) atomic_fetch_add_u64(&g_perfGameInputGt16Ms, 1);
}

static void perf_record_rumble_write(u64 us) {
    if (!us) return;
    atomic_max_u64(&g_perfRumbleWriteMaxUs, us);
    if (us > 4000ull) atomic_fetch_add_u64(&g_perfRumbleWriteGt4Ms, 1);
    if (us > 8000ull) atomic_fetch_add_u64(&g_perfRumbleWriteGt8Ms, 1);
    if (us > 16000ull) atomic_fetch_add_u64(&g_perfRumbleWriteGt16Ms, 1);
}

static void perf_record_audio_hook(u64 us) {
    if (!us) return;
    atomic_max_u64(&g_perfAudioHookMaxUs, us);
    if (us > 1000ull) atomic_fetch_add_u64(&g_perfAudioHookGt1Ms, 1);
    if (us > 2000ull) atomic_fetch_add_u64(&g_perfAudioHookGt2Ms, 1);
    if (us > 5000ull) atomic_fetch_add_u64(&g_perfAudioHookGt5Ms, 1);
}


static u8* g_gameBase{};
static u32 g_gameSize{};
static volatile u32 g_running = 1;
static volatile u32 g_enabled = 1;
static volatile u32 g_inputEnabled = 1;

static constexpr WCHAR kIniPath[] = L".\\ACBFHapticsBridge-v8.ini";
static constexpr WCHAR kLogPath[] = L".\\ACBFHapticsBridge-v8.log";

static usize cstrlen(const char* s) {
    usize n = 0;
    if (!s) return 0;
    while (s[n]) ++n;
    return n;
}
static char lower_ascii(char c) {
    return c >= 'A' && c <= 'Z' ? (char)(c + ('a' - 'A')) : c;
}
static WCHAR lower_wide(WCHAR c) {
    return c >= L'A' && c <= L'Z' ? (WCHAR)(c + (L'a' - L'A')) : c;
}
static bool streq(const char* a, const char* b) {
    if (!a || !b) return false;
    while (*a && *b) {
        if (*a++ != *b++) return false;
    }
    return *a == *b;
}
static bool streq_ci(const char* a, const char* b) {
    if (!a || !b) return false;
    while (*a && *b) {
        if (lower_ascii(*a++) != lower_ascii(*b++)) return false;
    }
    return *a == *b;
}

static bool module_name_eq(const WCHAR* w, u16 byteLen, const char* a) {
    if (!w || !a) return false;
    usize wn = byteLen / 2;
    usize an = cstrlen(a);
    usize coreAn = an;
    if (an >= 4 && lower_ascii(a[an - 4]) == '.' && lower_ascii(a[an - 3]) == 'd' &&
        lower_ascii(a[an - 2]) == 'l' && lower_ascii(a[an - 1]) == 'l') coreAn = an - 4;
    usize coreWn = wn;
    if (wn >= 4 && w[wn - 4] == L'.' && lower_wide(w[wn - 3]) == L'd' &&
        lower_wide(w[wn - 2]) == L'l' && lower_wide(w[wn - 1]) == L'l') coreWn = wn - 4;
    if (coreWn != coreAn) return false;
    for (usize i = 0; i < coreAn; ++i) {
        if ((char)lower_wide(w[i]) != lower_ascii(a[i])) return false;
    }
    return true;
}

static u8* find_module_ascii(const char* name) {
    u8* peb = (u8*)read_gs_qword(0x60);
    if (!peb) return nullptr;
    u8* ldr = *(u8**)(peb + 0x18);
    if (!ldr) return nullptr;
    LIST_ENTRY* head = (LIST_ENTRY*)(ldr + 0x20);
    for (LIST_ENTRY* p = head->Flink; p && p != head; p = p->Flink) {
        u8* ent = (u8*)p - 0x10;
        u8* base = *(u8**)(ent + 0x30);
        u16 len = *(u16*)(ent + 0x58);
        WCHAR* buf = *(WCHAR**)(ent + 0x60);
        if (base && module_name_eq(buf, len, name)) return base;
    }
    return nullptr;
}

static void* resolve_export_depth(u8* base, const char* name, int depth) {
    if (!base || !name || depth > 5 || *(u16*)base != 0x5A4D) return nullptr;
    u32 peoff = *(u32*)(base + 0x3C);
    u8* nt = base + peoff;
    if (*(u32*)nt != 0x00004550) return nullptr;
    u8* opt = nt + 24;
    if (*(u16*)opt != 0x20B) return nullptr;
    u32 expRva = *(u32*)(opt + 0x70);
    u32 expSize = *(u32*)(opt + 0x74);
    if (!expRva || !expSize) return nullptr;
    u8* exp = base + expRva;
    u32 nFuncs = *(u32*)(exp + 20);
    u32 nNames = *(u32*)(exp + 24);
    u32* funcs = (u32*)(base + *(u32*)(exp + 28));
    u32* names = (u32*)(base + *(u32*)(exp + 32));
    u16* ords = (u16*)(base + *(u32*)(exp + 36));
    for (u32 i = 0; i < nNames; ++i) {
        const char* nm = (const char*)(base + names[i]);
        if (!streq(nm, name)) continue;
        u16 ord = ords[i];
        if (ord >= nFuncs) return nullptr;
        u32 frva = funcs[ord];
        if (frva >= expRva && frva < expRva + expSize) {
            const char* fwd = (const char*)(base + frva);
            char mod[64]{};
            char sym[96]{};
            usize mi = 0, si = 0;
            const char* dot = nullptr;
            for (const char* q = fwd; *q; ++q) {
                if (*q == '.') { dot = q; break; }
            }
            if (!dot) return nullptr;
            for (const char* q = fwd; q < dot && mi < sizeof(mod) - 5; ++q) mod[mi++] = *q;
            mod[mi++] = '.'; mod[mi++] = 'd'; mod[mi++] = 'l'; mod[mi++] = 'l'; mod[mi] = 0;
            for (const char* q = dot + 1; *q && si < sizeof(sym) - 1; ++q) sym[si++] = *q;
            sym[si] = 0;
            if (sym[0] == '#') return nullptr;
            u8* module = find_module_ascii(mod);
            if (!module && g_api.LoadLibraryW) {
                WCHAR wmod[64]{};
                usize j = 0;
                for (; mod[j] && j < 63; ++j) wmod[j] = (WCHAR)(u8)mod[j];
                module = (u8*)g_api.LoadLibraryW(wmod);
            }
            return resolve_export_depth(module, sym, depth + 1);
        }
        return base + frva;
    }
    return nullptr;
}
static void* resolve_export(u8* base, const char* name) {
    return resolve_export_depth(base, name, 0);
}

static void* resolve_export_ordinal_depth(u8* base, u32 ordinal, int depth) {
    if (!base || depth > 5 || *(u16*)base != 0x5A4D) return nullptr;
    u8* nt = base + *(u32*)(base + 0x3C);
    if (*(u32*)nt != 0x00004550) return nullptr;
    u8* opt = nt + 24;
    if (*(u16*)opt != 0x20B) return nullptr;
    u32 expRva = *(u32*)(opt + 0x70);
    u32 expSize = *(u32*)(opt + 0x74);
    if (!expRva || !expSize) return nullptr;
    u8* exp = base + expRva;
    u32 ordinalBase = *(u32*)(exp + 16);
    u32 nFuncs = *(u32*)(exp + 20);
    if (ordinal < ordinalBase || ordinal - ordinalBase >= nFuncs) return nullptr;
    u32* funcs = (u32*)(base + *(u32*)(exp + 28));
    u32 frva = funcs[ordinal - ordinalBase];
    if (frva < expRva || frva >= expRva + expSize) return base + frva;

    const char* fwd = (const char*)(base + frva);
    char mod[64]{};
    char sym[96]{};
    usize mi = 0, si = 0;
    const char* dot = nullptr;
    for (const char* q = fwd; *q; ++q) if (*q == '.') { dot = q; break; }
    if (!dot) return nullptr;
    for (const char* q = fwd; q < dot && mi < sizeof(mod) - 5; ++q) mod[mi++] = *q;
    mod[mi++] = '.'; mod[mi++] = 'd'; mod[mi++] = 'l'; mod[mi++] = 'l'; mod[mi] = 0;
    for (const char* q = dot + 1; *q && si < sizeof(sym) - 1; ++q) sym[si++] = *q;
    sym[si] = 0;
    u8* module = find_module_ascii(mod);
    if (!module && g_api.LoadLibraryW) {
        WCHAR wmod[64]{};
        usize j = 0;
        for (; mod[j] && j < 63; ++j) wmod[j] = (WCHAR)(u8)mod[j];
        module = (u8*)g_api.LoadLibraryW(wmod);
    }
    if (!module) return nullptr;
    if (sym[0] == '#') {
        u32 targetOrdinal = 0;
        for (u32 i = 1; sym[i] >= '0' && sym[i] <= '9'; ++i)
            targetOrdinal = targetOrdinal * 10u + (u32)(sym[i] - '0');
        return targetOrdinal ? resolve_export_ordinal_depth(module, targetOrdinal, depth + 1) : nullptr;
    }
    return resolve_export_depth(module, sym, depth + 1);
}

static void* resolve_export_ordinal(u8* base, u32 ordinal) {
    return resolve_export_ordinal_depth(base, ordinal, 0);
}

template<class T> static T api_cast(void* p) {
    union { void* p; T f; } u{};
    u.p = p;
    return u.f;
}

static bool init_api() {
    u8* k32 = find_module_ascii("kernel32.dll");
    if (!k32) return false;
    g_api.LoadLibraryW = api_cast<PFN_LoadLibraryW>(resolve_export(k32, "LoadLibraryW"));
    g_api.VirtualAlloc = api_cast<PFN_VirtualAlloc>(resolve_export(k32, "VirtualAlloc"));
    g_api.VirtualProtect = api_cast<PFN_VirtualProtect>(resolve_export(k32, "VirtualProtect"));
    g_api.FlushInstructionCache = api_cast<PFN_FlushInstructionCache>(resolve_export(k32, "FlushInstructionCache"));
    g_api.CreateThread = api_cast<PFN_CreateThread>(resolve_export(k32, "CreateThread"));
    g_api.Sleep = api_cast<PFN_Sleep>(resolve_export(k32, "Sleep"));
    g_api.GetTickCount64 = api_cast<PFN_GetTickCount64>(resolve_export(k32, "GetTickCount64"));
    g_api.QueryPerformanceCounter = api_cast<PFN_QueryPerformanceCounter>(resolve_export(k32, "QueryPerformanceCounter"));
    g_api.QueryPerformanceFrequency = api_cast<PFN_QueryPerformanceFrequency>(resolve_export(k32, "QueryPerformanceFrequency"));
    g_api.GetCurrentThreadId = api_cast<PFN_GetCurrentThreadId>(resolve_export(k32, "GetCurrentThreadId"));
    g_api.GetLocalTime = api_cast<PFN_GetLocalTime>(resolve_export(k32, "GetLocalTime"));
    g_api.CreateFileW = api_cast<PFN_CreateFileW>(resolve_export(k32, "CreateFileW"));
    g_api.ReadFile = api_cast<PFN_ReadFile>(resolve_export(k32, "ReadFile"));
    g_api.WriteFile = api_cast<PFN_WriteFile>(resolve_export(k32, "WriteFile"));
    g_api.GetFileSizeEx = api_cast<PFN_GetFileSizeEx>(resolve_export(k32, "GetFileSizeEx"));
    g_api.CloseHandle = api_cast<PFN_CloseHandle>(resolve_export(k32, "CloseHandle"));
    g_api.DeleteFileW = api_cast<PFN_DeleteFileW>(resolve_export(k32, "DeleteFileW"));
    g_api.GetPrivateProfileStringW = api_cast<PFN_GetPrivateProfileStringW>(resolve_export(k32, "GetPrivateProfileStringW"));

    u8* user = find_module_ascii("user32.dll");
    if (!user && g_api.LoadLibraryW) user = (u8*)g_api.LoadLibraryW(L"user32.dll");
    if (user) g_api.GetAsyncKeyState = api_cast<PFN_GetAsyncKeyState>(resolve_export(user, "GetAsyncKeyState"));

    if (g_api.QueryPerformanceFrequency) {
        LARGE_INTEGER frequency{};
        if (g_api.QueryPerformanceFrequency(&frequency) && frequency.QuadPart > 0)
            atomic_store_u64(&g_perfQpcFrequency, (u64)frequency.QuadPart);
    }

    return g_api.VirtualAlloc && g_api.VirtualProtect && g_api.FlushInstructionCache &&
           g_api.CreateThread && g_api.Sleep && g_api.GetTickCount64 &&
           g_api.CreateFileW && g_api.ReadFile && g_api.WriteFile &&
           g_api.GetFileSizeEx && g_api.CloseHandle;
}

static bool init_bootstrap_api() {
    u8* k32 = find_module_ascii("kernel32.dll");
    if (!k32) return false;
    g_api.LoadLibraryW = api_cast<PFN_LoadLibraryW>(resolve_export(k32, "LoadLibraryW"));
    g_api.CreateThread = api_cast<PFN_CreateThread>(resolve_export(k32, "CreateThread"));
    return g_api.CreateThread != nullptr;
}

static void get_game_info() {
    g_gameBase = find_module_ascii("ACBlackFlag.exe");
    if (!g_gameBase) {
        u8* peb = (u8*)read_gs_qword(0x60);
        if (peb) g_gameBase = *(u8**)(peb + 0x10);
    }
    if (!g_gameBase || *(u16*)g_gameBase != 0x5A4D) return;
    u8* nt = g_gameBase + *(u32*)(g_gameBase + 0x3C);
    if (*(u32*)nt == 0x00004550) g_gameSize = *(u32*)(nt + 24 + 0x38);
}

struct TextBuf {
    char bytes[1536]{};
    u32 len{};
};
static void tb_char(TextBuf& b, char c) {
    if (b.len + 1 < sizeof(b.bytes)) b.bytes[b.len++] = c;
}
static void tb_str(TextBuf& b, const char* s) {
    if (!s) return;
    while (*s && b.len + 1 < sizeof(b.bytes)) b.bytes[b.len++] = *s++;
}
static void tb_u64(TextBuf& b, u64 v) {
    char tmp[32]{};
    u32 n = 0;
    do { tmp[n++] = (char)('0' + (v % 10)); v /= 10; } while (v && n < sizeof(tmp));
    while (n) tb_char(b, tmp[--n]);
}
static void tb_i64(TextBuf& b, i64 v) {
    if (v < 0) { tb_char(b, '-'); tb_u64(b, (u64)(-v)); }
    else tb_u64(b, (u64)v);
}
static void tb_hex(TextBuf& b, u64 v, u32 minDigits = 1) {
    static const char h[] = "0123456789ABCDEF";
    char tmp[32]{};
    u32 n = 0;
    do { tmp[n++] = h[v & 15u]; v >>= 4; } while (v && n < sizeof(tmp));
    while (n < minDigits && n < sizeof(tmp)) tmp[n++] = '0';
    while (n) tb_char(b, tmp[--n]);
}
static void tb_float3(TextBuf& b, float v) {
    if (v < 0.0f) { tb_char(b, '-'); v = -v; }
    u64 whole = (u64)v;
    float fraction = v - (float)whole;
    u32 milli = (u32)(fraction * 1000.0f + 0.5f);
    if (milli >= 1000) { ++whole; milli -= 1000; }
    tb_u64(b, whole);
    tb_char(b, '.');
    tb_char(b, (char)('0' + (milli / 100) % 10));
    tb_char(b, (char)('0' + (milli / 10) % 10));
    tb_char(b, (char)('0' + milli % 10));
}
static void tb_time(TextBuf& b, const SYSTEMTIME& st) {
    auto two = [&](u32 v) {
        tb_char(b, (char)('0' + (v / 10) % 10));
        tb_char(b, (char)('0' + v % 10));
    };
    two(st.wHour); tb_char(b, ':'); two(st.wMinute); tb_char(b, ':'); two(st.wSecond);
    tb_char(b, '.');
    tb_char(b, (char)('0' + (st.wMilliseconds / 100) % 10));
    tb_char(b, (char)('0' + (st.wMilliseconds / 10) % 10));
    tb_char(b, (char)('0' + st.wMilliseconds % 10));
}
static void tb_endline(TextBuf& b) {
    tb_char(b, '\r'); tb_char(b, '\n');
    if (b.len < sizeof(b.bytes)) b.bytes[b.len] = 0;
}

static void append_log(const char* data, u32 size) {
    if (!g_api.CreateFileW || !g_api.WriteFile || !data || !size) return;
    HANDLE file = g_api.CreateFileW(kLogPath, FILE_APPEND_DATA,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (!file || file == invalid_handle_value()) return;
    DWORD written = 0;
    g_api.WriteFile(file, data, size, &written, nullptr);
    g_api.CloseHandle(file);
}
static void log_simple(const char* message) {
    TextBuf b{};
    SYSTEMTIME st{};
    if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    tb_char(b, '['); tb_time(b, st); tb_str(b, "] "); tb_str(b, message); tb_endline(b);
    append_log(b.bytes, b.len);
}

static u64 parse_u64(const WCHAR* s) {
    if (!s) return 0;
    while (*s == L' ' || *s == L'\t') ++s;
    u32 base = 10;
    if (s[0] == L'0' && (s[1] == L'x' || s[1] == L'X')) { base = 16; s += 2; }
    u64 value = 0;
    while (*s) {
        u32 d;
        if (*s >= L'0' && *s <= L'9') d = (u32)(*s - L'0');
        else if (*s >= L'a' && *s <= L'f') d = (u32)(*s - L'a' + 10);
        else if (*s >= L'A' && *s <= L'F') d = (u32)(*s - L'A' + 10);
        else break;
        if (d >= base) break;
        value = value * base + d;
        ++s;
    }
    return value;
}
static i32 parse_i32(const WCHAR* s) {
    if (!s) return 0;
    while (*s == L' ' || *s == L'\t') ++s;
    bool neg = false;
    if (*s == L'-') { neg = true; ++s; }
    i64 v = (i64)parse_u64(s);
    return (i32)(neg ? -v : v);
}
static float parse_float(const WCHAR* s, float fallback) {
    if (!s || !*s) return fallback;
    while (*s == L' ' || *s == L'\t') ++s;
    bool neg = false;
    if (*s == L'-') { neg = true; ++s; }
    u64 whole = 0;
    bool any = false;
    while (*s >= L'0' && *s <= L'9') { any = true; whole = whole * 10 + (u32)(*s++ - L'0'); }
    float v = (float)whole;
    if (*s == L'.') {
        ++s;
        float place = 0.1f;
        while (*s >= L'0' && *s <= L'9' && place > 0.0000001f) {
            any = true;
            v += (float)(*s++ - L'0') * place;
            place *= 0.1f;
        }
    }
    if (!any) return fallback;
    return neg ? -v : v;
}
static u64 read_ini_u64(const WCHAR* section, const WCHAR* key, u64 fallback) {
    if (!g_api.GetPrivateProfileStringW) return fallback;
    WCHAR tmp[96]{};
    g_api.GetPrivateProfileStringW(section, key, L"", tmp, 96, kIniPath);
    return tmp[0] ? parse_u64(tmp) : fallback;
}
static i32 read_ini_i32(const WCHAR* section, const WCHAR* key, i32 fallback) {
    if (!g_api.GetPrivateProfileStringW) return fallback;
    WCHAR tmp[96]{};
    g_api.GetPrivateProfileStringW(section, key, L"", tmp, 96, kIniPath);
    return tmp[0] ? parse_i32(tmp) : fallback;
}
static float read_ini_float(const WCHAR* section, const WCHAR* key, float fallback) {
    if (!g_api.GetPrivateProfileStringW) return fallback;
    WCHAR tmp[96]{};
    g_api.GetPrivateProfileStringW(section, key, L"", tmp, 96, kIniPath);
    return tmp[0] ? parse_float(tmp, fallback) : fallback;
}
static void copy_ascii(char* out, u32 capacity, const char* value) {
    if (!out || !capacity) return;
    u32 i = 0;
    if (value) for (; value[i] && i + 1 < capacity; ++i) out[i] = value[i];
    out[i] = 0;
}
static void read_ini_ascii(const WCHAR* section, const WCHAR* key,
                           const char* fallback, char* out, u32 capacity) {
    if (!out || !capacity) return;
    WCHAR tmp[96]{};
    if (g_api.GetPrivateProfileStringW)
        g_api.GetPrivateProfileStringW(section, key, L"", tmp, 96, kIniPath);
    if (!tmp[0]) { copy_ascii(out, capacity, fallback); return; }
    u32 i = 0;
    for (; tmp[i] && i + 1 < capacity; ++i) out[i] = tmp[i] <= 0x7F ? (char)tmp[i] : '?';
    out[i] = 0;
}
static void read_ini_wide(const WCHAR* section, const WCHAR* key,
                          const WCHAR* fallback, WCHAR* out, u32 capacity) {
    if (!out || !capacity) return;
    WCHAR tmp[320]{};
    if (g_api.GetPrivateProfileStringW)
        g_api.GetPrivateProfileStringW(section, key, L"", tmp, 320, kIniPath);
    const WCHAR* src = tmp[0] ? tmp : fallback;
    u32 i = 0;
    if (src) for (; src[i] && i + 1 < capacity; ++i) out[i] = src[i];
    out[i] = 0;
}

// Per-action user controls are intentionally neutral by default.  The
// existing r4 gains, native-probe delays and measured waveform trims remain
// the calibrated baseline; these controls let users scale or offset one
// action without having to understand the rest of the bridge.
struct EffectControl {
    u32 enabled = 1;
    float intensityMultiplier = 1.0f;
    i32 triggerOffsetMs = 0;
    i32 startTrimOffsetMs = 0;
};

static void load_effect_control(const WCHAR* section, EffectControl& control) {
    control.enabled = read_ini_i32(
        section, L"Enabled", (i32)control.enabled) ? 1u : 0u;
    control.intensityMultiplier = read_ini_float(
        section, L"IntensityMultiplier", control.intensityMultiplier);
    control.triggerOffsetMs = read_ini_i32(
        section, L"TriggerOffsetMs", control.triggerOffsetMs);
    control.startTrimOffsetMs = read_ini_i32(
        section, L"StartTrimOffsetMs", control.startTrimOffsetMs);
    if (control.intensityMultiplier < 0.0f)
        control.intensityMultiplier = 0.0f;
    if (control.intensityMultiplier > 4.0f)
        control.intensityMultiplier = 4.0f;
    if (control.triggerOffsetMs < -5000) control.triggerOffsetMs = -5000;
    if (control.triggerOffsetMs > 5000) control.triggerOffsetMs = 5000;
    if (control.startTrimOffsetMs < -5000)
        control.startTrimOffsetMs = -5000;
    if (control.startTrimOffsetMs > 5000)
        control.startTrimOffsetMs = 5000;
}

static u32 read_ini_u32_bounded(const WCHAR* section, const WCHAR* key,
                                u32 fallback, u32 maximum) {
    i32 value = read_ini_i32(section, key, (i32)fallback);
    if (value < 0) return 0;
    if ((u32)value > maximum) return maximum;
    return (u32)value;
}

// The bridge normalizes GameInput and XInput into this common logical layout.
// Keep the names near the INI parser because user-remappable bindings are
// loaded before the controller backends are initialized.
static constexpr WORD XINPUT_DPAD_UP = 0x0001;
static constexpr WORD XINPUT_DPAD_DOWN = 0x0002;
static constexpr WORD XINPUT_DPAD_LEFT = 0x0004;
static constexpr WORD XINPUT_DPAD_RIGHT = 0x0008;
static constexpr WORD XINPUT_START = 0x0010;
static constexpr WORD XINPUT_BACK = 0x0020;
static constexpr WORD XINPUT_LEFT_THUMB = 0x0040;
static constexpr WORD XINPUT_RIGHT_THUMB = 0x0080;
static constexpr WORD XINPUT_LEFT_SHOULDER = 0x0100;
static constexpr WORD XINPUT_RIGHT_SHOULDER = 0x0200;
static constexpr WORD XINPUT_A = 0x1000;
static constexpr WORD XINPUT_B = 0x2000;
static constexpr WORD XINPUT_X = 0x4000;
static constexpr WORD XINPUT_Y = 0x8000;

enum InputBindingType : u32 {
    INPUT_BINDING_NONE = 0,
    INPUT_BINDING_BUTTON,
    INPUT_BINDING_LEFT_TRIGGER,
    INPUT_BINDING_RIGHT_TRIGGER
};

struct InputBinding {
    u32 type = INPUT_BINDING_NONE;
    WORD button = 0;
};

static bool binding_separator(char c) {
    return c == ' ' || c == '\t' || c == '-' || c == '_';
}

// Binding values are case-insensitive and may contain spaces, hyphens, or
// underscores.  For example, DPadLeft, d-pad-left, and dpad_left are equal.
static bool binding_token_eq(const char* value, const char* expected) {
    if (!value || !expected) return false;
    while (true) {
        while (*value && binding_separator(*value)) ++value;
        while (*expected && binding_separator(*expected)) ++expected;
        if (!*value || !*expected) break;
        if (lower_ascii(*value) != lower_ascii(*expected)) return false;
        ++value;
        ++expected;
    }
    while (*value && binding_separator(*value)) ++value;
    while (*expected && binding_separator(*expected)) ++expected;
    return !*value && !*expected;
}

static InputBinding button_binding(WORD button) {
    InputBinding result{};
    result.type = INPUT_BINDING_BUTTON;
    result.button = button;
    return result;
}

static InputBinding trigger_binding(u32 type) {
    InputBinding result{};
    result.type = type;
    return result;
}

static bool parse_input_binding(const char* value, InputBinding& result) {
    if (binding_token_eq(value, "None") ||
        binding_token_eq(value, "Disabled") ||
        binding_token_eq(value, "Off")) {
        result = {};
        return true;
    }
    if (binding_token_eq(value, "Cross") || binding_token_eq(value, "A") ||
        binding_token_eq(value, "South") || binding_token_eq(value, "PSCross") ||
        binding_token_eq(value, "XboxA")) {
        result = button_binding(XINPUT_A); return true;
    }
    if (binding_token_eq(value, "Circle") || binding_token_eq(value, "B") ||
        binding_token_eq(value, "East") || binding_token_eq(value, "PSCircle") ||
        binding_token_eq(value, "XboxB")) {
        result = button_binding(XINPUT_B); return true;
    }
    if (binding_token_eq(value, "Square") || binding_token_eq(value, "X") ||
        binding_token_eq(value, "West") || binding_token_eq(value, "PSSquare") ||
        binding_token_eq(value, "XboxX")) {
        result = button_binding(XINPUT_X); return true;
    }
    if (binding_token_eq(value, "Triangle") || binding_token_eq(value, "Y") ||
        binding_token_eq(value, "North") || binding_token_eq(value, "PSTriangle") ||
        binding_token_eq(value, "XboxY")) {
        result = button_binding(XINPUT_Y); return true;
    }
    if (binding_token_eq(value, "L1") || binding_token_eq(value, "LB") ||
        binding_token_eq(value, "LeftBumper") ||
        binding_token_eq(value, "LeftShoulder")) {
        result = button_binding(XINPUT_LEFT_SHOULDER); return true;
    }
    if (binding_token_eq(value, "R1") || binding_token_eq(value, "RB") ||
        binding_token_eq(value, "RightBumper") ||
        binding_token_eq(value, "RightShoulder")) {
        result = button_binding(XINPUT_RIGHT_SHOULDER); return true;
    }
    if (binding_token_eq(value, "L2") || binding_token_eq(value, "LT") ||
        binding_token_eq(value, "LeftTrigger")) {
        result = trigger_binding(INPUT_BINDING_LEFT_TRIGGER); return true;
    }
    if (binding_token_eq(value, "R2") || binding_token_eq(value, "RT") ||
        binding_token_eq(value, "RightTrigger")) {
        result = trigger_binding(INPUT_BINDING_RIGHT_TRIGGER); return true;
    }
    if (binding_token_eq(value, "L3") || binding_token_eq(value, "LS") ||
        binding_token_eq(value, "LeftStick") ||
        binding_token_eq(value, "LeftThumb")) {
        result = button_binding(XINPUT_LEFT_THUMB); return true;
    }
    if (binding_token_eq(value, "R3") || binding_token_eq(value, "RS") ||
        binding_token_eq(value, "RightStick") ||
        binding_token_eq(value, "RightThumb")) {
        result = button_binding(XINPUT_RIGHT_THUMB); return true;
    }
    if (binding_token_eq(value, "DPadUp") || binding_token_eq(value, "DUp")) {
        result = button_binding(XINPUT_DPAD_UP); return true;
    }
    if (binding_token_eq(value, "DPadDown") || binding_token_eq(value, "DDown")) {
        result = button_binding(XINPUT_DPAD_DOWN); return true;
    }
    if (binding_token_eq(value, "DPadLeft") || binding_token_eq(value, "DLeft")) {
        result = button_binding(XINPUT_DPAD_LEFT); return true;
    }
    if (binding_token_eq(value, "DPadRight") || binding_token_eq(value, "DRight")) {
        result = button_binding(XINPUT_DPAD_RIGHT); return true;
    }
    return false;
}

static InputBinding read_input_binding(const WCHAR* key,
                                       const char* fallbackName,
                                       const InputBinding& fallback,
                                       u32& invalidCount) {
    char value[64]{};
    read_ini_ascii(L"InputBindings", key, fallbackName, value, 64);
    InputBinding parsed{};
    if (parse_input_binding(value, parsed)) return parsed;
    ++invalidCount;
    return fallback;
}

struct Config {
    u64 flushRva = 0x04F78610ull;
    u32 enabled = 1;
    u32 inputBridge = 1;
    u32 semanticEvents = 1;
    u32 semanticMenu = 1;
    // v8-r4: confirmed semantic fallbacks are immediate/short. Attack keeps a
    // tiny native-Motion first-refusal window so combat stays Ubisoft-native.
    u32 semanticGaitTimeoutMs = 450; // cadence-history/run safety; stick release stops immediately
    u32 semanticAttackProbeMs = 32;
    u32 semanticOneShotProbeMs = 0;
    u32 semanticLandingProbeMs = 0;
    u32 semanticAirTimeoutMs = 6500;
    // r7 phase timing and water/run behavior.
    u32 heavyPhase1StartMs = 120;
    u32 heavyPhase1DurationMs = 180;
    u32 heavyPhase2DelayMs = 590; // r7 user-tuned baseline; canonical control is Effect.HeavyAttackSecond/DelayAfterFirstMs
    u32 heavyPhase2StartMs = 235;
    u32 heavyPhase2DurationMs = 190;
    u32 sheathePhase1DurationMs = 180;
    u32 sheathePhase2DelayMs = 620;
    u32 sheathePhase2StartMs = 900;
    u32 sheathePhase2DurationMs = 360;
    u32 waterLungeSustainMs = 3000;
    // r9c: 0x7ED9038C is not an input edge; accept it as a lunge only when
    // correlated with a recent L3/SprintLunge press.
    u32 waterLungeInputGateMs = 600;
    u32 semanticWalkPulseStartMs = 2440;
    u32 semanticWalkPulseDurationMs = 150;
    u32 semanticRunPulseStartMs = 3120;
    u32 semanticRunPulseDurationMs = 140;
    // r9 spatial foot contacts. Edward starts a fresh walk/run animation on
    // the right foot; accepted gait contacts alternate R/L until a reset.
    u32 spatialFootsteps = 1;
    u32 footstepResetAfterMs = 750;
    float rightFootRightGain = 1.00f;
    float rightFootLeftGain = 0.25f;
    float leftFootLeftGain = 1.00f;
    float leftFootRightGain = 0.25f;
    float sprintContactOverlayGain = 0.84f;
    // XInput/GameInput-class land sprint is step-pulsed in r9 instead of a
    // continuous motor envelope. Each fast-gait contact is 20% above jog.
    u32 xinputLandRunStepOnly = 1;
    float xinputSprintStepGain = 1.20f;
    // r9c: conventional-rumble-only tail caps for XInput-class pads.
    // These do not alter the DualSense 48 kHz haptic voice.
    u32 xinputJumpLandingMaxDurationMs = 300;
    u32 xinputLightAttackMaxDurationMs = 300;
    // r9d: once one of these short XInput-class action impulses ends, write an
    // explicit zero-motor gap so a following action cannot merge into its tail.
    // This never changes the native DualSense 48 kHz haptic timeline.
    u32 xinputActionZeroGapMs = 16;
    u32 semanticDodgeDurationMs = 330;
    u32 semanticLightAttackDurationMs = 360;
    u32 semanticHeavyAttackDurationMs = 520;
    u32 semanticJumpDurationMs = 260;
    u32 semanticLandingDurationMs = 260;
    u32 semanticKickDurationMs = 320;
    u32 semanticWaterLungeDurationMs = 330;
    u32 semanticSurfaceDurationMs = 220;
    u32 nativeHaptics = 1;
    u32 conventionalRumble = 1;
    u32 preferGameInput = 1;
    u32 rumbleOnDualSense = 1;
    // r9k: Ubisoft-only direct DualSense safety net. Steam/voices38 never use
    // this renderer and keep the exact v2.3.7.0/r9h native Quad output path.
    u32 ubisoftDirectDualSenseFallback = 1;
    u32 ubisoftDirectDualSenseFallbackDelayMs = 1000;
    // r9/v2.3.6 keeps the normalized user scale where 1.00
    // corresponds to the tuned release strength. Missing key keeps legacy raw gain.
    u32 normalizedGainBaseline = 0;
    u32 controllerIndex = 0;
    // r9g: 2 ms worker/input cadence = 500 Hz bridge-side polling. This does
    // not change the controller firmware/report interval; USB packets that arrive
    // at 250 Hz are simply sampled promptly by the 500 Hz worker.
    u32 pollMs = 2;
    u32 gameInputDeviceRefreshMs = 125;
    u32 gameInputStatusRefreshMs = 500;
    u32 rumbleWriteIntervalMs = 4;
    u32 maxFrames = 4096;
    u32 gameplayStateHooks = 1;
    u32 stateBindWindowMs = 900;
    u32 stateRebindIdleMs = 15000;
    u32 minimumAirborneMs = 120;
    u32 landingDelayMs = 32;
    u32 surfaceDelayMs = 140;
    u32 stateProbeDelayMs = 20;
    u32 jumpInputWindowMs = 1000;
    u32 swimInputWindowMs = 4000;
    // r7: entering water by ordinary forward movement can trigger Black Flag's
    // own automatic hop without a Jump/Dive button edge. Recent character
    // movement plus a verified Swimming/Shallow transition is authoritative.
    u32 autoWaterEntryReacquire = 1;
    u32 autoWaterMovementThreshold = 4500;
    u32 autoWaterEvidenceWindowMs = 2500;
    u32 inputJumpFallback = 1;
    u32 inputLandingFallbackMs = 2000;
    u32 inputLandingTimeoutMs = 8000;
    u32 stateEffectStartTrimMs = 236;
    i32 climbJumpStickThreshold = 4500;
    // r5: preserve the instant Edward leaves a climb surface so an ensuing
    // InAir/Freefall transition includes the otherwise-lost first frames.
    u32 climbAirCarryMs = 750;
    // r6 parkour: distance is estimated from held movement input integrated
    // through the measured airborne interval (full-stick-equivalent milliseconds).
    u32 parkourDistanceScaling = 1;
    u32 parkourDistanceReferenceMs = 1200;
    u32 parkourLandingCooldownMs = 120;
    float parkourDistanceMaxBoost = 0.35f;
    u32 landingShortAirMs = 900;
    u32 landingLongAirMs = 2000;
    u32 landingMaxAirMs = 3000;
    u32 nativeArbitration = 1;
    // r9b: action-scoped supplement for Ubisoft's own Quad Audio Haptics.
    // Sword attacks, parries, pistol fire and kicks arm a short window. If
    // native feedback arrives, only the native buffer is multiplied; if it
    // does not, the established bridge fallback path runs unchanged.
    u32 nativeCombatBoost = 1;
    u32 nativeCombatBoostWindowMs = 700;
    float nativeCombatHapticGain = 1.75f;
    u32 nativeHoldMs = 220;
    u32 nativeProbeDelayMs = 220;
    u32 dodgeProbeDelayMs = 20;
    u32 directionalDodgeProbeDelayMs = 20;
    u32 parryProbeDelayMs = 24;
    u32 attackProbeDelayMs = 60;
    u32 dpadLeftProbeDelayMs = 0;
    u32 dodgeStartTrimMs = 197;
    u32 directionalDodgeStartTrimMs = 236;
    u32 parryStartTrimMs = 48;
    i32 dodgeStickThreshold = 4500;
    u32 menuFeedback = 1;
    u32 menuProbeDelayMs = 20;
    u32 menuShoulderProbeDelayMs = 20;
    u32 menuCloseMovementMs = 80;
    u32 menuTimeoutMs = 180000;
    u32 menuNativeTabArm = 1;
    u32 menuNativeTabWindowMs = 2500;
    u32 menuNativeTabProbeMs = 300;
    u32 menuRawTouchpad = 1;
    u32 menuRawTouchpadButtonIndex = 13;
    u32 menuRawTouchpadSonyOnly = 1;
    u32 menuTransitionGuardMs = 48;
    i32 menuStickThreshold = 19000;
    u32 sinkRebindAfterMs = 750;
    u32 reloadSilenceResetMs = 1500;
    i32 walkThreshold = 11000;
    i32 runThreshold = 24500;
    i32 triggerThreshold = 160;
    u32 suppressMovementWithMenuButtons = 1;
    u32 sprintToggle = 1;
    u32 walkLoopStartMs = 2280;
    u32 walkLoopEndMs = 3810;
    u32 runLoopStartMs = 3100;
    u32 runLoopEndMs = 4935;
    u32 runOpeningDurationMs = 1500;
    u32 jumpCooldownMs = 300;
    u32 landingCooldownMs = 500;
    u32 haystackCooldownMs = 700;
    u32 diveStateCooldownMs = 900;
    u32 diveInputCooldownMs = 250;
    u32 surfaceCooldownMs = 700;
    u32 waterJumpCooldownMs = 250;
    u32 menuRepeatDelayMs = 360;
    u32 menuRepeatIntervalMs = 180;
    u32 xinputRunPulseEnabled = 0;
    u32 xinputRunPulseIntervalMs = 300;
    u32 xinputRunPulseDurationMs = 75;
    u32 rbHoldThresholdMs = 320;
    u32 rbHoldSecondDelayMs = 590;
    InputBinding jumpBinding{INPUT_BINDING_BUTTON, XINPUT_A};
    InputBinding dodgeDiveBinding{INPUT_BINDING_BUTTON, XINPUT_B};
    InputBinding parryBinding{INPUT_BINDING_BUTTON, XINPUT_LEFT_SHOULDER};
    InputBinding attackBinding{INPUT_BINDING_BUTTON, XINPUT_RIGHT_SHOULDER};
    InputBinding sprintLungeBinding{INPUT_BINDING_BUTTON, XINPUT_LEFT_THUMB};
    InputBinding sheatheHoodBinding{INPUT_BINDING_BUTTON, XINPUT_DPAD_LEFT};
    u32 invalidInputBindings = 0;
    float masterGain = 1.0f;
    float movementGain = 1.0f;
    float oneShotGain = 1.0f;
    float peakLimit = 2.0f;
    float movementFadeMs = 35.0f;
    float walkFadeMs = 35.0f;
    float runFadeMs = 35.0f;
    float walkGain = 0.65f;
    // r9b: the public walk control is normalized like MasterGain. With the
    // marker present, user 1.00 == r9 raw 4.80. Old INIs without the marker
    // remain legacy/raw so an existing 4.80 never becomes 23.04 accidentally.
    u32 normalizedWalkHapticBaseline = 0;
    float dualSenseWalkHapticMultiplier = 4.80f;
    // r9: jog/fast-gait contact pulses are 20% stronger on the native
    // DualSense haptic path without changing the conventional-rumble baseline.
    float dualSenseJogHapticMultiplier = 1.20f;
    // r9 movement baseline: +20% over r8 for L3 opening and sustained run.
    float runBaseGain = 1.80f;
    float runOpeningGain = 2.28f;
    float rumbleGain = 1.0f;
    float rumbleMovementGain = 2.0f;
    float rumbleLowScale = 3.0f;
    float rumbleHighScale = 1.6f;
    float rumbleDeadzone = 0.008f;
    float xinputRunPulseGain = 1.0f;
    float buttonLBGain = 0.45f;
    float buttonRBGain = 1.0f;
    float rbHoldGain = 0.45f;
    float sprintKickGain = 0.18f;
    float jumpGain = 0.55f;
    // r5: very short drops start softer and scale continuously up to the
    // existing LandingShortGain at LandingShortAirMs.
    float landingMinimumGain = 0.30f;
    float landingShortGain = 0.55f;
    float landingGain = 0.70f;
    // r9f: literal +125% over r9e's 1.40 long-fall ceiling. A 3.0 s
    // or longer fall therefore reaches 3.15 base gain, and parkour distance is
    // capped so it cannot exceed this explicit maximum.
    float landingLongGain = 3.15f;
    float haystackGain = 0.42f;
    float diveGain = 1.0f;
    float surfaceGain = 0.45f;
    float waterJumpGain = 0.65f;
    float waterLungeGain = 1.20f;
    float nativeHapticPeakThreshold = 0.006f;
    float nativeHapticMeanThreshold = 0.00035f;
    float nativeRumbleThreshold = 0.015f;
    float menuTickGain = 0.35f;
    float menuOpenGain = 0.28f;
    float menuShoulderGain = 0.315f;
    // r9e: DualSense 48 kHz bridge tuning is intentionally separate from
    // Ubisoft-native Quad haptics. Global 0.90 reduces bridge noise, while
    // jump/landing and land locomotion get additional category multipliers.
    float dualSenseBridgeHapticGain = 0.90f;
    float dualSenseJumpLandingHapticGain = 0.85f;
    // r9f: +15% relative to r9e's 0.90 locomotion category factor.
    // 0.90 * 1.15 = 1.035; global DualSense bridge gain remains 0.90.
    float dualSenseLandMovementHapticGain = 1.035f;
    float dualSenseMenuShoulderGain = 0.75f;
    // r9f: +30% DualSense menu navigation tick relative to r9e. This value
    // is used for both bridge fallback ticks and the verified Ubisoft-native
    // menu Quad tick, while leaving XInput menu ticks unchanged.
    float dualSenseMenuTickGain = 1.30f;
    u32 dualSenseMenuTickNativeWindowMs = 250;
    // Menu L1/R1 conventional-rumble attenuation by active controller class.
    float xinputMenuShoulderGain = 0.50f;
    EffectControl fxWalk{};
    EffectControl fxRun{};
    EffectControl fxSprintKick{};
    EffectControl fxJump{};
    EffectControl fxLandingShort{};
    EffectControl fxLandingNormal{};
    EffectControl fxLandingLong{};
    EffectControl fxHaystack{};
    EffectControl fxDodge{};
    EffectControl fxDirectionalDodge{};
    EffectControl fxDive{};
    EffectControl fxSurface{};
    EffectControl fxWaterJump{};
    EffectControl fxWaterLunge{};
    EffectControl fxParry{1, 1.0f, -50, 0};
    EffectControl fxLightAttack{};
    EffectControl fxHeavyFirst{};
    EffectControl fxHeavySecond{1, 1.0f, 50, 0};
    EffectControl fxSheatheHood{1, 1.0f, -40, -80};
    EffectControl fxMenuTick{};
    EffectControl fxMenuOpen{};
    EffectControl fxMenuShoulder{};
    WCHAR packPath[320]{};
    char walk[32]{};
    char run[32]{};
    char buttonA[32]{};
    char buttonB[32]{};
    char buttonX[32]{};
    char buttonY[32]{};
    char buttonLB[32]{};
    char buttonRB[32]{};
    char dpadUp[32]{};
    char dpadDown[32]{};
    char dpadLeft[32]{};
    char dpadRight[32]{};
    char leftTrigger[32]{};
    char rightTrigger[32]{};
    char sprintKick[32]{};
    char rbHold[32]{};
    char jump[32]{};
    char landing[32]{};
    char haystack[32]{};
    char dive[32]{};
    char surface[32]{};
    char waterJump[32]{};
    char waterLunge[32]{};
    char menuTick[32]{};
    char menuOpen[32]{};
};
static Config g_cfg;

// v8-r7 normalized user-facing baseline. INI value 1.00 is now the recommended
// release strength rather than the weak pre-v2.3.1 raw scale. These multipliers
// are shared by native DualSense haptics and conventional GameInput/XInput rumble.
static constexpr float NORMALIZED_MASTER_BASELINE = 2.50f;
static constexpr float NORMALIZED_ONESHOT_BASELINE = 1.80f;
static constexpr float NORMALIZED_WALK_HAPTIC_BASELINE = 4.80f;
static float effective_master_gain() {
    return g_cfg.masterGain * (g_cfg.normalizedGainBaseline ? NORMALIZED_MASTER_BASELINE : 1.0f);
}
static float effective_one_shot_gain() {
    return g_cfg.oneShotGain * (g_cfg.normalizedGainBaseline ? NORMALIZED_ONESHOT_BASELINE : 1.0f);
}
static float effective_dualsense_walk_haptic_multiplier() {
    return g_cfg.dualSenseWalkHapticMultiplier *
        (g_cfg.normalizedWalkHapticBaseline ? NORMALIZED_WALK_HAPTIC_BASELINE : 1.0f);
}

// -----------------------------------------------------------------------------
// v8 exact semantic event router.
//
// The IDs below were isolated with ACBFStateTracer 2.4 controlled x5 tests.
// Native-feedback actions are OBSERVED ONLY: v8 never duplicates their working
// Ubisoft Quad output. Missing-feedback actions are queued after a short native
// arbitration window so combat/native Motion always gets first refusal.
// -----------------------------------------------------------------------------
static constexpr u32 EVT_MENU_NATIVE_TICK = 0x67233A85u;
static constexpr u32 EVT_MENU_AUDIO_ONLY  = 0xEB10F213u;
static constexpr u32 EVT_WALK_STEP        = 0xF4919384u;
static constexpr u32 EVT_FAST_GAIT_STEP   = 0xC490DFC6u;
static constexpr u32 EVT_DODGE_ROLL       = 0x4F8D126Fu;
static constexpr u32 EVT_NORMAL_PARRY     = 0xD523F1E1u;
static constexpr u32 EVT_PERFECT_PARRY_A  = 0x6B0E9352u;
static constexpr u32 EVT_PERFECT_PARRY_B  = 0xC5DD775Eu;
static constexpr u32 EVT_PERFECT_PARRY_C  = 0x4DD3C123u;
static constexpr u32 EVT_LIGHT_ATTACK     = 0x8ABBEE18u;
static constexpr u32 EVT_HEAVY_ATTACK     = 0xC21A023Eu;
static constexpr u32 EVT_PLAYER_HIT       = 0x839CE745u;
static constexpr u32 EVT_PISTOL_FIRE      = 0x27C0E102u;
static constexpr u32 EVT_WATER_LUNGE      = 0x7ED9038Cu;
static constexpr u32 EVT_SURFACE          = 0x731177C1u;
static constexpr u32 EVT_SWIM_SPRINT      = 0x024F2AF6u;
static constexpr u32 EVT_DIVE_CANDIDATE   = 0x23B967BBu;
static constexpr u32 EVT_KICK             = 0xB50C8069u;
static constexpr u32 EVT_SHEATHE          = 0xA15A3B30u;
static constexpr u32 EVT_UNSHEATHE        = 0xD3B2B6DCu;
static constexpr u32 EVT_HOOD_ON          = 0x8CB7D98Cu;
static constexpr u32 EVT_HOOD_OFF         = 0xC88F29CDu;
static constexpr u32 EVT_JUMP_START       = 0x2E795D46u;
static constexpr u32 EVT_LANDING_A        = 0x4E92CCACu;
static constexpr u32 EVT_LANDING_B        = 0x43CD17F9u;
static constexpr u32 EVT_HELM_ENTER       = 0xDF7BEEAEu;
static constexpr u32 EVT_CANNON_BROADSIDE = 0x2FC9D3ADu;
static constexpr u32 EVT_CANNON_MOTION    = 0x774FC62Cu;

static constexpr u64 V8_POST_EVENT_RVA_106 = 0x0091DEA0ull;
static u64 g_v8PostEventRva = V8_POST_EVENT_RVA_106;
static constexpr u32 V8_SEM_RING = 2048u;
struct V8SemanticEvent {
    volatile u64 commit{};
    u64 tick{};
    u64 gameObject{};
    u64 nativeSerialBaseline{};
    u32 id{};
    u32 playingId{};
};
static V8SemanticEvent g_v8SemRing[V8_SEM_RING]{};
static volatile u64 g_v8SemWrite{};
static volatile u64 g_v8SemRead{};
static volatile u64 g_v8SemDrops{};
static volatile u64 g_v8SemProcessed{};
static volatile u32 g_v8PostHookInstalled{};

enum V8RuntimeMode : u32 {
    V8_MODE_UNKNOWN = 0,
    V8_MODE_MENU = 1,
    V8_MODE_CHARACTER = 2,
    V8_MODE_WATER = 3,
    V8_MODE_HELM = 4,
};
static volatile u32 g_v8RuntimeMode{V8_MODE_UNKNOWN};
static volatile u32 g_v8LastEventId{};
static volatile u64 g_v8LastEventTick{};
static volatile u64 g_v8LastMenuEventTick{};
static volatile u64 g_v8LastGameplayEventTick{};
static volatile u64 g_v8LastGaitTick{};
static volatile u64 g_v8PreviousFastGaitTick{};
static volatile u64 g_v8SemanticAirStartTick{};
static volatile u64 g_v8LastLandingTick{};
static volatile u64 g_v8SemanticJumpCount{};
static volatile u64 g_v8SemanticLandingCount{};
static volatile u64 g_v8SemanticMenuCount{};
static volatile u64 g_v8SemanticFallbacks{};
static volatile u64 g_v8SemanticNativeOnly{};
// v8-r4 learns the live Edward vs helm Wwise game-object affinity. This is
// deliberately NOT a sticky helm latch: strong character/water/menu evidence
// or a real load/desync recovery can release HELM. Ambiguous shared events
// (notably 0x2E795D46) must match Edward's learned object before doing so.
static volatile u64 g_v8CharacterObject{};
static volatile u64 g_v8HelmObject{};
static volatile u64 g_v8HelmReleases{};
static volatile u64 g_v8HelmAmbiguousIgnored{};
static volatile u64 g_v8SemanticAirTimeouts{};
static volatile u64 g_v8WalkPulses{};
static volatile u64 g_v8RunPulses{};
// r4 hybrid controls: exact events remain primary, but controller edges fill
// gaps where Black Flag exposes only the first combo stage. These live entirely
// on the worker thread; counters are atomic only for F10 reporting.
static bool g_r4LandSprintActive{};
// r9 audio-thread-safe mirror used to suppress continuous land-run rumble.
static volatile u32 g_r9LandSprintRumbleStepOnlyActive{};
static bool g_r4Underwater{};
static u64 g_r4WaterLungeUntil{};
static u64 g_r9cWaterLungeArmTick{};
static u64 g_r9cPendingWaterLungeTick{};
static u64 g_r9cPendingWaterLungeNativeSerial{};
static volatile u64 g_r9cWaterLungeInputArms{};
static volatile u64 g_r9cWaterLungeSemanticAccepted{};
static volatile u64 g_r9cWaterLungeSemanticRejected{};
static constexpr u32 R9C_WATER_LUNGE_PREINPUT_TOLERANCE_MS = 96;
static bool g_r4AttackPressActive{};
static bool g_r5AttackHeavyCommitted{};
static u64 g_r4AttackPressTick{};
static u64 g_r4AttackPressNativeSerial{};
static volatile u64 g_r4LastSemanticAttackTick{};
static volatile u64 g_r5LastSemanticHeavyTick{};
static volatile u64 g_r4RawLightFallbacks{};
static volatile u64 g_r4RawHeavyFallbacks{};
static volatile u64 g_r4ParryFallbacks{};
static volatile u64 g_r4HeavyPhase1Count{};
static volatile u64 g_r4HeavyPhase2Count{};
static volatile u64 g_r4SheathePhase2Count{};
static volatile u64 g_r4RunStarts{};
static volatile u64 g_r4WaterLungeStarts{};
static volatile u64 g_r5HeavyHoldCommits{};
static volatile u64 g_r5WaterStateReacquires{};
static volatile u64 g_r5ClimbAirCarries{};
static volatile u64 g_r5ClimbCatchImpacts{};
// r8: semantic gait contacts are spatialized. New animation cycles start RIGHT.
static bool g_r8NextFootRight{true};
static u32 g_r8LastGaitKind{}; // 0=none, 1=walk, 2=fast gait
static u64 g_r8LastFootstepTick{};
static volatile u64 g_r8RightFootContacts{};
static volatile u64 g_r8LeftFootContacts{};
static volatile u64 g_r8FootPhaseResets{};
// r9 XInput/GameInput-class land-run diagnostics.
static volatile u64 g_r9XInputRunStepContacts{};
static volatile u64 g_r9ContinuousRunSuppressedTicks{};
static void r4_schedule_heavy_phases(u64 now, u64 nativeSerialBaseline, u32 baseSource);
static void r4_reset_hybrid_runtime();
static void r8_reset_foot_phase();
// Domain helpers are defined after the Wwise hook but are also consumed by
// the earlier gameplay-state processor for non-sticky HELM recovery.
static void v8_enter_character_domain(u64 now);
static void v8_enter_water_domain(u64 now);

static bool v8_is_semantic_id(u32 id) {
    switch (id) {
    case EVT_MENU_NATIVE_TICK: case EVT_MENU_AUDIO_ONLY:
    case EVT_WALK_STEP: case EVT_FAST_GAIT_STEP: case EVT_DODGE_ROLL:
    case EVT_NORMAL_PARRY: case EVT_PERFECT_PARRY_A:
    case EVT_PERFECT_PARRY_B: case EVT_PERFECT_PARRY_C:
    case EVT_LIGHT_ATTACK: case EVT_HEAVY_ATTACK: case EVT_PLAYER_HIT:
    case EVT_PISTOL_FIRE: case EVT_WATER_LUNGE: case EVT_SURFACE:
    case EVT_SWIM_SPRINT: case EVT_DIVE_CANDIDATE: case EVT_KICK:
    case EVT_SHEATHE: case EVT_UNSHEATHE: case EVT_HOOD_ON:
    case EVT_HOOD_OFF: case EVT_JUMP_START: case EVT_LANDING_A:
    case EVT_LANDING_B: case EVT_HELM_ENTER: case EVT_CANNON_BROADSIDE:
    case EVT_CANNON_MOTION:
        return true;
    default:
        return false;
    }
}

static bool v8_is_native_combat_boost_id(u32 id) {
    switch (id) {
    case EVT_LIGHT_ATTACK:
    case EVT_HEAVY_ATTACK:
    case EVT_NORMAL_PARRY:
    case EVT_PERFECT_PARRY_A:
    case EVT_PERFECT_PARRY_B:
    case EVT_PERFECT_PARRY_C:
    case EVT_PISTOL_FIRE:
    case EVT_KICK:
        return true;
    default:
        return false;
    }
}

// These device fields are declared before the rumble renderer because v8
// still uses the proven v6 output backends and controller-specific rendering.
static volatile u32 g_inputBackend{}; // 0=none, 1=GameInput, 2=XInput
static volatile u32 g_activeVendor{};
static volatile u32 g_activeProduct{};
static volatile u32 g_activeDualSense{};
static volatile u32 g_controllerConnected{};
// r9k keeps a second identity only for the Ubisoft direct-haptics fallback.
// Baseline r9h behavior continues to use g_activeDualSense everywhere else.
static volatile u32 g_r9kPhysicalVendor{};
static volatile u32 g_r9kPhysicalProduct{};
static volatile u32 g_r9kPhysicalDualSense{};
static volatile u64 g_r9kPhysicalDualSenseSeenTick{};
enum R9KGameProfile : u32 {
    R9K_PROFILE_UNKNOWN = 0,
    R9K_PROFILE_STEAM_FAMILY = 1,
    R9K_PROFILE_UBISOFT_CONNECT = 2,
    R9K_PROFILE_UBISOFT_PLUS = 3,
    R9K_PROFILE_STEAM_TU_107 = 4
};
static volatile u32 g_r9kGameProfile{};
static volatile u64 g_r9kXInputIatRva{};
static volatile u32 g_r9kXInputResolveMethod{}; // 1=named,2=pointer,3=known-slot
static bool r9k_ubisoft_profile() {
    u32 p = atomic_load_u32(&g_r9kGameProfile);
    return p == R9K_PROFILE_UBISOFT_CONNECT || p == R9K_PROFILE_UBISOFT_PLUS;
}
static bool r9k_physical_dualsense_present() {
    return atomic_load_u32(&g_activeDualSense) != 0u ||
           atomic_load_u32(&g_r9kPhysicalDualSense) != 0u;
}
static bool r9_xinput_class_device();
static volatile u64 g_r9cXInputJumpLandingCaps{};
static volatile u64 g_r9cXInputLightAttackCaps{};
static volatile u64 g_r9dXInputZeroBarriers{};
static volatile u64 g_r9dXInputZeroTicks{};
static u64 g_r9dXInputForceZeroUntil{};

static void load_config() {
    g_cfg.enabled = (u32)read_ini_i32(L"General", L"Enabled", (i32)g_cfg.enabled);
    g_cfg.inputBridge = (u32)read_ini_i32(L"General", L"InputBridge", (i32)g_cfg.inputBridge);
    g_cfg.semanticEvents = (u32)read_ini_i32(L"SemanticEvents", L"Enabled", (i32)g_cfg.semanticEvents);
    g_cfg.semanticMenu = (u32)read_ini_i32(L"SemanticEvents", L"MenuState", (i32)g_cfg.semanticMenu);
    g_cfg.semanticGaitTimeoutMs = (u32)read_ini_i32(L"SemanticEvents", L"GaitTimeoutMs", (i32)g_cfg.semanticGaitTimeoutMs);
    g_cfg.semanticAttackProbeMs = (u32)read_ini_i32(L"SemanticEvents", L"AttackNativeProbeMs", (i32)g_cfg.semanticAttackProbeMs);
    g_cfg.semanticOneShotProbeMs = (u32)read_ini_i32(L"SemanticEvents", L"OneShotNativeProbeMs", (i32)g_cfg.semanticOneShotProbeMs);
    g_cfg.semanticLandingProbeMs = (u32)read_ini_i32(L"SemanticEvents", L"LandingNativeProbeMs", (i32)g_cfg.semanticLandingProbeMs);
    g_cfg.semanticAirTimeoutMs = (u32)read_ini_i32(L"SemanticEvents", L"AirTimeoutMs", (i32)g_cfg.semanticAirTimeoutMs);
    g_cfg.semanticWalkPulseStartMs = (u32)read_ini_i32(L"SemanticEvents", L"WalkPulseStartMs", (i32)g_cfg.semanticWalkPulseStartMs);
    g_cfg.semanticWalkPulseDurationMs = (u32)read_ini_i32(L"SemanticEvents", L"WalkPulseDurationMs", (i32)g_cfg.semanticWalkPulseDurationMs);
    g_cfg.semanticRunPulseStartMs = (u32)read_ini_i32(L"SemanticEvents", L"RunPulseStartMs", (i32)g_cfg.semanticRunPulseStartMs);
    g_cfg.semanticRunPulseDurationMs = (u32)read_ini_i32(L"SemanticEvents", L"RunPulseDurationMs", (i32)g_cfg.semanticRunPulseDurationMs);
    g_cfg.spatialFootsteps = (u32)read_ini_i32(L"Footsteps", L"SpatialFootsteps", (i32)g_cfg.spatialFootsteps);
    g_cfg.footstepResetAfterMs = (u32)read_ini_i32(L"Footsteps", L"ResetAfterMs", (i32)g_cfg.footstepResetAfterMs);
    g_cfg.semanticDodgeDurationMs = (u32)read_ini_i32(L"SemanticEvents", L"DodgePulseDurationMs", (i32)g_cfg.semanticDodgeDurationMs);
    g_cfg.semanticLightAttackDurationMs = (u32)read_ini_i32(L"SemanticEvents", L"LightAttackDurationMs", (i32)g_cfg.semanticLightAttackDurationMs);
    g_cfg.semanticHeavyAttackDurationMs = (u32)read_ini_i32(L"SemanticEvents", L"HeavyAttackDurationMs", (i32)g_cfg.semanticHeavyAttackDurationMs);
    g_cfg.semanticJumpDurationMs = (u32)read_ini_i32(L"SemanticEvents", L"JumpPulseDurationMs", (i32)g_cfg.semanticJumpDurationMs);
    g_cfg.semanticLandingDurationMs = (u32)read_ini_i32(L"SemanticEvents", L"LandingPulseDurationMs", (i32)g_cfg.semanticLandingDurationMs);
    g_cfg.semanticKickDurationMs = (u32)read_ini_i32(L"SemanticEvents", L"KickPulseDurationMs", (i32)g_cfg.semanticKickDurationMs);
    g_cfg.semanticWaterLungeDurationMs = (u32)read_ini_i32(L"SemanticEvents", L"WaterLungeDurationMs", (i32)g_cfg.semanticWaterLungeDurationMs);
    g_cfg.semanticSurfaceDurationMs = (u32)read_ini_i32(L"SemanticEvents", L"SurfaceDurationMs", (i32)g_cfg.semanticSurfaceDurationMs);
    g_cfg.heavyPhase1StartMs = (u32)read_ini_i32(L"SemanticEvents", L"HeavyPhase1StartMs", (i32)g_cfg.heavyPhase1StartMs);
    g_cfg.heavyPhase1DurationMs = (u32)read_ini_i32(L"SemanticEvents", L"HeavyPhase1DurationMs", (i32)g_cfg.heavyPhase1DurationMs);
    g_cfg.heavyPhase2DelayMs = (u32)read_ini_i32(L"SemanticEvents", L"HeavyPhase2DelayMs", (i32)g_cfg.heavyPhase2DelayMs);
    g_cfg.heavyPhase2StartMs = (u32)read_ini_i32(L"SemanticEvents", L"HeavyPhase2StartMs", (i32)g_cfg.heavyPhase2StartMs);
    g_cfg.heavyPhase2DurationMs = (u32)read_ini_i32(L"SemanticEvents", L"HeavyPhase2DurationMs", (i32)g_cfg.heavyPhase2DurationMs);
    g_cfg.sheathePhase1DurationMs = (u32)read_ini_i32(L"SemanticEvents", L"SheathePhase1DurationMs", (i32)g_cfg.sheathePhase1DurationMs);
    g_cfg.sheathePhase2DelayMs = (u32)read_ini_i32(L"SemanticEvents", L"SheathePhase2DelayMs", (i32)g_cfg.sheathePhase2DelayMs);
    g_cfg.sheathePhase2StartMs = (u32)read_ini_i32(L"SemanticEvents", L"SheathePhase2StartMs", (i32)g_cfg.sheathePhase2StartMs);
    g_cfg.sheathePhase2DurationMs = (u32)read_ini_i32(L"SemanticEvents", L"SheathePhase2DurationMs", (i32)g_cfg.sheathePhase2DurationMs);
    g_cfg.waterLungeSustainMs = (u32)read_ini_i32(L"SemanticEvents", L"WaterLungeSustainMs", (i32)g_cfg.waterLungeSustainMs);
    g_cfg.waterLungeInputGateMs = (u32)read_ini_i32(L"SemanticEvents", L"WaterLungeInputGateMs", (i32)g_cfg.waterLungeInputGateMs);
    if (g_cfg.semanticGaitTimeoutMs < 100) g_cfg.semanticGaitTimeoutMs = 100;
    if (g_cfg.semanticGaitTimeoutMs > 2000) g_cfg.semanticGaitTimeoutMs = 2000;
    if (g_cfg.semanticAttackProbeMs > 250) g_cfg.semanticAttackProbeMs = 250;
    if (g_cfg.semanticOneShotProbeMs > 250) g_cfg.semanticOneShotProbeMs = 250;
    if (g_cfg.semanticLandingProbeMs > 250) g_cfg.semanticLandingProbeMs = 250;
    if (g_cfg.semanticAirTimeoutMs < 1000) g_cfg.semanticAirTimeoutMs = 1000;
    if (g_cfg.semanticAirTimeoutMs > 15000) g_cfg.semanticAirTimeoutMs = 15000;
    if (g_cfg.semanticWalkPulseStartMs > 10000) g_cfg.semanticWalkPulseStartMs = 10000;
    if (g_cfg.semanticRunPulseStartMs > 10000) g_cfg.semanticRunPulseStartMs = 10000;
    if (g_cfg.semanticWalkPulseDurationMs > 1000) g_cfg.semanticWalkPulseDurationMs = 1000;
    if (g_cfg.semanticRunPulseDurationMs > 1000) g_cfg.semanticRunPulseDurationMs = 1000;
    if (g_cfg.footstepResetAfterMs < 300) g_cfg.footstepResetAfterMs = 300;
    if (g_cfg.footstepResetAfterMs > 3000) g_cfg.footstepResetAfterMs = 3000;
    if (g_cfg.semanticDodgeDurationMs > 2000) g_cfg.semanticDodgeDurationMs = 2000;
    if (g_cfg.semanticLightAttackDurationMs > 2000) g_cfg.semanticLightAttackDurationMs = 2000;
    if (g_cfg.semanticHeavyAttackDurationMs > 3000) g_cfg.semanticHeavyAttackDurationMs = 3000;
    if (g_cfg.semanticJumpDurationMs > 1500) g_cfg.semanticJumpDurationMs = 1500;
    if (g_cfg.semanticLandingDurationMs > 1500) g_cfg.semanticLandingDurationMs = 1500;
    if (g_cfg.semanticKickDurationMs > 1500) g_cfg.semanticKickDurationMs = 1500;
    if (g_cfg.semanticWaterLungeDurationMs > 1500) g_cfg.semanticWaterLungeDurationMs = 1500;
    if (g_cfg.semanticSurfaceDurationMs > 1500) g_cfg.semanticSurfaceDurationMs = 1500;
    if (g_cfg.heavyPhase1StartMs > 1500) g_cfg.heavyPhase1StartMs = 1500;
    if (g_cfg.heavyPhase1DurationMs > 1500) g_cfg.heavyPhase1DurationMs = 1500;
    if (g_cfg.heavyPhase2DelayMs > 1500) g_cfg.heavyPhase2DelayMs = 1500;
    if (g_cfg.heavyPhase2StartMs > 1500) g_cfg.heavyPhase2StartMs = 1500;
    if (g_cfg.heavyPhase2DurationMs > 1500) g_cfg.heavyPhase2DurationMs = 1500;
    if (g_cfg.sheathePhase1DurationMs > 1500) g_cfg.sheathePhase1DurationMs = 1500;
    if (g_cfg.sheathePhase2DelayMs > 2000) g_cfg.sheathePhase2DelayMs = 2000;
    if (g_cfg.sheathePhase2StartMs > 2000) g_cfg.sheathePhase2StartMs = 2000;
    if (g_cfg.sheathePhase2DurationMs > 1500) g_cfg.sheathePhase2DurationMs = 1500;
    if (g_cfg.waterLungeSustainMs < 500) g_cfg.waterLungeSustainMs = 500;
    if (g_cfg.waterLungeSustainMs > 10000) g_cfg.waterLungeSustainMs = 10000;
    if (g_cfg.waterLungeInputGateMs < 100) g_cfg.waterLungeInputGateMs = 100;
    if (g_cfg.waterLungeInputGateMs > 2000) g_cfg.waterLungeInputGateMs = 2000;
    g_cfg.nativeHaptics = (u32)read_ini_i32(
        L"Output", L"NativeHaptics", (i32)g_cfg.nativeHaptics);
    g_cfg.conventionalRumble = (u32)read_ini_i32(
        L"Output", L"ConventionalRumble", (i32)g_cfg.conventionalRumble);
    g_cfg.preferGameInput = (u32)read_ini_i32(
        L"Output", L"PreferGameInput", (i32)g_cfg.preferGameInput);
    g_cfg.rumbleOnDualSense = (u32)read_ini_i32(
        L"Output", L"RumbleOnDualSense", (i32)g_cfg.rumbleOnDualSense);
    g_cfg.ubisoftDirectDualSenseFallback = (u32)read_ini_i32(
        L"Output", L"UbisoftDirectDualSenseFallback",
        (i32)g_cfg.ubisoftDirectDualSenseFallback);
    g_cfg.ubisoftDirectDualSenseFallbackDelayMs = read_ini_u32_bounded(
        L"Output", L"UbisoftDirectDualSenseFallbackDelayMs",
        g_cfg.ubisoftDirectDualSenseFallbackDelayMs, 10000);
    if (g_cfg.ubisoftDirectDualSenseFallbackDelayMs < 100)
        g_cfg.ubisoftDirectDualSenseFallbackDelayMs = 100;
    g_cfg.normalizedGainBaseline = (u32)read_ini_i32(
        L"Playback", L"NormalizedBaseline", (i32)g_cfg.normalizedGainBaseline);
    g_cfg.controllerIndex = (u32)read_ini_i32(L"General", L"ControllerIndex", (i32)g_cfg.controllerIndex);
    g_cfg.pollMs = (u32)read_ini_i32(L"General", L"PollMs", (i32)g_cfg.pollMs);
    if (g_cfg.pollMs < 2) g_cfg.pollMs = 2;
    if (g_cfg.pollMs > 100) g_cfg.pollMs = 100;
    g_cfg.gameInputDeviceRefreshMs = read_ini_u32_bounded(
        L"General", L"GameInputDeviceRefreshMs", g_cfg.gameInputDeviceRefreshMs, 2000);
    g_cfg.gameInputStatusRefreshMs = read_ini_u32_bounded(
        L"General", L"GameInputStatusRefreshMs", g_cfg.gameInputStatusRefreshMs, 5000);
    g_cfg.rumbleWriteIntervalMs = read_ini_u32_bounded(
        L"General", L"RumbleWriteIntervalMs", g_cfg.rumbleWriteIntervalMs, 20);
    if (g_cfg.gameInputDeviceRefreshMs < 20) g_cfg.gameInputDeviceRefreshMs = 20;
    if (g_cfg.gameInputStatusRefreshMs < 100) g_cfg.gameInputStatusRefreshMs = 100;
    if (g_cfg.rumbleWriteIntervalMs < 1) g_cfg.rumbleWriteIntervalMs = 1;
    g_cfg.flushRva = read_ini_u64(L"Hook", L"QuadHapticsFlushRVA", g_cfg.flushRva);
    g_cfg.maxFrames = (u32)read_ini_i32(L"Hook", L"MaxFrames", (i32)g_cfg.maxFrames);
    if (g_cfg.maxFrames < 64) g_cfg.maxFrames = 64;
    if (g_cfg.maxFrames > 8192) g_cfg.maxFrames = 8192;

    g_cfg.gameplayStateHooks = (u32)read_ini_i32(
        L"GameplayState", L"Enabled", (i32)g_cfg.gameplayStateHooks);
    g_cfg.stateBindWindowMs = (u32)read_ini_i32(
        L"GameplayState", L"BindWindowMs", (i32)g_cfg.stateBindWindowMs);
    g_cfg.stateRebindIdleMs = (u32)read_ini_i32(
        L"GameplayState", L"RebindAfterIdleMs", (i32)g_cfg.stateRebindIdleMs);
    g_cfg.minimumAirborneMs = (u32)read_ini_i32(
        L"GameplayState", L"MinimumAirborneMs", (i32)g_cfg.minimumAirborneMs);
    g_cfg.landingDelayMs = (u32)read_ini_i32(
        L"GameplayState", L"LandingConfirmDelayMs", (i32)g_cfg.landingDelayMs);
    g_cfg.surfaceDelayMs = (u32)read_ini_i32(
        L"GameplayState", L"SurfaceConfirmDelayMs", (i32)g_cfg.surfaceDelayMs);
    g_cfg.stateProbeDelayMs = (u32)read_ini_i32(
        L"GameplayState", L"NativeProbeDelayMs", (i32)g_cfg.stateProbeDelayMs);
    g_cfg.jumpInputWindowMs = (u32)read_ini_i32(
        L"GameplayState", L"JumpInputWindowMs", (i32)g_cfg.jumpInputWindowMs);
    g_cfg.swimInputWindowMs = (u32)read_ini_i32(
        L"GameplayState", L"SwimInputWindowMs", (i32)g_cfg.swimInputWindowMs);
    g_cfg.autoWaterEntryReacquire = (u32)read_ini_i32(
        L"GameplayState", L"AutoWaterEntryReacquire",
        (i32)g_cfg.autoWaterEntryReacquire) ? 1u : 0u;
    g_cfg.autoWaterMovementThreshold = read_ini_u32_bounded(
        L"GameplayState", L"AutoWaterMovementThreshold",
        g_cfg.autoWaterMovementThreshold, 32767);
    g_cfg.autoWaterEvidenceWindowMs = read_ini_u32_bounded(
        L"GameplayState", L"AutoWaterEvidenceWindowMs",
        g_cfg.autoWaterEvidenceWindowMs, 10000);
    g_cfg.inputJumpFallback = (u32)read_ini_i32(
        L"GameplayState", L"InputJumpFallback", (i32)g_cfg.inputJumpFallback);
    g_cfg.inputLandingFallbackMs = (u32)read_ini_i32(
        L"GameplayState", L"InputLandingFallbackMs",
        (i32)g_cfg.inputLandingFallbackMs);
    g_cfg.inputLandingTimeoutMs = (u32)read_ini_i32(
        L"GameplayState", L"InputLandingTimeoutMs",
        (i32)g_cfg.inputLandingTimeoutMs);
    g_cfg.stateEffectStartTrimMs = (u32)read_ini_i32(
        L"GameplayState", L"EffectStartTrimMs",
        (i32)g_cfg.stateEffectStartTrimMs);
    g_cfg.climbAirCarryMs = (u32)read_ini_i32(
        L"GameplayState", L"ClimbAirCarryMs", (i32)g_cfg.climbAirCarryMs);
    g_cfg.parkourDistanceScaling = (u32)read_ini_i32(
        L"GameplayState", L"ParkourDistanceScaling", (i32)g_cfg.parkourDistanceScaling);
    g_cfg.parkourDistanceReferenceMs = (u32)read_ini_i32(
        L"GameplayState", L"ParkourDistanceReferenceMs", (i32)g_cfg.parkourDistanceReferenceMs);
    g_cfg.parkourLandingCooldownMs = (u32)read_ini_i32(
        L"GameplayState", L"ParkourLandingCooldownMs", (i32)g_cfg.parkourLandingCooldownMs);
    g_cfg.climbJumpStickThreshold = read_ini_i32(
        L"GameplayState", L"ClimbJumpStickThreshold", g_cfg.climbJumpStickThreshold);
    g_cfg.landingShortAirMs = (u32)read_ini_i32(
        L"GameplayState", L"LandingShortAirMs", (i32)g_cfg.landingShortAirMs);
    g_cfg.landingLongAirMs = (u32)read_ini_i32(
        L"GameplayState", L"LandingLongAirMs", (i32)g_cfg.landingLongAirMs);
    g_cfg.landingMaxAirMs = (u32)read_ini_i32(
        L"GameplayState", L"LandingMaxAirMs", (i32)g_cfg.landingMaxAirMs);
    if (g_cfg.stateBindWindowMs < 100) g_cfg.stateBindWindowMs = 100;
    if (g_cfg.stateBindWindowMs > 3000) g_cfg.stateBindWindowMs = 3000;
    if (g_cfg.stateRebindIdleMs < 3000) g_cfg.stateRebindIdleMs = 3000;
    if (g_cfg.stateRebindIdleMs > 120000) g_cfg.stateRebindIdleMs = 120000;
    if (g_cfg.minimumAirborneMs > 2000) g_cfg.minimumAirborneMs = 2000;
    if (g_cfg.landingDelayMs > 1000) g_cfg.landingDelayMs = 1000;
    if (g_cfg.surfaceDelayMs > 1000) g_cfg.surfaceDelayMs = 1000;
    if (g_cfg.stateProbeDelayMs > 1000) g_cfg.stateProbeDelayMs = 1000;
    if (g_cfg.jumpInputWindowMs < 100) g_cfg.jumpInputWindowMs = 100;
    if (g_cfg.jumpInputWindowMs > 3000) g_cfg.jumpInputWindowMs = 3000;
    if (g_cfg.swimInputWindowMs < 500) g_cfg.swimInputWindowMs = 500;
    if (g_cfg.swimInputWindowMs > 10000) g_cfg.swimInputWindowMs = 10000;
    if (g_cfg.autoWaterMovementThreshold > 32767)
        g_cfg.autoWaterMovementThreshold = 32767;
    if (g_cfg.autoWaterEvidenceWindowMs < 250)
        g_cfg.autoWaterEvidenceWindowMs = 250;
    if (g_cfg.autoWaterEvidenceWindowMs > 10000)
        g_cfg.autoWaterEvidenceWindowMs = 10000;
    if (g_cfg.inputLandingFallbackMs < 250) g_cfg.inputLandingFallbackMs = 250;
    if (g_cfg.inputLandingFallbackMs > 10000) g_cfg.inputLandingFallbackMs = 10000;
    if (g_cfg.inputLandingTimeoutMs < g_cfg.inputLandingFallbackMs)
        g_cfg.inputLandingTimeoutMs = g_cfg.inputLandingFallbackMs;
    if (g_cfg.inputLandingTimeoutMs > 60000) g_cfg.inputLandingTimeoutMs = 60000;
    if (g_cfg.climbAirCarryMs < 50) g_cfg.climbAirCarryMs = 50;
    if (g_cfg.climbAirCarryMs > 3000) g_cfg.climbAirCarryMs = 3000;
    if (g_cfg.parkourDistanceReferenceMs < 100) g_cfg.parkourDistanceReferenceMs = 100;
    if (g_cfg.parkourDistanceReferenceMs > 10000) g_cfg.parkourDistanceReferenceMs = 10000;
    if (g_cfg.parkourLandingCooldownMs > 2000) g_cfg.parkourLandingCooldownMs = 2000;
    if (g_cfg.stateEffectStartTrimMs > 1000) g_cfg.stateEffectStartTrimMs = 1000;
    if (g_cfg.climbJumpStickThreshold < 0)
        g_cfg.climbJumpStickThreshold = -g_cfg.climbJumpStickThreshold;
    if (g_cfg.climbJumpStickThreshold > 32000)
        g_cfg.climbJumpStickThreshold = 32000;
    if (g_cfg.landingShortAirMs < 100) g_cfg.landingShortAirMs = 100;
    if (g_cfg.landingLongAirMs < g_cfg.landingShortAirMs)
        g_cfg.landingLongAirMs = g_cfg.landingShortAirMs;
    if (g_cfg.landingMaxAirMs < g_cfg.landingLongAirMs)
        g_cfg.landingMaxAirMs = g_cfg.landingLongAirMs;
    if (g_cfg.landingMaxAirMs > 60000) g_cfg.landingMaxAirMs = 60000;

    g_cfg.nativeArbitration = (u32)read_ini_i32(
        L"NativeArbitration", L"Enabled", (i32)g_cfg.nativeArbitration);
    g_cfg.nativeCombatBoost = (u32)read_ini_i32(
        L"NativeArbitration", L"CombatNativeBoost", (i32)g_cfg.nativeCombatBoost);
    g_cfg.nativeCombatBoostWindowMs = (u32)read_ini_i32(
        L"NativeArbitration", L"CombatNativeBoostWindowMs",
        (i32)g_cfg.nativeCombatBoostWindowMs);
    g_cfg.nativeCombatHapticGain = read_ini_float(
        L"NativeArbitration", L"CombatNativeHapticGain",
        g_cfg.nativeCombatHapticGain);
    g_cfg.nativeHoldMs = (u32)read_ini_i32(
        L"NativeArbitration", L"HoldMs", (i32)g_cfg.nativeHoldMs);
    g_cfg.nativeProbeDelayMs = (u32)read_ini_i32(
        L"NativeArbitration", L"ButtonProbeDelayMs", (i32)g_cfg.nativeProbeDelayMs);
    g_cfg.dodgeProbeDelayMs = (u32)read_ini_i32(
        L"Latency", L"DodgeProbeDelayMs", (i32)g_cfg.dodgeProbeDelayMs);
    g_cfg.directionalDodgeProbeDelayMs = (u32)read_ini_i32(
        L"Latency", L"DirectionalDodgeProbeDelayMs",
        (i32)g_cfg.directionalDodgeProbeDelayMs);
    g_cfg.parryProbeDelayMs = (u32)read_ini_i32(
        L"Latency", L"ParryProbeDelayMs", (i32)g_cfg.parryProbeDelayMs);
    g_cfg.attackProbeDelayMs = (u32)read_ini_i32(
        L"Latency", L"AttackProbeDelayMs", (i32)g_cfg.attackProbeDelayMs);
    g_cfg.dpadLeftProbeDelayMs = (u32)read_ini_i32(
        L"Latency", L"DPadLeftProbeDelayMs", (i32)g_cfg.dpadLeftProbeDelayMs);
    g_cfg.dodgeStartTrimMs = (u32)read_ini_i32(
        L"Latency", L"DodgeStartTrimMs", (i32)g_cfg.dodgeStartTrimMs);
    g_cfg.directionalDodgeStartTrimMs = (u32)read_ini_i32(
        L"Latency", L"DirectionalDodgeStartTrimMs",
        (i32)g_cfg.directionalDodgeStartTrimMs);
    g_cfg.parryStartTrimMs = (u32)read_ini_i32(
        L"Latency", L"ParryStartTrimMs", (i32)g_cfg.parryStartTrimMs);
    g_cfg.dodgeStickThreshold = read_ini_i32(
        L"Latency", L"DirectionalDodgeStickThreshold", g_cfg.dodgeStickThreshold);
    if (g_cfg.nativeCombatBoostWindowMs < 50) g_cfg.nativeCombatBoostWindowMs = 50;
    if (g_cfg.nativeCombatBoostWindowMs > 2000) g_cfg.nativeCombatBoostWindowMs = 2000;
    if (g_cfg.nativeCombatHapticGain < 0.0f) g_cfg.nativeCombatHapticGain = 0.0f;
    if (g_cfg.nativeCombatHapticGain > 4.0f) g_cfg.nativeCombatHapticGain = 4.0f;
    if (g_cfg.nativeHoldMs < 20) g_cfg.nativeHoldMs = 20;
    if (g_cfg.nativeHoldMs > 2000) g_cfg.nativeHoldMs = 2000;
    if (g_cfg.nativeProbeDelayMs > 1000) g_cfg.nativeProbeDelayMs = 1000;
    if (g_cfg.dodgeProbeDelayMs > 1000) g_cfg.dodgeProbeDelayMs = 1000;
    if (g_cfg.directionalDodgeProbeDelayMs > 1000)
        g_cfg.directionalDodgeProbeDelayMs = 1000;
    if (g_cfg.parryProbeDelayMs > 1000) g_cfg.parryProbeDelayMs = 1000;
    if (g_cfg.attackProbeDelayMs > 1000) g_cfg.attackProbeDelayMs = 1000;
    if (g_cfg.dpadLeftProbeDelayMs > 1000) g_cfg.dpadLeftProbeDelayMs = 1000;
    if (g_cfg.dodgeStartTrimMs > 1000) g_cfg.dodgeStartTrimMs = 1000;
    if (g_cfg.directionalDodgeStartTrimMs > 1000)
        g_cfg.directionalDodgeStartTrimMs = 1000;
    if (g_cfg.parryStartTrimMs > 1000) g_cfg.parryStartTrimMs = 1000;
    if (g_cfg.dodgeStickThreshold < 0)
        g_cfg.dodgeStickThreshold = -g_cfg.dodgeStickThreshold;
    if (g_cfg.dodgeStickThreshold > 32000) g_cfg.dodgeStickThreshold = 32000;
    g_cfg.menuFeedback = (u32)read_ini_i32(
        L"Menus", L"Enabled", (i32)g_cfg.menuFeedback);
    g_cfg.menuProbeDelayMs = (u32)read_ini_i32(
        L"Menus", L"NativeProbeDelayMs", (i32)g_cfg.menuProbeDelayMs);
    g_cfg.menuShoulderProbeDelayMs = (u32)read_ini_i32(
        L"Menus", L"ShoulderNativeProbeDelayMs",
        (i32)g_cfg.menuShoulderProbeDelayMs);
    g_cfg.menuCloseMovementMs = (u32)read_ini_i32(
        L"Menus", L"CloseMovementConfirmMs", (i32)g_cfg.menuCloseMovementMs);
    g_cfg.menuTimeoutMs = (u32)read_ini_i32(
        L"Menus", L"SessionTimeoutMs", (i32)g_cfg.menuTimeoutMs);
    g_cfg.menuNativeTabArm = (u32)read_ini_i32(
        L"Menus", L"NativeTabArm", (i32)g_cfg.menuNativeTabArm);
    g_cfg.menuNativeTabWindowMs = (u32)read_ini_i32(
        L"Menus", L"NativeTabArmWindowMs", (i32)g_cfg.menuNativeTabWindowMs);
    g_cfg.menuNativeTabProbeMs = (u32)read_ini_i32(
        L"Menus", L"NativeTabProbeMs", (i32)g_cfg.menuNativeTabProbeMs);
    g_cfg.menuRawTouchpad = (u32)read_ini_i32(
        L"Menus", L"RawTouchpadDetection", (i32)g_cfg.menuRawTouchpad);
    g_cfg.menuRawTouchpadButtonIndex = (u32)read_ini_i32(
        L"Menus", L"RawTouchpadButtonIndex",
        (i32)g_cfg.menuRawTouchpadButtonIndex);
    g_cfg.menuRawTouchpadSonyOnly = (u32)read_ini_i32(
        L"Menus", L"RawTouchpadSonyOnly",
        (i32)g_cfg.menuRawTouchpadSonyOnly);
    g_cfg.menuTransitionGuardMs = (u32)read_ini_i32(
        L"Menus", L"TransitionGuardMs", (i32)g_cfg.menuTransitionGuardMs);
    g_cfg.menuStickThreshold = read_ini_i32(
        L"Menus", L"StickThreshold", g_cfg.menuStickThreshold);
    if (g_cfg.menuProbeDelayMs > 1000) g_cfg.menuProbeDelayMs = 1000;
    if (g_cfg.menuShoulderProbeDelayMs > 1000)
        g_cfg.menuShoulderProbeDelayMs = 1000;
    if (g_cfg.menuCloseMovementMs < 50) g_cfg.menuCloseMovementMs = 50;
    if (g_cfg.menuCloseMovementMs > 2000) g_cfg.menuCloseMovementMs = 2000;
    if (g_cfg.menuTimeoutMs < 10000) g_cfg.menuTimeoutMs = 10000;
    if (g_cfg.menuTimeoutMs > 600000) g_cfg.menuTimeoutMs = 600000;
    if (g_cfg.menuNativeTabWindowMs < 500) g_cfg.menuNativeTabWindowMs = 500;
    if (g_cfg.menuNativeTabWindowMs > 10000) g_cfg.menuNativeTabWindowMs = 10000;
    if (g_cfg.menuNativeTabProbeMs < 20) g_cfg.menuNativeTabProbeMs = 20;
    if (g_cfg.menuNativeTabProbeMs > 1000) g_cfg.menuNativeTabProbeMs = 1000;
    if (g_cfg.menuRawTouchpadButtonIndex > 63)
        g_cfg.menuRawTouchpadButtonIndex = 63;
    if (g_cfg.menuTransitionGuardMs > 1000)
        g_cfg.menuTransitionGuardMs = 1000;
    if (g_cfg.menuStickThreshold < 4000) g_cfg.menuStickThreshold = 4000;
    if (g_cfg.menuStickThreshold > 32000) g_cfg.menuStickThreshold = 32000;

    g_cfg.sinkRebindAfterMs = (u32)read_ini_i32(
        L"Recovery", L"SinkRebindAfterMs", (i32)g_cfg.sinkRebindAfterMs);
    g_cfg.reloadSilenceResetMs = (u32)read_ini_i32(
        L"Recovery", L"ReloadSilenceResetMs",
        (i32)g_cfg.reloadSilenceResetMs);
    if (g_cfg.sinkRebindAfterMs < 100) g_cfg.sinkRebindAfterMs = 100;
    if (g_cfg.sinkRebindAfterMs > 10000) g_cfg.sinkRebindAfterMs = 10000;
    if (g_cfg.reloadSilenceResetMs < 250) g_cfg.reloadSilenceResetMs = 250;
    if (g_cfg.reloadSilenceResetMs > 60000) g_cfg.reloadSilenceResetMs = 60000;

    g_cfg.walkThreshold = read_ini_i32(L"Movement", L"WalkThreshold", g_cfg.walkThreshold);
    g_cfg.runThreshold = read_ini_i32(L"Movement", L"RunThreshold", g_cfg.runThreshold);
    g_cfg.suppressMovementWithMenuButtons = (u32)read_ini_i32(
        L"Movement", L"SuppressWithStartOrBack", (i32)g_cfg.suppressMovementWithMenuButtons);
    g_cfg.sprintToggle = (u32)read_ini_i32(
        L"Movement", L"SprintToggle", (i32)g_cfg.sprintToggle);
    g_cfg.walkLoopStartMs = (u32)read_ini_i32(
        L"Movement", L"WalkLoopStartMs", (i32)g_cfg.walkLoopStartMs);
    g_cfg.walkLoopEndMs = (u32)read_ini_i32(
        L"Movement", L"WalkLoopEndMs", (i32)g_cfg.walkLoopEndMs);
    g_cfg.runLoopStartMs = (u32)read_ini_i32(
        L"Movement", L"RunLoopStartMs", (i32)g_cfg.runLoopStartMs);
    g_cfg.runLoopEndMs = (u32)read_ini_i32(
        L"Movement", L"RunLoopEndMs", (i32)g_cfg.runLoopEndMs);
    g_cfg.runOpeningDurationMs = (u32)read_ini_i32(
        L"Movement", L"RunOpeningDurationMs", (i32)g_cfg.runOpeningDurationMs);
    if (g_cfg.walkLoopStartMs > 5100) g_cfg.walkLoopStartMs = 5100;
    if (g_cfg.walkLoopEndMs > 5130) g_cfg.walkLoopEndMs = 5130;
    if (g_cfg.runLoopStartMs > 8500) g_cfg.runLoopStartMs = 8500;
    if (g_cfg.runLoopEndMs > 8590) g_cfg.runLoopEndMs = 8590;
    if (g_cfg.runOpeningDurationMs > 30000) g_cfg.runOpeningDurationMs = 30000;
    g_cfg.rbHoldThresholdMs = (u32)read_ini_i32(
        L"Buttons", L"RBHoldThresholdMs", (i32)g_cfg.rbHoldThresholdMs);
    g_cfg.rbHoldSecondDelayMs = (u32)read_ini_i32(
        L"Buttons", L"RBHoldSecondDelayMs", (i32)g_cfg.rbHoldSecondDelayMs);
    if (g_cfg.rbHoldThresholdMs < 100) g_cfg.rbHoldThresholdMs = 100;
    if (g_cfg.rbHoldThresholdMs > 2000) g_cfg.rbHoldThresholdMs = 2000;
    if (g_cfg.rbHoldSecondDelayMs < 100) g_cfg.rbHoldSecondDelayMs = 100;
    if (g_cfg.rbHoldSecondDelayMs > 3000) g_cfg.rbHoldSecondDelayMs = 3000;
    g_cfg.triggerThreshold = read_ini_i32(L"Input", L"TriggerThreshold", g_cfg.triggerThreshold);
    if (g_cfg.triggerThreshold < 1) g_cfg.triggerThreshold = 1;
    if (g_cfg.triggerThreshold > 255) g_cfg.triggerThreshold = 255;
    g_cfg.xinputRunPulseEnabled = (u32)read_ini_i32(
        L"Rumble", L"XInputRunPulseEnabled",
        (i32)g_cfg.xinputRunPulseEnabled);
    g_cfg.xinputRunPulseIntervalMs = (u32)read_ini_i32(
        L"Rumble", L"XInputRunPulseIntervalMs",
        (i32)g_cfg.xinputRunPulseIntervalMs);
    g_cfg.xinputRunPulseDurationMs = (u32)read_ini_i32(
        L"Rumble", L"XInputRunPulseDurationMs",
        (i32)g_cfg.xinputRunPulseDurationMs);
    if (g_cfg.xinputRunPulseIntervalMs < 100)
        g_cfg.xinputRunPulseIntervalMs = 100;
    if (g_cfg.xinputRunPulseIntervalMs > 2000)
        g_cfg.xinputRunPulseIntervalMs = 2000;
    if (g_cfg.xinputRunPulseDurationMs < 10)
        g_cfg.xinputRunPulseDurationMs = 10;
    if (g_cfg.xinputRunPulseDurationMs >= g_cfg.xinputRunPulseIntervalMs)
        g_cfg.xinputRunPulseDurationMs = g_cfg.xinputRunPulseIntervalMs - 10;

    // These bindings describe actions, not waveform slots.  Changing Jump to
    // Triangle, for example, moves the immediate takeoff request and the
    // Jump/InAir state-correlation gate together so Cross cannot remain as a
    // false hidden trigger. Invalid names fail safely to the tested defaults.
    g_cfg.invalidInputBindings = 0;
    g_cfg.jumpBinding = read_input_binding(
        L"Jump", "Cross", g_cfg.jumpBinding, g_cfg.invalidInputBindings);
    g_cfg.dodgeDiveBinding = read_input_binding(
        L"DodgeDive", "Circle", g_cfg.dodgeDiveBinding,
        g_cfg.invalidInputBindings);
    g_cfg.parryBinding = read_input_binding(
        L"Parry", "L1", g_cfg.parryBinding, g_cfg.invalidInputBindings);
    g_cfg.attackBinding = read_input_binding(
        L"Attack", "R1", g_cfg.attackBinding, g_cfg.invalidInputBindings);
    g_cfg.sprintLungeBinding = read_input_binding(
        L"SprintLunge", "L3", g_cfg.sprintLungeBinding,
        g_cfg.invalidInputBindings);
    g_cfg.sheatheHoodBinding = read_input_binding(
        L"SheatheHood", "DPadLeft", g_cfg.sheatheHoodBinding,
        g_cfg.invalidInputBindings);

    g_cfg.masterGain = read_ini_float(L"Playback", L"MasterGain", g_cfg.masterGain);
    g_cfg.movementGain = read_ini_float(L"Playback", L"MovementGain", g_cfg.movementGain);
    g_cfg.oneShotGain = read_ini_float(L"Playback", L"OneShotGain", g_cfg.oneShotGain);
    g_cfg.peakLimit = read_ini_float(L"Playback", L"PeakLimit", g_cfg.peakLimit);
    g_cfg.movementFadeMs = read_ini_float(L"Playback", L"MovementFadeMs", g_cfg.movementFadeMs);
    g_cfg.walkGain = read_ini_float(L"Movement", L"WalkGain", g_cfg.walkGain);
    {
        // Missing marker means legacy/raw semantics. This protects users who
        // carry forward an r9 INI containing DualSenseHapticMultiplier=4.80.
        i32 marker = read_ini_i32(L"Effect.Walk",
            L"DualSenseHapticNormalizedBaseline", -1);
        g_cfg.normalizedWalkHapticBaseline = marker == 1 ? 1u : 0u;
    }
    g_cfg.dualSenseWalkHapticMultiplier = read_ini_float(
        L"Effect.Walk", L"DualSenseHapticMultiplier",
        g_cfg.dualSenseWalkHapticMultiplier);
    g_cfg.dualSenseJogHapticMultiplier = read_ini_float(
        L"Effect.Run", L"DualSenseJogHapticMultiplier",
        g_cfg.dualSenseJogHapticMultiplier);
    g_cfg.runBaseGain = read_ini_float(L"Movement", L"RunBaseGain", g_cfg.runBaseGain);
    g_cfg.runOpeningGain = read_ini_float(
        L"Movement", L"RunOpeningGain", g_cfg.runOpeningGain);
    g_cfg.rightFootRightGain = read_ini_float(L"Footsteps", L"RightFootRightGain", g_cfg.rightFootRightGain);
    g_cfg.rightFootLeftGain = read_ini_float(L"Footsteps", L"RightFootLeftGain", g_cfg.rightFootLeftGain);
    g_cfg.leftFootLeftGain = read_ini_float(L"Footsteps", L"LeftFootLeftGain", g_cfg.leftFootLeftGain);
    g_cfg.leftFootRightGain = read_ini_float(L"Footsteps", L"LeftFootRightGain", g_cfg.leftFootRightGain);
    g_cfg.sprintContactOverlayGain = read_ini_float(L"Footsteps", L"SprintContactOverlayGain", g_cfg.sprintContactOverlayGain);
    g_cfg.rumbleGain = read_ini_float(L"Rumble", L"Gain", g_cfg.rumbleGain);
    g_cfg.rumbleMovementGain = read_ini_float(
        L"Rumble", L"MovementGain", g_cfg.rumbleMovementGain);
    g_cfg.rumbleLowScale = read_ini_float(L"Rumble", L"LowMotorScale", g_cfg.rumbleLowScale);
    g_cfg.rumbleHighScale = read_ini_float(L"Rumble", L"HighMotorScale", g_cfg.rumbleHighScale);
    g_cfg.rumbleDeadzone = read_ini_float(L"Rumble", L"Deadzone", g_cfg.rumbleDeadzone);
    g_cfg.xinputRunPulseGain = read_ini_float(
        L"Rumble", L"XInputRunPulseGain", g_cfg.xinputRunPulseGain);
    g_cfg.xinputLandRunStepOnly = (u32)read_ini_i32(
        L"Rumble", L"LandRunStepOnly", (i32)g_cfg.xinputLandRunStepOnly);
    g_cfg.xinputSprintStepGain = read_ini_float(
        L"Rumble", L"SprintStepGain", g_cfg.xinputSprintStepGain);
    g_cfg.xinputJumpLandingMaxDurationMs = (u32)read_ini_i32(
        L"Rumble", L"XInputJumpLandingMaxDurationMs",
        (i32)g_cfg.xinputJumpLandingMaxDurationMs);
    g_cfg.xinputLightAttackMaxDurationMs = (u32)read_ini_i32(
        L"Rumble", L"XInputLightAttackMaxDurationMs",
        (i32)g_cfg.xinputLightAttackMaxDurationMs);
    g_cfg.xinputActionZeroGapMs = (u32)read_ini_i32(
        L"Rumble", L"XInputActionZeroGapMs",
        (i32)g_cfg.xinputActionZeroGapMs);
    g_cfg.buttonLBGain = read_ini_float(L"Buttons", L"LBEffectGain", g_cfg.buttonLBGain);
    g_cfg.buttonRBGain = read_ini_float(L"Buttons", L"RBEffectGain", g_cfg.buttonRBGain);
    g_cfg.rbHoldGain = read_ini_float(L"Buttons", L"RBHoldGain", g_cfg.rbHoldGain);
    g_cfg.sprintKickGain = read_ini_float(
        L"Movement", L"SprintKickGain", g_cfg.sprintKickGain);
    g_cfg.jumpGain = read_ini_float(L"GameplayState", L"JumpGain", g_cfg.jumpGain);
    g_cfg.parkourDistanceMaxBoost = read_ini_float(
        L"GameplayState", L"ParkourDistanceMaxBoost", g_cfg.parkourDistanceMaxBoost);
    g_cfg.landingMinimumGain = read_ini_float(
        L"GameplayState", L"LandingMinimumGain", g_cfg.landingMinimumGain);
    g_cfg.landingShortGain = read_ini_float(
        L"GameplayState", L"LandingShortGain", g_cfg.landingShortGain);
    g_cfg.landingGain = read_ini_float(
        L"GameplayState", L"LandingGain", g_cfg.landingGain);
    g_cfg.landingLongGain = read_ini_float(
        L"GameplayState", L"LandingLongGain", g_cfg.landingLongGain);
    g_cfg.haystackGain = read_ini_float(
        L"GameplayState", L"HaystackGain", g_cfg.haystackGain);
    g_cfg.diveGain = read_ini_float(L"GameplayState", L"DiveGain", g_cfg.diveGain);
    g_cfg.surfaceGain = read_ini_float(
        L"GameplayState", L"SurfaceGain", g_cfg.surfaceGain);
    g_cfg.waterJumpGain = read_ini_float(
        L"GameplayState", L"WaterJumpGain", g_cfg.waterJumpGain);
    g_cfg.waterLungeGain = read_ini_float(
        L"GameplayState", L"WaterLungeGain", g_cfg.waterLungeGain);
    g_cfg.nativeHapticPeakThreshold = read_ini_float(
        L"NativeArbitration", L"HapticPeakThreshold", g_cfg.nativeHapticPeakThreshold);
    g_cfg.nativeHapticMeanThreshold = read_ini_float(
        L"NativeArbitration", L"HapticMeanThreshold", g_cfg.nativeHapticMeanThreshold);
    g_cfg.nativeRumbleThreshold = read_ini_float(
        L"NativeArbitration", L"RumbleThreshold", g_cfg.nativeRumbleThreshold);
    g_cfg.menuTickGain = read_ini_float(L"Menus", L"TickGain", g_cfg.menuTickGain);
    g_cfg.menuOpenGain = read_ini_float(L"Menus", L"OpenGain", g_cfg.menuOpenGain);
    g_cfg.menuShoulderGain = read_ini_float(
        L"Menus", L"ShoulderGain", g_cfg.menuShoulderGain);
    g_cfg.dualSenseBridgeHapticGain = read_ini_float(
        L"DualSense", L"BridgeHapticGain", g_cfg.dualSenseBridgeHapticGain);
    g_cfg.dualSenseJumpLandingHapticGain = read_ini_float(
        L"DualSense", L"JumpLandingHapticGain",
        g_cfg.dualSenseJumpLandingHapticGain);
    g_cfg.dualSenseLandMovementHapticGain = read_ini_float(
        L"DualSense", L"LandMovementHapticGain",
        g_cfg.dualSenseLandMovementHapticGain);
    g_cfg.dualSenseMenuShoulderGain = read_ini_float(
        L"Menus", L"DualSenseShoulderGainMultiplier",
        g_cfg.dualSenseMenuShoulderGain);
    g_cfg.dualSenseMenuTickGain = read_ini_float(
        L"Menus", L"DualSenseTickGainMultiplier",
        g_cfg.dualSenseMenuTickGain);
    g_cfg.dualSenseMenuTickNativeWindowMs = (u32)read_ini_i32(
        L"Menus", L"DualSenseTickNativeWindowMs",
        (i32)g_cfg.dualSenseMenuTickNativeWindowMs);
    g_cfg.xinputMenuShoulderGain = read_ini_float(
        L"Menus", L"XInputShoulderGainMultiplier",
        g_cfg.xinputMenuShoulderGain);
    if (g_cfg.masterGain < 0.0f) g_cfg.masterGain = 0.0f;
    if (g_cfg.masterGain > 4.0f) g_cfg.masterGain = 4.0f;
    if (g_cfg.movementGain < 0.0f) g_cfg.movementGain = 0.0f;
    if (g_cfg.movementGain > 8.0f) g_cfg.movementGain = 8.0f;
    if (g_cfg.oneShotGain < 0.0f) g_cfg.oneShotGain = 0.0f;
    if (g_cfg.oneShotGain > 4.0f) g_cfg.oneShotGain = 4.0f;
    if (g_cfg.peakLimit < 0.0f) g_cfg.peakLimit = 0.0f;
    if (g_cfg.peakLimit > 8.0f) g_cfg.peakLimit = 8.0f;
    if (g_cfg.movementFadeMs < 0.0f) g_cfg.movementFadeMs = 0.0f;
    if (g_cfg.movementFadeMs > 1000.0f) g_cfg.movementFadeMs = 1000.0f;
    if (g_cfg.walkGain < 0.0f) g_cfg.walkGain = 0.0f;
    if (g_cfg.walkGain > 4.0f) g_cfg.walkGain = 4.0f;
    if (g_cfg.dualSenseWalkHapticMultiplier < 0.0f)
        g_cfg.dualSenseWalkHapticMultiplier = 0.0f;
    if (g_cfg.normalizedWalkHapticBaseline) {
        // User-facing normalized scale: 1.00 == old raw 4.80, up to 4.80x
        // that baseline (effective 23.04). PeakLimit remains the final limiter.
        if (g_cfg.dualSenseWalkHapticMultiplier > 4.80f)
            g_cfg.dualSenseWalkHapticMultiplier = 4.80f;
    } else if (g_cfg.dualSenseWalkHapticMultiplier > 8.0f) {
        // Legacy r9/raw INI compatibility.
        g_cfg.dualSenseWalkHapticMultiplier = 8.0f;
    }
    if (g_cfg.dualSenseJogHapticMultiplier < 0.0f)
        g_cfg.dualSenseJogHapticMultiplier = 0.0f;
    if (g_cfg.dualSenseJogHapticMultiplier > 4.0f)
        g_cfg.dualSenseJogHapticMultiplier = 4.0f;
    if (g_cfg.runBaseGain < 0.0f) g_cfg.runBaseGain = 0.0f;
    if (g_cfg.runBaseGain > 4.0f) g_cfg.runBaseGain = 4.0f;
    if (g_cfg.runOpeningGain < 0.0f) g_cfg.runOpeningGain = 0.0f;
    if (g_cfg.runOpeningGain > 4.0f) g_cfg.runOpeningGain = 4.0f;
    if (g_cfg.rumbleGain < 0.0f) g_cfg.rumbleGain = 0.0f;
    if (g_cfg.rumbleGain > 8.0f) g_cfg.rumbleGain = 8.0f;
    if (g_cfg.rumbleMovementGain < 0.0f) g_cfg.rumbleMovementGain = 0.0f;
    if (g_cfg.rumbleMovementGain > 16.0f) g_cfg.rumbleMovementGain = 16.0f;
    if (g_cfg.rumbleLowScale < 0.0f) g_cfg.rumbleLowScale = 0.0f;
    if (g_cfg.rumbleLowScale > 16.0f) g_cfg.rumbleLowScale = 16.0f;
    if (g_cfg.rumbleHighScale < 0.0f) g_cfg.rumbleHighScale = 0.0f;
    if (g_cfg.rumbleHighScale > 16.0f) g_cfg.rumbleHighScale = 16.0f;
    if (g_cfg.rumbleDeadzone < 0.0f) g_cfg.rumbleDeadzone = 0.0f;
    if (g_cfg.rumbleDeadzone > 1.0f) g_cfg.rumbleDeadzone = 1.0f;
    if (g_cfg.xinputRunPulseGain < 0.0f) g_cfg.xinputRunPulseGain = 0.0f;
    if (g_cfg.xinputRunPulseGain > 4.0f) g_cfg.xinputRunPulseGain = 4.0f;
    if (g_cfg.xinputSprintStepGain < 0.0f) g_cfg.xinputSprintStepGain = 0.0f;
    if (g_cfg.xinputSprintStepGain > 4.0f) g_cfg.xinputSprintStepGain = 4.0f;
    if (g_cfg.xinputJumpLandingMaxDurationMs < 50)
        g_cfg.xinputJumpLandingMaxDurationMs = 50;
    if (g_cfg.xinputJumpLandingMaxDurationMs > 5000)
        g_cfg.xinputJumpLandingMaxDurationMs = 5000;
    if (g_cfg.xinputLightAttackMaxDurationMs < 50)
        g_cfg.xinputLightAttackMaxDurationMs = 50;
    if (g_cfg.xinputLightAttackMaxDurationMs > 2000)
        g_cfg.xinputLightAttackMaxDurationMs = 2000;
    if (g_cfg.xinputActionZeroGapMs > 100)
        g_cfg.xinputActionZeroGapMs = 100;
    if (g_cfg.rightFootRightGain < 0.0f) g_cfg.rightFootRightGain = 0.0f;
    if (g_cfg.rightFootRightGain > 2.0f) g_cfg.rightFootRightGain = 2.0f;
    if (g_cfg.rightFootLeftGain < 0.0f) g_cfg.rightFootLeftGain = 0.0f;
    if (g_cfg.rightFootLeftGain > 2.0f) g_cfg.rightFootLeftGain = 2.0f;
    if (g_cfg.leftFootLeftGain < 0.0f) g_cfg.leftFootLeftGain = 0.0f;
    if (g_cfg.leftFootLeftGain > 2.0f) g_cfg.leftFootLeftGain = 2.0f;
    if (g_cfg.leftFootRightGain < 0.0f) g_cfg.leftFootRightGain = 0.0f;
    if (g_cfg.leftFootRightGain > 2.0f) g_cfg.leftFootRightGain = 2.0f;
    if (g_cfg.sprintContactOverlayGain < 0.0f) g_cfg.sprintContactOverlayGain = 0.0f;
    if (g_cfg.sprintContactOverlayGain > 2.0f) g_cfg.sprintContactOverlayGain = 2.0f;
    if (g_cfg.buttonLBGain < 0.0f) g_cfg.buttonLBGain = 0.0f;
    if (g_cfg.buttonLBGain > 4.0f) g_cfg.buttonLBGain = 4.0f;
    if (g_cfg.buttonRBGain < 0.0f) g_cfg.buttonRBGain = 0.0f;
    if (g_cfg.buttonRBGain > 4.0f) g_cfg.buttonRBGain = 4.0f;
    if (g_cfg.rbHoldGain < 0.0f) g_cfg.rbHoldGain = 0.0f;
    if (g_cfg.rbHoldGain > 4.0f) g_cfg.rbHoldGain = 4.0f;
    if (g_cfg.sprintKickGain < 0.0f) g_cfg.sprintKickGain = 0.0f;
    if (g_cfg.sprintKickGain > 2.0f) g_cfg.sprintKickGain = 2.0f;
    if (g_cfg.jumpGain < 0.0f) g_cfg.jumpGain = 0.0f;
    if (g_cfg.jumpGain > 4.0f) g_cfg.jumpGain = 4.0f;
    if (g_cfg.parkourDistanceMaxBoost < 0.0f) g_cfg.parkourDistanceMaxBoost = 0.0f;
    if (g_cfg.parkourDistanceMaxBoost > 2.0f) g_cfg.parkourDistanceMaxBoost = 2.0f;
    if (g_cfg.landingMinimumGain < 0.0f) g_cfg.landingMinimumGain = 0.0f;
    if (g_cfg.landingMinimumGain > 4.0f) g_cfg.landingMinimumGain = 4.0f;
    if (g_cfg.landingShortGain < 0.0f) g_cfg.landingShortGain = 0.0f;
    if (g_cfg.landingShortGain > 4.0f) g_cfg.landingShortGain = 4.0f;
    if (g_cfg.landingGain < 0.0f) g_cfg.landingGain = 0.0f;
    if (g_cfg.landingGain > 4.0f) g_cfg.landingGain = 4.0f;
    if (g_cfg.landingLongGain < 0.0f) g_cfg.landingLongGain = 0.0f;
    if (g_cfg.landingLongGain > 4.0f) g_cfg.landingLongGain = 4.0f;
    if (g_cfg.haystackGain < 0.0f) g_cfg.haystackGain = 0.0f;
    if (g_cfg.haystackGain > 4.0f) g_cfg.haystackGain = 4.0f;
    if (g_cfg.diveGain < 0.0f) g_cfg.diveGain = 0.0f;
    if (g_cfg.diveGain > 4.0f) g_cfg.diveGain = 4.0f;
    if (g_cfg.surfaceGain < 0.0f) g_cfg.surfaceGain = 0.0f;
    if (g_cfg.surfaceGain > 4.0f) g_cfg.surfaceGain = 4.0f;
    if (g_cfg.waterJumpGain < 0.0f) g_cfg.waterJumpGain = 0.0f;
    if (g_cfg.waterJumpGain > 4.0f) g_cfg.waterJumpGain = 4.0f;
    if (g_cfg.waterLungeGain < 0.0f) g_cfg.waterLungeGain = 0.0f;
    if (g_cfg.waterLungeGain > 4.0f) g_cfg.waterLungeGain = 4.0f;
    if (g_cfg.nativeHapticPeakThreshold < 0.0f) g_cfg.nativeHapticPeakThreshold = 0.0f;
    if (g_cfg.nativeHapticPeakThreshold > 1.0f) g_cfg.nativeHapticPeakThreshold = 1.0f;
    if (g_cfg.nativeHapticMeanThreshold < 0.0f) g_cfg.nativeHapticMeanThreshold = 0.0f;
    if (g_cfg.nativeHapticMeanThreshold > 1.0f) g_cfg.nativeHapticMeanThreshold = 1.0f;
    if (g_cfg.nativeRumbleThreshold < 0.0f) g_cfg.nativeRumbleThreshold = 0.0f;
    if (g_cfg.nativeRumbleThreshold > 1.0f) g_cfg.nativeRumbleThreshold = 1.0f;
    if (g_cfg.menuTickGain < 0.0f) g_cfg.menuTickGain = 0.0f;
    if (g_cfg.menuTickGain > 4.0f) g_cfg.menuTickGain = 4.0f;
    if (g_cfg.menuOpenGain < 0.0f) g_cfg.menuOpenGain = 0.0f;
    if (g_cfg.menuOpenGain > 4.0f) g_cfg.menuOpenGain = 4.0f;
    if (g_cfg.menuShoulderGain < 0.0f) g_cfg.menuShoulderGain = 0.0f;
    if (g_cfg.menuShoulderGain > 4.0f) g_cfg.menuShoulderGain = 4.0f;
    if (g_cfg.dualSenseBridgeHapticGain < 0.0f) g_cfg.dualSenseBridgeHapticGain = 0.0f;
    if (g_cfg.dualSenseBridgeHapticGain > 4.0f) g_cfg.dualSenseBridgeHapticGain = 4.0f;
    if (g_cfg.dualSenseJumpLandingHapticGain < 0.0f) g_cfg.dualSenseJumpLandingHapticGain = 0.0f;
    if (g_cfg.dualSenseJumpLandingHapticGain > 4.0f) g_cfg.dualSenseJumpLandingHapticGain = 4.0f;
    if (g_cfg.dualSenseLandMovementHapticGain < 0.0f) g_cfg.dualSenseLandMovementHapticGain = 0.0f;
    if (g_cfg.dualSenseLandMovementHapticGain > 4.0f) g_cfg.dualSenseLandMovementHapticGain = 4.0f;
    if (g_cfg.dualSenseMenuShoulderGain < 0.0f) g_cfg.dualSenseMenuShoulderGain = 0.0f;
    if (g_cfg.dualSenseMenuShoulderGain > 4.0f) g_cfg.dualSenseMenuShoulderGain = 4.0f;
    if (g_cfg.dualSenseMenuTickGain < 0.0f) g_cfg.dualSenseMenuTickGain = 0.0f;
    if (g_cfg.dualSenseMenuTickGain > 4.0f) g_cfg.dualSenseMenuTickGain = 4.0f;
    if (g_cfg.dualSenseMenuTickNativeWindowMs < 20) g_cfg.dualSenseMenuTickNativeWindowMs = 20;
    if (g_cfg.dualSenseMenuTickNativeWindowMs > 1000) g_cfg.dualSenseMenuTickNativeWindowMs = 1000;
    if (g_cfg.xinputMenuShoulderGain < 0.0f) g_cfg.xinputMenuShoulderGain = 0.0f;
    if (g_cfg.xinputMenuShoulderGain > 4.0f) g_cfg.xinputMenuShoulderGain = 4.0f;

    read_ini_wide(L"Files", L"HapticsPack", L".\\ShadowsHapticsPack-v5.hfx",
                   g_cfg.packPath, 320);
    read_ini_ascii(L"Movement", L"WalkEffect", "walk_footsteps_sample", g_cfg.walk, 32);
    read_ini_ascii(L"Movement", L"RunEffect", "run_sustained_sample", g_cfg.run, 32);
    // AEffect is the optional legacy jump waveform slot. The configured Jump
    // action is handled contextually because it can mean jump, parkour-up, or
    // swim-up and input alone cannot prove every animation transition.
    read_ini_ascii(L"Buttons", L"AEffect", "", g_cfg.buttonA, 32);
    read_ini_ascii(L"Buttons", L"BEffect", "dodge", g_cfg.buttonB, 32);
    read_ini_ascii(L"Buttons", L"XEffect", "", g_cfg.buttonX, 32);
    read_ini_ascii(L"Buttons", L"YEffect", "", g_cfg.buttonY, 32);
    read_ini_ascii(L"Buttons", L"LBEffect", "parry", g_cfg.buttonLB, 32);
    read_ini_ascii(L"Buttons", L"RBEffect", "light_attack_single", g_cfg.buttonRB, 32);
    read_ini_ascii(L"Buttons", L"DPadUpEffect", "", g_cfg.dpadUp, 32);
    read_ini_ascii(L"Buttons", L"DPadDownEffect", "", g_cfg.dpadDown, 32);
    read_ini_ascii(L"Buttons", L"DPadLeftEffect", "weapon_sheathe_unsheathe_full", g_cfg.dpadLeft, 32);
    read_ini_ascii(L"Buttons", L"DPadRightEffect", "", g_cfg.dpadRight, 32);
    read_ini_ascii(L"Buttons", L"LeftTriggerEffect", "", g_cfg.leftTrigger, 32);
    read_ini_ascii(L"Buttons", L"RightTriggerEffect", "", g_cfg.rightTrigger, 32);
    read_ini_ascii(L"Movement", L"SprintKickEffect", "parry", g_cfg.sprintKick, 32);
    read_ini_ascii(L"Buttons", L"RBHoldEffect", "parry", g_cfg.rbHold, 32);
    read_ini_ascii(L"GameplayState", L"JumpEffect", "dodge", g_cfg.jump, 32);
    read_ini_ascii(L"GameplayState", L"LandingEffect", "dodge", g_cfg.landing, 32);
    read_ini_ascii(L"GameplayState", L"HaystackEffect", "dodge", g_cfg.haystack, 32);
    read_ini_ascii(L"GameplayState", L"DiveEffect", "dodge", g_cfg.dive, 32);
    read_ini_ascii(L"GameplayState", L"SurfaceEffect", "dodge", g_cfg.surface, 32);
    read_ini_ascii(L"GameplayState", L"WaterJumpEffect", "dodge", g_cfg.waterJump, 32);
    read_ini_ascii(L"GameplayState", L"WaterLungeEffect", "dodge", g_cfg.waterLunge, 32);
    read_ini_ascii(L"Menus", L"TickEffect", "weapon_transition_short", g_cfg.menuTick, 32);
    read_ini_ascii(L"Menus", L"OpenEffect", "dodge", g_cfg.menuOpen, 32);

    // User-facing per-effect layer.  These neutral controls are loaded after
    // the legacy r4 settings so an existing INI remains fully compatible.
    load_effect_control(L"Effect.Walk", g_cfg.fxWalk);
    load_effect_control(L"Effect.Run", g_cfg.fxRun);
    load_effect_control(L"Effect.SprintKick", g_cfg.fxSprintKick);
    load_effect_control(L"Effect.Jump", g_cfg.fxJump);
    load_effect_control(L"Effect.LandingShort", g_cfg.fxLandingShort);
    load_effect_control(L"Effect.LandingNormal", g_cfg.fxLandingNormal);
    load_effect_control(L"Effect.LandingLong", g_cfg.fxLandingLong);
    load_effect_control(L"Effect.HaystackLanding", g_cfg.fxHaystack);
    load_effect_control(L"Effect.GroundDodge", g_cfg.fxDodge);
    load_effect_control(L"Effect.DirectionalDodge", g_cfg.fxDirectionalDodge);
    load_effect_control(L"Effect.WaterDive", g_cfg.fxDive);
    load_effect_control(L"Effect.WaterSurface", g_cfg.fxSurface);
    load_effect_control(L"Effect.SwimUp", g_cfg.fxWaterJump);
    load_effect_control(L"Effect.WaterLunge", g_cfg.fxWaterLunge);
    load_effect_control(L"Effect.Parry", g_cfg.fxParry);
    load_effect_control(L"Effect.LightAttack", g_cfg.fxLightAttack);
    load_effect_control(L"Effect.HeavyAttackFirst", g_cfg.fxHeavyFirst);
    load_effect_control(L"Effect.HeavyAttackSecond", g_cfg.fxHeavySecond);
    load_effect_control(L"Effect.SheatheHood", g_cfg.fxSheatheHood);
    load_effect_control(L"Effect.MenuTick", g_cfg.fxMenuTick);
    load_effect_control(L"Effect.MenuOpen", g_cfg.fxMenuOpen);
    load_effect_control(L"Effect.MenuShoulder", g_cfg.fxMenuShoulder);

    g_cfg.walkLoopStartMs = read_ini_u32_bounded(
        L"Effect.Walk", L"LoopStartMs", g_cfg.walkLoopStartMs, 5100);
    g_cfg.walkLoopEndMs = read_ini_u32_bounded(
        L"Effect.Walk", L"LoopEndMs", g_cfg.walkLoopEndMs, 5130);
    g_cfg.walkFadeMs = read_ini_float(
        L"Effect.Walk", L"FadeMs", g_cfg.movementFadeMs);
    g_cfg.runLoopStartMs = read_ini_u32_bounded(
        L"Effect.Run", L"LoopStartMs", g_cfg.runLoopStartMs, 8500);
    g_cfg.runLoopEndMs = read_ini_u32_bounded(
        L"Effect.Run", L"LoopEndMs", g_cfg.runLoopEndMs, 8590);
    g_cfg.runOpeningDurationMs = read_ini_u32_bounded(
        L"Effect.Run", L"OpeningDurationMs",
        g_cfg.runOpeningDurationMs, 30000);
    g_cfg.runFadeMs = read_ini_float(
        L"Effect.Run", L"FadeMs", g_cfg.movementFadeMs);

    g_cfg.jumpCooldownMs = read_ini_u32_bounded(
        L"Effect.Jump", L"CooldownMs", g_cfg.jumpCooldownMs, 60000);
    g_cfg.landingCooldownMs = read_ini_u32_bounded(
        L"Effect.LandingNormal", L"CooldownMs",
        g_cfg.landingCooldownMs, 60000);
    g_cfg.haystackCooldownMs = read_ini_u32_bounded(
        L"Effect.HaystackLanding", L"CooldownMs",
        g_cfg.haystackCooldownMs, 60000);
    g_cfg.diveStateCooldownMs = read_ini_u32_bounded(
        L"Effect.WaterDive", L"StateCooldownMs",
        g_cfg.diveStateCooldownMs, 60000);
    g_cfg.diveInputCooldownMs = read_ini_u32_bounded(
        L"Effect.WaterDive", L"InputCooldownMs",
        g_cfg.diveInputCooldownMs, 60000);
    g_cfg.surfaceCooldownMs = read_ini_u32_bounded(
        L"Effect.WaterSurface", L"CooldownMs",
        g_cfg.surfaceCooldownMs, 60000);
    g_cfg.waterJumpCooldownMs = read_ini_u32_bounded(
        L"Effect.SwimUp", L"CooldownMs", g_cfg.waterJumpCooldownMs, 60000);
    g_cfg.menuRepeatDelayMs = read_ini_u32_bounded(
        L"Effect.MenuTick", L"InitialRepeatDelayMs",
        g_cfg.menuRepeatDelayMs, 10000);
    g_cfg.menuRepeatIntervalMs = read_ini_u32_bounded(
        L"Effect.MenuTick", L"RepeatIntervalMs",
        g_cfg.menuRepeatIntervalMs, 10000);
    g_cfg.rbHoldThresholdMs = read_ini_u32_bounded(
        L"Effect.HeavyAttackFirst", L"HoldThresholdMs",
        g_cfg.rbHoldThresholdMs, 2000);
    g_cfg.rbHoldSecondDelayMs = read_ini_u32_bounded(
        L"Effect.HeavyAttackSecond", L"DelayAfterFirstMs",
        g_cfg.heavyPhase2DelayMs, 3000);
    // One user-visible start-to-start delay now controls both semantic and
    // legacy heavy paths. Older SemanticEvents/HeavyPhase2DelayMs values remain
    // a fallback when DelayAfterFirstMs is absent.
    g_cfg.heavyPhase2DelayMs = g_cfg.rbHoldSecondDelayMs;

    if (g_cfg.walkLoopStartMs > 5100) g_cfg.walkLoopStartMs = 5100;
    if (g_cfg.walkLoopEndMs > 5130) g_cfg.walkLoopEndMs = 5130;
    if (g_cfg.runLoopStartMs > 8500) g_cfg.runLoopStartMs = 8500;
    if (g_cfg.runLoopEndMs > 8590) g_cfg.runLoopEndMs = 8590;
    if (g_cfg.runOpeningDurationMs > 30000)
        g_cfg.runOpeningDurationMs = 30000;
    if (g_cfg.walkFadeMs < 0.0f) g_cfg.walkFadeMs = 0.0f;
    if (g_cfg.walkFadeMs > 1000.0f) g_cfg.walkFadeMs = 1000.0f;
    if (g_cfg.runFadeMs < 0.0f) g_cfg.runFadeMs = 0.0f;
    if (g_cfg.runFadeMs > 1000.0f) g_cfg.runFadeMs = 1000.0f;
    if (g_cfg.jumpCooldownMs > 60000) g_cfg.jumpCooldownMs = 60000;
    if (g_cfg.landingCooldownMs > 60000) g_cfg.landingCooldownMs = 60000;
    if (g_cfg.haystackCooldownMs > 60000) g_cfg.haystackCooldownMs = 60000;
    if (g_cfg.diveStateCooldownMs > 60000) g_cfg.diveStateCooldownMs = 60000;
    if (g_cfg.diveInputCooldownMs > 60000) g_cfg.diveInputCooldownMs = 60000;
    if (g_cfg.surfaceCooldownMs > 60000) g_cfg.surfaceCooldownMs = 60000;
    if (g_cfg.waterJumpCooldownMs > 60000) g_cfg.waterJumpCooldownMs = 60000;
    if (g_cfg.menuRepeatDelayMs > 10000) g_cfg.menuRepeatDelayMs = 10000;
    if (g_cfg.menuRepeatIntervalMs < 20) g_cfg.menuRepeatIntervalMs = 20;
    if (g_cfg.menuRepeatIntervalMs > 10000) g_cfg.menuRepeatIntervalMs = 10000;
    if (g_cfg.rbHoldThresholdMs < 100) g_cfg.rbHoldThresholdMs = 100;
    if (g_cfg.rbHoldThresholdMs > 2000) g_cfg.rbHoldThresholdMs = 2000;
    if (g_cfg.rbHoldSecondDelayMs < 100) g_cfg.rbHoldSecondDelayMs = 100;
    if (g_cfg.rbHoldSecondDelayMs > 3000) g_cfg.rbHoldSecondDelayMs = 3000;

    atomic_store_u32(&g_enabled, g_cfg.enabled ? 1u : 0u);
    atomic_store_u32(&g_inputEnabled, g_cfg.inputBridge ? 1u : 0u);
}

struct HfxHeaderRaw {
    char magic[8];
    u32 version;
    u32 sampleRate;
    u32 channels;
    u32 effectCount;
    u32 headerBytes;
    u32 reserved;
    u64 dataOffset;
};
struct HfxEntryRaw {
    char name[32];
    u64 dataOffset;
    u64 frames;
    float maxAbs;
    u32 sampleRate;
    u32 channels;
    u32 flags;
    u8 reserved[16];
};
static_assert(sizeof(HfxHeaderRaw) == 40, "Unexpected HFX header layout");
static_assert(sizeof(HfxEntryRaw) == 80, "Unexpected HFX entry layout");

struct Effect {
    char name[33]{};
    const float* interleaved{};
    u64 frames{};
    float maxAbs{};
};
static constexpr u32 MAX_EFFECTS = 64;
static Effect g_effects[MAX_EFFECTS]{};
static u32 g_effectCount{};
static void* g_packMemory{};
static u64 g_packBytes{};

static bool byte_range_valid(u64 offset, u64 size, u64 total) {
    return offset <= total && size <= total - offset;
}

static bool load_haptics_pack() {
    HANDLE file = g_api.CreateFileW(g_cfg.packPath, GENERIC_READ, FILE_SHARE_READ,
                                    nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (!file || file == invalid_handle_value()) {
        log_simple("ERROR could not open Shadows haptics pack");
        return false;
    }
    LARGE_INTEGER size{};
    if (!g_api.GetFileSizeEx(file, &size) || size.QuadPart < (i64)sizeof(HfxHeaderRaw) ||
        size.QuadPart > 64ll * 1024ll * 1024ll) {
        g_api.CloseHandle(file);
        log_simple("ERROR haptics pack size is invalid");
        return false;
    }
    u8* memory = (u8*)g_api.VirtualAlloc(nullptr, (usize)size.QuadPart,
                                         MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!memory) {
        g_api.CloseHandle(file);
        log_simple("ERROR could not allocate haptics pack memory");
        return false;
    }
    u64 done = 0;
    while (done < (u64)size.QuadPart) {
        u64 remain = (u64)size.QuadPart - done;
        DWORD request = remain > 16u * 1024u * 1024u ? 16u * 1024u * 1024u : (DWORD)remain;
        DWORD got = 0;
        if (!g_api.ReadFile(file, memory + done, request, &got, nullptr) || got != request) {
            g_api.CloseHandle(file);
            log_simple("ERROR haptics pack read was incomplete");
            return false;
        }
        done += got;
    }
    g_api.CloseHandle(file);

    const HfxHeaderRaw* header = (const HfxHeaderRaw*)memory;
    static const char expectedMagic[8] = {'S','H','F','X','P','K','1',0};
    if (memcmp(header->magic, expectedMagic, 8) != 0 || header->version != 1 ||
        header->sampleRate != 48000 || header->channels != 2 ||
        header->effectCount == 0 || header->effectCount > MAX_EFFECTS ||
        header->headerBytes < sizeof(HfxHeaderRaw)) {
        log_simple("ERROR haptics pack header is unsupported; expected SHFXPK1, 48 kHz stereo");
        return false;
    }
    u64 tableBytes = (u64)header->effectCount * sizeof(HfxEntryRaw);
    if (!byte_range_valid(header->headerBytes, tableBytes, (u64)size.QuadPart) ||
        header->dataOffset < (u64)header->headerBytes + tableBytes ||
        header->dataOffset > (u64)size.QuadPart) {
        log_simple("ERROR haptics pack table is out of range");
        return false;
    }

    const HfxEntryRaw* entries = (const HfxEntryRaw*)(memory + header->headerBytes);
    for (u32 i = 0; i < header->effectCount; ++i) {
        const HfxEntryRaw& raw = entries[i];
        if (!raw.frames || raw.sampleRate != 48000 || raw.channels != 2 ||
            raw.frames > 10000000ull ||
            !byte_range_valid(raw.dataOffset, raw.frames * 2ull * sizeof(float),
                              (u64)size.QuadPart)) {
            log_simple("ERROR invalid effect entry in haptics pack");
            return false;
        }
        Effect& effect = g_effects[i];
        u32 n = 0;
        while (n < 32 && raw.name[n]) { effect.name[n] = raw.name[n]; ++n; }
        effect.name[n] = 0;
        if (!effect.name[0]) {
            log_simple("ERROR unnamed effect in haptics pack");
            return false;
        }
        effect.interleaved = (const float*)(memory + raw.dataOffset);
        effect.frames = raw.frames;
        effect.maxAbs = raw.maxAbs;
    }

    g_packMemory = memory;
    g_packBytes = (u64)size.QuadPart;
    g_effectCount = header->effectCount;

    TextBuf b{};
    SYSTEMTIME st{};
    if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    tb_char(b, '['); tb_time(b, st); tb_str(b, "] Loaded ShadowsHapticsPack: effects=");
    tb_u64(b, g_effectCount); tb_str(b, " bytes="); tb_u64(b, g_packBytes); tb_endline(b);
    append_log(b.bytes, b.len);
    for (u32 i = 0; i < g_effectCount; ++i) {
        TextBuf e{};
        if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
        tb_char(e, '['); tb_time(e, st); tb_str(e, "]   effect["); tb_u64(e, i);
        tb_str(e, "] "); tb_str(e, g_effects[i].name); tb_str(e, " frames=");
        tb_u64(e, g_effects[i].frames); tb_str(e, " peak="); tb_float3(e, g_effects[i].maxAbs);
        tb_endline(e); append_log(e.bytes, e.len);
    }
    return true;
}

static i32 find_effect(const char* name) {
    if (!name || !name[0]) return -1;
    for (u32 i = 0; i < g_effectCount; ++i)
        if (streq_ci(name, g_effects[i].name)) return (i32)i;
    return -1;
}

struct ResolvedMapping {
    i32 walk{-1};
    i32 run{-1};
    i32 buttonA{-1};
    i32 buttonB{-1};
    i32 buttonX{-1};
    i32 buttonY{-1};
    i32 buttonLB{-1};
    i32 buttonRB{-1};
    i32 dpadUp{-1};
    i32 dpadDown{-1};
    i32 dpadLeft{-1};
    i32 dpadRight{-1};
    i32 leftTrigger{-1};
    i32 rightTrigger{-1};
    i32 sprintKick{-1};
    i32 rbHold{-1};
    i32 jump{-1};
    i32 landing{-1};
    i32 haystack{-1};
    i32 dive{-1};
    i32 surface{-1};
    i32 waterJump{-1};
    i32 waterLunge{-1};
    i32 menuTick{-1};
    i32 menuOpen{-1};
    i32 dodge{-1};
    i32 light{-1};
    i32 heavy{-1};
    i32 parry{-1};
    i32 playerHit{-1};
    i32 enemyHit{-1};
    i32 weapon{-1};
    i32 charged{-1};
    i32 environment{-1};
};
static ResolvedMapping g_map;

static void log_mapping(const char* label, const char* configured, i32 resolved) {
    TextBuf b{};
    SYSTEMTIME st{};
    if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    tb_char(b, '['); tb_time(b, st); tb_str(b, "] Mapping "); tb_str(b, label);
    tb_str(b, "="); tb_str(b, configured && configured[0] ? configured : "<disabled>");
    tb_str(b, " index="); tb_i64(b, resolved); tb_endline(b); append_log(b.bytes, b.len);
}

static void resolve_mappings() {
    g_map.walk = find_effect(g_cfg.walk);
    g_map.run = find_effect(g_cfg.run);
    g_map.buttonA = find_effect(g_cfg.buttonA);
    g_map.buttonB = find_effect(g_cfg.buttonB);
    g_map.buttonX = find_effect(g_cfg.buttonX);
    g_map.buttonY = find_effect(g_cfg.buttonY);
    g_map.buttonLB = find_effect(g_cfg.buttonLB);
    g_map.buttonRB = find_effect(g_cfg.buttonRB);
    g_map.dpadUp = find_effect(g_cfg.dpadUp);
    g_map.dpadDown = find_effect(g_cfg.dpadDown);
    g_map.dpadLeft = find_effect(g_cfg.dpadLeft);
    g_map.dpadRight = find_effect(g_cfg.dpadRight);
    g_map.leftTrigger = find_effect(g_cfg.leftTrigger);
    g_map.rightTrigger = find_effect(g_cfg.rightTrigger);
    g_map.sprintKick = find_effect(g_cfg.sprintKick);
    g_map.rbHold = find_effect(g_cfg.rbHold);
    g_map.jump = find_effect(g_cfg.jump);
    g_map.landing = find_effect(g_cfg.landing);
    g_map.haystack = find_effect(g_cfg.haystack);
    g_map.dive = find_effect(g_cfg.dive);
    g_map.surface = find_effect(g_cfg.surface);
    g_map.waterJump = find_effect(g_cfg.waterJump);
    g_map.waterLunge = find_effect(g_cfg.waterLunge);
    g_map.menuTick = find_effect(g_cfg.menuTick);
    g_map.menuOpen = find_effect(g_cfg.menuOpen);
    g_map.dodge = find_effect("dodge");
    g_map.light = find_effect("light_attack_single");
    g_map.heavy = find_effect("heavy_r2");
    g_map.parry = find_effect("parry");
    g_map.playerHit = find_effect("player_hit");
    g_map.enemyHit = find_effect("enemy_hit");
    g_map.weapon = find_effect("weapon_sheathe_unsheathe_full");
    g_map.charged = find_effect("charged_r1");
    g_map.environment = find_effect("environment_general_sample");

    log_mapping("Walk", g_cfg.walk, g_map.walk);
    log_mapping("Run", g_cfg.run, g_map.run);
    log_mapping("A", g_cfg.buttonA, g_map.buttonA);
    log_mapping("B", g_cfg.buttonB, g_map.buttonB);
    log_mapping("X", g_cfg.buttonX, g_map.buttonX);
    log_mapping("Y", g_cfg.buttonY, g_map.buttonY);
    log_mapping("LB", g_cfg.buttonLB, g_map.buttonLB);
    log_mapping("RB", g_cfg.buttonRB, g_map.buttonRB);
    log_mapping("DPadLeft", g_cfg.dpadLeft, g_map.dpadLeft);
    log_mapping("SprintKick", g_cfg.sprintKick, g_map.sprintKick);
    log_mapping("RBHold", g_cfg.rbHold, g_map.rbHold);
    log_mapping("JumpState", g_cfg.jump, g_map.jump);
    log_mapping("LandingState", g_cfg.landing, g_map.landing);
    log_mapping("HaystackState", g_cfg.haystack, g_map.haystack);
    log_mapping("DiveState", g_cfg.dive, g_map.dive);
    log_mapping("SurfaceState", g_cfg.surface, g_map.surface);
    log_mapping("WaterJumpInput", g_cfg.waterJump, g_map.waterJump);
    log_mapping("WaterLungeInput", g_cfg.waterLunge, g_map.waterLunge);
    log_mapping("MenuTick", g_cfg.menuTick, g_map.menuTick);
    log_mapping("MenuOpen", g_cfg.menuOpen, g_map.menuOpen);
}

static constexpr u32 COMMAND_RING_SIZE = 256;
struct TriggerCommand {
    volatile u32 ready{};
    i32 effect{-1};
    float gain{1.0f};
    float leftGain{1.0f};
    float rightGain{1.0f};
    u32 source{};
    u32 startFrame{};
    u32 maxFrames{}; // 0 = play to end of effect
    u32 generation{};
};
static TriggerCommand g_commands[COMMAND_RING_SIZE]{};
static volatile u32 g_commandWrite{};
static volatile u32 g_commandRead{};
static TriggerCommand g_rumbleCommands[COMMAND_RING_SIZE]{};
static volatile u32 g_rumbleCommandWrite{};
static volatile u32 g_rumbleCommandRead{};
static volatile u64 g_commandsQueued{};
static volatile u64 g_commandsDropped{};
static volatile u64 g_rumbleCommandsQueued{};
static volatile u64 g_rumbleCommandsDropped{};
static volatile u32 g_movementMode{};
static volatile u64 g_runStartTick{};
static volatile u32 g_movementScalePermille = 1000;
static volatile u32 g_cancelGeneration{};
static volatile u64 g_workerNowTick{};
static volatile u64 g_nativeHapticUntil{};
static volatile u64 g_nativeRumbleUntil{};
static volatile u64 g_nativeHapticDetections{};
static volatile u64 g_nativeRumbleDetections{};
static volatile u64 g_nativeFeedbackSerial{};
static volatile u64 g_nativeSuppressedEffects{};
static volatile u64 g_nativeSuppressedHapticBlocks{};
static volatile u64 g_nativeSuppressedRumbleTicks{};
static volatile u32 g_lastNativePeakMillion{};
static volatile u32 g_lastNativeMeanMillion{};
static volatile u32 g_lastNativeRumblePermille{};
// r9b action-scoped Ubisoft-native combat haptic supplement.
static volatile u64 g_nativeCombatBoostUntil{};
static volatile u64 g_nativeCombatBoostArms{};
static volatile u64 g_nativeCombatBoostBlocks{};
static volatile u64 g_nativeCombatBoostFrames{};
static volatile u32 g_nativeCombatBoostLastEvent{};
// r9f: verified Ubisoft-native menu tick boost on DualSense only.
static volatile u64 g_nativeMenuTickBoostUntil{};
static volatile u64 g_nativeMenuTickBoostArms{};
static volatile u64 g_nativeMenuTickBoostBlocks{};
static volatile u64 g_nativeMenuTickBoostFrames{};

static void atomic_extend_until(volatile u64* destination, u64 value) {
    u64 current = atomic_load_u64(destination);
    while (value > current) {
        if (atomic_cas_u64(destination, current, value)) return;
        current = atomic_load_u64(destination);
    }
}

static bool native_haptic_active(u64 now) {
    return g_cfg.nativeArbitration && now < atomic_load_u64(&g_nativeHapticUntil);
}

static bool native_rumble_active(u64 now) {
    return g_cfg.nativeArbitration && now < atomic_load_u64(&g_nativeRumbleUntil);
}

static bool native_feedback_active(u64 now) {
    return native_haptic_active(now) || native_rumble_active(now);
}

static void mark_native_rumble(float strength, u64 now) {
    if (strength < 0.0f) strength = 0.0f;
    if (strength > 1.0f) strength = 1.0f;
    atomic_store_u32(&g_lastNativeRumblePermille,
                     (u32)(strength * 1000.0f + 0.5f));
    if (!g_cfg.nativeArbitration || strength < g_cfg.nativeRumbleThreshold) return;
    atomic_extend_until(&g_nativeRumbleUntil, now + g_cfg.nativeHoldMs);
    atomic_fetch_add_u64(&g_nativeRumbleDetections, 1);
    atomic_fetch_add_u64(&g_nativeFeedbackSerial, 1);
}

static u32 frames_from_ms(u32 milliseconds) {
    u64 frames = (u64)milliseconds * 48ull;
    return frames > 0xFFFFFFFFull ? 0xFFFFFFFFu : (u32)frames;
}

static u32 adjusted_milliseconds(u32 baseline, i32 offset) {
    i64 adjusted = (i64)baseline + (i64)offset;
    if (adjusted < 0) adjusted = 0;
    if (adjusted > 60000) adjusted = 60000;
    return (u32)adjusted;
}

static u32 bounded_start_frame(i32 effect, u32 requested) {
    if (effect < 0 || (u32)effect >= g_effectCount || !requested) return 0;
    u64 frames = g_effects[effect].frames;
    if (frames <= 1) return 0;
    return (u64)requested < frames ? requested : (u32)(frames - 1);
}

static bool queue_haptic_effect(i32 effect, float gain, u32 source,
                                u32 startFrame = 0, u32 maxFrames = 0,
                                float leftGain = 1.0f, float rightGain = 1.0f) {
    if (effect < 0 || (u32)effect >= g_effectCount) return false;
    u32 write = atomic_load_u32(&g_commandWrite);
    u32 read = atomic_load_u32(&g_commandRead);
    if (write - read >= COMMAND_RING_SIZE) {
        atomic_fetch_add_u64(&g_commandsDropped, 1);
        return false;
    }
    TriggerCommand& cmd = g_commands[write & (COMMAND_RING_SIZE - 1)];
    cmd.effect = effect;
    cmd.gain = gain;
    cmd.leftGain = leftGain;
    cmd.rightGain = rightGain;
    cmd.source = source;
    cmd.startFrame = bounded_start_frame(effect, startFrame);
    cmd.maxFrames = maxFrames;
    cmd.generation = atomic_load_u32(&g_cancelGeneration);
    atomic_store_u32(&cmd.ready, write + 1);
    atomic_store_u32(&g_commandWrite, write + 1);
    atomic_fetch_add_u64(&g_commandsQueued, 1);
    return true;
}

static bool r9c_is_jump_or_landing_rumble_source(u32 source) {
    // Canonical land-jump / landing sources. Special haystack and water effects
    // keep their established timing.
    return source == 201u || source == 208u || source == 202u ||
           source == 209u || source == 214u;
}

// r9f: these are bridge-only DualSense multipliers. They never touch the
// game's native Quad buffer, including the r9b native-combat supplement.
static float r9e_dualsense_haptic_source_gain(u32 source) {
    if (!atomic_load_u32(&g_activeDualSense)) return 1.0f;
    float gain = g_cfg.dualSenseBridgeHapticGain;
    if (r9c_is_jump_or_landing_rumble_source(source))
        gain *= g_cfg.dualSenseJumpLandingHapticGain;
    // Real land gait-contact pulses. Continuous land walk/run is handled in
    // mix_movement because it has no TriggerCommand source tag.
    if (source == 820u || source == 821u)
        gain *= g_cfg.dualSenseLandMovementHapticGain;
    if (source == 303u)
        gain *= g_cfg.dualSenseMenuShoulderGain;
    if (source == 302u || source == 801u)
        gain *= g_cfg.dualSenseMenuTickGain;
    return gain;
}

static float r9e_rumble_source_gain(u32 source) {
    // DualSense conventional rumble is a parallel bridge output, not an
    // overflow/fallback from the 48 kHz haptic stream. Apply the same r9f
    // bridge/category cleanup to that parallel copy so perceived feedback
    // follows the requested tuning on DualSense. Ubisoft-native output is not
    // routed through this function.
    if (atomic_load_u32(&g_activeDualSense))
        return r9e_dualsense_haptic_source_gain(source);
    if (source == 303u && r9_xinput_class_device())
        return g_cfg.xinputMenuShoulderGain;
    return 1.0f;
}

static u32 r9c_xinput_rumble_max_frames(i32 effect, u32 source, u32 requested) {
    if (!r9_xinput_class_device()) return requested;
    u32 capMs = 0;
    bool jumpLanding = r9c_is_jump_or_landing_rumble_source(source);
    bool lightAttack = effect == g_map.light &&
        (source == 6u || source == 803u || source == 829u);
    if (jumpLanding) capMs = g_cfg.xinputJumpLandingMaxDurationMs;
    else if (lightAttack) capMs = g_cfg.xinputLightAttackMaxDurationMs;
    if (!capMs) return requested;
    u32 capFrames = frames_from_ms(capMs);
    if (jumpLanding) atomic_fetch_add_u64(&g_r9cXInputJumpLandingCaps, 1);
    else atomic_fetch_add_u64(&g_r9cXInputLightAttackCaps, 1);
    if (!requested || requested > capFrames) return capFrames;
    return requested;
}

static bool r9d_is_short_xinput_action(i32 effect, u32 source) {
    if (r9c_is_jump_or_landing_rumble_source(source)) return true;
    return effect == g_map.light &&
        (source == 6u || source == 803u || source == 829u);
}

static bool queue_rumble_effect(i32 effect, float gain, u32 source,
                                u32 startFrame = 0, u32 maxFrames = 0,
                                float leftGain = 1.0f, float rightGain = 1.0f) {
    if (!g_cfg.conventionalRumble || effect < 0 || (u32)effect >= g_effectCount) return false;
    maxFrames = r9c_xinput_rumble_max_frames(effect, source, maxFrames);
    gain *= r9e_rumble_source_gain(source);
    u32 write = atomic_load_u32(&g_rumbleCommandWrite);
    u32 read = atomic_load_u32(&g_rumbleCommandRead);
    if (write - read >= COMMAND_RING_SIZE) {
        atomic_fetch_add_u64(&g_rumbleCommandsDropped, 1);
        return false;
    }
    TriggerCommand& cmd = g_rumbleCommands[write & (COMMAND_RING_SIZE - 1)];
    cmd.effect = effect;
    cmd.gain = gain;
    cmd.leftGain = leftGain;
    cmd.rightGain = rightGain;
    cmd.source = source;
    cmd.startFrame = bounded_start_frame(effect, startFrame);
    cmd.maxFrames = maxFrames;
    cmd.generation = atomic_load_u32(&g_cancelGeneration);
    atomic_store_u32(&cmd.ready, write + 1);
    atomic_store_u32(&g_rumbleCommandWrite, write + 1);
    atomic_fetch_add_u64(&g_rumbleCommandsQueued, 1);
    return true;
}

static bool queue_effect(i32 effect, float gain, u32 source,
                         u32 startFrame = 0, u32 maxFrames = 0) {
    bool haptic = g_cfg.nativeHaptics ?
        queue_haptic_effect(effect, gain, source, startFrame, maxFrames) : false;
    bool rumble = queue_rumble_effect(effect, gain, source, startFrame, maxFrames);
    return haptic || rumble;
}

static constexpr u32 DEFERRED_EFFECT_COUNT = 32;
struct DeferredEffect {
    bool active{};
    i32 effect{-1};
    float gain{1.0f};
    float rumbleGain{1.0f};
    float leftGain{1.0f};
    float rightGain{1.0f};
    u32 source{};
    u32 startFrame{};
    u32 maxFrames{};
    u64 dueTick{};
    u64 nativeSerialBaseline{};
};
static DeferredEffect g_deferredEffects[DEFERRED_EFFECT_COUNT]{};
static volatile u64 g_deferredQueued{};
static volatile u64 g_deferredDropped{};

static void clear_deferred_effects() {
    for (u32 i = 0; i < DEFERRED_EFFECT_COUNT; ++i) g_deferredEffects[i] = {};
}

static bool schedule_arbitrated_effect(i32 effect, float gain, u32 source,
                                       u64 now, u32 delayMs,
                                       u64 nativeSerialBaseline = ~0ull,
                                       u32 startFrame = 0,
                                       u32 maxFrames = 0) {
    // Semantic/state-driven effects are independent of the raw input bridge.
    // F12 disables only unresolved input fallbacks; confirmed Wwise events must
    // continue to drive v8 even with raw controller inference disabled.
    if (!atomic_load_u32(&g_enabled))
        return false;
    if (effect < 0 || (u32)effect >= g_effectCount) return false;
    u64 nativeSerial = atomic_load_u64(&g_nativeFeedbackSerial);
    if (nativeSerialBaseline == ~0ull) nativeSerialBaseline = nativeSerial;
    if (native_feedback_active(now) || nativeSerial != nativeSerialBaseline) {
        atomic_fetch_add_u64(&g_nativeSuppressedEffects, 1);
        return false;
    }
    if (!g_cfg.nativeArbitration || !delayMs) {
        return queue_effect(effect, gain, source, startFrame, maxFrames);
    }
    for (u32 i = 0; i < DEFERRED_EFFECT_COUNT; ++i) {
        if (g_deferredEffects[i].active) continue;
        g_deferredEffects[i].effect = effect;
        g_deferredEffects[i].gain = gain;
        g_deferredEffects[i].rumbleGain = gain;
        g_deferredEffects[i].source = source;
        g_deferredEffects[i].startFrame = bounded_start_frame(effect, startFrame);
        g_deferredEffects[i].maxFrames = maxFrames;
        g_deferredEffects[i].dueTick = now + delayMs;
        g_deferredEffects[i].nativeSerialBaseline = nativeSerialBaseline;
        g_deferredEffects[i].active = true;
        atomic_fetch_add_u64(&g_deferredQueued, 1);
        return true;
    }
    atomic_fetch_add_u64(&g_deferredDropped, 1);
    return false;
}

static bool schedule_split_arbitrated_effect(i32 effect, float hapticGain,
                                             float rumbleGain, u32 source,
                                             u64 now, u32 delayMs,
                                             u64 nativeSerialBaseline,
                                             u32 startFrame, u32 maxFrames,
                                             float leftGain = 1.0f,
                                             float rightGain = 1.0f) {
    if (!atomic_load_u32(&g_enabled) || effect < 0 || (u32)effect >= g_effectCount)
        return false;
    u64 nativeSerial = atomic_load_u64(&g_nativeFeedbackSerial);
    if (nativeSerialBaseline == ~0ull) nativeSerialBaseline = nativeSerial;
    if (native_feedback_active(now) || nativeSerial != nativeSerialBaseline) {
        atomic_fetch_add_u64(&g_nativeSuppressedEffects, 1);
        return false;
    }
    if (!g_cfg.nativeArbitration || !delayMs) {
        bool h = g_cfg.nativeHaptics ?
            queue_haptic_effect(effect, hapticGain, source, startFrame, maxFrames,
                                leftGain, rightGain) : false;
        bool r = queue_rumble_effect(effect, rumbleGain, source, startFrame, maxFrames,
                                     leftGain, rightGain);
        return h || r;
    }
    for (u32 i = 0; i < DEFERRED_EFFECT_COUNT; ++i) {
        if (g_deferredEffects[i].active) continue;
        g_deferredEffects[i].effect = effect;
        g_deferredEffects[i].gain = hapticGain;
        g_deferredEffects[i].rumbleGain = rumbleGain;
        g_deferredEffects[i].leftGain = leftGain;
        g_deferredEffects[i].rightGain = rightGain;
        g_deferredEffects[i].source = source;
        g_deferredEffects[i].startFrame = bounded_start_frame(effect, startFrame);
        g_deferredEffects[i].maxFrames = maxFrames;
        g_deferredEffects[i].dueTick = now + delayMs;
        g_deferredEffects[i].nativeSerialBaseline = nativeSerialBaseline;
        g_deferredEffects[i].active = true;
        atomic_fetch_add_u64(&g_deferredQueued, 1);
        return true;
    }
    atomic_fetch_add_u64(&g_deferredDropped, 1);
    return false;
}

static bool schedule_configured_effect(i32 effect, float calibratedGain,
                                       const EffectControl& control,
                                       u32 source, u64 now,
                                       u32 calibratedDelayMs,
                                       u64 nativeSerialBaseline = ~0ull,
                                       u32 calibratedStartTrimMs = 0,
                                       u32 durationMs = 0) {
    if (!control.enabled) return false;
    float gain = calibratedGain * control.intensityMultiplier;
    if (gain < 0.0f) gain = 0.0f;
    if (gain > 16.0f) gain = 16.0f;
    if (gain <= 0.0f) return false;
    u32 delayMs = adjusted_milliseconds(
        calibratedDelayMs, control.triggerOffsetMs);
    u32 startTrimMs = adjusted_milliseconds(
        calibratedStartTrimMs, control.startTrimOffsetMs);
    return schedule_arbitrated_effect(effect, gain, source, now, delayMs,
                                      nativeSerialBaseline,
                                      frames_from_ms(startTrimMs),
                                      frames_from_ms(durationMs));
}

static void process_deferred_effects(u64 now) {
    for (u32 i = 0; i < DEFERRED_EFFECT_COUNT; ++i) {
        DeferredEffect& item = g_deferredEffects[i];
        if (!item.active || now < item.dueTick) continue;
        i32 effect = item.effect;
        float gain = item.gain;
        float rumbleGain = item.rumbleGain;
        float leftGain = item.leftGain;
        float rightGain = item.rightGain;
        u32 source = item.source;
        u32 startFrame = item.startFrame;
        u32 maxFrames = item.maxFrames;
        u64 nativeSerialBaseline = item.nativeSerialBaseline;
        item = {};
        if (native_feedback_active(now) ||
            atomic_load_u64(&g_nativeFeedbackSerial) != nativeSerialBaseline) {
            atomic_fetch_add_u64(&g_nativeSuppressedEffects, 1);
            continue;
        }
        if (g_cfg.nativeHaptics)
            queue_haptic_effect(effect, gain, source, startFrame, maxFrames,
                                leftGain, rightGain);
        queue_rumble_effect(effect, rumbleGain, source, startFrame, maxFrames,
                            leftGain, rightGain);
    }
}

static constexpr u32 MAX_VOICES = 8;
struct PlaybackVoice {
    i32 effect{-1};
    u64 position{};
    u64 endPosition{};
    float gain{1.0f};
    float leftGain{1.0f};
    float rightGain{1.0f};
    u64 serial{};
    u32 source{};
};
static PlaybackVoice g_voices[MAX_VOICES]{};
static u64 g_voiceSerial{};
struct MovementPlayback {
    i32 effect{-1};
    u64 position{};
    float gain{};
    u32 mode{};
};
static MovementPlayback g_movement;
static PlaybackVoice g_rumbleVoices[MAX_VOICES]{};
static u64 g_rumbleVoiceSerial{};
static MovementPlayback g_rumbleMovement;
static u32 g_rumbleCancelSeen{};
static volatile u32 g_rumbleActiveVoices{};
static u64 g_rumbleRunPulseStartTick{};
static u64 g_rumbleRunPulseLastIndex{~0ull};
static volatile u32 g_xinputRunPulseActive{};
static volatile u64 g_xinputRunPulses{};

static void clear_playback() {
    for (u32 i = 0; i < MAX_VOICES; ++i) {
        g_voices[i].effect = -1;
        g_voices[i].position = 0;
        g_voices[i].endPosition = 0;
        g_voices[i].gain = 1.0f;
        g_voices[i].leftGain = 1.0f;
        g_voices[i].rightGain = 1.0f;
        g_voices[i].serial = 0;
        g_voices[i].source = 0;
    }
    g_movement.effect = -1;
    g_movement.position = 0;
    g_movement.gain = 0.0f;
    g_movement.mode = 0;
}

static void clear_rumble_playback() {
    for (u32 i = 0; i < MAX_VOICES; ++i) {
        g_rumbleVoices[i].effect = -1;
        g_rumbleVoices[i].position = 0;
        g_rumbleVoices[i].endPosition = 0;
        g_rumbleVoices[i].gain = 1.0f;
        g_rumbleVoices[i].leftGain = 1.0f;
        g_rumbleVoices[i].rightGain = 1.0f;
        g_rumbleVoices[i].serial = 0;
        g_rumbleVoices[i].source = 0;
    }
    g_rumbleMovement.effect = -1;
    g_rumbleMovement.position = 0;
    g_rumbleMovement.gain = 0.0f;
    g_rumbleMovement.mode = 0;
    g_rumbleRunPulseStartTick = 0;
    g_rumbleRunPulseLastIndex = ~0ull;
    atomic_store_u32(&g_xinputRunPulseActive, 0);
    atomic_store_u32(&g_rumbleActiveVoices, 0);
    g_r9dXInputForceZeroUntil = 0;
}

static void start_voice(i32 effect, float gain, u32 startFrame, u32 source,
                        u32 maxFrames, float leftGain, float rightGain) {
    if (effect < 0 || (u32)effect >= g_effectCount) return;
    u32 slot = MAX_VOICES;
    for (u32 i = 0; i < MAX_VOICES; ++i) {
        if (g_voices[i].effect == effect) { slot = i; break; }
        if (slot == MAX_VOICES && g_voices[i].effect < 0) slot = i;
    }
    if (slot == MAX_VOICES) {
        slot = 0;
        for (u32 i = 1; i < MAX_VOICES; ++i)
            if (g_voices[i].serial < g_voices[slot].serial) slot = i;
    }
    PlaybackVoice& v = g_voices[slot];
    v.effect = effect;
    v.position = bounded_start_frame(effect, startFrame);
    u64 requestedEnd = maxFrames ? v.position + (u64)maxFrames : g_effects[effect].frames;
    v.endPosition = requestedEnd < g_effects[effect].frames ? requestedEnd : g_effects[effect].frames;
    v.gain = gain;
    v.leftGain = leftGain;
    v.rightGain = rightGain;
    v.serial = ++g_voiceSerial;
    v.source = source;
}

static void start_rumble_voice(i32 effect, float gain, u32 startFrame,
                               u32 source, u32 maxFrames,
                               float leftGain, float rightGain) {
    if (effect < 0 || (u32)effect >= g_effectCount) return;
    u32 slot = MAX_VOICES;
    for (u32 i = 0; i < MAX_VOICES; ++i) {
        if (g_rumbleVoices[i].effect == effect) { slot = i; break; }
        if (slot == MAX_VOICES && g_rumbleVoices[i].effect < 0) slot = i;
    }
    if (slot == MAX_VOICES) {
        slot = 0;
        for (u32 i = 1; i < MAX_VOICES; ++i)
            if (g_rumbleVoices[i].serial < g_rumbleVoices[slot].serial) slot = i;
    }
    PlaybackVoice& v = g_rumbleVoices[slot];
    v.effect = effect;
    v.position = bounded_start_frame(effect, startFrame);
    u64 requestedEnd = maxFrames ? v.position + (u64)maxFrames : g_effects[effect].frames;
    v.endPosition = requestedEnd < g_effects[effect].frames ? requestedEnd : g_effects[effect].frames;
    v.gain = gain;
    v.leftGain = leftGain;
    v.rightGain = rightGain;
    v.serial = ++g_rumbleVoiceSerial;
    v.source = source;
}

static void drain_commands() {
    for (;;) {
        u32 read = atomic_load_u32(&g_commandRead);
        TriggerCommand& cmd = g_commands[read & (COMMAND_RING_SIZE - 1)];
        if (atomic_load_u32(&cmd.ready) != read + 1) break;
        if (cmd.generation == atomic_load_u32(&g_cancelGeneration))
            start_voice(cmd.effect, cmd.gain, cmd.startFrame, cmd.source, cmd.maxFrames,
                        cmd.leftGain, cmd.rightGain);
        atomic_store_u32(&cmd.ready, 0);
        atomic_store_u32(&g_commandRead, read + 1);
    }
}

static void drain_rumble_commands() {
    for (;;) {
        u32 read = atomic_load_u32(&g_rumbleCommandRead);
        TriggerCommand& cmd = g_rumbleCommands[read & (COMMAND_RING_SIZE - 1)];
        if (atomic_load_u32(&cmd.ready) != read + 1) break;
        if (cmd.generation == atomic_load_u32(&g_cancelGeneration))
            start_rumble_voice(cmd.effect, cmd.gain, cmd.startFrame,
                               cmd.source, cmd.maxFrames,
                               cmd.leftGain, cmd.rightGain);
        atomic_store_u32(&cmd.ready, 0);
        atomic_store_u32(&g_rumbleCommandRead, read + 1);
    }
}

static float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static float absf(float v) { return v < 0.0f ? -v : v; }

static float movement_dynamic_scale() {
    return (float)atomic_load_u32(&g_movementScalePermille) * 0.001f;
}

static constexpr u32 SOURCE_SPRINT_KICK = 13;

static bool use_xinput_run_pulses(u32 movementMode) {
    if (!g_cfg.xinputRunPulseEnabled || movementMode != 2) return false;
    // Direct XInput has no device-information object. GameInput-class Xbox
    // and DSX/ViGEm Xbox 360 devices normally expose Microsoft's 0x045E VID.
    // Native Sony GameInput devices retain the captured continuous envelope.
    return atomic_load_u32(&g_inputBackend) == 2 ||
           atomic_load_u32(&g_activeVendor) == 0x045Eu;
}

static bool r9_xinput_class_device() {
    // Covers direct XInput and GameInput's Xbox/XInput-class Microsoft path.
    // Native DualSense remains on the stereo continuous haptic path.
    return atomic_load_u32(&g_inputBackend) == 2 ||
           atomic_load_u32(&g_activeVendor) == 0x045Eu;
}

static bool r9_step_only_land_run_rumble(u32 movementMode, i32 effect) {
    return g_cfg.xinputLandRunStepOnly && movementMode == 2 &&
           effect == g_map.run &&
           atomic_load_u32(&g_r9LandSprintRumbleStepOnlyActive) &&
           atomic_load_u32(&g_v8RuntimeMode) == V8_MODE_CHARACTER &&
           r9_xinput_class_device();
}

struct RumbleSample {
    float low{};
    float high{};
    bool timelineActive{};
};

static void movement_loop_bounds(i32 effectIndex, const Effect& effect,
                                 u64& loopStart, u64& loopEnd) {
    loopStart = 0;
    loopEnd = effect.frames;
    if (effectIndex == g_map.walk || effectIndex == g_map.run) {
        u64 configuredStart = (u64)(effectIndex == g_map.walk ?
            g_cfg.walkLoopStartMs : g_cfg.runLoopStartMs) * 48ull;
        u64 configuredEnd = (u64)(effectIndex == g_map.walk ?
            g_cfg.walkLoopEndMs : g_cfg.runLoopEndMs) * 48ull;
        // Reject malformed or tiny windows and safely fall back to the full
        // capture.  The selected walk window contains four regularly spaced
        // contacts; the run window contains six.  A useful gait loop needs at
        // least 250 ms of source data.
        if (configuredStart < effect.frames && configuredEnd <= effect.frames &&
            configuredEnd > configuredStart + 12000ull) {
            loopStart = configuredStart;
            loopEnd = configuredEnd;
        }
    }
}

static void measure_rumble_window(const Effect& effect, u64 position, u32 frames,
                                  bool loop, u64 loopStart, u64 loopEnd,
                                  float gain, float& low, float& high,
                                  float leftMotorGain = 1.0f,
                                  float rightMotorGain = 1.0f) {
    if (!effect.frames || !frames || gain <= 0.0f) return;
    if (!loop || loopStart >= loopEnd || loopEnd > effect.frames) {
        loopStart = 0;
        loopEnd = effect.frames;
    }
    float sum = 0.0f;
    float peak = 0.0f;
    u32 measured = 0;
    u64 p = position;
    for (u32 i = 0; i < frames; ++i) {
        if (p >= loopEnd) {
            if (!loop) break;
            p = loopStart;
        }
        float a = absf(effect.interleaved[p * 2ull]);
        float b = absf(effect.interleaved[p * 2ull + 1ull]);
        float mean = (a + b) * 0.5f;
        sum += mean;
        float framePeak = a > b ? a : b;
        if (framePeak > peak) peak = framePeak;
        ++measured;
        ++p;
    }
    if (!measured) return;
    float candidateLow = (sum / (float)measured) * g_cfg.rumbleLowScale * gain * leftMotorGain;
    float candidateHigh = peak * g_cfg.rumbleHighScale * gain * rightMotorGain;
    if (candidateLow > low) low = candidateLow;
    if (candidateHigh > high) high = candidateHigh;
}

static RumbleSample render_rumble(u32 frames, u64 now) {
    RumbleSample result{};
    if (!atomic_load_u32(&g_enabled) || !g_cfg.conventionalRumble) {
        clear_rumble_playback();
        return result;
    }

    u32 generation = atomic_load_u32(&g_cancelGeneration);
    if (generation != g_rumbleCancelSeen) {
        clear_rumble_playback();
        g_rumbleCancelSeen = generation;
    }
    drain_rumble_commands();
    // r9d: if a short XInput action ended during the previous render block,
    // force a tiny explicit-zero gap on this block. Other timelines continue
    // advancing underneath it, avoiding a long cooldown or missed animation.
    bool r9dForceZeroThisTick = r9_xinput_class_device() &&
        g_cfg.xinputActionZeroGapMs && g_r9dXInputForceZeroUntil &&
        now < g_r9dXInputForceZeroUntil;
    if (native_feedback_active(now)) {
        clear_rumble_playback();
        atomic_fetch_add_u64(&g_nativeSuppressedRumbleTicks, 1);
        return result;
    }

    u32 desiredMode = atomic_load_u32(&g_movementMode);
    i32 desiredEffect = desiredMode == 1 && g_cfg.fxWalk.enabled &&
        g_cfg.fxWalk.intensityMultiplier > 0.0f ? g_map.walk :
        (desiredMode == 2 && g_cfg.fxRun.enabled &&
         g_cfg.fxRun.intensityMultiplier > 0.0f ? g_map.run : -1);
    // r9: on land, XInput/GameInput-class rumble gets ONLY the real gait
    // contact voices. The old continuous run loop is suppressed so the motors
    // return to zero between right/left foot contacts. Water fast-swim keeps
    // its continuous texture because the land-sprint mirror is not armed there.
    bool stepOnlyLandRun = r9_step_only_land_run_rumble(desiredMode, desiredEffect);
    if (stepOnlyLandRun) {
        atomic_fetch_add_u64(&g_r9ContinuousRunSuppressedTicks, 1);
        desiredMode = 0;
        desiredEffect = -1;
    }
    bool pulsedXInputRun = desiredEffect == g_map.run &&
        use_xinput_run_pulses(desiredMode);

    u32 activeVoices = 0;
    const float oneShotGain = effective_master_gain() * effective_one_shot_gain() * g_cfg.rumbleGain;
    for (u32 i = 0; i < MAX_VOICES; ++i) {
        PlaybackVoice& voice = g_rumbleVoices[i];
        if (voice.effect < 0 || (u32)voice.effect >= g_effectCount) continue;
        const Effect& effect = g_effects[voice.effect];
        u64 voiceEnd = voice.endPosition && voice.endPosition < effect.frames ?
            voice.endPosition : effect.frames;
        if (voice.position >= voiceEnd) { voice.effect = -1; continue; }
        // The initial parry-derived sprint kick would bridge the first two
        // pulse gaps and make Xbox running feel continuous again. Its native
        // haptic copy remains intact; only the XInput motor copy yields to the
        // new gait envelope.
        if (!(pulsedXInputRun && voice.source == SOURCE_SPRINT_KICK))
            measure_rumble_window(effect, voice.position, frames, false, 0,
                                  voiceEnd, oneShotGain * voice.gain,
                                  result.low, result.high,
                                  voice.leftGain, voice.rightGain);
        u64 remaining = voiceEnd - voice.position;
        voice.position += (u64)frames < remaining ? frames : remaining;
        if (voice.position >= voiceEnd) {
            if (r9_xinput_class_device() && g_cfg.xinputActionZeroGapMs &&
                r9d_is_short_xinput_action(voice.effect, voice.source)) {
                g_r9dXInputForceZeroUntil = now + (u64)g_cfg.xinputActionZeroGapMs;
                atomic_fetch_add_u64(&g_r9dXInputZeroBarriers, 1);
            }
            voice.effect = -1;
        } else ++activeVoices;
        result.timelineActive = true;
    }

    if (!desiredMode || desiredEffect < 0) {
        g_rumbleMovement.effect = -1;
        g_rumbleMovement.position = 0;
        g_rumbleMovement.mode = 0;
        g_rumbleRunPulseStartTick = 0;
        g_rumbleRunPulseLastIndex = ~0ull;
        atomic_store_u32(&g_xinputRunPulseActive, 0);
    } else {
        bool movementRestarted = g_rumbleMovement.mode != desiredMode ||
            g_rumbleMovement.effect != desiredEffect;
        if (movementRestarted) {
            g_rumbleMovement.effect = desiredEffect;
            g_rumbleMovement.mode = desiredMode;
            u64 start = 0, end = 0;
            movement_loop_bounds(desiredEffect, g_effects[desiredEffect], start, end);
            g_rumbleMovement.position = start;
            g_rumbleRunPulseStartTick = 0;
            g_rumbleRunPulseLastIndex = ~0ull;
        }
        const Effect& effect = g_effects[g_rumbleMovement.effect];
        u64 loopStart = 0, loopEnd = 0;
        movement_loop_bounds(g_rumbleMovement.effect, effect, loopStart, loopEnd);
        float dualSenseRumbleScale = 1.0f;
        if (atomic_load_u32(&g_activeDualSense)) {
            dualSenseRumbleScale = g_cfg.dualSenseBridgeHapticGain;
            if (atomic_load_u32(&g_v8RuntimeMode) != V8_MODE_WATER)
                dualSenseRumbleScale *= g_cfg.dualSenseLandMovementHapticGain;
        }
        float gain = effective_master_gain() * g_cfg.movementGain *
                     movement_dynamic_scale() * g_cfg.rumbleMovementGain *
                     g_cfg.rumbleGain * dualSenseRumbleScale;
        if (pulsedXInputRun) {
            if (!g_rumbleRunPulseStartTick) {
                g_rumbleRunPulseStartTick = now;
                g_rumbleRunPulseLastIndex = ~0ull;
            }
            u64 elapsed = now >= g_rumbleRunPulseStartTick ?
                now - g_rumbleRunPulseStartTick : 0;
            u64 interval = g_cfg.xinputRunPulseIntervalMs;
            u64 pulseIndex = interval ? elapsed / interval : 0;
            u64 phase = interval ? elapsed % interval : 0;
            bool pulseOn = phase < (u64)g_cfg.xinputRunPulseDurationMs;
            atomic_store_u32(&g_xinputRunPulseActive, pulseOn ? 1u : 0u);
            if (pulseOn) {
                // Use the capture's measured peak, not a random 8 ms slice,
                // so every 300 ms contact reaches the same maximum envelope.
                float peakGain = effect.maxAbs * gain *
                    g_cfg.xinputRunPulseGain;
                float candidateLow = peakGain * g_cfg.rumbleLowScale;
                float candidateHigh = peakGain * g_cfg.rumbleHighScale;
                if (candidateLow > result.low) result.low = candidateLow;
                if (candidateHigh > result.high) result.high = candidateHigh;
                if (pulseIndex != g_rumbleRunPulseLastIndex) {
                    g_rumbleRunPulseLastIndex = pulseIndex;
                    atomic_fetch_add_u64(&g_xinputRunPulses, 1);
                }
            }
        } else {
            g_rumbleRunPulseStartTick = 0;
            g_rumbleRunPulseLastIndex = ~0ull;
            atomic_store_u32(&g_xinputRunPulseActive, 0);
            measure_rumble_window(effect, g_rumbleMovement.position, frames,
                                  true, loopStart, loopEnd, gain,
                                  result.low, result.high);
        }
        if (loopEnd > loopStart) {
            u64 next = g_rumbleMovement.position + frames;
            if (next >= loopEnd) next = loopStart + (next - loopEnd) % (loopEnd - loopStart);
            g_rumbleMovement.position = next;
        }
        result.timelineActive = true;
    }

    atomic_store_u32(&g_rumbleActiveVoices,
                     activeVoices + (desiredEffect >= 0 ? 1u : 0u));
    if (r9dForceZeroThisTick) {
        result.low = 0.0f;
        result.high = 0.0f;
        result.timelineActive = true;
        atomic_fetch_add_u64(&g_r9dXInputZeroTicks, 1);
    }
    result.low = clampf(result.low, 0.0f, 1.0f);
    result.high = clampf(result.high, 0.0f, 1.0f);
    if (result.low < g_cfg.rumbleDeadzone) result.low = 0.0f;
    if (result.high < g_cfg.rumbleDeadzone) result.high = 0.0f;
    return result;
}

static volatile u64 g_primarySink{};
static volatile u64 g_primarySinkLastTick{};
static volatile u64 g_lastAnySinkTick{};
static volatile u64 g_sinkRebinds{};
static volatile u64 g_flushResumes{};
static volatile u64 g_recoverySerial{};
static volatile u32 g_recoveryCause{}; // bit0=long silence/resume, bit1=sink rebound
static volatile u64 g_recoveryDomainResets{};
static volatile u64 g_recoveryDomainPreserves{};
static volatile u64 g_flushCalls{};
static volatile u64 g_injectedBlocks{};
static volatile u64 g_injectedFrames{};
static volatile u64 g_invalidSinkFrames{};
static volatile u32 g_lastMaxFrames{};
static volatile u32 g_flushThreadId{};
static u32 g_audioCancelSeen{};
static volatile u64 g_r9kLastNativeHapticSignalTick{};

static bool process_native_haptic_buffer(float* left, float* right,
                                         u32 frames, u64 now) {
    if (!left || !right || !frames) return false;

    bool detected = false;
    if (g_cfg.nativeArbitration) {
        // Detection always observes Ubisoft's original, unmodified signal so
        // boost gains cannot make the detector self-trigger.
        float peak = 0.0f;
        float sum = 0.0f;
        for (u32 i = 0; i < frames; ++i) {
            float a = absf(left[i]);
            float b = absf(right[i]);
            sum += a + b;
            if (a > peak) peak = a;
            if (b > peak) peak = b;
        }
        float mean = sum / (float)(frames * 2u);
        atomic_store_u32(&g_lastNativePeakMillion,
                         (u32)(clampf(peak, 0.0f, 4.0f) * 1000000.0f));
        atomic_store_u32(&g_lastNativeMeanMillion,
                         (u32)(clampf(mean, 0.0f, 4.0f) * 1000000.0f));
        detected =
            (peak >= g_cfg.nativeHapticPeakThreshold &&
             mean >= g_cfg.nativeHapticMeanThreshold) ||
            peak >= g_cfg.nativeHapticPeakThreshold * 4.0f;
        if (detected) {
            atomic_extend_until(&g_nativeHapticUntil, now + g_cfg.nativeHoldMs);
            atomic_fetch_add_u64(&g_nativeHapticDetections, 1);
            atomic_fetch_add_u64(&g_nativeFeedbackSerial, 1);
            // r9k: timestamp the original Ubisoft signal before bridge mixing.
            // Used only to arbitrate the Ubisoft-only direct fallback.
            atomic_store_u64(&g_r9kLastNativeHapticSignalTick, now);
        }
    }

    bool nativeActive = detected || native_haptic_active(now);
    bool combatBoost = nativeActive && g_cfg.nativeCombatBoost &&
        g_cfg.nativeCombatHapticGain != 1.0f &&
        now < atomic_load_u64(&g_nativeCombatBoostUntil);
    bool menuBoost = nativeActive && atomic_load_u32(&g_activeDualSense) &&
        g_cfg.dualSenseMenuTickGain != 1.0f &&
        now < atomic_load_u64(&g_nativeMenuTickBoostUntil);

    if (!combatBoost && !menuBoost) return detected;

    // r9g consolidates the old combat/menu gain loops into one shared pass.
    // If windows overlap, multiplication preserves the previous sequential
    // behavior while PeakLimit remains the final native-buffer safety cap.
    float gain = 1.0f;
    if (combatBoost) gain *= g_cfg.nativeCombatHapticGain;
    if (menuBoost) gain *= g_cfg.dualSenseMenuTickGain;
    const float limit = g_cfg.peakLimit;
    bool any = false;
    for (u32 i = 0; i < frames; ++i) {
        float l = left[i];
        float r = right[i];
        if (absf(l) > 0.0000001f || absf(r) > 0.0000001f) any = true;
        l *= gain;
        r *= gain;
        if (limit > 0.0f) {
            l = clampf(l, -limit, limit);
            r = clampf(r, -limit, limit);
        }
        left[i] = l;
        right[i] = r;
    }
    if (any && combatBoost) {
        atomic_fetch_add_u64(&g_nativeCombatBoostBlocks, 1);
        atomic_fetch_add_u64(&g_nativeCombatBoostFrames, frames);
    }
    if (any && menuBoost) {
        atomic_fetch_add_u64(&g_nativeMenuTickBoostBlocks, 1);
        atomic_fetch_add_u64(&g_nativeMenuTickBoostFrames, frames);
    }
    return detected;
}

static bool mix_one_shots(float* left, float* right, u32 frames) {
    bool active = false;
    const float baseGain = effective_master_gain() * effective_one_shot_gain();
    for (u32 vindex = 0; vindex < MAX_VOICES; ++vindex) {
        PlaybackVoice& voice = g_voices[vindex];
        if (voice.effect < 0 || (u32)voice.effect >= g_effectCount) continue;
        const Effect& effect = g_effects[voice.effect];
        u64 pos = voice.position;
        u64 voiceEnd = voice.endPosition && voice.endPosition < effect.frames ?
            voice.endPosition : effect.frames;
        if (pos >= voiceEnd) { voice.effect = -1; continue; }
        active = true;
        float gain = baseGain * voice.gain *
            r9e_dualsense_haptic_source_gain(voice.source);
        u32 count = frames;
        u64 remain = voiceEnd - pos;
        if ((u64)count > remain) count = (u32)remain;
        const float* samples = effect.interleaved + pos * 2ull;
        for (u32 i = 0; i < count; ++i) {
            left[i] += samples[i * 2u] * gain * voice.leftGain;
            right[i] += samples[i * 2u + 1u] * gain * voice.rightGain;
        }
        voice.position += count;
        if (voice.position >= voiceEnd) voice.effect = -1;
    }
    return active;
}

static bool mix_movement(float* left, float* right, u32 frames) {
    u32 desiredMode = atomic_load_u32(&g_movementMode);
    i32 desiredEffect = desiredMode == 1 && g_cfg.fxWalk.enabled &&
        g_cfg.fxWalk.intensityMultiplier > 0.0f ? g_map.walk :
        (desiredMode == 2 && g_cfg.fxRun.enabled &&
         g_cfg.fxRun.intensityMultiplier > 0.0f ? g_map.run : -1);

    if (desiredMode != g_movement.mode ||
        (desiredMode && desiredEffect != g_movement.effect)) {
        if (desiredMode && desiredEffect >= 0) {
            // Movement samples are weak sustained textures. Restarting the new
            // mode at zero gain gives a clean walk<->run transition.
            g_movement.effect = desiredEffect;
            u64 loopStart = 0, loopEnd = 0;
            movement_loop_bounds(desiredEffect, g_effects[desiredEffect], loopStart, loopEnd);
            g_movement.position = loopStart;
            g_movement.gain = 0.0f;
            g_movement.mode = desiredMode;
        } else {
            // Retain the current sample only long enough to fade out.
            g_movement.mode = 0;
        }
    }

    if (g_movement.effect < 0 || (u32)g_movement.effect >= g_effectCount) return false;
    const Effect& effect = g_effects[g_movement.effect];
    if (!effect.frames) { g_movement.effect = -1; return false; }
    u64 loopStart = 0, loopEnd = 0;
    movement_loop_bounds(g_movement.effect, effect, loopStart, loopEnd);
    u64 loopFrames = loopEnd > loopStart ? loopEnd - loopStart : effect.frames;

    // This multiplier exists only in the native 48 kHz haptic mixer. It makes
    // normal DualSense walking twice as perceptible without changing run or
    // the conventional-rumble strength already working on Xbox/DS4.
    float hapticOutputScale = desiredMode == 1 ?
        effective_dualsense_walk_haptic_multiplier() : 1.0f;
    float target = desiredMode && desiredEffect == g_movement.effect ?
        g_cfg.movementGain * movement_dynamic_scale() * hapticOutputScale :
        0.0f;
    float configuredFadeMs = g_movement.effect == g_map.run ?
        g_cfg.runFadeMs : g_cfg.walkFadeMs;
    u32 fadeFrames = (u32)(48000.0f * configuredFadeMs / 1000.0f);
    if (fadeFrames > loopFrames / 4ull) fadeFrames = (u32)(loopFrames / 4ull);
    float maxScale = g_cfg.walkGain * g_cfg.fxWalk.intensityMultiplier *
        effective_dualsense_walk_haptic_multiplier();
    float runBaseScale = g_cfg.runBaseGain * g_cfg.fxRun.intensityMultiplier;
    float runOpeningScale = g_cfg.runOpeningGain * g_cfg.fxRun.intensityMultiplier;
    if (runBaseScale > maxScale) maxScale = runBaseScale;
    if (runOpeningScale > maxScale) maxScale = runOpeningScale;
    float fullStep = g_cfg.movementGain * maxScale;
    float step = fadeFrames ? (fullStep / (float)fadeFrames) : fullStep;
    if (step <= 0.0f) step = 1.0f;
    bool active = g_movement.gain > 0.0f || target > 0.0f;

    for (u32 i = 0; i < frames; ++i) {
        if (g_movement.gain < target) {
            g_movement.gain += step;
            if (g_movement.gain > target) g_movement.gain = target;
        } else if (g_movement.gain > target) {
            g_movement.gain -= step;
            if (g_movement.gain < target) g_movement.gain = target;
        }

        u64 p = g_movement.position;
        if (p < loopStart || p >= loopEnd) p = loopStart;
        float sampleL = effect.interleaved[p * 2ull];
        float sampleR = effect.interleaved[p * 2ull + 1ull];

        // Crossfade the selected loop tail into its beginning and resume after
        // the overlap. The run window excludes the silent/irregular capture
        // tail that caused feedback to disappear after roughly five seconds.
        if (fadeFrames && loopFrames > (u64)fadeFrames * 2ull &&
            p >= loopEnd - fadeFrames) {
            u64 q = loopStart + p - (loopEnd - fadeFrames);
            float t = (float)(p - (loopEnd - fadeFrames)) / (float)fadeFrames;
            sampleL = sampleL * (1.0f - t) + effect.interleaved[q * 2ull] * t;
            sampleR = sampleR * (1.0f - t) + effect.interleaved[q * 2ull + 1ull] * t;
        }

        float dualSenseScale = 1.0f;
        if (atomic_load_u32(&g_activeDualSense)) {
            dualSenseScale = g_cfg.dualSenseBridgeHapticGain;
            if (atomic_load_u32(&g_v8RuntimeMode) != V8_MODE_WATER)
                dualSenseScale *= g_cfg.dualSenseLandMovementHapticGain;
        }
        float gain = g_movement.gain * effective_master_gain() * dualSenseScale;
        left[i] += sampleL * gain;
        right[i] += sampleR * gain;
        ++p;
        if (p >= loopEnd)
            p = fadeFrames && fadeFrames < loopFrames ? loopStart + fadeFrames : loopStart;
        g_movement.position = p;
    }

    if (target <= 0.0f && g_movement.gain <= 0.0f) {
        g_movement.effect = -1;
        g_movement.position = 0;
        g_movement.mode = 0;
    }
    return active;
}

static bool inject_sink_block(void* self) {
    if (!self || !atomic_load_u32(&g_enabled) || !g_cfg.nativeHaptics) return false;
    u8* object = (u8*)self;
    float* output = *(float**)(object + 0x30);
    u16 maxFrames = *(u16*)(object + 0x40);
    u16 validFrames = *(u16*)(object + 0x42);
    if (!output || maxFrames == 0 || maxFrames > g_cfg.maxFrames || validFrames > maxFrames) {
        atomic_fetch_add_u64(&g_invalidSinkFrames, 1);
        return false;
    }
    atomic_store_u32(&g_lastMaxFrames, maxFrames);

    float* left = output;
    float* right = output + maxFrames;
    u64 now = atomic_load_u64(&g_workerNowTick);
    process_native_haptic_buffer(left, right, validFrames, now);

    u32 generation = atomic_load_u32(&g_cancelGeneration);
    if (generation != g_audioCancelSeen) {
        clear_playback();
        g_audioCancelSeen = generation;
    }
    drain_commands();

    if (native_feedback_active(now)) {
        // Discard overlapping one-shots instead of letting a delayed duplicate
        // escape when the game's native effect finishes.  Movement restarts
        // automatically from its stable gait loop after the native holdoff.
        clear_playback();
        atomic_fetch_add_u64(&g_nativeSuppressedHapticBlocks, 1);
        return false;
    }

    bool hadOneShot = mix_one_shots(left, right, maxFrames);
    bool hadMovement = mix_movement(left, right, maxFrames);
    if (!hadOneShot && !hadMovement) return false;

    if (g_cfg.peakLimit > 0.0f) {
        const float limit = g_cfg.peakLimit;
        for (u32 i = 0; i < maxFrames; ++i) {
            left[i] = clampf(left[i], -limit, limit);
            right[i] = clampf(right[i], -limit, limit);
        }
    }
    *(u16*)(object + 0x42) = maxFrames;
    atomic_fetch_add_u64(&g_injectedBlocks, 1);
    atomic_fetch_add_u64(&g_injectedFrames, maxFrames);
    return true;
}


// v8-r9l / v2.3.7.4 Ubisoft-only direct DualSense fallback retained from r9k.
// Steam/voices38 never enter this path: their HFX continues to be injected into
// Black Flag's native Wwise Quad sink exactly as in the working v2.3.7.0/r9h.
#pragma pack(push,1)
struct R9K_WAVEFORMATEX {
    WORD wFormatTag; WORD nChannels; DWORD nSamplesPerSec; DWORD nAvgBytesPerSec;
    WORD nBlockAlign; WORD wBitsPerSample; WORD cbSize;
};
struct R9K_GUID { u32 Data1; u16 Data2; u16 Data3; u8 Data4[8]; };
struct R9K_WAVEFORMATEXTENSIBLE {
    R9K_WAVEFORMATEX Format;
    union { WORD wValidBitsPerSample; WORD wSamplesPerBlock; WORD wReserved; } Samples;
    DWORD dwChannelMask; R9K_GUID SubFormat;
};
#pragma pack(pop)
static_assert(sizeof(R9K_WAVEFORMATEX)==18,"r9k wave format");
static_assert(sizeof(R9K_WAVEFORMATEXTENSIBLE)==40,"r9k extensible format");
struct R9K_WAVEHDR {
    char* lpData; DWORD dwBufferLength; DWORD dwBytesRecorded; usize dwUser;
    DWORD dwFlags; DWORD dwLoops; R9K_WAVEHDR* lpNext; usize reserved;
};
struct R9K_WAVEOUTCAPSW {
    WORD wMid,wPid; DWORD vDriverVersion; WCHAR szPname[32];
    DWORD dwFormats; WORD wChannels,wReserved1; DWORD dwSupport;
};
using R9K_HWAVEOUT=void*;
using R9K_MMRESULT=u32;
using R9K_GetNumFn=u32 (WINABI*)();
using R9K_GetCapsFn=R9K_MMRESULT (WINABI*)(usize,R9K_WAVEOUTCAPSW*,u32);
using R9K_OpenFn=R9K_MMRESULT (WINABI*)(R9K_HWAVEOUT*,usize,const R9K_WAVEFORMATEX*,usize,usize,DWORD);
using R9K_HeaderFn=R9K_MMRESULT (WINABI*)(R9K_HWAVEOUT,R9K_WAVEHDR*,u32);
using R9K_ResetFn=R9K_MMRESULT (WINABI*)(R9K_HWAVEOUT);
using R9K_CloseFn=R9K_MMRESULT (WINABI*)(R9K_HWAVEOUT);
static R9K_GetNumFn g_r9kGetNum{};
static R9K_GetCapsFn g_r9kGetCaps{};
static R9K_OpenFn g_r9kOpen{};
static R9K_HeaderFn g_r9kPrepare{},g_r9kUnprepare{},g_r9kWrite{};
static R9K_ResetFn g_r9kReset{};
static R9K_CloseFn g_r9kClose{};
static R9K_HWAVEOUT g_r9kWaveOut{};
static constexpr u32 R9K_DS_FRAMES=480; // 10 ms @ 48 kHz
static constexpr u32 R9K_DS_BUFFERS=3;
static constexpr DWORD R9K_WHDR_DONE=1u;
static constexpr DWORD R9K_DS_MASK=0x33u; // FL|FR|BL|BR, haptic actuators on lanes 3/4
static constexpr WORD R9K_WAVE_FORMAT_EXTENSIBLE=0xFFFEu;
struct R9K_DsBuf { R9K_WAVEHDR hdr{}; i16 pcm[R9K_DS_FRAMES*4]{}; u32 prepared{}; };
static R9K_DsBuf g_r9kDsBuf[R9K_DS_BUFFERS]{};
static volatile u32 g_r9kDirectActive{};
static volatile u32 g_r9kDirectDeviceId{};
static volatile u32 g_r9kDirectPeakPermille{};
static volatile u64 g_r9kDirectOpenAttempts{};
static volatile u64 g_r9kDirectOpenFailures{};
static volatile u64 g_r9kDirectWriteFailures{};
static volatile u64 g_r9kDirectBlocks{};
static volatile u64 g_r9kDirectNonZeroBlocks{};
static volatile u64 g_r9kDirectFrames{};
static volatile u64 g_r9kNativeObservedWhileDirect{};
static u32 g_r9kAudioCancelSeen{};
static u64 g_r9kRetryTick{};
static u64 g_r9kWorkerStartTick{};

static bool r9k_name_contains(const WCHAR* s,const char* needle) {
    if (!s||!needle||!*needle) return false;
    char n[40]{}; u32 nn=0;
    while (needle[nn] && nn+1<(u32)sizeof(n)) { n[nn]=lower_ascii(needle[nn]); ++nn; }
    for (u32 i=0;s[i]&&i<64u;++i) {
        u32 j=0;
        while (j<nn && s[i+j] && lower_ascii((char)(s[i+j]&0x7F))==n[j]) ++j;
        if (j==nn) return true;
    }
    return false;
}
static bool r9k_init_wave() {
    if (g_r9kGetNum&&g_r9kOpen&&g_r9kWrite) return true;
    u8* module=find_module_ascii("winmm.dll");
    if (!module&&g_api.LoadLibraryW) module=(u8*)g_api.LoadLibraryW(L"winmm.dll");
    if (!module) return false;
    g_r9kGetNum=api_cast<R9K_GetNumFn>(resolve_export(module,"waveOutGetNumDevs"));
    g_r9kGetCaps=api_cast<R9K_GetCapsFn>(resolve_export(module,"waveOutGetDevCapsW"));
    g_r9kOpen=api_cast<R9K_OpenFn>(resolve_export(module,"waveOutOpen"));
    g_r9kPrepare=api_cast<R9K_HeaderFn>(resolve_export(module,"waveOutPrepareHeader"));
    g_r9kUnprepare=api_cast<R9K_HeaderFn>(resolve_export(module,"waveOutUnprepareHeader"));
    g_r9kWrite=api_cast<R9K_HeaderFn>(resolve_export(module,"waveOutWrite"));
    g_r9kReset=api_cast<R9K_ResetFn>(resolve_export(module,"waveOutReset"));
    g_r9kClose=api_cast<R9K_CloseFn>(resolve_export(module,"waveOutClose"));
    return g_r9kGetNum&&g_r9kGetCaps&&g_r9kOpen&&g_r9kPrepare&&g_r9kUnprepare&&
           g_r9kWrite&&g_r9kReset&&g_r9kClose;
}
static i16 r9k_s16(float v) {
    v=clampf(v,-1.0f,1.0f); i32 q=(i32)(v*32767.0f);
    if(q<-32767) q=-32767;
    if(q>32767) q=32767;
    return (i16)q;
}
static float r9k_abs(float v) { return v<0.0f?-v:v; }
static bool r9k_haptic_work_pending() {
    return atomic_load_u32(&g_commandWrite)!=atomic_load_u32(&g_commandRead) ||
           atomic_load_u32(&g_movementMode)!=0u;
}
static bool r9k_native_haptic_recent(u64 now) {
    if (native_haptic_active(now)) return true;
    u64 last=atomic_load_u64(&g_r9kLastNativeHapticSignalTick);
    if(!last||now<last) return false;
    return now-last<(u64)g_cfg.ubisoftDirectDualSenseFallbackDelayMs;
}
static void r9k_render(R9K_DsBuf& buffer,u64 now) {
    float left[R9K_DS_FRAMES]{},right[R9K_DS_FRAMES]{};
    u32 generation=atomic_load_u32(&g_cancelGeneration);
    if(generation!=g_r9kAudioCancelSeen) { clear_playback(); g_r9kAudioCancelSeen=generation; }
    drain_commands();
    // Direct waveform output arbitrates only against native haptic signal.
    // Conventional motor rumble keeps the exact r9h behavior/output logic.
    if(atomic_load_u32(&g_enabled)&&g_cfg.nativeHaptics&&!native_haptic_active(now)) {
        mix_one_shots(left,right,R9K_DS_FRAMES);
        mix_movement(left,right,R9K_DS_FRAMES);
    } else if(native_haptic_active(now)) {
        clear_playback();
        atomic_fetch_add_u64(&g_nativeSuppressedHapticBlocks,1);
    }
    float peak=0.0f; const float limit=g_cfg.peakLimit;
    for(u32 i=0;i<R9K_DS_FRAMES;++i) {
        if(limit>0.0f) { left[i]=clampf(left[i],-limit,limit); right[i]=clampf(right[i],-limit,limit); }
        float a=r9k_abs(left[i]),b=r9k_abs(right[i]); if(b>a)a=b; if(a>peak)peak=a;
        buffer.pcm[i*4u+0u]=0; buffer.pcm[i*4u+1u]=0;
        buffer.pcm[i*4u+2u]=r9k_s16(left[i]); buffer.pcm[i*4u+3u]=r9k_s16(right[i]);
    }
    if(peak>0.0001f) {
        atomic_fetch_add_u64(&g_r9kDirectNonZeroBlocks,1);
        u32 q=(u32)(clampf(peak,0.0f,100.0f)*1000.0f);
        if(q>atomic_load_u32(&g_r9kDirectPeakPermille)) atomic_store_u32(&g_r9kDirectPeakPermille,q);
    }
    buffer.hdr.lpData=(char*)buffer.pcm; buffer.hdr.dwBufferLength=(DWORD)sizeof(buffer.pcm);
}
static void r9k_direct_shutdown() {
    if(!g_r9kWaveOut) { atomic_store_u32(&g_r9kDirectActive,0); return; }
    if(g_r9kReset) g_r9kReset(g_r9kWaveOut);
    for(u32 i=0;i<R9K_DS_BUFFERS;++i) {
        if(g_r9kDsBuf[i].prepared&&g_r9kUnprepare)
            g_r9kUnprepare(g_r9kWaveOut,&g_r9kDsBuf[i].hdr,(u32)sizeof(R9K_WAVEHDR));
        g_r9kDsBuf[i].prepared=0;
    }
    if(g_r9kClose) g_r9kClose(g_r9kWaveOut);
    g_r9kWaveOut=nullptr; atomic_store_u32(&g_r9kDirectActive,0);
}
static bool r9k_direct_open(u64 now) {
    atomic_fetch_add_u64(&g_r9kDirectOpenAttempts,1);
    if(!r9k_init_wave()) { atomic_fetch_add_u64(&g_r9kDirectOpenFailures,1); return false; }
    R9K_WAVEFORMATEXTENSIBLE fmt{};
    fmt.Format.wFormatTag=R9K_WAVE_FORMAT_EXTENSIBLE; fmt.Format.nChannels=4;
    fmt.Format.nSamplesPerSec=48000; fmt.Format.nBlockAlign=8; fmt.Format.nAvgBytesPerSec=384000;
    fmt.Format.wBitsPerSample=16; fmt.Format.cbSize=22; fmt.Samples.wValidBitsPerSample=16;
    fmt.dwChannelMask=R9K_DS_MASK; fmt.SubFormat.Data1=1; fmt.SubFormat.Data2=0; fmt.SubFormat.Data3=0x0010;
    const u8 tail[8]={0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}; memcpy(fmt.SubFormat.Data4,tail,8);
    u32 count=g_r9kGetNum();
    // Explicit DualSense names first; generic Wireless Controller only second.
    for(u32 pass=0;pass<2u;++pass) for(u32 dev=0;dev<count;++dev) {
        R9K_WAVEOUTCAPSW caps{}; if(g_r9kGetCaps((usize)dev,&caps,(u32)sizeof(caps))!=0) continue;
        bool exact=r9k_name_contains(caps.szPname,"dualsense");
        bool generic=r9k_name_contains(caps.szPname,"wireless controller");
        if(pass==0u?!exact:(exact||!generic)) continue;
        R9K_HWAVEOUT out{}; if(g_r9kOpen(&out,(usize)dev,&fmt.Format,0,0,0)!=0||!out) continue;
        g_r9kWaveOut=out; atomic_store_u32(&g_r9kDirectDeviceId,dev+1u);
        atomic_store_u32(&g_r9kDirectActive,1u); // before first render prevents native double-consume
        bool ok=true;
        for(u32 i=0;i<R9K_DS_BUFFERS;++i) {
            g_r9kDsBuf[i]={}; r9k_render(g_r9kDsBuf[i],now);
            if(g_r9kPrepare(out,&g_r9kDsBuf[i].hdr,(u32)sizeof(R9K_WAVEHDR))!=0) { ok=false; break; }
            g_r9kDsBuf[i].prepared=1;
            if(g_r9kWrite(out,&g_r9kDsBuf[i].hdr,(u32)sizeof(R9K_WAVEHDR))!=0) { ok=false; break; }
            atomic_fetch_add_u64(&g_r9kDirectBlocks,1); atomic_fetch_add_u64(&g_r9kDirectFrames,R9K_DS_FRAMES);
        }
        if(!ok) { atomic_fetch_add_u64(&g_r9kDirectWriteFailures,1); r9k_direct_shutdown(); continue; }
        TextBuf log{}; SYSTEMTIME st{}; if(g_api.GetLocalTime)g_api.GetLocalTime(&st);
        tb_char(log,'[');tb_time(log,st);tb_str(log,"] R9K Ubisoft direct DualSense haptics ACTIVE device=");tb_u64(log,dev);
        tb_str(log," name="); for(u32 k=0;k<32u&&caps.szPname[k];++k) { WCHAR w=caps.szPname[k]; tb_char(log,(w>=32&&w<127)?(char)w:'?'); }
        tb_str(log," 48000Hz/4ch/16bit actuatorChannels=3/4");tb_endline(log);append_log(log.bytes,log.len);
        return true;
    }
    atomic_fetch_add_u64(&g_r9kDirectOpenFailures,1); return false;
}
static void r9k_service_ubisoft_direct_haptics(u64 now) {
    if(!r9k_ubisoft_profile() || !g_cfg.ubisoftDirectDualSenseFallback || !g_cfg.nativeHaptics ||
       !atomic_load_u32(&g_enabled) || !atomic_load_u32(&g_controllerConnected) ||
       !r9k_physical_dualsense_present()) { r9k_direct_shutdown(); return; }
    if(r9k_native_haptic_recent(now)) {
        // Preserve r9h native-first behavior: discard bridge one-shots that would
        // otherwise emerge after a genuine Ubisoft native haptic finishes.
        clear_playback();
        r9k_direct_shutdown();
        return;
    }
    if(!g_r9kWorkerStartTick || now<g_r9kWorkerStartTick+(u64)g_cfg.ubisoftDirectDualSenseFallbackDelayMs) return;
    if(!g_r9kWaveOut) {
        if(!r9k_haptic_work_pending()) return;
        if(now<g_r9kRetryTick) return;
        g_r9kRetryTick=now+2000ull;
        if(!r9k_direct_open(now)) {
            if(atomic_load_u64(&g_r9kDirectOpenFailures)==1u)
                log_simple("WARNING r9k Ubisoft fallback could not open a 48kHz 4-channel DualSense endpoint; native r9h path remains active and fallback will retry");
            return;
        }
    }
    for(u32 i=0;i<R9K_DS_BUFFERS;++i) {
        R9K_DsBuf& buffer=g_r9kDsBuf[i];
        if(!buffer.prepared || !(buffer.hdr.dwFlags&R9K_WHDR_DONE)) continue;
        if(g_r9kUnprepare(g_r9kWaveOut,&buffer.hdr,(u32)sizeof(R9K_WAVEHDR))!=0) continue;
        buffer.prepared=0; buffer.hdr={}; r9k_render(buffer,now);
        if(g_r9kPrepare(g_r9kWaveOut,&buffer.hdr,(u32)sizeof(R9K_WAVEHDR))!=0) {
            atomic_fetch_add_u64(&g_r9kDirectWriteFailures,1); r9k_direct_shutdown(); return;
        }
        buffer.prepared=1;
        if(g_r9kWrite(g_r9kWaveOut,&buffer.hdr,(u32)sizeof(R9K_WAVEHDR))!=0) {
            atomic_fetch_add_u64(&g_r9kDirectWriteFailures,1); r9k_direct_shutdown(); return;
        }
        atomic_fetch_add_u64(&g_r9kDirectBlocks,1); atomic_fetch_add_u64(&g_r9kDirectFrames,R9K_DS_FRAMES);
    }
}
static bool r9k_observe_native_sink_only(void* self) {
    if(!self||!g_cfg.nativeHaptics) return false;
    u8* object=(u8*)self; float* output=*(float**)(object+0x30);
    u16 maxFrames=*(u16*)(object+0x40); u16 validFrames=*(u16*)(object+0x42);
    if(!output||maxFrames==0||maxFrames>g_cfg.maxFrames||validFrames>maxFrames) {
        atomic_fetch_add_u64(&g_invalidSinkFrames,1); return false;
    }
    atomic_store_u32(&g_lastMaxFrames,maxFrames);
    float* left=output; float* right=output+maxFrames;
    u64 now=atomic_load_u64(&g_workerNowTick);
    atomic_fetch_add_u64(&g_r9kNativeObservedWhileDirect,1);
    return process_native_haptic_buffer(left,right,validFrames,now);
}

using FlushFn = void (FASTABI*)(void*);
static FlushFn g_originalFlush{};
static void* g_flushTrampoline{};
static volatile u32 g_hookInstalled{};

static void FASTABI flush_hook(void* self) {
    u64 perfStart = perf_counter_now();
    atomic_fetch_add_u64(&g_flushCalls, 1);
    if (!atomic_load_u32(&g_flushThreadId) && g_api.GetCurrentThreadId)
        atomic_store_u32(&g_flushThreadId, g_api.GetCurrentThreadId());

    u64 now = g_api.GetTickCount64 ? g_api.GetTickCount64() :
              atomic_load_u64(&g_workerNowTick);
    u64 previousAny = atomic_load_u64(&g_lastAnySinkTick);
    bool resumed = previousAny && now >= previousAny &&
        now - previousAny >= (u64)g_cfg.reloadSilenceResetMs;
    atomic_store_u64(&g_lastAnySinkTick, now);

    u64 sink = (u64)self;
    u64 primary = atomic_load_u64(&g_primarySink);
    if (!primary && sink) {
        atomic_cas_u64(&g_primarySink, 0, sink);
        primary = atomic_load_u64(&g_primarySink);
        if (primary == sink) atomic_store_u64(&g_primarySinkLastTick, now);
    } else if (sink && sink != primary) {
        u64 lastPrimary = atomic_load_u64(&g_primarySinkLastTick);
        if ((!lastPrimary || (now >= lastPrimary &&
             now - lastPrimary >= (u64)g_cfg.sinkRebindAfterMs)) &&
            atomic_cas_u64(&g_primarySink, primary, sink)) {
            primary = sink;
            atomic_store_u64(&g_primarySinkLastTick, now);
            atomic_fetch_add_u64(&g_sinkRebinds, 1);
        } else {
            primary = atomic_load_u64(&g_primarySink);
        }
    }
    if (sink && sink == primary) {
        atomic_store_u64(&g_primarySinkLastTick, now);
    }
    if (resumed) {
        atomic_fetch_add_u64(&g_flushResumes, 1);
        // r9g: only a genuine long silence/resume performs cancellation and
        // worker recovery. Menu/category transitions frequently replace the
        // QuadAudio sink object; a pointer rebound is now just an ownership
        // handoff to the new valid sink and does not clear playback, reset
        // controller edge history, or synchronously open/write the log file.
        atomic_fetch_or_u32(&g_recoveryCause, 1u);
        atomic_fetch_add_u32(&g_cancelGeneration, 1);
        atomic_fetch_add_u64(&g_recoverySerial, 1);
    }
    if (sink && sink == primary) {
        // Exact r9h injection remains authoritative unless the Ubisoft-only
        // direct fallback is currently rendering the same bridge HFX.
        if (r9k_ubisoft_profile() && atomic_load_u32(&g_r9kDirectActive))
            r9k_observe_native_sink_only(self);
        else
            inject_sink_block(self);
    }
    u64 perfEnd = perf_counter_now();
    perf_record_audio_hook(perf_elapsed_us(perfStart, perfEnd));
    if (g_originalFlush) g_originalFlush(self);
}

static bool bytes_equal(const u8* a, const u8* b, u32 count) {
    for (u32 i = 0; i < count; ++i) if (a[i] != b[i]) return false;
    return true;
}

static constexpr u64 QUAD_FLUSH_RVA_106 = 0x04F78610ull;
static constexpr u64 QUAD_FLUSH_RVA_107 = 0x05028310ull;
static const u8 kQuadFlushSignature[16] = {
    0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57,
    0x48,0x83,0xEC,0x20,0x0F,0xB7,0x79,0x42
};

static bool compat_rva_valid(u64 rva, u64 size) {
    return g_gameBase && rva < (u64)g_gameSize &&
           size <= (u64)g_gameSize - rva;
}

static u64 find_unique_exact_rva(const u8* signature, u32 length) {
    if (!g_gameBase || !signature || !length || (u64)g_gameSize < length)
        return 0;
    u64 found = 0;
    u32 matches = 0;
    for (u32 rva = 0; rva <= g_gameSize - length; ++rva) {
        const u8* at = g_gameBase + rva;
        if (at[0] != signature[0] || !bytes_equal(at, signature, length))
            continue;
        found = rva;
        if (++matches > 1) return 0;
    }
    return matches == 1 ? found : 0;
}

// The six-byte PostEvent prologue is common in this executable. These
// additional stable bytes identify the complete function entry while ignoring
// the three build-relative displacements.
static const u8 kV8PostPrefix[21] = {
    0x41,0x57,0x41,0x56,0x41,0x55,0x41,0x54,0x56,0x57,0x55,
    0x53,0x48,0x83,0xEC,0x58,0x45,0x31,0xF6,0x80,0x3D
};
static const u8 kV8PostMiddle[21] = {
    0x89,0xCD,0x44,0x8B,0x94,0x24,0xD0,0x00,0x00,0x00,
    0x4C,0x8B,0x9C,0x24,0xC0,0x00,0x00,0x00,0x4C,0x8B,0x2D
};
static const u8 kV8PostSuffix[7] = {
    0x49,0x8D,0x85,0x10,0x01,0x00,0x00
};

static bool v8_post_anchor_matches(const u8* at) {
    return at && bytes_equal(at, kV8PostPrefix, 21) &&
           at[25] == 0x00 && at[26] == 0x0F && at[27] == 0x84 &&
           bytes_equal(at + 32, kV8PostMiddle, 21) &&
           bytes_equal(at + 57, kV8PostSuffix, 7);
}

static u64 find_unique_v8_post_event_rva() {
    if (!g_gameBase || (u64)g_gameSize < 64ull) return 0;
    u64 found = 0;
    u32 matches = 0;
    for (u32 rva = 0; rva <= g_gameSize - 64u; ++rva) {
        const u8* at = g_gameBase + rva;
        if (at[0] != kV8PostPrefix[0] || !v8_post_anchor_matches(at))
            continue;
        found = rva;
        if (++matches > 1) return 0;
    }
    return matches == 1 ? found : 0;
}

static bool compat_is_107_layout() {
    return compat_rva_valid(QUAD_FLUSH_RVA_107, sizeof(kQuadFlushSignature)) &&
           bytes_equal(g_gameBase + QUAD_FLUSH_RVA_107,
                       kQuadFlushSignature, sizeof(kQuadFlushSignature));
}

static bool install_flush_hook() {
    u64 targetRva = g_cfg.flushRva;
    bool configuredMatch = compat_rva_valid(targetRva, sizeof(kQuadFlushSignature)) &&
                           bytes_equal(g_gameBase + targetRva,
                                       kQuadFlushSignature,
                                       sizeof(kQuadFlushSignature));
    if (!configuredMatch)
        targetRva = find_unique_exact_rva(kQuadFlushSignature,
                                          sizeof(kQuadFlushSignature));
    if (!targetRva) {
        log_simple("ERROR QuadAudioHaptics flush signature was not unique; no code was patched");
        return false;
    }
    if (!compat_rva_valid(targetRva, sizeof(kQuadFlushSignature))) {
        log_simple("ERROR discovered flush RVA is outside ACBlackFlag.exe");
        return false;
    }
    g_cfg.flushRva = targetRva;
    u8* target = g_gameBase + targetRva;

    u8* trampoline = (u8*)g_api.VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE,
                                              PAGE_EXECUTE_READWRITE);
    if (!trampoline) {
        log_simple("ERROR could not allocate flush trampoline");
        return false;
    }
    for (u32 i = 0; i < 16; ++i) trampoline[i] = target[i];
    u32 p = 16;
    trampoline[p++] = 0xFF; trampoline[p++] = 0x25;
    trampoline[p++] = 0; trampoline[p++] = 0; trampoline[p++] = 0; trampoline[p++] = 0;
    *(u64*)(trampoline + p) = (u64)(target + 16); p += 8;

    DWORD oldProtect = 0;
    if (!g_api.VirtualProtect(target, 16, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_simple("ERROR VirtualProtect failed for flush hook");
        return false;
    }
    u8 patch[16]{};
    u32 k = 0;
    patch[k++] = 0xFF; patch[k++] = 0x25;
    patch[k++] = 0; patch[k++] = 0; patch[k++] = 0; patch[k++] = 0;
    *(u64*)(patch + k) = (u64)&flush_hook; k += 8;
    while (k < 16) patch[k++] = 0x90;
    for (u32 i = 0; i < 16; ++i) target[i] = patch[i];
    g_api.FlushInstructionCache(invalid_handle_value(), target, 16);
    DWORD ignored = 0;
    g_api.VirtualProtect(target, 16, oldProtect, &ignored);

    g_flushTrampoline = trampoline;
    g_originalFlush = (FlushFn)trampoline;
    atomic_store_u32(&g_hookInstalled, 1);
    TextBuf b{}; SYSTEMTIME st{};
    if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    tb_char(b, '['); tb_time(b, st);
    tb_str(b, "] Installed Black Flag native Quad Audio Haptics flush injector at RVA 0x");
    tb_hex(b, targetRva); tb_endline(b); append_log(b.bytes, b.len);
    return true;
}


// -----------------------------------------------------------------------------
// Exact Wwise PostEvent producer hook used by the v8 production router.
// Only a compact event record is written on the producer thread; all semantic
// decisions and haptic scheduling are performed by the worker thread.
// -----------------------------------------------------------------------------
struct V8RelHook { u8* target{}; u8* tramp{}; u32 len{}; };
static u8* g_v8RelayPage{};
static u32 g_v8RelayUsed{};
static V8RelHook g_v8PostHook{};
using V8PostEventFn = u32 (FASTABI*)(u32,u64,u32,void*,u64,u64,u32,u64);
static V8PostEventFn g_v8PostEventOriginal{};

static u8* v8_make_near_relay(u8* target, void* hook) {
    auto in_range = [&](u8* q)->bool {
        i64 d = (i64)(u64)q - (i64)((u64)target + 5ull);
        return d >= -2147483648ll && d <= 2147483647ll;
    };
    if (!g_v8RelayPage) {
        u64 base = ((u64)g_gameBase + (u64)g_gameSize + 0xFFFFull) & ~0xFFFFull;
        for (u32 i=1; i<8192 && !g_v8RelayPage; ++i) {
            u64 cand = base + (u64)i * 0x10000ull;
            u8* q = (u8*)g_api.VirtualAlloc((void*)cand, 4096,
                                            MEM_COMMIT|MEM_RESERVE,
                                            PAGE_EXECUTE_READWRITE);
            if (q && in_range(q)) g_v8RelayPage = q;
        }
        if (!g_v8RelayPage) {
            u64 gb = (u64)g_gameBase & ~0xFFFFull;
            for (u32 i=1; i<8192 && !g_v8RelayPage; ++i) {
                if (gb < (u64)i * 0x10000ull) break;
                u64 cand = gb - (u64)i * 0x10000ull;
                u8* q = (u8*)g_api.VirtualAlloc((void*)cand, 4096,
                                                MEM_COMMIT|MEM_RESERVE,
                                                PAGE_EXECUTE_READWRITE);
                if (q && in_range(q)) g_v8RelayPage = q;
            }
        }
    }
    if (!g_v8RelayPage || !in_range(g_v8RelayPage) || g_v8RelayUsed + 16 > 4096)
        return nullptr;
    u8* r = g_v8RelayPage + g_v8RelayUsed;
    g_v8RelayUsed += 16;
    r[0]=0xFF; r[1]=0x25; r[2]=0; r[3]=0; r[4]=0; r[5]=0;
    *(u64*)(r+6)=(u64)hook; r[14]=0x90; r[15]=0x90;
    if (g_api.FlushInstructionCache)
        g_api.FlushInstructionCache(invalid_handle_value(), r, 16);
    return r;
}

static bool v8_install_rel32(V8RelHook& h, u8* target, void* hook,
                             const u8* signature, u32 len) {
    if (!target || !hook || !signature || len < 5 || len > 24 ||
        !g_api.VirtualAlloc || !g_api.VirtualProtect) return false;
    if (!bytes_equal(target, signature, len)) return false;
    u8* jumpTarget = (u8*)hook;
    i64 rel = (i64)(u64)jumpTarget - (i64)((u64)target + 5ull);
    if (rel < -2147483648ll || rel > 2147483647ll) {
        jumpTarget = v8_make_near_relay(target, hook);
        if (!jumpTarget) return false;
        rel = (i64)(u64)jumpTarget - (i64)((u64)target + 5ull);
    }
    u8* tramp = (u8*)g_api.VirtualAlloc(nullptr, 64, MEM_COMMIT|MEM_RESERVE,
                                         PAGE_EXECUTE_READWRITE);
    if (!tramp) return false;
    memcpy(tramp, target, len);
    u32 k=len;
    tramp[k++]=0xFF; tramp[k++]=0x25;
    tramp[k++]=0; tramp[k++]=0; tramp[k++]=0; tramp[k++]=0;
    *(u64*)(tramp+k)=(u64)(target+len); k+=8;
    DWORD old=0;
    if (!g_api.VirtualProtect(target, len, PAGE_EXECUTE_READWRITE, &old)) return false;
    target[0]=0xE9; *(i32*)(target+1)=(i32)rel;
    for (u32 i=5;i<len;++i) target[i]=0x90;
    if (g_api.FlushInstructionCache)
        g_api.FlushInstructionCache(invalid_handle_value(), target, len);
    DWORD ignored=0; g_api.VirtualProtect(target, len, old, &ignored);
    h.target=target; h.tramp=tramp; h.len=len;
    return true;
}

static void v8_publish_semantic(u32 eventId, u64 gameObject, u32 playingId, u64 nativeSerialBaseline) {
    u64 seq = atomic_fetch_add_u64(&g_v8SemWrite, 1) + 1;
    V8SemanticEvent& slot = g_v8SemRing[seq & (V8_SEM_RING - 1)];
    u64 old = atomic_load_u64(&slot.commit);
    if (old && seq > old + V8_SEM_RING) atomic_fetch_add_u64(&g_v8SemDrops, 1);
    slot.tick = g_api.GetTickCount64 ? g_api.GetTickCount64() :
                atomic_load_u64(&g_workerNowTick);
    slot.gameObject = gameObject;
    slot.nativeSerialBaseline = nativeSerialBaseline;
    slot.id = eventId;
    slot.playingId = playingId;
    atomic_store_u64(&slot.commit, seq);
}

static u32 FASTABI v8_post_event_hook(u32 eventId, u64 gameObject, u32 flags,
                                      void* callback, u64 arg5, u64 arg6,
                                      u32 arg7, u64 arg8) {
    // r9f: the verified native menu tick is the primary zero-lag UI haptic on
    // DualSense. Arm its +30% window before Ubisoft posts the event so a
    // synchronous first Quad block is included. Clear any recent combat boost
    // window to prevent a fast gameplay->menu transition from stacking gains.
    if (eventId == EVT_MENU_NATIVE_TICK && atomic_load_u32(&g_activeDualSense) &&
        g_cfg.dualSenseMenuTickGain != 1.0f) {
        u64 menuNow = g_api.GetTickCount64 ? g_api.GetTickCount64() :
                      atomic_load_u64(&g_workerNowTick);
        atomic_store_u64(&g_nativeCombatBoostUntil, 0);
        atomic_extend_until(&g_nativeMenuTickBoostUntil,
                            menuNow + g_cfg.dualSenseMenuTickNativeWindowMs);
        atomic_fetch_add_u64(&g_nativeMenuTickBoostArms, 1);
    }
    // r9b arms the native-combat supplement before calling Ubisoft so even a
    // synchronous/very-early Quad block is inside the action window. MENU/HELM
    // are excluded; the mapped IDs are the verified Edward combat family.
    if (g_cfg.semanticEvents && g_cfg.nativeCombatBoost &&
        v8_is_native_combat_boost_id(eventId)) {
        u32 mode = atomic_load_u32(&g_v8RuntimeMode);
        u64 knownCharacter = atomic_load_u64(&g_v8CharacterObject);
        bool objectMatches = !knownCharacter || !gameObject ||
                             gameObject == knownCharacter;
        if (mode != V8_MODE_MENU && mode != V8_MODE_HELM && objectMatches) {
            u64 armNow = g_api.GetTickCount64 ? g_api.GetTickCount64() :
                         atomic_load_u64(&g_workerNowTick);
            atomic_extend_until(&g_nativeCombatBoostUntil,
                                armNow + g_cfg.nativeCombatBoostWindowMs);
            atomic_store_u32(&g_nativeCombatBoostLastEvent, eventId);
            atomic_fetch_add_u64(&g_nativeCombatBoostArms, 1);
        }
    }
    // Snapshot before the real PostEvent call. If Ubisoft Motion produces Quad
    // synchronously or immediately on another Wwise thread, the serial change
    // is still visible to the deferred fallback arbitration.
    u64 nativeBaseline = atomic_load_u64(&g_nativeFeedbackSerial);
    u32 playing = g_v8PostEventOriginal ?
        g_v8PostEventOriginal(eventId, gameObject, flags, callback,
                              arg5, arg6, arg7, arg8) : 0;
    if (g_cfg.semanticEvents && v8_is_semantic_id(eventId))
        v8_publish_semantic(eventId, gameObject, playing, nativeBaseline);
    return playing;
}

static bool install_v8_post_event_hook() {
    if (!g_cfg.semanticEvents) {
        log_simple("v8 semantic Wwise event router disabled by INI");
        return true;
    }
    if (!g_gameBase || !g_gameSize) return false;
    static const u8 signature[6] = {0x41,0x57,0x41,0x56,0x41,0x55};
    u64 targetRva = g_v8PostEventRva;
    bool configuredMatch = compat_rva_valid(targetRva, 64) &&
                           v8_post_anchor_matches(g_gameBase + targetRva);
    if (!configuredMatch)
        targetRva = find_unique_v8_post_event_rva();
    if (!targetRva || !compat_rva_valid(targetRva, 64)) {
        log_simple("ERROR v8 PostEvent anchor was not unique; semantic router disabled");
        return false;
    }
    g_v8PostEventRva = targetRva;
    u8* target = g_gameBase + targetRva;
    if (!v8_install_rel32(g_v8PostHook, target, (void*)&v8_post_event_hook,
                          signature, 6)) {
        log_simple("ERROR v8 exact PostEvent signature/range mismatch; semantic router disabled");
        return false;
    }
    g_v8PostEventOriginal = (V8PostEventFn)g_v8PostHook.tramp;
    atomic_store_u32(&g_v8PostHookInstalled, 1);
    TextBuf b{}; SYSTEMTIME st{};
    if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    tb_char(b, '['); tb_time(b, st);
    tb_str(b, "] Installed v8 exact Wwise PostEvent router at RVA 0x");
    tb_hex(b, targetRva); tb_endline(b); append_log(b.bytes, b.len);
    return true;
}

// Black Flag Resynced keeps its human locomotion states in Anvil state-machine
// vtables.  The exact target executable names the relevant entry states
// (InAir, FreeJump, TargetedJump, Climb, Swimming, and related transitions),
// but does not export a public gameplay-state API.  Patch the verified vtable
// slots instead of detouring code prologues: this preserves every original
// instruction and fails closed when a slot does not contain the expected RVA.
enum GameplayStateEvent : u32 {
    GSE_AIR_ENTER,
    GSE_AIR_EXIT,
    GSE_JUMP,
    GSE_FREEFALL,
    GSE_LEDGE_FALL_ENTER,
    GSE_LEDGE_FALL_EXIT,
    GSE_TRANSITION_EXIT,
    GSE_CLIMB_ENTER,
    GSE_CLIMB_EXIT,
    GSE_SWIM_ENTER,
    GSE_SWIM_EXIT,
    GSE_SHALLOW_ENTER,
    GSE_SHALLOW_EXIT,
    GSE_HAYSTACK_ENTER,
    GSE_HAYSTACK_EXIT,
    GSE_COUNT
};

static constexpr u32 CS_AIR      = 1u << 0;
static constexpr u32 CS_CLIMB_A  = 1u << 1;
static constexpr u32 CS_CLIMB_B  = 1u << 2;
static constexpr u32 CS_SWIM_A   = 1u << 3;
static constexpr u32 CS_SWIM_B   = 1u << 4;
static constexpr u32 CS_SHALLOW  = 1u << 5;
static constexpr u32 CS_LEDGE_FALL = 1u << 6;
static constexpr u32 CS_HAYSTACK = 1u << 7;
static constexpr u32 CS_CLIMB_MASK = CS_CLIMB_A | CS_CLIMB_B;
static constexpr u32 CS_WATER_MASK = CS_SWIM_A | CS_SWIM_B | CS_SHALLOW;
static constexpr u32 CS_AIR_MASK = CS_AIR | CS_LEDGE_FALL;
static constexpr u32 CS_NON_GROUND_MASK =
    CS_AIR_MASK | CS_CLIMB_MASK | CS_WATER_MASK | CS_HAYSTACK;
static constexpr u32 BIND_JUMP = 1u;
static constexpr u32 BIND_DODGE_DIVE = 2u;

static volatile u64 g_stateRaw[GSE_COUNT]{};
static volatile u64 g_stateAccepted[GSE_COUNT]{};
static volatile u64 g_stateActor[GSE_COUNT]{};
static volatile u32 g_stateFlagsAtEvent[GSE_COUNT]{};
static volatile u32 g_statePrevFlagsAtEvent[GSE_COUNT]{};
static volatile u32 g_stateMovementAtEvent[GSE_COUNT]{};
static volatile u64 g_playerContext{};
static volatile u64 g_lastAcceptedStateTick{};
static volatile u64 g_lastJumpInputTick{};
static volatile u64 g_lastDodgeDiveInputTick{};
static volatile u32 g_characterStateFlags{};
static volatile u32 g_stateHooksInstalled{};
static volatile u32 g_stateHookFailures{};
static volatile u64 g_playerBindCount{};
static volatile u64 g_candidateActor[GSE_COUNT]{};
static volatile u64 g_candidateTick[GSE_COUNT]{};
static volatile u64 g_candidateSequence[GSE_COUNT]{};
static volatile u32 g_candidateFlag[GSE_COUNT]{};
static volatile u32 g_candidateSet[GSE_COUNT]{};
static volatile u32 g_candidateBindMask[GSE_COUNT]{};
static volatile u64 g_statePublishSequence{};
static volatile u64 g_staleAirFlagRecoveries{};
static volatile u64 g_r6HeldParkourDirectBinds{};
static volatile u64 g_r6HeldParkourCandidateBinds{};
static volatile u64 g_r7AutoWaterDirectBinds{};
static volatile u64 g_r7AutoWaterEntryEffects{};
static volatile u32 g_currentLeftStickMagnitudeSq{};

static u32 character_flags_for_input(u64 now) {
    u32 flags = atomic_load_u32(&g_characterStateFlags);
    u64 lastState = atomic_load_u64(&g_lastAcceptedStateTick);
    // The supplied r3 log contained an AIR bit that survived for 83 seconds
    // after a missed exit callback. Let a new input recover from that stale
    // latch without expiring persistent swim or climb states.
    if ((flags & CS_AIR_MASK) && lastState && now >= lastState &&
        now - lastState >= (u64)g_cfg.inputLandingTimeoutMs) {
        atomic_fetch_and_u32(&g_characterStateFlags, ~CS_AIR_MASK);
        flags &= ~CS_AIR_MASK;
        atomic_fetch_add_u64(&g_staleAirFlagRecoveries, 1);
    }
    return flags;
}

static bool tick_is_recent(u64 now, u64 then, u32 windowMs) {
    return then && now >= then && now - then <= (u64)windowMs;
}

static u64 state_context_key(void* state) {
    if (!state) return 0;
    // Every verified state callback below performs this same pair of
    // dereferences before doing its own work.  The +0xA8 context is stable
    // across the sibling jump/climb/swim state objects for one actor.
    u8* owner = *(u8**)((u8*)state + 0x08);
    if (!owner) return 0;
    return (u64)*(u8**)(owner + 0xA8);
}

static bool bind_input_is_recent(u64 now, u32 bindMask,
                                 u32 event = GSE_COUNT) {
    u32 window = event == GSE_SWIM_ENTER ? g_cfg.swimInputWindowMs :
                                           g_cfg.stateBindWindowMs;
    if ((bindMask & BIND_JUMP) &&
        tick_is_recent(now, atomic_load_u64(&g_lastJumpInputTick), window))
        return true;
    if ((bindMask & BIND_DODGE_DIVE) &&
        tick_is_recent(now, atomic_load_u64(&g_lastDodgeDiveInputTick), window))
        return true;
    return false;
}

static void accept_state_event(u32 event, u64 actor, u32 flag, bool setFlag,
                               u64 now, bool newBinding) {
    if (event >= GSE_COUNT || !actor) return;
    u32 previousFlags = newBinding ? 0u : atomic_load_u32(&g_characterStateFlags);
    if (newBinding) {
        atomic_store_u64(&g_playerContext, actor);
        atomic_store_u32(&g_characterStateFlags, 0);
        atomic_fetch_add_u64(&g_playerBindCount, 1);
    }
    atomic_store_u32(&g_statePrevFlagsAtEvent[event], previousFlags);
    if (flag) {
        if (setFlag) atomic_fetch_or_u32(&g_characterStateFlags, flag);
        else atomic_fetch_and_u32(&g_characterStateFlags, ~flag);
    }
    atomic_store_u64(&g_stateActor[event], actor);
    atomic_store_u32(&g_stateFlagsAtEvent[event],
                     atomic_load_u32(&g_characterStateFlags));
    atomic_store_u32(&g_stateMovementAtEvent[event],
                     atomic_load_u32(&g_movementMode));
    atomic_store_u64(&g_lastAcceptedStateTick, now);
    atomic_fetch_add_u64(&g_stateAccepted[event], 1);
}

static bool r7_auto_water_entry_evidence(u64 now, u32 event) {
    if (!atomic_load_u32(&g_enabled) || !atomic_load_u32(&g_inputEnabled))
        return false;
    if (!g_cfg.autoWaterEntryReacquire ||
        (event != GSE_SWIM_ENTER && event != GSE_SHALLOW_ENTER))
        return false;
    u32 mode = atomic_load_u32(&g_v8RuntimeMode);
    if (mode == V8_MODE_HELM || mode == V8_MODE_MENU) return false;
    u64 threshold = (u64)g_cfg.autoWaterMovementThreshold;
    u64 stickSq = (u64)atomic_load_u32(&g_currentLeftStickMagnitudeSq);
    if (stickSq < threshold * threshold) return false;
    // Once semantic routing already knows we are in WATER, the real swim-state
    // callback should be allowed to repair missing gameplay-state ownership.
    if (mode == V8_MODE_WATER) return true;
    // On first entry, require recent Edward movement/gameplay evidence as well
    // as held movement so an unrelated NPC swim callback cannot steal binding.
    u64 gait = atomic_load_u64(&g_v8LastGaitTick);
    u64 gameplay = atomic_load_u64(&g_v8LastGameplayEventTick);
    return tick_is_recent(now, gait, g_cfg.autoWaterEvidenceWindowMs) ||
           tick_is_recent(now, gameplay, g_cfg.autoWaterEvidenceWindowMs);
}

static void publish_state_event(void* state, u32 event, u32 flag,
                                bool setFlag, u32 bindMask) {
    if (event >= GSE_COUNT) return;
    atomic_fetch_add_u64(&g_stateRaw[event], 1);
    u64 actor = state_context_key(state);
    if (!actor) return;
    u64 publishSequence = atomic_fetch_add_u64(&g_statePublishSequence, 1) + 1;
    u64 now = g_api.GetTickCount64 ? g_api.GetTickCount64() : 0;
    u64 player = atomic_load_u64(&g_playerContext);
    bool newBinding = false;
    if (player != actor) {
        u64 lastAccepted = atomic_load_u64(&g_lastAcceptedStateTick);
        bool stale = !player || (now >= lastAccepted &&
            now - lastAccepted >= (u64)g_cfg.stateRebindIdleMs);
        bool recentBoundInput = bindMask &&
            bind_input_is_recent(now, bindMask, event);
        // r6: Black Flag automatic parkour reuses one continuously-held
        // Cross/A press across multiple Jump/InAir/Climb transitions. Treat
        // those callbacks as explicit player evidence while the held input
        // keeps its timestamp fresh instead of requiring a new rising edge.
        bool heldParkourEvidence = recentBoundInput &&
            (event == GSE_JUMP || event == GSE_AIR_ENTER ||
             event == GSE_FREEFALL || event == GSE_LEDGE_FALL_ENTER ||
             event == GSE_CLIMB_ENTER);
        bool explicitPlayerAction = recentBoundInput &&
            (event == GSE_SWIM_ENTER || heldParkourEvidence);
        bool autoWaterEvidence = r7_auto_water_entry_evidence(now, event);
        if (bindMask && (((stale || explicitPlayerAction) && recentBoundInput) ||
                         autoWaterEvidence)) {
            newBinding = true;
            if (heldParkourEvidence && player && player != actor)
                atomic_fetch_add_u64(&g_r6HeldParkourDirectBinds, 1);
            if (autoWaterEvidence)
                atomic_fetch_add_u64(&g_r7AutoWaterDirectBinds, 1);
        } else {
            // State transitions can precede the independent 8 ms controller
            // poll. Retain one candidate per event type so a following InAir
            // callback cannot overwrite a Jump callback before the configured
            // jump-action edge binds Edward's actor context.
            if (bindMask) {
                atomic_store_u64(&g_candidateActor[event], actor);
                atomic_store_u32(&g_candidateFlag[event], flag);
                atomic_store_u32(&g_candidateSet[event], setFlag ? 1u : 0u);
                atomic_store_u32(&g_candidateBindMask[event], bindMask);
                atomic_store_u64(&g_candidateTick[event], now);
                atomic_store_u64(&g_candidateSequence[event], publishSequence);
            }
            return;
        }
    }
    accept_state_event(event, actor, flag, setFlag, now, newBinding);
}

static void try_bind_recent_state_candidate(u64 now, u32 inputBit) {
    u64 actor = 0;
    u64 newestTick = 0;
    u64 newestSequence = 0;
    u32 bestPriority = 0;
    for (u32 event = 0; event < GSE_COUNT; ++event) {
        u32 mask = atomic_load_u32(&g_candidateBindMask[event]);
        u64 tick = atomic_load_u64(&g_candidateTick[event]);
        u64 sequence = atomic_load_u64(&g_candidateSequence[event]);
        if (!(mask & inputBit) || !tick_is_recent(now, tick, 120)) continue;
        u32 priority = 1;
        bool parkourCandidate = inputBit == BIND_JUMP &&
            (event == GSE_JUMP || event == GSE_AIR_ENTER ||
             event == GSE_FREEFALL || event == GSE_LEDGE_FALL_ENTER ||
             event == GSE_CLIMB_ENTER);
        if (parkourCandidate ||
            (inputBit == BIND_DODGE_DIVE && event == GSE_SWIM_ENTER))
            priority = 2;
        if (priority > bestPriority ||
            (priority == bestPriority &&
             (tick > newestTick || (tick == newestTick &&
                                    sequence >= newestSequence)))) {
            bestPriority = priority;
            newestTick = tick;
            newestSequence = sequence;
            actor = atomic_load_u64(&g_candidateActor[event]);
        }
    }
    if (!actor) return;
    u64 player = atomic_load_u64(&g_playerContext);
    u64 lastAccepted = atomic_load_u64(&g_lastAcceptedStateTick);
    bool stale = !player || (now >= lastAccepted &&
        now - lastAccepted >= (u64)g_cfg.stateRebindIdleMs);
    // A Jump or Swimming-entry callback adjacent to the matching player input
    // is stronger evidence than an old actor binding.  r3's stale-only rule
    // accepted just 3/21 jump and 0/3 swim transitions in the supplied log.
    if (!stale && player != actor && bestPriority < 2) return;

    bool newBinding = player != actor;
    if (newBinding && inputBit == BIND_JUMP && bestPriority >= 2)
        atomic_fetch_add_u64(&g_r6HeldParkourCandidateBinds, 1);
    // Replay every recent candidate for the chosen actor in timestamp order.
    // This preserves Jump -> InAir ordering when both arrive before the poll.
    for (u32 accepted = 0; accepted < GSE_COUNT; ++accepted) {
        u32 chosen = GSE_COUNT;
        u64 oldestTick = ~0ull;
        u64 oldestSequence = ~0ull;
        for (u32 event = 0; event < GSE_COUNT; ++event) {
            u32 mask = atomic_load_u32(&g_candidateBindMask[event]);
            u64 tick = atomic_load_u64(&g_candidateTick[event]);
            u64 sequence = atomic_load_u64(&g_candidateSequence[event]);
            if (!(mask & inputBit) || !tick_is_recent(now, tick, 120) ||
                atomic_load_u64(&g_candidateActor[event]) != actor)
                continue;
            if (sequence < oldestSequence) {
                oldestTick = tick;
                oldestSequence = sequence;
                chosen = event;
            }
        }
        if (chosen >= GSE_COUNT) break;
        atomic_store_u32(&g_candidateBindMask[chosen], 0);
        accept_state_event(chosen, actor,
                           atomic_load_u32(&g_candidateFlag[chosen]),
                           atomic_load_u32(&g_candidateSet[chosen]) != 0,
                           oldestTick, newBinding);
        newBinding = false;
    }
}

using StateVirtualFn = void (FASTABI*)(void*, u64, u64, u64);
static StateVirtualFn g_origAirEnter{}, g_origAirExit{};
static StateVirtualFn g_origJumpOnSpot{}, g_origFreeJump{}, g_origTargetedJump{};
static StateVirtualFn g_origFreefall{}, g_origTransitionExit{};
static StateVirtualFn g_origLedgeFallEnter{}, g_origLedgeFallExit{};
static StateVirtualFn g_origClimbAEnter{}, g_origClimbAExit{};
static StateVirtualFn g_origClimbBEnter{}, g_origClimbBExit{};
static StateVirtualFn g_origSwimAEnter{}, g_origSwimAExit{};
static StateVirtualFn g_origSwimBEnter{}, g_origSwimBExit{};
static StateVirtualFn g_origShallowEnter{}, g_origShallowExit{};
static StateVirtualFn g_origHaystackEnter{}, g_origHaystackExit{};

static void FASTABI state_air_enter(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_AIR_ENTER, CS_AIR, true,
                        BIND_JUMP | BIND_DODGE_DIVE);
    if (g_origAirEnter) g_origAirEnter(s, a2, a3, a4);
}
static void FASTABI state_air_exit(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_AIR_EXIT, CS_AIR, false, 0);
    if (g_origAirExit) g_origAirExit(s, a2, a3, a4);
}
static void FASTABI state_jump_spot(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_JUMP, 0, true, BIND_JUMP);
    if (g_origJumpOnSpot) g_origJumpOnSpot(s, a2, a3, a4);
}
static void FASTABI state_free_jump(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_JUMP, 0, true, BIND_JUMP);
    if (g_origFreeJump) g_origFreeJump(s, a2, a3, a4);
}
static void FASTABI state_targeted_jump(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_JUMP, 0, true, BIND_JUMP);
    if (g_origTargetedJump) g_origTargetedJump(s, a2, a3, a4);
}
static void FASTABI state_freefall(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_FREEFALL, CS_AIR, true,
                        BIND_JUMP | BIND_DODGE_DIVE);
    if (g_origFreefall) g_origFreefall(s, a2, a3, a4);
}
static void FASTABI state_ledge_fall_enter(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_LEDGE_FALL_ENTER, CS_LEDGE_FALL, true,
                        BIND_JUMP | BIND_DODGE_DIVE);
    if (g_origLedgeFallEnter) g_origLedgeFallEnter(s, a2, a3, a4);
}
static void FASTABI state_ledge_fall_exit(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_LEDGE_FALL_EXIT, CS_LEDGE_FALL, false, 0);
    if (g_origLedgeFallExit) g_origLedgeFallExit(s, a2, a3, a4);
}
static void FASTABI state_transition_exit(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_TRANSITION_EXIT, 0, true, 0);
    if (g_origTransitionExit) g_origTransitionExit(s, a2, a3, a4);
}
static void FASTABI state_climb_a_enter(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_CLIMB_ENTER, CS_CLIMB_A, true, BIND_JUMP);
    if (g_origClimbAEnter) g_origClimbAEnter(s, a2, a3, a4);
}
static void FASTABI state_climb_a_exit(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_CLIMB_EXIT, CS_CLIMB_A, false, 0);
    if (g_origClimbAExit) g_origClimbAExit(s, a2, a3, a4);
}
static void FASTABI state_climb_b_enter(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_CLIMB_ENTER, CS_CLIMB_B, true, BIND_JUMP);
    if (g_origClimbBEnter) g_origClimbBEnter(s, a2, a3, a4);
}
static void FASTABI state_climb_b_exit(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_CLIMB_EXIT, CS_CLIMB_B, false, 0);
    if (g_origClimbBExit) g_origClimbBExit(s, a2, a3, a4);
}
static void FASTABI state_swim_a_enter(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_SWIM_ENTER, CS_SWIM_A, true,
                        BIND_JUMP | BIND_DODGE_DIVE);
    if (g_origSwimAEnter) g_origSwimAEnter(s, a2, a3, a4);
}
static void FASTABI state_swim_a_exit(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_SWIM_EXIT, CS_SWIM_A, false, 0);
    if (g_origSwimAExit) g_origSwimAExit(s, a2, a3, a4);
}
static void FASTABI state_swim_b_enter(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_SWIM_ENTER, CS_SWIM_B, true,
                        BIND_JUMP | BIND_DODGE_DIVE);
    if (g_origSwimBEnter) g_origSwimBEnter(s, a2, a3, a4);
}
static void FASTABI state_swim_b_exit(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_SWIM_EXIT, CS_SWIM_B, false, 0);
    if (g_origSwimBExit) g_origSwimBExit(s, a2, a3, a4);
}
static void FASTABI state_shallow_enter(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_SHALLOW_ENTER, CS_SHALLOW, true,
                        BIND_JUMP | BIND_DODGE_DIVE);
    if (g_origShallowEnter) g_origShallowEnter(s, a2, a3, a4);
}
static void FASTABI state_shallow_exit(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_SHALLOW_EXIT, CS_SHALLOW, false, 0);
    if (g_origShallowExit) g_origShallowExit(s, a2, a3, a4);
}
static void FASTABI state_haystack_enter(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_HAYSTACK_ENTER, CS_HAYSTACK, true,
                        BIND_JUMP | BIND_DODGE_DIVE);
    if (g_origHaystackEnter) g_origHaystackEnter(s, a2, a3, a4);
}
static void FASTABI state_haystack_exit(void* s, u64 a2, u64 a3, u64 a4) {
    publish_state_event(s, GSE_HAYSTACK_EXIT, CS_HAYSTACK, false, 0);
    if (g_origHaystackExit) g_origHaystackExit(s, a2, a3, a4);
}

static bool patch_state_vtable(u64 slotRva, u64 expectedFunctionRva,
                               StateVirtualFn replacement, StateVirtualFn* original,
                               const char* label) {
    if (!g_gameBase || slotRva + sizeof(u64) > g_gameSize ||
        expectedFunctionRva >= g_gameSize || !replacement || !original) {
        atomic_fetch_add_u32(&g_stateHookFailures, 1);
        return false;
    }
    u64* slot = (u64*)(g_gameBase + slotRva);
    u64 expected = (u64)(g_gameBase + expectedFunctionRva);
    if (*slot != expected) {
        TextBuf b{}; SYSTEMTIME st{};
        if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
        tb_char(b, '['); tb_time(b, st); tb_str(b, "] STATE HOOK mismatch ");
        tb_str(b, label); tb_str(b, " slotRVA=0x"); tb_hex(b, slotRva);
        tb_str(b, " expected=0x"); tb_hex(b, expected);
        tb_str(b, " actual=0x"); tb_hex(b, *slot); tb_endline(b);
        append_log(b.bytes, b.len);
        atomic_fetch_add_u32(&g_stateHookFailures, 1);
        return false;
    }
    DWORD oldProtect = 0;
    if (!g_api.VirtualProtect(slot, sizeof(u64), PAGE_READWRITE, &oldProtect)) {
        atomic_fetch_add_u32(&g_stateHookFailures, 1);
        return false;
    }
    *original = api_cast<StateVirtualFn>((void*)*slot);
    *slot = (u64)replacement;
    DWORD ignored = 0;
    g_api.VirtualProtect(slot, sizeof(u64), oldProtect, &ignored);
    atomic_fetch_add_u32(&g_stateHooksInstalled, 1);
    return true;
}

struct StateHookTarget {
    u64 slot106;
    u64 function106;
    u64 slot107;
    u64 function107;
    StateVirtualFn replacement;
    StateVirtualFn* original;
    const char* label;
};

static const StateHookTarget kStateHookTargets[] = {
    {0x09151C18ull,0x015273A0ull,0x091E0E98ull,0x01521C20ull,
        &state_air_enter,&g_origAirEnter,"InAir enter"},
    {0x09151C20ull,0x017FC510ull,0x091E0EA0ull,0x017FC1E0ull,
        &state_air_exit,&g_origAirExit,"InAir exit"},
    {0x09152898ull,0x05E7F6C0ull,0x091E1B18ull,0x05F1D4C0ull,
        &state_jump_spot,&g_origJumpOnSpot,"JumpOnSpot enter"},
    {0x09152848ull,0x06B118E0ull,0x091E1AC8ull,0x06BAF660ull,
        &state_free_jump,&g_origFreeJump,"FreeJump enter"},
    {0x0914E4E8ull,0x02750BF0ull,0x091DD768ull,0x027D5940ull,
        &state_targeted_jump,&g_origTargetedJump,"TargetedJump enter"},
    {0x0914E3F8ull,0x01FCD920ull,0x091DD678ull,0x01FA4D60ull,
        &state_freefall,&g_origFreefall,"Freefall enter"},
    {0x09151F38ull,0x06B01790ull,0x091E11B8ull,0x06B9F510ull,
        &state_ledge_fall_enter,&g_origLedgeFallEnter,"FallFromLedge enter"},
    {0x09151F40ull,0x06B01A30ull,0x091E11C0ull,0x06B9F7B0ull,
        &state_ledge_fall_exit,&g_origLedgeFallExit,"FallFromLedge exit"},
    {0x0914E218ull,0x06AFDB60ull,0x091DD498ull,0x06B9B8E0ull,
        &state_transition_exit,&g_origTransitionExit,"TransitionExit enter"},
    {0x09147A38ull,0x0152D180ull,0x091D6CB8ull,0x01527A00ull,
        &state_climb_a_enter,&g_origClimbAEnter,"Climb A enter"},
    {0x09147A40ull,0x0102CC10ull,0x091D6CC0ull,0x0102B6F0ull,
        &state_climb_a_exit,&g_origClimbAExit,"Climb A exit"},
    {0x0920DC90ull,0x06A98A70ull,0x0929D130ull,0x06B367F0ull,
        &state_climb_b_enter,&g_origClimbBEnter,"Climb B enter"},
    {0x0920DC98ull,0x06A99E00ull,0x0929D138ull,0x06B37B80ull,
        &state_climb_b_exit,&g_origClimbBExit,"Climb B exit"},
    {0x09159718ull,0x027D3CF0ull,0x091E89B8ull,0x02859AA0ull,
        &state_swim_a_enter,&g_origSwimAEnter,"Swimming A enter"},
    {0x09159720ull,0x018A3D00ull,0x091E89C0ull,0x018A05B0ull,
        &state_swim_a_exit,&g_origSwimAExit,"Swimming A exit"},
    {0x091A3928ull,0x08065C00ull,0x09232D28ull,0x080F8A70ull,
        &state_swim_b_enter,&g_origSwimBEnter,"Swimming B enter"},
    {0x091A3930ull,0x08066540ull,0x09232D30ull,0x080F93B0ull,
        &state_swim_b_exit,&g_origSwimBExit,"Swimming B exit"},
    {0x0915AB98ull,0x06C9ED00ull,0x091E9E38ull,0x06D3CB40ull,
        &state_shallow_enter,&g_origShallowEnter,"ShallowSwim enter"},
    {0x0915ABA0ull,0x06C9FAC0ull,0x091E9E40ull,0x06D3D900ull,
        &state_shallow_exit,&g_origShallowExit,"ShallowSwim exit"},
    {0x09145208ull,0x01B6C060ull,0x091D4488ull,0x01B6C0C0ull,
        &state_haystack_enter,&g_origHaystackEnter,"HayStack enter"},
    {0x09145210ull,0x054F1850ull,0x091D4490ull,0x0559F190ull,
        &state_haystack_exit,&g_origHaystackExit,"HayStack exit"}
};

static void install_gameplay_state_hooks() {
    if (!g_cfg.gameplayStateHooks) {
        log_simple("Gameplay state hooks disabled by INI; button fallbacks remain active");
        return;
    }
    bool use107 = compat_is_107_layout();
    log_simple(use107 ?
        "Detected attached Assassin's Creed IV 1.07 executable layout; using verified 1.07 state targets" :
        "Using verified 1.06-compatible gameplay state targets");
    for (const StateHookTarget& target : kStateHookTargets) {
        patch_state_vtable(use107 ? target.slot107 : target.slot106,
                           use107 ? target.function107 : target.function106,
                           target.replacement, target.original, target.label);
    }

    TextBuf b{}; SYSTEMTIME st{};
    if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    tb_char(b, '['); tb_time(b, st); tb_str(b, "] Gameplay state vtable hooks installed=");
    tb_u64(b, atomic_load_u32(&g_stateHooksInstalled)); tb_str(b, " failed=");
    tb_u64(b, atomic_load_u32(&g_stateHookFailures)); tb_endline(b);
    append_log(b.bytes, b.len);
}

struct XINPUT_GAMEPAD {
    WORD wButtons;
    u8 bLeftTrigger;
    u8 bRightTrigger;
    i16 sThumbLX;
    i16 sThumbLY;
    i16 sThumbRX;
    i16 sThumbRY;
};
struct XINPUT_STATE {
    u32 dwPacketNumber;
    XINPUT_GAMEPAD Gamepad;
};
struct XINPUT_VIBRATION {
    WORD wLeftMotorSpeed;
    WORD wRightMotorSpeed;
};
static_assert(sizeof(XINPUT_STATE) == 16, "Unexpected XInput state layout");
static_assert(sizeof(XINPUT_VIBRATION) == 4, "Unexpected XInput vibration layout");

static bool input_binding_down(const InputBinding& binding,
                               const XINPUT_STATE& state) {
    if (binding.type == INPUT_BINDING_BUTTON)
        return binding.button && (state.Gamepad.wButtons & binding.button) != 0;
    if (binding.type == INPUT_BINDING_LEFT_TRIGGER)
        return state.Gamepad.bLeftTrigger >= (u8)g_cfg.triggerThreshold;
    if (binding.type == INPUT_BINDING_RIGHT_TRIGGER)
        return state.Gamepad.bRightTrigger >= (u8)g_cfg.triggerThreshold;
    return false;
}

static bool input_binding_rising(const InputBinding& binding,
                                 const XINPUT_STATE& current,
                                 const XINPUT_STATE& previous,
                                 bool havePrevious) {
    return input_binding_down(binding, current) &&
           (!havePrevious || !input_binding_down(binding, previous));
}

static bool input_binding_falling(const InputBinding& binding,
                                  const XINPUT_STATE& current,
                                  const XINPUT_STATE& previous,
                                  bool havePrevious) {
    return havePrevious && !input_binding_down(binding, current) &&
           input_binding_down(binding, previous);
}

static const char* input_binding_name(const InputBinding& binding) {
    if (binding.type == INPUT_BINDING_LEFT_TRIGGER) return "L2";
    if (binding.type == INPUT_BINDING_RIGHT_TRIGGER) return "R2";
    if (binding.type != INPUT_BINDING_BUTTON || !binding.button) return "None";
    switch (binding.button) {
    case XINPUT_A: return "Cross";
    case XINPUT_B: return "Circle";
    case XINPUT_X: return "Square";
    case XINPUT_Y: return "Triangle";
    case XINPUT_LEFT_SHOULDER: return "L1";
    case XINPUT_RIGHT_SHOULDER: return "R1";
    case XINPUT_LEFT_THUMB: return "L3";
    case XINPUT_RIGHT_THUMB: return "R3";
    case XINPUT_DPAD_UP: return "DPadUp";
    case XINPUT_DPAD_DOWN: return "DPadDown";
    case XINPUT_DPAD_LEFT: return "DPadLeft";
    case XINPUT_DPAD_RIGHT: return "DPadRight";
    case XINPUT_START: return "Options";
    case XINPUT_BACK: return "Share";
    default: return "None";
    }
}

using XInputGetStateFn = DWORD (WINABI*)(DWORD, XINPUT_STATE*);
using XInputSetStateFn = DWORD (WINABI*)(DWORD, XINPUT_VIBRATION*);
static XInputGetStateFn g_xinputGetState{};
static XInputSetStateFn g_xinputSetState{};
static volatile u64 g_controllerPackets{};
static volatile u32 g_lastControllerButtons{};
static volatile u32 g_lastGameInputButtons{};
static volatile u32 g_rawControllerButtonCount{};
static volatile u64 g_rawControllerButtons{};
static volatile u32 g_rawTouchpadDown{};
static volatile u64 g_rawTouchpadEdges{};
static volatile u32 g_rumbleBackend{}; // 0=none, 1=GameInput, 2=XInput
static volatile u32 g_activeDeviceStatus{};
static volatile u64 g_rumbleWrites{};
static volatile u64 g_gameInputRumbleWrites{};
static volatile u64 g_xinputRumbleWrites{};
static volatile u32 g_lastRumbleLow{};
static volatile u32 g_lastRumbleHigh{};
static XInputSetStateFn g_gameXInputSetStateOriginal{};
static volatile u32 g_gameXInputObserverInstalled{};
static volatile u32 g_gameXInputObserverFailures{};
static volatile u64 g_gameNativeXInputWrites{};

static DWORD WINABI game_xinput_set_state_observer(DWORD index,
                                                    XINPUT_VIBRATION* vibration) {
    if (vibration && index == g_cfg.controllerIndex) {
        WORD strongest = vibration->wLeftMotorSpeed > vibration->wRightMotorSpeed ?
            vibration->wLeftMotorSpeed : vibration->wRightMotorSpeed;
        float strength = (float)strongest / 65535.0f;
        u64 now = g_api.GetTickCount64 ? g_api.GetTickCount64() :
                  atomic_load_u64(&g_workerNowTick);
        if (strength >= g_cfg.nativeRumbleThreshold)
            atomic_fetch_add_u64(&g_gameNativeXInputWrites, 1);
        mark_native_rumble(strength, now);
    }
    return g_gameXInputSetStateOriginal ?
        g_gameXInputSetStateOriginal(index, vibration) : 1u;
}

// v8-r9l: dynamically resolve XInputSetState from the game's PE import table,
// while classifying only the four executable layouts already verified by r9h.
// Unknown layouts remain fail-closed and never enable the Ubisoft direct fallback.
static bool r9k_game_rva_valid(u64 rva, u64 size) {
    return g_gameBase && rva < (u64)g_gameSize && size <= (u64)g_gameSize - rva;
}
static bool r9k_ascii_starts_ci(const char* value, const char* prefix) {
    if (!value || !prefix) return false;
    while (*prefix) {
        if (!*value || lower_ascii(*value++) != lower_ascii(*prefix++)) return false;
    }
    return true;
}
static bool r9k_ascii_equal(const char* a, const char* b) {
    if (!a || !b) return false;
    while (*a && *b) if (*a++ != *b++) return false;
    return *a == 0 && *b == 0;
}
static u64* r9k_find_xinput_setstate_iat(u64* outRva, u32* outMethod) {
    if (outRva) *outRva = 0;
    if (outMethod) *outMethod = 0;
    if (!g_gameBase || !g_xinputSetState || !r9k_game_rva_valid(0, 0x40) ||
        *(u16*)g_gameBase != 0x5A4D) return nullptr;

    u32 peoff = *(u32*)(g_gameBase + 0x3C);
    if (!r9k_game_rva_valid(peoff, 24)) return nullptr;
    u8* nt = g_gameBase + peoff;
    if (*(u32*)nt != 0x00004550) return nullptr;
    u8* opt = nt + 24;
    if (*(u16*)opt != 0x20B) return nullptr;

    u32 importRva = *(u32*)(opt + 0x78);
    u32 importSize = *(u32*)(opt + 0x7C);
    if (!importRva || !r9k_game_rva_valid(importRva, 20)) return nullptr;
    u32 descriptors = importSize ? importSize / 20u + 1u : 1024u;
    if (descriptors > 4096u) descriptors = 4096u;

    for (u32 d = 0; d < descriptors; ++d) {
        u64 descRva = (u64)importRva + (u64)d * 20ull;
        if (!r9k_game_rva_valid(descRva, 20)) break;
        u8* desc = g_gameBase + descRva;
        u32 oft = *(u32*)(desc + 0x00);
        u32 time = *(u32*)(desc + 0x04);
        u32 fwd = *(u32*)(desc + 0x08);
        u32 nameRva = *(u32*)(desc + 0x0C);
        u32 firstThunk = *(u32*)(desc + 0x10);
        if (!(oft | time | fwd | nameRva | firstThunk)) break;
        if (!nameRva || !firstThunk || !r9k_game_rva_valid(nameRva, 1) ||
            !r9k_game_rva_valid(firstThunk, 8)) continue;
        const char* dll = (const char*)(g_gameBase + nameRva);
        if (!r9k_ascii_starts_ci(dll, "xinput")) continue;

        for (u32 i = 0; i < 4096u; ++i) {
            u64 slotRva = (u64)firstThunk + (u64)i * sizeof(u64);
            if (!r9k_game_rva_valid(slotRva, sizeof(u64))) break;
            u64* slot = (u64*)(g_gameBase + slotRva);
            u64 target = *slot;
            if (!target) break;
            if (oft) {
                u64 intRva = (u64)oft + (u64)i * sizeof(u64);
                if (!r9k_game_rva_valid(intRva, sizeof(u64))) break;
                u64 nameThunk = *(u64*)(g_gameBase + intRva);
                if (!nameThunk) break;
                static constexpr u64 ORD64 = 0x8000000000000000ull;
                if (!(nameThunk & ORD64) && nameThunk <= 0xFFFFFFFFull &&
                    r9k_game_rva_valid(nameThunk, 3)) {
                    u64 symRva = nameThunk + 2ull;
                    if (r9k_game_rva_valid(symRva, 15)) {
                        const char* sym = (const char*)(g_gameBase + symRva);
                        if (r9k_ascii_equal(sym, "XInputSetState")) {
                            if (outRva) *outRva = slotRva;
                            if (outMethod) *outMethod = 1;
                            return slot;
                        }
                    }
                }
            }
            if (target == (u64)g_xinputSetState) {
                if (outRva) *outRva = slotRva;
                if (outMethod) *outMethod = 2;
                return slot;
            }
        }
    }
    return nullptr;
}
static u32 r9k_profile_from_xinput_rva(u64 rva) {
    if (rva == 0x1D0119E0ull) return R9K_PROFILE_STEAM_FAMILY;
    if (rva == 0x1C5769E0ull) return R9K_PROFILE_UBISOFT_CONNECT;
    if (rva == 0x1EC409E0ull) return R9K_PROFILE_UBISOFT_PLUS;
    if (rva == 0x1C6109E0ull) return R9K_PROFILE_STEAM_TU_107;
    return R9K_PROFILE_UNKNOWN;
}
static const char* r9k_profile_name(u32 profile) {
    if (profile == R9K_PROFILE_STEAM_FAMILY) return "Steam/voices38";
    if (profile == R9K_PROFILE_UBISOFT_CONNECT) return "Ubisoft Connect";
    if (profile == R9K_PROFILE_UBISOFT_PLUS) return "Ubisoft+";
    if (profile == R9K_PROFILE_STEAM_TU_107) return "Steam/voices38 TU 1.0.7";
    return "unknown";
}
static u64* r9k_resolve_and_classify_xinput(u64* outRva, u32* outMethod) {
    u64 rva = 0; u32 method = 0;
    u64* slot = r9k_find_xinput_setstate_iat(&rva, &method);
    if (!slot) {
        static constexpr u64 known[] = {
            0x1D0119E0ull,0x1C5769E0ull,0x1EC409E0ull,0x1C6109E0ull
        };
        for (u32 i=0;i<(u32)(sizeof(known)/sizeof(known[0]));++i) {
            if (!r9k_game_rva_valid(known[i], sizeof(u64))) continue;
            u64* candidate=(u64*)(g_gameBase+known[i]);
            if (*candidate==(u64)g_xinputSetState) {
                slot=candidate; rva=known[i]; method=3; break;
            }
        }
    }
    u32 profile = r9k_profile_from_xinput_rva(rva);
    atomic_store_u64(&g_r9kXInputIatRva, rva);
    atomic_store_u32(&g_r9kXInputResolveMethod, method);
    atomic_store_u32(&g_r9kGameProfile, profile);
    if (outRva) *outRva=rva;
    if (outMethod) *outMethod=method;
    return slot;
}
static void install_game_xinput_observer() {
    if (!g_xinputSetState || !g_gameBase) return;
    u64 slotRva=0; u32 method=0;
    u64* slot=r9k_resolve_and_classify_xinput(&slotRva,&method);
    u32 profile=atomic_load_u32(&g_r9kGameProfile);

    {
        TextBuf b{}; SYSTEMTIME st{}; if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
        tb_char(b,'['); tb_time(b,st); tb_str(b,"] R9K executable profile=");
        tb_str(b,r9k_profile_name(profile)); tb_str(b," xinputIAT=0x"); tb_hex(b,slotRva);
        tb_str(b," method="); tb_str(b,method==1?"named-import":method==2?"pointer-match":method==3?"profile-fallback":"none");
        tb_str(b," directFallbackEligible="); tb_u64(b,r9k_ubisoft_profile()?1u:0u);
        tb_endline(b); append_log(b.bytes,b.len);
    }

    if (!g_cfg.nativeArbitration) return;
    if (!slot || profile==R9K_PROFILE_UNKNOWN || !*slot) {
        atomic_fetch_add_u32(&g_gameXInputObserverFailures, 1);
        log_simple("WARNING r9k XInput observer could not resolve a verified XInputSetState IAT; leaving game imports untouched");
        return;
    }
    DWORD oldProtect=0;
    if (!g_api.VirtualProtect || !g_api.VirtualProtect(slot,sizeof(u64),PAGE_READWRITE,&oldProtect)) {
        atomic_fetch_add_u32(&g_gameXInputObserverFailures, 1);
        log_simple("WARNING r9k XInput observer could not make IAT writable");
        return;
    }
    g_gameXInputSetStateOriginal=api_cast<XInputSetStateFn>((void*)*slot);
    *slot=(u64)&game_xinput_set_state_observer;
    DWORD ignored=0; g_api.VirtualProtect(slot,sizeof(u64),oldProtect,&ignored);
    atomic_store_u32(&g_gameXInputObserverInstalled,1);
    TextBuf b{}; SYSTEMTIME st{}; if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    tb_char(b,'['); tb_time(b,st); tb_str(b,"] Installed r9k XInput observer IAT RVA 0x");
    tb_hex(b,slotRva); tb_str(b," profile="); tb_str(b,r9k_profile_name(profile));
    tb_endline(b); append_log(b.bytes,b.len);
}

// Black Flag ships against the public GameInput v0 ABI. Only the small ABI
// surface needed here is declared, keeping the ASI freestanding and avoiding a
// redistributable dependency.
struct GameInputGamepadStateV0 {
    u32 buttons;
    float leftTrigger;
    float rightTrigger;
    float leftThumbstickX;
    float leftThumbstickY;
    float rightThumbstickX;
    float rightThumbstickY;
};
struct GameInputRumbleParamsV0 {
    float lowFrequency;
    float highFrequency;
    float leftTrigger;
    float rightTrigger;
};
struct GameInputDeviceInfoPrefixV0 {
    u32 infoSize;
    u16 vendorId;
    u16 productId;
    u16 revisionNumber;
    u8 interfaceNumber;
    u8 collectionNumber;
    u16 usagePage;
    u16 usageId;
};

using GameInputCreateFn = i32 (WINABI*)(void**);
using ComReleaseFn = u32 (FASTABI*)(void*);
using GameInputGetCurrentReadingFn = i32 (FASTABI*)(void*, u32, void*, void**);
using GameInputReadingGetDeviceFn = void (FASTABI*)(void*, void**);
using GameInputReadingGetControllerButtonCountFn = u32 (FASTABI*)(void*);
using GameInputReadingGetControllerButtonStateFn = u32 (FASTABI*)(void*, u32, u8*);
using GameInputReadingGetGamepadStateFn = u8 (FASTABI*)(void*, GameInputGamepadStateV0*);
using GameInputDeviceGetInfoFn = const GameInputDeviceInfoPrefixV0* (FASTABI*)(void*);
using GameInputDeviceGetStatusFn = u32 (FASTABI*)(void*);
using GameInputDeviceSetRumbleFn = void (FASTABI*)(void*, const GameInputRumbleParamsV0*);

static void* g_gameInput{};
static void* g_gameInputDevice{};
// r9g: metadata and raw-layout caches. Controller identity is refreshed at a
// modest cadence instead of performing GetDevice/GetInfo/GetStatus/vtable
// observer work on every 2 ms input sample.
static u64 g_gameInputDeviceIdentityTick{};
static u64 g_gameInputDeviceStatusTick{};
static u32 g_gameInputRawButtonCountCached{};
static bool g_gameInputRawButtonCountValid{};
static constexpr u32 GAMEINPUT_OBSERVER_SLOTS = 4;
static void** g_gameInputObservedSlots[GAMEINPUT_OBSERVER_SLOTS]{};
static GameInputDeviceSetRumbleFn g_gameInputObservedOriginals[GAMEINPUT_OBSERVER_SLOTS]{};
static volatile u32 g_gameInputObserversInstalled{};
static volatile u32 g_gameInputObserverFailures{};
static volatile u32 g_pluginGameInputWriterThread{};
static volatile u64 g_gameNativeGameInputWrites{};

static void FASTABI gameinput_rumble_observer(void* self,
                                              const GameInputRumbleParamsV0* params) {
    u32 currentThread = g_api.GetCurrentThreadId ? g_api.GetCurrentThreadId() : 0;
    bool bridgeWrite = currentThread &&
        currentThread == atomic_load_u32(&g_pluginGameInputWriterThread);
    if (!bridgeWrite && params) {
        float strength = params->lowFrequency;
        if (params->highFrequency > strength) strength = params->highFrequency;
        if (params->leftTrigger > strength) strength = params->leftTrigger;
        if (params->rightTrigger > strength) strength = params->rightTrigger;
        u64 now = g_api.GetTickCount64 ? g_api.GetTickCount64() :
                  atomic_load_u64(&g_workerNowTick);
        if (strength >= g_cfg.nativeRumbleThreshold)
            atomic_fetch_add_u64(&g_gameNativeGameInputWrites, 1);
        mark_native_rumble(strength, now);
    }

    void** slot = self && *(void***)self ? &(*(void***)self)[10] : nullptr;
    for (u32 i = 0; i < GAMEINPUT_OBSERVER_SLOTS; ++i) {
        if (g_gameInputObservedSlots[i] == slot && g_gameInputObservedOriginals[i]) {
            g_gameInputObservedOriginals[i](self, params);
            return;
        }
    }
}

static void install_gameinput_rumble_observer(void* device) {
    if (!g_cfg.nativeArbitration || !device || !*(void***)device) return;
    void** slot = &(*(void***)device)[10];
    for (u32 i = 0; i < GAMEINPUT_OBSERVER_SLOTS; ++i)
        if (g_gameInputObservedSlots[i] == slot) return;

    u32 freeIndex = GAMEINPUT_OBSERVER_SLOTS;
    for (u32 i = 0; i < GAMEINPUT_OBSERVER_SLOTS; ++i) {
        if (!g_gameInputObservedSlots[i]) { freeIndex = i; break; }
    }
    if (freeIndex == GAMEINPUT_OBSERVER_SLOTS || !*slot ||
        *slot == (void*)&gameinput_rumble_observer) {
        atomic_fetch_add_u32(&g_gameInputObserverFailures, 1);
        return;
    }
    DWORD oldProtect = 0;
    if (!g_api.VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &oldProtect)) {
        atomic_fetch_add_u32(&g_gameInputObserverFailures, 1);
        return;
    }
    g_gameInputObservedOriginals[freeIndex] =
        api_cast<GameInputDeviceSetRumbleFn>(*slot);
    g_gameInputObservedSlots[freeIndex] = slot;
    *slot = (void*)&gameinput_rumble_observer;
    DWORD ignored = 0;
    g_api.VirtualProtect(slot, sizeof(void*), oldProtect, &ignored);
    atomic_fetch_add_u32(&g_gameInputObserversInstalled, 1);
}

static void* com_method(void* object, u32 index) {
    if (!object) return nullptr;
    void*** p = (void***)object;
    return p && *p ? (*p)[index] : nullptr;
}

static void com_release(void* object) {
    ComReleaseFn release = api_cast<ComReleaseFn>(com_method(object, 2));
    if (release) release(object);
}

static bool is_dualsense_product(u16 vendor, u16 product) {
    if (vendor != 0x054C) return false;
    // DualSense, DualSense Edge, and the common virtual DualSense identities.
    return product == 0x0CE6 || product == 0x0DF2;
}

static bool init_gameinput() {
    u8* module = find_module_ascii("gameinput.dll");
    if (!module && g_api.LoadLibraryW) module = (u8*)g_api.LoadLibraryW(L"GameInput.dll");
    if (!module) {
        log_simple("GameInput unavailable; XInput fallback remains enabled");
        return false;
    }
    void* createAddress = resolve_export(module, "GameInputCreate");
    if (!createAddress) createAddress = resolve_export_ordinal(module, 1);
    GameInputCreateFn create = api_cast<GameInputCreateFn>(createAddress);
    if (!create || create(&g_gameInput) < 0 || !g_gameInput) {
        log_simple("GameInputCreate failed; XInput fallback remains enabled");
        g_gameInput = nullptr;
        return false;
    }
    log_simple("GameInput v0 initialized for native, virtual, HID, and XInput-class pads");
    return true;
}

static i16 float_to_thumb(float value) {
    value = clampf(value, -1.0f, 1.0f);
    return (i16)(value * (value < 0.0f ? 32768.0f : 32767.0f));
}

static void convert_gameinput_state(const GameInputGamepadStateV0& source,
                                    XINPUT_STATE& target) {
    // GameInput gamepad button bits are intentionally mapped to the legacy
    // XInput-shaped internal state used by the rest of the bridge.
    atomic_store_u32(&g_lastGameInputButtons, source.buttons);
    if (source.buttons & 0x00000001u) target.Gamepad.wButtons |= XINPUT_START;
    if (source.buttons & 0x00000002u) target.Gamepad.wButtons |= XINPUT_BACK;
    if (source.buttons & 0x00000004u) target.Gamepad.wButtons |= XINPUT_A;
    if (source.buttons & 0x00000008u) target.Gamepad.wButtons |= XINPUT_B;
    if (source.buttons & 0x00000010u) target.Gamepad.wButtons |= XINPUT_X;
    if (source.buttons & 0x00000020u) target.Gamepad.wButtons |= XINPUT_Y;
    if (source.buttons & 0x00000040u) target.Gamepad.wButtons |= XINPUT_DPAD_UP;
    if (source.buttons & 0x00000080u) target.Gamepad.wButtons |= XINPUT_DPAD_DOWN;
    if (source.buttons & 0x00000100u) target.Gamepad.wButtons |= XINPUT_DPAD_LEFT;
    if (source.buttons & 0x00000200u) target.Gamepad.wButtons |= XINPUT_DPAD_RIGHT;
    if (source.buttons & 0x00000400u) target.Gamepad.wButtons |= XINPUT_LEFT_SHOULDER;
    if (source.buttons & 0x00000800u) target.Gamepad.wButtons |= XINPUT_RIGHT_SHOULDER;
    if (source.buttons & 0x00001000u) target.Gamepad.wButtons |= XINPUT_LEFT_THUMB;
    // Right-stick click is not consumed by the haptic mappings, but preserving
    // it in the internal/XInput-shaped trace makes controller-layout logs
    // complete and helps identify DSX button translation issues.
    if (source.buttons & 0x00002000u) target.Gamepad.wButtons |= XINPUT_RIGHT_THUMB;
    target.Gamepad.bLeftTrigger = (u8)(clampf(source.leftTrigger, 0.0f, 1.0f) * 255.0f);
    target.Gamepad.bRightTrigger = (u8)(clampf(source.rightTrigger, 0.0f, 1.0f) * 255.0f);
    target.Gamepad.sThumbLX = float_to_thumb(source.leftThumbstickX);
    target.Gamepad.sThumbLY = float_to_thumb(source.leftThumbstickY);
    target.Gamepad.sThumbRX = float_to_thumb(source.rightThumbstickX);
    target.Gamepad.sThumbRY = float_to_thumb(source.rightThumbstickY);
}

static void update_gameinput_device(void* device, u64 now) {
    if (!device) return;
    bool changed = device != g_gameInputDevice;
    if (!changed) {
        // GetDevice returns a retained reference. Drop the temporary reference;
        // the bridge keeps its own cached device reference from the first sighting.
        com_release(device);
    } else {
        if (g_gameInputDevice) com_release(g_gameInputDevice);
        g_gameInputDevice = device;
        g_gameInputRawButtonCountCached = 0;
        g_gameInputRawButtonCountValid = false;
        g_gameInputDeviceStatusTick = 0;

        GameInputDeviceGetInfoFn getInfo = api_cast<GameInputDeviceGetInfoFn>(
            com_method(g_gameInputDevice, 3));
        const GameInputDeviceInfoPrefixV0* info =
            getInfo ? getInfo(g_gameInputDevice) : nullptr;
        u16 vendor = info ? info->vendorId : 0;
        u16 product = info ? info->productId : 0;
        bool dualSense = is_dualsense_product(vendor, product);
        atomic_store_u32(&g_activeVendor, vendor);
        atomic_store_u32(&g_activeProduct, product);
        atomic_store_u32(&g_activeDualSense, dualSense ? 1u : 0u);
        // Preserve physical Sony identity for the Ubisoft-only direct fallback.
        // Do not substitute this cache into the baseline r9h rumble/render path.
        if (dualSense) {
            atomic_store_u32(&g_r9kPhysicalVendor, vendor);
            atomic_store_u32(&g_r9kPhysicalProduct, product);
            atomic_store_u32(&g_r9kPhysicalDualSense, 1u);
            atomic_store_u64(&g_r9kPhysicalDualSenseSeenTick, now);
        }
        install_gameinput_rumble_observer(g_gameInputDevice);
        atomic_fetch_add_u64(&g_perfDeviceIdentityRefreshes, 1);
    }
    g_gameInputDeviceIdentityTick = now;

    if (g_gameInputDevice &&
        (!g_gameInputDeviceStatusTick || now < g_gameInputDeviceStatusTick ||
         now - g_gameInputDeviceStatusTick >=
             (u64)g_cfg.gameInputStatusRefreshMs)) {
        GameInputDeviceGetStatusFn getStatus = api_cast<GameInputDeviceGetStatusFn>(
            com_method(g_gameInputDevice, 4));
        atomic_store_u32(&g_activeDeviceStatus,
                         getStatus ? getStatus(g_gameInputDevice) : 0u);
        g_gameInputDeviceStatusTick = now;
        atomic_fetch_add_u64(&g_perfDeviceStatusRefreshes, 1);
    }
}

static bool poll_gameinput(XINPUT_STATE& state) {
    if (!g_gameInput) return false;
    GameInputGetCurrentReadingFn getReading = api_cast<GameInputGetCurrentReadingFn>(
        com_method(g_gameInput, 4));
    if (!getReading) return false;
    void* reading = nullptr;
    // Request both the normalized Gamepad view and ControllerButton view.
    // The latter exposes the DualSense touchpad click that the normalized
    // Gamepad bitfield intentionally omits.
    static constexpr u32 kGameInputKindControllerButton = 0x00000004u;
    static constexpr u32 kGameInputKindGamepad = 0x00040000u;
    if (getReading(g_gameInput,
                   kGameInputKindGamepad | kGameInputKindControllerButton,
                   nullptr, &reading) < 0 || !reading) {
        if (getReading(g_gameInput, kGameInputKindGamepad,
                       nullptr, &reading) < 0 || !reading)
            return false;
    }

    GameInputGamepadStateV0 source{};
    GameInputReadingGetGamepadStateFn getState = api_cast<GameInputReadingGetGamepadStateFn>(
        com_method(reading, 22));
    bool ok = getState && getState(reading, &source);
    u64 now = atomic_load_u64(&g_workerNowTick);
    bool refreshIdentity = !g_gameInputDevice || !g_gameInputDeviceIdentityTick ||
        now < g_gameInputDeviceIdentityTick ||
        now - g_gameInputDeviceIdentityTick >=
            (u64)g_cfg.gameInputDeviceRefreshMs;
    if (refreshIdentity) {
        GameInputReadingGetDeviceFn getDevice = api_cast<GameInputReadingGetDeviceFn>(
            com_method(reading, 6));
        void* device = nullptr;
        if (getDevice) getDevice(reading, &device);
        if (device) update_gameinput_device(device, now);
    }

    u32 rawCount = 0;
    u64 rawMask = 0;
    bool rawTouchpad = false;
    bool allowedVendor = !g_cfg.menuRawTouchpadSonyOnly ||
        atomic_load_u32(&g_activeVendor) == 0x054Cu;
    if (g_cfg.menuRawTouchpad && allowedVendor) {
        GameInputReadingGetControllerButtonStateFn getButtonState =
            api_cast<GameInputReadingGetControllerButtonStateFn>(
                com_method(reading, 11));
        if (!g_gameInputRawButtonCountValid) {
            GameInputReadingGetControllerButtonCountFn getButtonCount =
                api_cast<GameInputReadingGetControllerButtonCountFn>(
                    com_method(reading, 10));
            g_gameInputRawButtonCountCached = getButtonCount ?
                getButtonCount(reading) : 0;
            g_gameInputRawButtonCountValid = true;
            atomic_fetch_add_u64(&g_perfRawButtonCountQueries, 1);
        }
        rawCount = g_gameInputRawButtonCountCached;
        u32 request = rawCount > 64u ? 64u : rawCount;
        u8 raw[64]{};
        u32 copied = request && getButtonState ?
            getButtonState(reading, request, raw) : 0;
        if (copied < request) request = copied;
        for (u32 i = 0; i < request; ++i)
            if (raw[i]) rawMask |= 1ull << i;
        u32 index = g_cfg.menuRawTouchpadButtonIndex;
        rawTouchpad = index < request && raw[index] != 0;
    }
    atomic_store_u32(&g_rawControllerButtonCount, rawCount);
    atomic_store_u64(&g_rawControllerButtons, rawMask);
    u32 wasRawTouchpad = atomic_load_u32(&g_rawTouchpadDown);
    atomic_store_u32(&g_rawTouchpadDown, rawTouchpad ? 1u : 0u);
    if (rawTouchpad && !wasRawTouchpad)
        atomic_fetch_add_u64(&g_rawTouchpadEdges, 1);

    if (ok) convert_gameinput_state(source, state);
    com_release(reading);
    return ok;
}

static bool set_gameinput_rumble(float low, float high) {
    if (!g_gameInputDevice) return false;
    GameInputDeviceSetRumbleFn setRumble = api_cast<GameInputDeviceSetRumbleFn>(
        com_method(g_gameInputDevice, 10));
    if (!setRumble) return false;
    GameInputRumbleParamsV0 params{};
    params.lowFrequency = clampf(low, 0.0f, 1.0f);
    params.highFrequency = clampf(high, 0.0f, 1.0f);
    u32 writerThread = g_api.GetCurrentThreadId ? g_api.GetCurrentThreadId() : 0;
    atomic_store_u32(&g_pluginGameInputWriterThread, writerThread);
    setRumble(g_gameInputDevice, &params);
    atomic_store_u32(&g_pluginGameInputWriterThread, 0);
    atomic_fetch_add_u64(&g_gameInputRumbleWrites, 1);
    return true;
}

static bool init_xinput() {
    const WCHAR* modules[] = {
        L"xinput9_1_0.dll", L"xinput1_4.dll", L"xinput1_3.dll"
    };
    for (u32 i = 0; i < 3; ++i) {
        u8* module = nullptr;
        if (i == 0) module = find_module_ascii("xinput9_1_0.dll");
        else if (i == 1) module = find_module_ascii("xinput1_4.dll");
        else module = find_module_ascii("xinput1_3.dll");
        if (!module && g_api.LoadLibraryW) module = (u8*)g_api.LoadLibraryW(modules[i]);
        if (!module) continue;
        g_xinputGetState = api_cast<XInputGetStateFn>(resolve_export(module, "XInputGetState"));
        g_xinputSetState = api_cast<XInputSetStateFn>(resolve_export(module, "XInputSetState"));
        if (g_xinputGetState) {
            if (i == 0) log_simple("Input bridge using xinput9_1_0.dll");
            else if (i == 1) log_simple("Input bridge using xinput1_4.dll");
            else log_simple("Input bridge using xinput1_3.dll");
            return true;
        }
    }
    log_simple("WARNING XInputGetState was unavailable; hotkey previews still work");
    return false;
}

static WORD rumble_to_word(float value) {
    value = clampf(value, 0.0f, 1.0f);
    return (WORD)(value * 65535.0f + 0.5f);
}

static bool set_xinput_rumble(float low, float high) {
    if (!g_xinputSetState) return false;
    XINPUT_VIBRATION vibration{};
    vibration.wLeftMotorSpeed = rumble_to_word(low);
    vibration.wRightMotorSpeed = rumble_to_word(high);
    bool wrote = g_xinputSetState(g_cfg.controllerIndex, &vibration) == ERROR_SUCCESS;
    if (wrote) atomic_fetch_add_u64(&g_xinputRumbleWrites, 1);
    return wrote;
}

static bool g_rumbleOutputWasActive{};
static u64 g_lastRumblePhysicalWriteTick{};

static void write_rumble_output(const RumbleSample& sample) {
    u64 now = atomic_load_u64(&g_workerNowTick);
    if (native_rumble_active(now)) {
        // The game's write already replaced the motor state owned by the
        // bridge.  A zero here would cancel the native effect, so relinquish
        // the device without issuing any competing write.
        g_rumbleOutputWasActive = false;
        atomic_store_u32(&g_rumbleBackend, 0);
        return;
    }
    if (native_haptic_active(now)) {
        // Native audio haptics do not necessarily overwrite a conventional
        // motor state. Clear only a motor state that this bridge previously
        // owned, once, while leaving the native haptic buffer untouched.
        if (g_rumbleOutputWasActive) {
            bool clearedGI = false;
            if (atomic_load_u32(&g_inputBackend) == 1 && g_gameInputDevice)
                clearedGI = set_gameinput_rumble(0.0f, 0.0f);
            if (!atomic_load_u32(&g_activeDualSense) || !clearedGI)
                set_xinput_rumble(0.0f, 0.0f);
        }
        g_rumbleOutputWasActive = false;
        atomic_store_u32(&g_rumbleBackend, 0);
        return;
    }
    bool shouldWrite = sample.timelineActive || g_rumbleOutputWasActive;
    if (!shouldWrite) return;

    if (atomic_load_u32(&g_activeDualSense) && !g_cfg.rumbleOnDualSense) {
        // Do not continuously write zeros: that would still fight DSX/the
        // game's own DualSense rumble layer. Only clear a state this plugin
        // previously owned, then leave the device alone.
        if (g_rumbleOutputWasActive) set_gameinput_rumble(0.0f, 0.0f);
        g_rumbleOutputWasActive = false;
        atomic_store_u32(&g_rumbleBackend, 0);
        return;
    }

    // r9g: input/state polling runs at 500 Hz, but conventional rumble motor
    // writes do not need a 2 ms command cadence. Preserve immediate start/stop
    // transitions, while steady active timelines update at the configured
    // cadence (4 ms / 250 Hz by default) to avoid hammering GameInput/XInput.
    bool transitionWrite = sample.timelineActive != g_rumbleOutputWasActive;
    if (!transitionWrite && sample.timelineActive && g_lastRumblePhysicalWriteTick &&
        now >= g_lastRumblePhysicalWriteTick &&
        now - g_lastRumblePhysicalWriteTick < (u64)g_cfg.rumbleWriteIntervalMs) {
        atomic_fetch_add_u64(&g_perfRumbleCadenceSkips, 1);
        return;
    }

    float low = sample.timelineActive ? sample.low : 0.0f;
    float high = sample.timelineActive ? sample.high : 0.0f;
    bool gameInputWrote = false;
    bool xinputWrote = false;
    u32 backend = atomic_load_u32(&g_inputBackend);
    if (backend == 1 && g_gameInputDevice) {
        gameInputWrote = set_gameinput_rumble(low, high);
    }

    // GameInput v0's SetRumbleState ABI returns void. A call can therefore
    // look successful even when a virtual Xbox 360 device ignores it. Mirror
    // the same motor state through XInput for every non-DualSense controller.
    // Native DS4 pads simply return DEVICE_NOT_CONNECTED on this route, while
    // DSX/ViGEm Xbox devices receive the write they require.
    if (!atomic_load_u32(&g_activeDualSense) || !gameInputWrote) {
        xinputWrote = set_xinput_rumble(low, high);
    }
    bool wrote = gameInputWrote || xinputWrote;
    if (xinputWrote) atomic_store_u32(&g_rumbleBackend, 2);
    else if (gameInputWrote) atomic_store_u32(&g_rumbleBackend, 1);
    if (!wrote) atomic_store_u32(&g_rumbleBackend, 0);
    else {
        g_lastRumblePhysicalWriteTick = now;
        atomic_fetch_add_u64(&g_rumbleWrites, 1);
        atomic_store_u32(&g_lastRumbleLow, rumble_to_word(low));
        atomic_store_u32(&g_lastRumbleHigh, rumble_to_word(high));
    }
    g_rumbleOutputWasActive = sample.timelineActive && wrote;
}

static bool g_sprintLatched{};

struct RBHoldState {
    bool pressPending{};
    bool heavyFired{};
    bool secondPending{};
    u64 pressTick{};
    u64 secondTick{};
    u64 nativeSerialAtPress{};
};
static RBHoldState g_rbHoldState;

static void reset_rb_hold_state() {
    g_rbHoldState = {};
}

static void process_attack_mapping(bool down, bool rising, bool falling,
                                   u64 now) {

    // Finish an already-committed two-stage heavy even if the configured
    // attack input was released after the hold threshold. Polling jitter is
    // bounded by PollMs.
    if (g_rbHoldState.secondPending && now >= g_rbHoldState.secondTick) {
        schedule_configured_effect(g_map.rbHold, g_cfg.rbHoldGain,
                                   g_cfg.fxHeavySecond, 15, now,
                                   g_cfg.attackProbeDelayMs,
                                   g_rbHoldState.nativeSerialAtPress,
                                   g_cfg.parryStartTrimMs);
        g_rbHoldState.secondPending = false;
    }

    if (rising) {
        g_rbHoldState.pressPending = true;
        g_rbHoldState.heavyFired = false;
        g_rbHoldState.pressTick = now;
        g_rbHoldState.nativeSerialAtPress =
            atomic_load_u64(&g_nativeFeedbackSerial);
    }

    if (g_rbHoldState.pressPending && down && !g_rbHoldState.heavyFired &&
        now - g_rbHoldState.pressTick >= g_cfg.rbHoldThresholdMs) {
        schedule_configured_effect(g_map.rbHold, g_cfg.rbHoldGain,
                                   g_cfg.fxHeavyFirst, 14, now,
                                   g_cfg.attackProbeDelayMs,
                                   g_rbHoldState.nativeSerialAtPress,
                                   g_cfg.parryStartTrimMs);
        g_rbHoldState.heavyFired = true;
        g_rbHoldState.secondPending = true;
        g_rbHoldState.secondTick = now + g_cfg.rbHoldSecondDelayMs;
    }

    if (falling) {
        // A release before the threshold is a normal light attack. Delaying
        // until release is what prevents a heavy hold from also playing the
        // light-attack waveform at its press edge.
        if (g_rbHoldState.pressPending && !g_rbHoldState.heavyFired)
            schedule_configured_effect(g_map.buttonRB, g_cfg.buttonRBGain,
                                       g_cfg.fxLightAttack, 6, now,
                                       g_cfg.attackProbeDelayMs,
                                       g_rbHoldState.nativeSerialAtPress);
        g_rbHoldState.pressPending = false;
        g_rbHoldState.heavyFired = false;
    }

}

static void set_movement_mode_timed(u32 mode, u64 now) {
    u32 previous = atomic_load_u32(&g_movementMode);
    if (mode == 2 && previous != 2) atomic_store_u64(&g_runStartTick, now);

    float scale = 1.0f;
    if (mode == 1) {
        scale = g_cfg.walkGain * g_cfg.fxWalk.intensityMultiplier;
    } else if (mode == 2) {
        u64 started = atomic_load_u64(&g_runStartTick);
        bool opening = started && now >= started &&
            now - started < (u64)g_cfg.runOpeningDurationMs;
        scale = (opening ? g_cfg.runOpeningGain : g_cfg.runBaseGain) *
                g_cfg.fxRun.intensityMultiplier;
    }
    if (scale < 0.0f) scale = 0.0f;
    if (scale > 16.0f) scale = 16.0f;
    atomic_store_u32(&g_movementScalePermille, (u32)(scale * 1000.0f + 0.5f));
    atomic_store_u32(&g_movementMode, mode);
}

static void set_movement_from_stick(const XINPUT_STATE& state,
                                    bool sprintRising, bool sprintDown) {
    u64 now = g_api.GetTickCount64 ? g_api.GetTickCount64() : 0;
    if (!atomic_load_u32(&g_enabled) || !atomic_load_u32(&g_inputEnabled)) {
        set_movement_mode_timed(0, now);
        g_sprintLatched = false;
        return;
    }
    WORD buttons = state.Gamepad.wButtons;
    if (g_cfg.suppressMovementWithMenuButtons && (buttons & (XINPUT_START | XINPUT_BACK))) {
        set_movement_mode_timed(0, now);
        g_sprintLatched = false;
        return;
    }
    i64 x = state.Gamepad.sThumbLX;
    i64 y = state.Gamepad.sThumbLY;
    u64 magnitudeSquared = (u64)(x * x + y * y);
    i64 run = g_cfg.runThreshold;
    i64 walk = g_cfg.walkThreshold;
    if (run < 0) run = -run;
    if (walk < 0) walk = -walk;
    if (g_cfg.sprintToggle && sprintRising)
        g_sprintLatched = !g_sprintLatched;
    if (magnitudeSquared < (u64)(walk * walk)) g_sprintLatched = false;
    bool sprinting = g_cfg.sprintToggle ? g_sprintLatched : sprintDown;
    u32 stateFlags = character_flags_for_input(now);
    if (g_cfg.semanticEvents && !(stateFlags & CS_WATER_MASK)) {
        // Ground locomotion is owned by exact Black Flag gait events in v8.
        // Raw stick magnitude no longer starts/stops footsteps or chooses walk/run.
        // State flags can still suppress a stale event-driven movement tail.
        if (stateFlags & (CS_AIR_MASK | CS_CLIMB_MASK | CS_HAYSTACK))
            set_movement_mode_timed(0, now);
        return;
    }
    if (stateFlags & (CS_AIR_MASK | CS_CLIMB_MASK | CS_HAYSTACK)) {
        // Never replay ground contacts while airborne or climbing.  This is
        // the state-aware guard that stick input alone could not provide.
        set_movement_mode_timed(0, now);
    } else if (stateFlags & CS_WATER_MASK) {
        // Surface/ordinary swim can sustain the L3 texture. Once the explicit
        // dive/lunge semantic latch says Edward is underwater, a lunge is hard
        // capped at WaterLungeSustainMs (3 s default) until another L3 lunge.
        bool fast = sprinting && magnitudeSquared >= (u64)(run * run);
        if (g_r4Underwater) {
            if (sprintRising && fast) {
                g_r4WaterLungeUntil = now + g_cfg.waterLungeSustainMs;
                atomic_store_u64(&g_runStartTick, now);
                atomic_fetch_add_u64(&g_r4WaterLungeStarts, 1);
            }
            fast = fast && g_r4WaterLungeUntil && now < g_r4WaterLungeUntil;
        }
        set_movement_mode_timed(fast ? 2u : 0u, now);
    } else if (sprinting && magnitudeSquared >= (u64)(run * run)) {
        set_movement_mode_timed(2, now);
    } else if (magnitudeSquared >= (u64)(walk * walk)) {
        set_movement_mode_timed(1, now);
    } else {
        set_movement_mode_timed(0, now);
    }
}

static bool r9c_water_lunge_arm_is_valid(u64 now) {
    if (!g_r9cWaterLungeArmTick || now < g_r9cWaterLungeArmTick) return false;
    return now - g_r9cWaterLungeArmTick <= (u64)g_cfg.waterLungeInputGateMs;
}

static void r9c_accept_water_lunge_semantic(u64 now, u64 nativeSerialBaseline) {
    // A semantic 0x7ED9038C is only allowed to become a real lunge after a
    // correlated L3/SprintLunge press. This keeps ordinary direction changes
    // underwater from manufacturing the 3-second lunge texture.
    g_r4Underwater = true;
    g_r4WaterLungeUntil = now + g_cfg.waterLungeSustainMs;
    atomic_store_u64(&g_runStartTick, now);
    set_movement_mode_timed(2, now);
    schedule_configured_effect(g_map.waterLunge, g_cfg.waterLungeGain,
                               g_cfg.fxWaterLunge, 809, now, 0,
                               nativeSerialBaseline,
                               g_cfg.stateEffectStartTrimMs,
                               g_cfg.semanticWaterLungeDurationMs);
    atomic_fetch_add_u64(&g_r4WaterLungeStarts, 1);
    atomic_fetch_add_u64(&g_r9cWaterLungeSemanticAccepted, 1);
    atomic_fetch_add_u64(&g_v8SemanticFallbacks, 1);
    g_r9cWaterLungeArmTick = 0;
    g_r9cPendingWaterLungeTick = 0;
    g_r9cPendingWaterLungeNativeSerial = 0;
}

static u64 g_stateSeen[GSE_COUNT]{};
static bool g_hadAirborne{};
static bool g_airborneConfirmedJump{};
static bool g_airborneLongFall{};
static bool g_landingPending{};
static bool g_surfacePending{};
static u64 g_airborneStartTick{};
static u64 g_lastAirborneEndTick{};
static u64 g_r5LastClimbExitTick{};
static u64 g_pendingLandingAirMs{};
static u64 g_landingDueTick{};
static u64 g_surfaceDueTick{};
static u64 g_lastJumpEffectTick{};
static u64 g_lastLandingEffectTick{};
static u64 g_lastHaystackEffectTick{};
static u64 g_lastDiveEffectTick{};
static u64 g_lastSurfaceEffectTick{};
static u64 g_lastWaterJumpEffectTick{};
static volatile u64 g_suppressedClimbDodge{};
static volatile u64 g_armedAirDive{};
static volatile u64 g_lastAirborneDurationMs{};
static volatile u64 g_maxAirborneDurationMs{};
static volatile u64 g_shortAirborneRejected{};
static volatile u64 g_groundLandingEffects{};
static volatile u64 g_haystackLandingEffects{};
static volatile u32 g_lastLandingCategory{}; // 1=short, 2=normal, 3=long, 4=haystack
static volatile u32 g_lastLandingGainPermille{};
static volatile u32 g_lastTakeoffMovementMode{};
static volatile u32 g_currentJumpInputDown{};
static volatile u32 g_lastJumpStickMagnitudeSq{};
static bool g_r6ParkourAirCycle{};
static u64 g_r6ParkourTravelMs{};
static u64 g_r6ParkourTravelPollTick{};
static volatile u64 g_r6ParkourLandings{};
static volatile u64 g_r6ParkourClimbImpacts{};
static volatile u64 g_r6LastParkourTravelMs{};
static volatile u32 g_r6LastParkourBoostPermille{};
static volatile u64 g_confirmedClimbJumps{};
static volatile u64 g_suppressedJumpWithoutInput{};
static volatile u64 g_suppressedClimbJumpDirection{};
static volatile u32 g_lastClimbJumpStickMagnitudeSq{};
static bool g_inputLandingPending{};
static u64 g_inputTakeoffTick{};
static u64 g_inputLandingDueTick{};
static volatile u64 g_inputJumpEffects{};
static volatile u64 g_waterJumpEffects{};
static volatile u64 g_inputFallbackLandings{};
static volatile u64 g_inputFallbackCancels{};
static volatile u64 g_waterLungeEffects{};
static volatile u64 g_groundDodgeEffects{};
static volatile u64 g_directionalDodgeEffects{};

static bool effect_cooldown_elapsed(u64 now, u64 last, u32 cooldownMs) {
    return !last || (now >= last && now - last >= (u64)cooldownMs);
}

static float landing_gain_for_airtime(u64 durationMs, u32& category) {
    // r9f release curve: <=0.9 s remains the soft/short region; 0.9-2.0 s
    // rises to calibrated normal 0.70; 2.0-3.0 s rises sharply to 3.15.
    // 3.15 is a literal +125% over r9e's 1.40 long-fall ceiling. Every
    // 3.0 s or longer ground/climb impact holds this explicit maximum.
    if (durationMs <= (u64)g_cfg.landingShortAirMs) {
        category = 1;
        // r5: scale even the short bucket instead of assigning every 120-900 ms
        // drop the exact same strength.
        u64 startMs = (u64)g_cfg.minimumAirborneMs;
        if (durationMs <= startMs || g_cfg.landingShortAirMs <= startMs)
            return g_cfg.landingMinimumGain;
        float t = (float)(durationMs - startMs) /
                  (float)((u64)g_cfg.landingShortAirMs - startMs);
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return g_cfg.landingMinimumGain +
            (g_cfg.landingShortGain - g_cfg.landingMinimumGain) * t;
    }
    if (durationMs < (u64)g_cfg.landingLongAirMs) {
        category = 2;
        u64 span = (u64)g_cfg.landingLongAirMs - g_cfg.landingShortAirMs;
        float t = span ? (float)(durationMs - g_cfg.landingShortAirMs) /
                         (float)span : 1.0f;
        return g_cfg.landingShortGain +
            (g_cfg.landingGain - g_cfg.landingShortGain) * t;
    }
    category = 3;
    if (durationMs >= (u64)g_cfg.landingMaxAirMs)
        return g_cfg.landingLongGain;
    u64 span = (u64)g_cfg.landingMaxAirMs - g_cfg.landingLongAirMs;
    float t = span ? (float)(durationMs - g_cfg.landingLongAirMs) /
                     (float)span : 1.0f;
    return g_cfg.landingGain +
        (g_cfg.landingLongGain - g_cfg.landingGain) * t;
}

static float parkour_distance_adjusted_gain(float airtimeGain, u64 travelMs,
                                            u32& boostPermille) {
    boostPermille = 0;
    if (!g_cfg.parkourDistanceScaling || !g_r6ParkourAirCycle || !travelMs ||
        !g_cfg.parkourDistanceReferenceMs || g_cfg.parkourDistanceMaxBoost <= 0.0f)
        return airtimeGain;
    float t = (float)travelMs / (float)g_cfg.parkourDistanceReferenceMs;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float boost = g_cfg.parkourDistanceMaxBoost * t;
    boostPermille = (u32)(boost * 1000.0f + 0.5f);
    return clampf(airtimeGain * (1.0f + boost), 0.0f, 4.0f);
}

static float r9f_landing_gain_with_distance(u64 durationMs, float airtimeGain,
                                             u64 travelMs, u32& boostPermille) {
    float gain = parkour_distance_adjusted_gain(airtimeGain, travelMs, boostPermille);
    // The user-defined long-fall value is the true ceiling. Distance can make
    // 2-3 s parkour impacts approach it sooner, but 3 s+ always lands at exactly
    // the same maximum on both DualSense and conventional XInput rumble.
    if (durationMs >= (u64)g_cfg.landingMaxAirMs) {
        boostPermille = 0;
        return g_cfg.landingLongGain;
    }
    if (durationMs >= (u64)g_cfg.landingLongAirMs && gain > g_cfg.landingLongGain) {
        gain = g_cfg.landingLongGain;
    }
    return gain;
}

static void update_parkour_distance_proxy(u64 now, u32 stickMagnitudeSq,
                                          bool jumpDown) {
    if (!g_r6ParkourTravelPollTick) g_r6ParkourTravelPollTick = now;
    u64 dt = now >= g_r6ParkourTravelPollTick ? now - g_r6ParkourTravelPollTick : 0;
    g_r6ParkourTravelPollTick = now;
    if (dt > 50) dt = 50;
    if (!g_hadAirborne || !g_r6ParkourAirCycle || !jumpDown || !dt) return;
    constexpr u64 FULL_STICK_SQ = 32767ull * 32767ull;
    u64 mag = stickMagnitudeSq;
    if (mag > FULL_STICK_SQ) mag = FULL_STICK_SQ;
    g_r6ParkourTravelMs += (dt * mag + FULL_STICK_SQ / 2ull) / FULL_STICK_SQ;
}

static void log_parkour_effect(const char* label, u64 duration, u64 travelMs,
                               float gain, u32 category, u32 boostPermille) {
    TextBuf b{};
    SYSTEMTIME st{};
    if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    tb_char(b, '['); tb_time(b, st); tb_str(b, "] STATE effect: ");
    tb_str(b, label); tb_str(b, " airMs="); tb_u64(b, duration);
    tb_str(b, " travelMs="); tb_u64(b, travelMs);
    tb_str(b, " distanceBoostP="); tb_u64(b, boostPermille);
    tb_str(b, " gain="); tb_float3(b, gain);
    tb_str(b, " category="); tb_u64(b, category); tb_endline(b);
    append_log(b.bytes, b.len);
}

static const EffectControl& landing_control_for_category(u32 category) {
    if (category == 1) return g_cfg.fxLandingShort;
    if (category == 3) return g_cfg.fxLandingLong;
    return g_cfg.fxLandingNormal;
}

static void remember_air_duration(u64 duration) {
    atomic_store_u64(&g_lastAirborneDurationMs, duration);
    atomic_extend_until(&g_maxAirborneDurationMs, duration);
}

static void begin_airborne_cycle(u64 now, u32 event) {
    if (!g_hadAirborne || !g_airborneStartTick) {
        u64 origin = now;
        if (g_r5LastClimbExitTick && now >= g_r5LastClimbExitTick &&
            now - g_r5LastClimbExitTick <= (u64)g_cfg.climbAirCarryMs) {
            origin = g_r5LastClimbExitTick;
            atomic_fetch_add_u64(&g_r5ClimbAirCarries, 1);
        }
        g_airborneStartTick = origin;
        g_airborneConfirmedJump = false;
        g_airborneLongFall = false;
        u32 magSq = atomic_load_u32(&g_currentLeftStickMagnitudeSq);
        u64 threshold = (u64)g_cfg.climbJumpStickThreshold;
        bool heldJump = atomic_load_u32(&g_currentJumpInputDown) != 0 ||
            tick_is_recent(now, atomic_load_u64(&g_lastJumpInputTick),
                           g_cfg.stateBindWindowMs);
        bool recentClimbExit = g_r5LastClimbExitTick && now >= g_r5LastClimbExitTick &&
            now - g_r5LastClimbExitTick <= (u64)g_cfg.climbAirCarryMs;
        g_r6ParkourAirCycle = heldJump &&
            (recentClimbExit || (u64)magSq >= threshold * threshold);
        g_r6ParkourTravelMs = 0;
        g_r6ParkourTravelPollTick = now;
        atomic_store_u32(&g_lastTakeoffMovementMode,
                         atomic_load_u32(&g_stateMovementAtEvent[event]));
    }
    if (event == GSE_FREEFALL || event == GSE_LEDGE_FALL_ENTER)
        g_airborneLongFall = true;
    g_hadAirborne = true;
    g_landingPending = false;
}

static void clear_airborne_cycle() {
    g_hadAirborne = false;
    g_airborneConfirmedJump = false;
    g_airborneLongFall = false;
    g_airborneStartTick = 0;
    g_landingPending = false;
    g_pendingLandingAirMs = 0;
    g_r6ParkourAirCycle = false;
    g_r6ParkourTravelMs = 0;
    g_r6ParkourTravelPollTick = 0;
}

static void arm_ground_landing(u64 now) {
    if (!g_hadAirborne) return;
    u64 duration = g_airborneStartTick && now >= g_airborneStartTick ?
        now - g_airborneStartTick : 0;
    remember_air_duration(duration);
    g_lastAirborneEndTick = now;
    if (duration >= (u64)g_cfg.minimumAirborneMs ||
        g_airborneConfirmedJump) {
        g_landingPending = true;
        g_pendingLandingAirMs = duration;
        g_landingDueTick = now + g_cfg.landingDelayMs;
    } else {
        atomic_fetch_add_u64(&g_shortAirborneRejected, 1);
        clear_airborne_cycle();
    }
}

static void log_timed_state_effect(const char* label, u64 duration,
                                   float gain, u32 category) {
    TextBuf b{};
    SYSTEMTIME st{};
    if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    tb_char(b, '['); tb_time(b, st); tb_str(b, "] STATE effect: ");
    tb_str(b, label); tb_str(b, " airMs="); tb_u64(b, duration);
    tb_str(b, " gain="); tb_float3(b, gain);
    tb_str(b, " category="); tb_u64(b, category); tb_endline(b);
    append_log(b.bytes, b.len);
}

static void cancel_input_landing_fallback(bool countCancellation) {
    if (g_inputLandingPending && countCancellation)
        atomic_fetch_add_u64(&g_inputFallbackCancels, 1);
    g_inputLandingPending = false;
    g_inputTakeoffTick = 0;
    g_inputLandingDueTick = 0;
}

static void arm_input_landing_fallback(u64 now) {
    if (!g_cfg.inputJumpFallback) return;
    g_inputLandingPending = true;
    g_inputTakeoffTick = now;
    g_inputLandingDueTick = now + g_cfg.inputLandingFallbackMs;
}

static void handle_jump_input(u64 now, u32 stickMagnitudeSq) {
    if (!g_cfg.inputJumpFallback) {
        if (!atomic_load_u32(&g_stateHooksInstalled))
            schedule_configured_effect(g_map.buttonA, 1.0f, g_cfg.fxJump,
                                       1, now, g_cfg.nativeProbeDelayMs);
        return;
    }

    u32 flags = character_flags_for_input(now);
    if (flags & CS_WATER_MASK) {
        if (effect_cooldown_elapsed(now, g_lastWaterJumpEffectTick,
                                    g_cfg.waterJumpCooldownMs)) {
            schedule_configured_effect(g_map.waterJump, g_cfg.waterJumpGain,
                                       g_cfg.fxWaterJump, 207, now,
                                       g_cfg.stateProbeDelayMs, ~0ull,
                                       g_cfg.stateEffectStartTrimMs);
            g_lastWaterJumpEffectTick = now;
            atomic_fetch_add_u64(&g_waterJumpEffects, 1);
            log_simple("INPUT effect: water jump / swim up");
        }
        cancel_input_landing_fallback(true);
        return;
    }
    if (flags & CS_HAYSTACK) return;

    if (flags & CS_CLIMB_MASK) {
        u64 threshold = (u64)g_cfg.climbJumpStickThreshold;
        if ((u64)stickMagnitudeSq < threshold * threshold) {
            atomic_fetch_add_u64(&g_suppressedClimbJumpDirection, 1);
            return;
        }
        atomic_fetch_add_u64(&g_confirmedClimbJumps, 1);
        atomic_store_u32(&g_lastClimbJumpStickMagnitudeSq, stickMagnitudeSq);
    } else if (flags & CS_AIR_MASK) {
        // If a verified Jump/InAir callback was replayed by the input-correlated
        // bind immediately before this edge, the state processor below owns
        // takeoff and exact landing timing. Do not make a second pulse.
        return;
    }

    if (effect_cooldown_elapsed(now, g_lastJumpEffectTick,
                                g_cfg.jumpCooldownMs)) {
        schedule_configured_effect(g_map.jump, g_cfg.jumpGain, g_cfg.fxJump,
                                   208, now, g_cfg.stateProbeDelayMs,
                                   ~0ull, g_cfg.stateEffectStartTrimMs);
        g_lastJumpEffectTick = now;
        atomic_fetch_add_u64(&g_inputJumpEffects, 1);
        atomic_store_u32(&g_lastTakeoffMovementMode,
                         atomic_load_u32(&g_movementMode));
        log_simple("INPUT effect: provisional land/climb jump");
    }
    arm_input_landing_fallback(now);
}

static void process_gameplay_state_signals(u64 now) {
    // r4 restores the proven Anvil gameplay-state airtime path even while the
    // Wwise semantic router is enabled. Jump/InAir/landing timing is more
    // complete than the sound-event family (notably forward/targeted jumps).
    // Semantic jump/landing IDs below are therefore context-only, avoiding
    // duplicate effects while preserving exact event routing for other actions.
    for (u32 event = 0; event < GSE_COUNT; ++event) {
        u64 sequence = atomic_load_u64(&g_stateAccepted[event]);
        if (sequence == g_stateSeen[event]) continue;
        g_stateSeen[event] = sequence;
        switch (event) {
        case GSE_AIR_ENTER:
            // A verified Edward InAir transition is strong gameplay evidence
            // that wheel control ended (for example a cannon knock-off). HELM
            // is therefore non-sticky without trusting a shared Wwise sound ID.
            if (g_cfg.semanticEvents &&
                atomic_load_u32(&g_v8RuntimeMode) == V8_MODE_HELM) {
                atomic_fetch_add_u64(&g_v8HelmReleases, 1);
                v8_enter_character_domain(now);
            }
            begin_airborne_cycle(now, event);
            break;
        case GSE_AIR_EXIT:
            arm_ground_landing(now);
            break;
        case GSE_JUMP:
            // Accepted Jump state is also safe HELM-release evidence. The
            // effect itself still requires the normal recent jump-input guard.
            if (g_cfg.semanticEvents &&
                atomic_load_u32(&g_v8RuntimeMode) == V8_MODE_HELM) {
                atomic_fetch_add_u64(&g_v8HelmReleases, 1);
                v8_enter_character_domain(now);
            }
            {
                bool jumpInput = atomic_load_u32(&g_currentJumpInputDown) != 0 ||
                    tick_is_recent(now, atomic_load_u64(&g_lastJumpInputTick),
                                   g_cfg.jumpInputWindowMs);
                u32 eventFlags = atomic_load_u32(&g_stateFlagsAtEvent[GSE_JUMP]);
                bool climbJump = (eventFlags & CS_CLIMB_MASK) != 0;
                u32 currentMagnitude =
                    atomic_load_u32(&g_currentLeftStickMagnitudeSq);
                u32 inputMagnitude = atomic_load_u32(&g_lastJumpStickMagnitudeSq);
                if (currentMagnitude > inputMagnitude) inputMagnitude = currentMagnitude;
                u64 threshold = (u64)g_cfg.climbJumpStickThreshold;
                bool directionInput = !climbJump ||
                    (u64)inputMagnitude >= threshold * threshold;
                if (!jumpInput) {
                    atomic_fetch_add_u64(&g_suppressedJumpWithoutInput, 1);
                    break;
                }
                if (!directionInput) {
                    atomic_fetch_add_u64(&g_suppressedClimbJumpDirection, 1);
                    break;
                }
                if (climbJump) {
                    atomic_fetch_add_u64(&g_confirmedClimbJumps, 1);
                    atomic_store_u32(&g_lastClimbJumpStickMagnitudeSq,
                                     inputMagnitude);
                }
            }
            begin_airborne_cycle(now, event);
            g_airborneConfirmedJump = true;
            if (!g_inputLandingPending) arm_input_landing_fallback(now);
            atomic_store_u32(&g_lastTakeoffMovementMode,
                             atomic_load_u32(&g_stateMovementAtEvent[event]));
            if (effect_cooldown_elapsed(now, g_lastJumpEffectTick,
                                        g_cfg.jumpCooldownMs)) {
                schedule_configured_effect(g_map.jump, g_cfg.jumpGain,
                                           g_cfg.fxJump, 201, now,
                                           g_cfg.stateProbeDelayMs, ~0ull,
                                           g_cfg.stateEffectStartTrimMs);
                g_lastJumpEffectTick = now;
                log_simple("STATE effect: confirmed jump");
            }
            break;
        case GSE_FREEFALL:
            begin_airborne_cycle(now, event);
            break;
        case GSE_LEDGE_FALL_ENTER:
            begin_airborne_cycle(now, event);
            break;
        case GSE_LEDGE_FALL_EXIT:
            arm_ground_landing(now);
            break;
        case GSE_TRANSITION_EXIT:
            // Counted for diagnostics, but landing timing is owned by the
            // canonical InAir exit. TransitionExit is shared by other
            // traversal transitions and is not sufficient proof of ground.
            break;
        case GSE_CLIMB_ENTER:
            // If Edward was airborne, grabbing a climb surface resolves that
            // measured air cycle instead of silently discarding it.
            if (g_hadAirborne && g_airborneStartTick && now >= g_airborneStartTick) {
                u64 duration = now - g_airborneStartTick;
                remember_air_duration(duration);
                if (duration >= (u64)g_cfg.minimumAirborneMs) {
                    u32 category = 1;
                    float gain = landing_gain_for_airtime(duration, category);
                    u32 parkourBoost = 0;
                    u64 travelMs = g_r6ParkourTravelMs;
                    gain = r9f_landing_gain_with_distance(duration, gain, travelMs, parkourBoost);
                    schedule_configured_effect(
                        g_map.landing, gain, landing_control_for_category(category),
                        214, now, g_cfg.stateProbeDelayMs, ~0ull,
                        g_cfg.stateEffectStartTrimMs);
                    g_lastLandingEffectTick = now;
                    atomic_fetch_add_u64(&g_r5ClimbCatchImpacts, 1);
                    if (g_r6ParkourAirCycle) atomic_fetch_add_u64(&g_r6ParkourClimbImpacts, 1);
                    atomic_store_u64(&g_r6LastParkourTravelMs, travelMs);
                    atomic_store_u32(&g_r6LastParkourBoostPermille, parkourBoost);
                    atomic_store_u32(&g_lastLandingCategory, category);
                    atomic_store_u32(&g_lastLandingGainPermille,
                                     (u32)(gain * 1000.0f + 0.5f));
                    if (g_r6ParkourAirCycle)
                        log_parkour_effect("parkour climb catch", duration, travelMs,
                                           gain, category, parkourBoost);
                    else
                        log_timed_state_effect("climb catch impact",
                                               duration, gain, category);
                }
            }
            clear_airborne_cycle();
            cancel_input_landing_fallback(true);
            break;
        case GSE_CLIMB_EXIT:
            // Remember the exit and let the next verified airborne state use
            // it as the start of airtime.
            g_r5LastClimbExitTick = now;
            break;
        case GSE_SWIM_ENTER:
            {
                // Swimming itself is authoritative WATER evidence. Reacquire
                // even after a stale CHARACTER/HELM/UNKNOWN classification.
                u32 previousMode = atomic_load_u32(&g_v8RuntimeMode);
                if (g_cfg.semanticEvents) {
                    if (previousMode == V8_MODE_HELM)
                        atomic_fetch_add_u64(&g_v8HelmReleases, 1);
                    if (previousMode != V8_MODE_WATER)
                        atomic_fetch_add_u64(&g_r5WaterStateReacquires, 1);
                    v8_enter_water_domain(now);
                }
                u64 waterAirMs = 0;
                if (g_airborneStartTick && now >= g_airborneStartTick)
                    waterAirMs = now - g_airborneStartTick;
                else if (g_r5LastClimbExitTick && now >= g_r5LastClimbExitTick &&
                         now - g_r5LastClimbExitTick <= (u64)g_cfg.inputLandingTimeoutMs)
                    waterAirMs = now - g_r5LastClimbExitTick;
                if (waterAirMs) remember_air_duration(waterAirMs);
                g_surfacePending = false;
                clear_airborne_cycle();
                cancel_input_landing_fallback(true);
                u32 previousStateFlags =
                    atomic_load_u32(&g_statePrevFlagsAtEvent[GSE_SWIM_ENTER]);
                bool firstWaterState = !(previousStateFlags & CS_WATER_MASK);
                if (firstWaterState &&
                    effect_cooldown_elapsed(now, g_lastDiveEffectTick,
                                            g_cfg.diveStateCooldownMs)) {
                    schedule_configured_effect(g_map.dive, g_cfg.diveGain,
                                               g_cfg.fxDive, 203, now,
                                               g_cfg.stateProbeDelayMs, ~0ull,
                                               g_cfg.stateEffectStartTrimMs);
                    g_lastDiveEffectTick = now;
                    atomic_fetch_add_u64(&g_r7AutoWaterEntryEffects, 1);
                    log_timed_state_effect("water entry / dive splash",
                                           waterAirMs, g_cfg.diveGain, 5);
                }
            }
            break;
        case GSE_SWIM_EXIT:
            g_r4Underwater = false;
            g_r4WaterLungeUntil = 0;
            g_surfacePending = true;
            g_surfaceDueTick = now + g_cfg.surfaceDelayMs;
            break;
        case GSE_SHALLOW_ENTER:
            {
                u32 previousMode = atomic_load_u32(&g_v8RuntimeMode);
                if (g_cfg.semanticEvents) {
                    if (previousMode == V8_MODE_HELM)
                        atomic_fetch_add_u64(&g_v8HelmReleases, 1);
                    if (previousMode != V8_MODE_WATER)
                        atomic_fetch_add_u64(&g_r5WaterStateReacquires, 1);
                    v8_enter_water_domain(now);
                }
                u32 previousStateFlags =
                    atomic_load_u32(&g_statePrevFlagsAtEvent[GSE_SHALLOW_ENTER]);
                bool firstWaterState = !(previousStateFlags & CS_WATER_MASK);
                if (firstWaterState &&
                    effect_cooldown_elapsed(now, g_lastDiveEffectTick,
                                            g_cfg.diveStateCooldownMs)) {
                    schedule_configured_effect(g_map.dive, g_cfg.diveGain,
                                               g_cfg.fxDive, 215, now,
                                               g_cfg.stateProbeDelayMs, ~0ull,
                                               g_cfg.stateEffectStartTrimMs);
                    g_lastDiveEffectTick = now;
                    atomic_fetch_add_u64(&g_r7AutoWaterEntryEffects, 1);
                    log_timed_state_effect("shallow/automatic water entry",
                                           0, g_cfg.diveGain, 5);
                }
            }
            g_r4Underwater = false;
            g_r4WaterLungeUntil = 0;
            // A generic-swim -> shallow-swim handoff is the best available
            // executable state for breaking the water surface.  Delay the
            // decision so sibling swim variants can settle first.
            g_surfacePending = true;
            g_surfaceDueTick = now + g_cfg.surfaceDelayMs;
            break;
        case GSE_SHALLOW_EXIT:
            g_surfacePending = true;
            g_surfaceDueTick = now + g_cfg.surfaceDelayMs;
            break;
        case GSE_HAYSTACK_ENTER:
            {
                u64 duration = g_airborneStartTick && now >= g_airborneStartTick ?
                    now - g_airborneStartTick : 0;
                if (!duration && g_landingPending) duration = g_pendingLandingAirMs;
                if (!duration && g_inputLandingPending && now >= g_inputTakeoffTick)
                    duration = now - g_inputTakeoffTick;
                if (!duration && g_r5LastClimbExitTick && now >= g_r5LastClimbExitTick &&
                    now - g_r5LastClimbExitTick <= (u64)g_cfg.inputLandingTimeoutMs)
                    duration = now - g_r5LastClimbExitTick;
                if (!duration && tick_is_recent(now, g_lastAirborneEndTick, 700))
                    duration = atomic_load_u64(&g_lastAirborneDurationMs);
                remember_air_duration(duration);
                u32 airCategory = 1;
                float landingGain = landing_gain_for_airtime(duration, airCategory);
                float reference = g_cfg.landingGain > 0.001f ?
                    g_cfg.landingGain : 1.0f;
                float gain = clampf(g_cfg.haystackGain * landingGain / reference,
                                    0.0f, 4.0f);
                clear_airborne_cycle();
                cancel_input_landing_fallback(true);
                if (effect_cooldown_elapsed(now, g_lastHaystackEffectTick,
                                            g_cfg.haystackCooldownMs)) {
                    schedule_configured_effect(g_map.haystack, gain,
                                               g_cfg.fxHaystack, 206, now,
                                               g_cfg.stateProbeDelayMs, ~0ull,
                                               g_cfg.stateEffectStartTrimMs);
                    g_lastHaystackEffectTick = now;
                    atomic_fetch_add_u64(&g_haystackLandingEffects, 1);
                    atomic_store_u32(&g_lastLandingCategory, 4);
                    atomic_store_u32(&g_lastLandingGainPermille,
                                     (u32)(gain * 1000.0f + 0.5f));
                    log_timed_state_effect("haystack / leap-of-faith landing",
                                           duration, gain, 4);
                }
            }
            break;
        case GSE_HAYSTACK_EXIT:
            break;
        default:
            break;
        }
    }

    u32 flags = character_flags_for_input(now);
    if (g_landingPending && now >= g_landingDueTick) {
        if (flags & CS_AIR_MASK) {
            // InAir and FallFromLedge can leave on adjacent game frames. Keep
            // the pending impact until both verified airborne states settle.
            g_landingDueTick = now + g_cfg.landingDelayMs;
        } else if (flags & (CS_WATER_MASK | CS_HAYSTACK)) {
            cancel_input_landing_fallback(true);
            clear_airborne_cycle();
        } else if (flags & CS_CLIMB_MASK) {
            g_landingDueTick = now + g_cfg.landingDelayMs;
        } else {
            g_landingPending = false;
            u32 landingCooldown = g_r6ParkourAirCycle ?
                g_cfg.parkourLandingCooldownMs : g_cfg.landingCooldownMs;
            if (effect_cooldown_elapsed(now, g_lastLandingEffectTick,
                                        landingCooldown)) {
                u32 category = 1;
                float gain = landing_gain_for_airtime(g_pendingLandingAirMs,
                                                       category);
                u64 travelMs = g_r6ParkourTravelMs;
                u32 parkourBoost = 0;
                bool parkour = g_r6ParkourAirCycle;
                gain = r9f_landing_gain_with_distance(g_pendingLandingAirMs, gain, travelMs, parkourBoost);
                schedule_configured_effect(
                    g_map.landing, gain, landing_control_for_category(category),
                    202, now, g_cfg.stateProbeDelayMs, ~0ull,
                    g_cfg.stateEffectStartTrimMs);
                g_lastLandingEffectTick = now;
                atomic_fetch_add_u64(&g_groundLandingEffects, 1);
                if (parkour) atomic_fetch_add_u64(&g_r6ParkourLandings, 1);
                atomic_store_u64(&g_r6LastParkourTravelMs, travelMs);
                atomic_store_u32(&g_r6LastParkourBoostPermille, parkourBoost);
                atomic_store_u32(&g_lastLandingCategory, category);
                atomic_store_u32(&g_lastLandingGainPermille,
                                 (u32)(gain * 1000.0f + 0.5f));
                if (parkour)
                    log_parkour_effect("parkour ground landing",
                                       g_pendingLandingAirMs, travelMs, gain,
                                       category, parkourBoost);
                else
                    log_timed_state_effect("confirmed ground landing",
                                           g_pendingLandingAirMs, gain, category);
            }
            cancel_input_landing_fallback(false);
            clear_airborne_cycle();
        }
    }
    if (g_inputLandingPending && now >= g_inputLandingDueTick) {
        u64 duration = now >= g_inputTakeoffTick ? now - g_inputTakeoffTick : 0;
        if ((flags & CS_AIR_MASK) && !g_airborneLongFall &&
            duration >= (u64)g_cfg.inputLandingFallbackMs) {
            // A normal jump with no accepted exit is the r3 failure mode. The
            // 2 s fallback may settle it; verified Freefall/FallFromLedge
            // cycles keep waiting for their real exit instead.
            atomic_fetch_and_u32(&g_characterStateFlags, ~CS_AIR_MASK);
            flags &= ~CS_AIR_MASK;
            atomic_fetch_add_u64(&g_staleAirFlagRecoveries, 1);
        }
        if (flags & (CS_WATER_MASK | CS_HAYSTACK)) {
            cancel_input_landing_fallback(true);
        } else if (flags & CS_CLIMB_MASK) {
            if (duration >= (u64)g_cfg.inputLandingTimeoutMs)
                cancel_input_landing_fallback(true);
            else
                g_inputLandingDueTick = now + g_cfg.landingDelayMs;
        } else if (flags & CS_AIR_MASK) {
            if (duration >= (u64)g_cfg.inputLandingTimeoutMs) {
                cancel_input_landing_fallback(true);
            } else {
                u32 retryMs = g_cfg.landingDelayMs < g_cfg.pollMs ?
                    g_cfg.pollMs : g_cfg.landingDelayMs;
                g_inputLandingDueTick = now + retryMs;
            }
        } else {
            u32 category = 1;
            float gain = landing_gain_for_airtime(duration, category);
            u64 travelMs = g_r6ParkourTravelMs;
            u32 parkourBoost = 0;
            bool parkour = g_r6ParkourAirCycle;
            gain = r9f_landing_gain_with_distance(duration, gain, travelMs, parkourBoost);
            schedule_configured_effect(
                g_map.landing, gain, landing_control_for_category(category),
                209, now, g_cfg.stateProbeDelayMs, ~0ull,
                g_cfg.stateEffectStartTrimMs);
            g_lastLandingEffectTick = now;
            atomic_fetch_add_u64(&g_inputFallbackLandings, 1);
            if (parkour) atomic_fetch_add_u64(&g_r6ParkourLandings, 1);
            atomic_store_u64(&g_r6LastParkourTravelMs, travelMs);
            atomic_store_u32(&g_r6LastParkourBoostPermille, parkourBoost);
            atomic_store_u32(&g_lastLandingCategory, category);
            atomic_store_u32(&g_lastLandingGainPermille,
                             (u32)(gain * 1000.0f + 0.5f));
            if (parkour)
                log_parkour_effect("parkour fallback landing", duration, travelMs,
                                   gain, category, parkourBoost);
            else
                log_timed_state_effect("input fallback ground landing",
                                       duration, gain, category);
            cancel_input_landing_fallback(false);
            clear_airborne_cycle();
        }
    }
    if (g_surfacePending && now >= g_surfaceDueTick) {
        g_surfacePending = false;
        // If either full Swimming variant remains active, this was an internal
        // swim-state handoff rather than a water/air surface transition.
        if (!(flags & (CS_SWIM_A | CS_SWIM_B)) &&
            effect_cooldown_elapsed(now, g_lastSurfaceEffectTick,
                                    g_cfg.surfaceCooldownMs)) {
            schedule_configured_effect(g_map.surface, g_cfg.surfaceGain,
                                       g_cfg.fxSurface, 204, now,
                                       g_cfg.stateProbeDelayMs, ~0ull,
                                       g_cfg.stateEffectStartTrimMs);
            g_lastSurfaceEffectTick = now;
            log_simple("STATE effect: soft water surface / exit");
        }
    }
}

static void process_contextual_dodge_dive(u64 now, u32 stickMagnitudeSq) {
    u32 flags = character_flags_for_input(now);
    if (atomic_load_u32(&g_stateHooksInstalled)) {
        if (flags & (CS_CLIMB_MASK | CS_HAYSTACK)) {
            atomic_fetch_add_u64(&g_suppressedClimbDodge, 1);
            return;
        }
        if (flags & CS_WATER_MASK) {
            if (effect_cooldown_elapsed(now, g_lastDiveEffectTick,
                                        g_cfg.diveInputCooldownMs)) {
                schedule_configured_effect(g_map.dive, g_cfg.diveGain,
                                           g_cfg.fxDive, 205, now,
                                           g_cfg.stateProbeDelayMs, ~0ull,
                                           g_cfg.stateEffectStartTrimMs);
                g_lastDiveEffectTick = now;
            }
            return;
        }
        if (flags & CS_AIR_MASK) {
            // The configured dodge/dive action while airborne is a drop/dive
            // request. Do not play a ground dodge; Swimming entry supplies the
            // actual splash.
            atomic_fetch_add_u64(&g_armedAirDive, 1);
            return;
        }
    }
    u64 threshold = (u64)g_cfg.dodgeStickThreshold;
    bool directional = (u64)stickMagnitudeSq >= threshold * threshold;
    u32 delay = directional ? g_cfg.directionalDodgeProbeDelayMs :
                              g_cfg.dodgeProbeDelayMs;
    u32 trimMs = directional ? g_cfg.directionalDodgeStartTrimMs :
                               g_cfg.dodgeStartTrimMs;
    const EffectControl& control = directional ?
        g_cfg.fxDirectionalDodge : g_cfg.fxDodge;
    schedule_configured_effect(g_map.buttonB, 1.0f, control, 2,
                               now, delay, ~0ull, trimMs);
    atomic_fetch_add_u64(directional ? &g_directionalDodgeEffects :
                                      &g_groundDodgeEffects, 1);
}

static volatile u32 g_menuMode{};
static volatile u64 g_menuSessions{};
static volatile u64 g_menuPulseRequests{};
static volatile u64 g_menuMovementCloses{};
static volatile u64 g_menuNativeTabArms{};
static volatile u64 g_menuRawTouchpadOpens{};
static volatile u64 g_menuShoulderRequests{};
static volatile u64 g_menuDomainSwitches{};
static volatile u64 g_reloadRecoveries{};
static u64 g_menuOpenedTick{};
static u64 g_menuLastInputTick{};
static u64 g_menuExitArmedTick{};
static u64 g_menuExitMovementTick{};
static u64 g_menuNextDirectionRepeat{};
static WORD g_menuOpener{};
static u32 g_menuDirectionLatch{};
static bool g_menuTabProbePending{};
static WORD g_menuTabProbeShoulder{};
static u64 g_menuTabProbeTick{};
static u64 g_menuTabProbeNativeSerial{};
static u64 g_menuNativeTabFirstTick{};
static u32 g_menuNativeTabMask{};
static u64 g_gameplayResumeGuardUntil{};

static void request_menu_effect(i32 effect, float gain,
                                const EffectControl& control, u32 source,
                                u64 now, u32 delayMs,
                                u64 nativeSerialBaseline = ~0ull);

static void synchronize_state_event_cursor() {
    for (u32 event = 0; event < GSE_COUNT; ++event)
        g_stateSeen[event] = atomic_load_u64(&g_stateAccepted[event]);
}

static void clear_state_candidates() {
    for (u32 event = 0; event < GSE_COUNT; ++event) {
        atomic_store_u64(&g_candidateActor[event], 0);
        atomic_store_u64(&g_candidateTick[event], 0);
        atomic_store_u64(&g_candidateSequence[event], 0);
        atomic_store_u32(&g_candidateFlag[event], 0);
        atomic_store_u32(&g_candidateSet[event], 0);
        atomic_store_u32(&g_candidateBindMask[event], 0);
    }
}

static void cancel_automatic_runtime(u64 now, bool clearPlayerBinding) {
    clear_deferred_effects();
    atomic_fetch_add_u32(&g_cancelGeneration, 1);
    set_movement_mode_timed(0, now);
    g_sprintLatched = false;
    reset_rb_hold_state();
    cancel_input_landing_fallback(false);
    clear_airborne_cycle();
    g_surfacePending = false;
    g_surfaceDueTick = 0;
    g_lastAirborneEndTick = 0;
    g_r5LastClimbExitTick = 0;
    g_lastJumpEffectTick = 0;
    g_lastLandingEffectTick = 0;
    g_lastHaystackEffectTick = 0;
    g_lastDiveEffectTick = 0;
    g_lastSurfaceEffectTick = 0;
    g_lastWaterJumpEffectTick = 0;
    atomic_store_u64(&g_lastJumpInputTick, 0);
    atomic_store_u64(&g_lastDodgeDiveInputTick, 0);
    atomic_store_u32(&g_currentJumpInputDown, 0);
    atomic_store_u32(&g_currentLeftStickMagnitudeSq, 0);
    atomic_store_u32(&g_lastJumpStickMagnitudeSq, 0);
    clear_state_candidates();
    synchronize_state_event_cursor();
    if (clearPlayerBinding) {
        atomic_store_u64(&g_playerContext, 0);
        atomic_store_u64(&g_lastAcceptedStateTick, 0);
        atomic_store_u32(&g_characterStateFlags, 0);
        for (u32 event = 0; event < GSE_COUNT; ++event) {
            atomic_store_u64(&g_stateActor[event], 0);
            atomic_store_u32(&g_stateFlagsAtEvent[event], 0);
            atomic_store_u32(&g_statePrevFlagsAtEvent[event], 0);
            atomic_store_u32(&g_stateMovementAtEvent[event], 0);
        }
    }
}

static void reset_menu_mode() {
    atomic_store_u32(&g_menuMode, 0);
    g_menuOpenedTick = 0;
    g_menuLastInputTick = 0;
    g_menuExitArmedTick = 0;
    g_menuExitMovementTick = 0;
    g_menuNextDirectionRepeat = 0;
    g_menuOpener = 0;
    g_menuDirectionLatch = 0;
    g_menuTabProbePending = false;
    g_menuTabProbeShoulder = 0;
    g_menuTabProbeTick = 0;
    g_menuTabProbeNativeSerial = 0;
    g_menuNativeTabFirstTick = 0;
    g_menuNativeTabMask = 0;
}

static void leave_menu_mode(u64 now) {
    reset_menu_mode();
    if (g_cfg.semanticEvents && atomic_load_u32(&g_v8RuntimeMode) == V8_MODE_MENU)
        atomic_store_u32(&g_v8RuntimeMode, V8_MODE_UNKNOWN);
    cancel_automatic_runtime(now, false);
    g_gameplayResumeGuardUntil = now + g_cfg.menuTransitionGuardMs;
    atomic_fetch_add_u64(&g_menuDomainSwitches, 1);
}

static void enter_menu_mode(u64 now, WORD opener, bool playOpenEffect,
                            bool nativeTabArm) {
    // Cancel gameplay movement/one-shots before the first menu command is
    // queued. Generation-stamped command rings prevent even a deferred command
    // from crossing this boundary on the audio or rumble thread.
    cancel_automatic_runtime(now, false);
    if (g_cfg.semanticEvents) {
        atomic_store_u32(&g_v8RuntimeMode, V8_MODE_MENU);
        atomic_store_u64(&g_v8LastGaitTick, 0);
        atomic_store_u64(&g_v8PreviousFastGaitTick, 0);
        atomic_store_u64(&g_v8SemanticAirStartTick, 0);
    }
    atomic_store_u32(&g_menuMode, 1);
    atomic_fetch_add_u64(&g_menuSessions, 1);
    atomic_fetch_add_u64(&g_menuDomainSwitches, 1);
    if (nativeTabArm) atomic_fetch_add_u64(&g_menuNativeTabArms, 1);
    g_menuOpenedTick = now;
    g_menuLastInputTick = now;
    g_menuOpener = opener;
    g_menuExitArmedTick = 0;
    g_menuExitMovementTick = 0;
    g_menuNextDirectionRepeat = 0;
    g_menuDirectionLatch = 0;
    g_menuTabProbePending = false;
    g_menuNativeTabFirstTick = 0;
    g_menuNativeTabMask = 0;
    if (playOpenEffect)
        request_menu_effect(g_map.menuOpen, g_cfg.menuOpenGain,
                            g_cfg.fxMenuOpen, 301, now,
                            g_cfg.menuProbeDelayMs);
}

static void request_menu_effect(i32 effect, float gain,
                                const EffectControl& control, u32 source,
                                u64 now, u32 delayMs,
                                u64 nativeSerialBaseline) {
    if (!g_cfg.menuFeedback) return;
    atomic_fetch_add_u64(&g_menuPulseRequests, 1);
    schedule_configured_effect(effect, gain, control, source, now, delayMs,
                               nativeSerialBaseline);
}

static u32 menu_direction(const XINPUT_STATE& state) {
    u32 direction = 0;
    i32 threshold = g_cfg.menuStickThreshold;
    if ((state.Gamepad.wButtons & XINPUT_DPAD_UP) || state.Gamepad.sThumbLY >= threshold)
        direction |= 1u;
    if ((state.Gamepad.wButtons & XINPUT_DPAD_DOWN) || state.Gamepad.sThumbLY <= -threshold)
        direction |= 2u;
    if ((state.Gamepad.wButtons & XINPUT_DPAD_LEFT) || state.Gamepad.sThumbLX <= -threshold)
        direction |= 4u;
    if ((state.Gamepad.wButtons & XINPUT_DPAD_RIGHT) || state.Gamepad.sThumbLX >= threshold)
        direction |= 8u;
    return direction;
}

static bool process_menu_input(const XINPUT_STATE& current, WORD rising,
                               bool rawTouchpadRising, u64 now) {
    WORD openerEdge = (WORD)(rising & (XINPUT_START | XINPUT_BACK));
    bool active = atomic_load_u32(&g_menuMode) != 0;

    if (rawTouchpadRising) {
        if (!active) {
            enter_menu_mode(now, 0, true, false);
            atomic_fetch_add_u64(&g_menuRawTouchpadOpens, 1);
        } else {
            // Touchpad may switch back to World while already inside the
            // Animus tabs. Assert the menu domain instead of treating it as a
            // gameplay toggle or a close request.
            g_menuLastInputTick = now;
            g_menuExitArmedTick = 0;
            g_menuExitMovementTick = 0;
        }
        return true;
    }

    if (!active && g_cfg.menuNativeTabArm &&
        (!g_cfg.semanticEvents || !g_cfg.semanticMenu)) {
        u64 nativeSerial = atomic_load_u64(&g_nativeFeedbackSerial);
        if (g_menuTabProbePending) {
            if (nativeSerial != g_menuTabProbeNativeSerial) {
                if (!g_menuNativeTabFirstTick ||
                    now - g_menuNativeTabFirstTick >
                        (u64)g_cfg.menuNativeTabWindowMs) {
                    g_menuNativeTabFirstTick = now;
                    g_menuNativeTabMask = 0;
                }
                if (g_menuTabProbeShoulder & XINPUT_LEFT_SHOULDER)
                    g_menuNativeTabMask |= 1u;
                if (g_menuTabProbeShoulder & XINPUT_RIGHT_SHOULDER)
                    g_menuNativeTabMask |= 2u;
                g_menuTabProbePending = false;
            } else if (now - g_menuTabProbeTick >=
                       (u64)g_cfg.menuNativeTabProbeMs) {
                g_menuTabProbePending = false;
            }
        }
        if (g_menuNativeTabFirstTick &&
            now - g_menuNativeTabFirstTick >
                (u64)g_cfg.menuNativeTabWindowMs) {
            g_menuNativeTabFirstTick = 0;
            g_menuNativeTabMask = 0;
        }
        if (g_menuNativeTabMask == 3u) {
            // Native L1 and R1 pulses are the reliable observable signature
            // available when a DualSense touchpad click is not surfaced by
            // GameInput. They arm the Inventory/World/Codex menu without
            // replacing either native tab pulse.
            enter_menu_mode(now, 0, false, true);
            return true;
        }

        WORD gameplayEdge = (WORD)(rising &
            (XINPUT_A | XINPUT_B | XINPUT_X | XINPUT_Y |
             XINPUT_LEFT_THUMB));
        if (gameplayEdge) {
            g_menuTabProbePending = false;
            g_menuNativeTabFirstTick = 0;
            g_menuNativeTabMask = 0;
        }
        WORD shoulderEdge = (WORD)(rising &
            (XINPUT_LEFT_SHOULDER | XINPUT_RIGHT_SHOULDER));
        i64 x = current.Gamepad.sThumbLX;
        i64 y = current.Gamepad.sThumbLY;
        u64 magnitudeSquared = (u64)(x * x + y * y);
        i64 walk = g_cfg.walkThreshold < 0 ? -g_cfg.walkThreshold :
                                             g_cfg.walkThreshold;
        u32 flags = character_flags_for_input(now);
        if (shoulderEdge && magnitudeSquared < (u64)(walk * walk) &&
            !(flags & CS_NON_GROUND_MASK)) {
            g_menuTabProbePending = true;
            g_menuTabProbeShoulder = shoulderEdge;
            g_menuTabProbeTick = now;
            g_menuTabProbeNativeSerial = nativeSerial;
        }
    }

    if (openerEdge && !active) {
        enter_menu_mode(now, openerEdge, true, false);
        return true;
    }

    if (!active) return false;

    // Do not close on arbitrary locomotion callbacks. r3 did that and could
    // drop out of menu inference after two navigation presses, allowing the
    // third D-pad Left press to leak into the long sheathe waveform.
    if (g_menuLastInputTick && now >= g_menuLastInputTick &&
        now - g_menuLastInputTick >= (u64)g_cfg.menuTimeoutMs) {
        leave_menu_mode(now);
        return true;
    }

    if (openerEdge && now >= g_menuOpenedTick + 250u &&
        (!g_menuOpener || (openerEdge & g_menuOpener))) {
        // Both the Options/System menu and the map shortcut accept their own
        // opener as a close action. Consume this edge so it cannot become a
        // gameplay proxy in the same polling frame.
        leave_menu_mode(now);
        return true;
    }

    // r9g performance fast path: when exact Wwise semantic menu routing is
    // active, Black Flag's own menu events already identify direction/box
    // changes and refresh the menu session. Do not rescan the analog stick,
    // D-pad, face buttons or movement-close heuristic at 500 Hz. L1/R1 stays
    // input-owned because it has a deliberate menu-specific fallback texture.
    if (g_cfg.semanticEvents && g_cfg.semanticMenu) {
        WORD shoulderEdge = (WORD)(rising &
            (XINPUT_LEFT_SHOULDER | XINPUT_RIGHT_SHOULDER));
        if (shoulderEdge) {
            request_menu_effect(g_map.parry, g_cfg.menuShoulderGain,
                                g_cfg.fxMenuShoulder, 303, now,
                                g_cfg.menuShoulderProbeDelayMs);
            atomic_fetch_add_u64(&g_menuShoulderRequests, 1);
            g_menuLastInputTick = now;
        }
        set_movement_mode_timed(0, now);
        g_sprintLatched = false;
        reset_rb_hold_state();
        return true;
    }

    if (rising & XINPUT_B) {
        g_menuExitArmedTick = now;
        g_menuExitMovementTick = 0;
    }

    i64 x = current.Gamepad.sThumbLX;
    i64 y = current.Gamepad.sThumbLY;
    u64 magnitudeSquared = (u64)(x * x + y * y);
    i64 walk = g_cfg.walkThreshold;
    if (walk < 0) walk = -walk;
    if ((!g_cfg.semanticEvents || !g_cfg.semanticMenu) && g_menuExitArmedTick &&
        magnitudeSquared >= (u64)(walk * walk)) {
        // Circle may only step back one level, so movement remains the close
        // confirmation.  Start timing it immediately: the old fixed 100 ms
        // pre-delay plus this confirmation window made a genuine menu exit
        // feel stale and let the inferred domain trail the UI unnecessarily.
        if (!g_menuExitMovementTick) g_menuExitMovementTick = now;
        if (now - g_menuExitMovementTick >= (u64)g_cfg.menuCloseMovementMs) {
            leave_menu_mode(now);
            atomic_fetch_add_u64(&g_menuMovementCloses, 1);
            return true;
        }
    } else if (magnitudeSquared < (u64)(walk * walk)) {
        g_menuExitMovementTick = 0;
    }

    u32 direction = menu_direction(current);
    bool directionPulse = false;
    if (direction != g_menuDirectionLatch) {
        directionPulse = direction != 0;
        g_menuDirectionLatch = direction;
        g_menuNextDirectionRepeat = direction ?
            now + g_cfg.menuRepeatDelayMs : 0;
    } else if (direction && g_menuNextDirectionRepeat && now >= g_menuNextDirectionRepeat) {
        directionPulse = true;
        g_menuNextDirectionRepeat = now + g_cfg.menuRepeatIntervalMs;
    }

    WORD navigationButtons = (WORD)(rising &
        (XINPUT_A | XINPUT_B | XINPUT_X | XINPUT_Y));
    if (directionPulse || navigationButtons) {
        // v8: exact Wwise menu events own ordinary navigation haptics.
        // The input path only keeps the menu session alive. This prevents the
        // old D-pad/stick heuristic from double-firing native menu ticks.
        if (!g_cfg.semanticEvents || !g_cfg.semanticMenu)
            request_menu_effect(g_map.menuTick, g_cfg.menuTickGain,
                                g_cfg.fxMenuTick, 302, now,
                                g_cfg.menuProbeDelayMs);
        g_menuLastInputTick = now;
    }

    WORD shoulderEdge = (WORD)(rising &
        (XINPUT_LEFT_SHOULDER | XINPUT_RIGHT_SHOULDER));
    if (shoulderEdge) {
        // Menu-only L1/R1 uses the parry texture at 70% of the calibrated
        // gameplay-parry amplitude (0.45 * 0.70 = 0.315). Native UI output is
        // still given first refusal during the short probe window.
        request_menu_effect(g_map.parry, g_cfg.menuShoulderGain,
                            g_cfg.fxMenuShoulder, 303, now,
                            g_cfg.menuShoulderProbeDelayMs);
        atomic_fetch_add_u64(&g_menuShoulderRequests, 1);
        g_menuLastInputTick = now;
    }

    set_movement_mode_timed(0, now);
    g_sprintLatched = false;
    reset_rb_hold_state();
    return true;
}

static void process_controller_edges(const XINPUT_STATE& current,
                                     const XINPUT_STATE& previous,
                                     bool havePrevious,
                                     bool rawTouchpadRising) {
    WORD oldButtons = havePrevious ? previous.Gamepad.wButtons : 0;
    WORD rising = (WORD)(current.Gamepad.wButtons & ~oldButtons);
    u64 now = g_api.GetTickCount64 ? g_api.GetTickCount64() :
              atomic_load_u64(&g_workerNowTick);
    i64 stickX = current.Gamepad.sThumbLX;
    i64 stickY = current.Gamepad.sThumbLY;
    u32 stickMagnitudeSq = (u32)(stickX * stickX + stickY * stickY);

    bool jumpDown = input_binding_down(g_cfg.jumpBinding, current);
    bool jumpRising = input_binding_rising(
        g_cfg.jumpBinding, current, previous, havePrevious);
    bool dodgeDiveRising = input_binding_rising(
        g_cfg.dodgeDiveBinding, current, previous, havePrevious);
    bool parryRising = input_binding_rising(
        g_cfg.parryBinding, current, previous, havePrevious);
    bool attackDown = input_binding_down(g_cfg.attackBinding, current);
    bool attackRising = input_binding_rising(
        g_cfg.attackBinding, current, previous, havePrevious);
    bool attackFalling = input_binding_falling(
        g_cfg.attackBinding, current, previous, havePrevious);
    bool sprintDown = input_binding_down(g_cfg.sprintLungeBinding, current);
    bool sprintRising = input_binding_rising(
        g_cfg.sprintLungeBinding, current, previous, havePrevious);
    bool sheatheHoodRising = input_binding_rising(
        g_cfg.sheatheHoodBinding, current, previous, havePrevious);

    // r4 tracks the explicit L3 land-run intent separately from the gait
    // sound events. This is only a run-mode discriminator; walk/jog still use
    // real Black Flag gait events.
    u32 r4Mode = atomic_load_u32(&g_v8RuntimeMode);
    i64 r4RunThreshold = g_cfg.runThreshold < 0 ? -g_cfg.runThreshold : g_cfg.runThreshold;
    bool r4MovingFast = (u64)stickMagnitudeSq >= (u64)r4RunThreshold * (u64)r4RunThreshold;

    if (!atomic_load_u32(&g_enabled) || !atomic_load_u32(&g_inputEnabled)) {
        set_movement_mode_timed(0, now);
        reset_rb_hold_state();
        return;
    }
    if (process_menu_input(current, rising, rawTouchpadRising, now)) return;
    if (now < g_gameplayResumeGuardUntil) {
        set_movement_mode_timed(0, now);
        reset_rb_hold_state();
        return;
    }
    if (g_cfg.semanticEvents &&
        atomic_load_u32(&g_v8RuntimeMode) == V8_MODE_HELM) {
        // At the wheel, character/button heuristics must never leak into the
        // native naval haptic domain. Real cannon/ship Motion remains untouched.
        set_movement_mode_timed(0, now);
        reset_rb_hold_state();
        return;
    }
    atomic_store_u32(&g_currentJumpInputDown, jumpDown ? 1u : 0u);
    atomic_store_u32(&g_currentLeftStickMagnitudeSq, stickMagnitudeSq);
    if (jumpRising)
        atomic_store_u32(&g_lastJumpStickMagnitudeSq, stickMagnitudeSq);

    // r9c: only an actual configured SprintLunge/L3 rising edge can authorize
    // the underwater lunge semantic event. The Wwise event itself is also
    // emitted by ordinary underwater direction changes, so it is correlation
    // evidence, not proof of an L3 press.
    if (sprintRising) {
        // r9g: the r9c lunge gate only needs water-state correlation on the
        // actual L3 rising edge. Avoid even the small state-flag/stale-air
        // bookkeeping cost on every ordinary left-stick movement sample.
        u32 r9cWaterFlags = character_flags_for_input(now);
        bool r9cWaterContext = r4Mode == V8_MODE_WATER ||
            (r9cWaterFlags & CS_WATER_MASK);
        if (r9cWaterContext) {
            g_r9cWaterLungeArmTick = now;
            atomic_fetch_add_u64(&g_r9cWaterLungeInputArms, 1);

            // The worker drains semantic events just before it polls the pad. If a
            // true lunge event landed a few milliseconds before this L3 edge,
            // accept that one pending event rather than losing the real lunge.
            if (g_r9cPendingWaterLungeTick &&
                now >= g_r9cPendingWaterLungeTick &&
                now - g_r9cPendingWaterLungeTick <=
                    (u64)R9C_WATER_LUNGE_PREINPUT_TOLERANCE_MS) {
                u64 baseline = g_r9cPendingWaterLungeNativeSerial;
                r9c_accept_water_lunge_semantic(now, baseline);
            }
        }
    }

    // A held Cross/A is a continuous automatic-parkour intent in Black Flag.
    // Keep the correlation window alive and adopt any matching callback that
    // arrived immediately before this controller poll.
    if (jumpDown) {
        atomic_store_u64(&g_lastJumpInputTick, now);
        if (!jumpRising) try_bind_recent_state_candidate(now, BIND_JUMP);
    }
    update_parkour_distance_proxy(now, stickMagnitudeSq, jumpDown);
    // v8-r4: real gait events own walk/jog; L3 run uses a bounded continuous texture. Keep raw-stick movement
    // only for unresolved continuous WATER texture. This removes the inherited
    // 700 ms post-stop character tail and prevents a steering stick from
    // manufacturing ground movement feedback.
    if (!g_cfg.semanticEvents || r4Mode == V8_MODE_WATER) {
        set_movement_from_stick(current, sprintRising, sprintDown);
    } else {
        i64 r4WalkThreshold = g_cfg.walkThreshold < 0 ? -g_cfg.walkThreshold : g_cfg.walkThreshold;
        bool moving = (u64)stickMagnitudeSq >= (u64)r4WalkThreshold * (u64)r4WalkThreshold;
        if (!moving) {
            g_r4LandSprintActive = false;
            atomic_store_u32(&g_r9LandSprintRumbleStepOnlyActive, 0);
            g_sprintLatched = false;
            set_movement_mode_timed(0, now);
        } else if (r4Mode == V8_MODE_CHARACTER && sprintRising && r4MovingFast) {
            g_r4LandSprintActive = true;
            atomic_store_u32(&g_r9LandSprintRumbleStepOnlyActive, 1);
            // Seed the gait safety timer from the L3 press itself so a stale
            // pre-run walk event cannot cancel the new sprint on the next poll.
            atomic_store_u64(&g_v8LastGaitTick, now);
            atomic_store_u64(&g_runStartTick, now);
            set_movement_mode_timed(2, now);
            atomic_fetch_add_u64(&g_r4RunStarts, 1);
            log_simple("R4 run: L3 land sprint opening started (1.5 s high -> sustained)");
        } else if (!g_r4LandSprintActive) {
            set_movement_mode_timed(0, now);
        }
    }
    // r9b also arms from the configured R1/L1 edges. This covers combo-stage
    // sword Motion that can produce native Quad feedback without one of the
    // mapped first-stage Wwise IDs. The semantic event, when present, simply
    // refreshes the same short window.
    if (g_cfg.nativeCombatBoost && r4Mode == V8_MODE_CHARACTER &&
        (attackRising || parryRising)) {
        atomic_extend_until(&g_nativeCombatBoostUntil,
                            now + g_cfg.nativeCombatBoostWindowMs);
        atomic_store_u32(&g_nativeCombatBoostLastEvent,
                         attackRising ? EVT_LIGHT_ATTACK : EVT_NORMAL_PARRY);
        atomic_fetch_add_u64(&g_nativeCombatBoostArms, 1);
    }
    if (!g_cfg.semanticEvents) {
        process_attack_mapping(attackDown, attackRising, attackFalling, now);
    } else {
        if (attackRising && r4Mode == V8_MODE_CHARACTER) {
            g_r4AttackPressActive = true;
            u64 recentHeavy = atomic_load_u64(&g_r5LastSemanticHeavyTick);
            g_r5AttackHeavyCommitted = recentHeavy && now >= recentHeavy &&
                now - recentHeavy <= 120;
            g_r4AttackPressTick = now;
            g_r4AttackPressNativeSerial = atomic_load_u64(&g_nativeFeedbackSerial);
        }
        // Restore v7-style heavy timing: commit while R1 is still held.
        if (g_r4AttackPressActive && !g_r5AttackHeavyCommitted &&
            attackDown && r4Mode == V8_MODE_CHARACTER) {
            u64 held = now >= g_r4AttackPressTick ? now - g_r4AttackPressTick : 0;
            if (held >= (u64)g_cfg.rbHoldThresholdMs) {
                u64 semHeavy = atomic_load_u64(&g_r5LastSemanticHeavyTick);
                bool semanticHeavyOwned = semHeavy && semHeavy <= now + 40 &&
                    (semHeavy >= g_r4AttackPressTick ||
                     g_r4AttackPressTick - semHeavy <= 120);
                if (!semanticHeavyOwned) {
                    r4_schedule_heavy_phases(now, g_r4AttackPressNativeSerial, 830);
                    atomic_fetch_add_u64(&g_r4RawHeavyFallbacks, 1);
                }
                g_r5AttackHeavyCommitted = true;
                atomic_fetch_add_u64(&g_r5HeavyHoldCommits, 1);
            }
        }
        if (attackFalling && g_r4AttackPressActive) {
            u64 sem = atomic_load_u64(&g_r4LastSemanticAttackTick);
            bool semanticOwned = sem && sem <= now + 40 &&
                (sem >= g_r4AttackPressTick || g_r4AttackPressTick - sem <= 120);
            if (!g_r5AttackHeavyCommitted && !semanticOwned &&
                r4Mode == V8_MODE_CHARACTER) {
                schedule_configured_effect(g_map.light, 1.0f, g_cfg.fxLightAttack,
                                           829, now, g_cfg.semanticAttackProbeMs,
                                           g_r4AttackPressNativeSerial, 0,
                                           g_cfg.semanticLightAttackDurationMs);
                atomic_fetch_add_u64(&g_r4RawLightFallbacks, 1);
            }
            g_r4AttackPressActive = false;
            g_r5AttackHeavyCommitted = false;
        }
    }
    if (jumpRising) {
        try_bind_recent_state_candidate(now, BIND_JUMP);
        u32 flags = character_flags_for_input(now);
        // Ground jump/landing is semantic in v8. Keep input only for unresolved
        // climb-jump and swim-up contexts.
        if (!g_cfg.semanticEvents || (flags & (CS_CLIMB_MASK | CS_WATER_MASK)))
            handle_jump_input(now, stickMagnitudeSq);
    }
    if (dodgeDiveRising) {
        atomic_store_u64(&g_lastDodgeDiveInputTick, now);
        try_bind_recent_state_candidate(now, BIND_DODGE_DIVE);
        u32 flags = character_flags_for_input(now);
        // Land dodge is exact-event driven. Water/air dive remains an input
        // fallback until its event family receives another dedicated run.
        if (!g_cfg.semanticEvents || (flags & (CS_WATER_MASK | CS_AIR_MASK)))
            process_contextual_dodge_dive(now, stickMagnitudeSq);
    }
    if (!g_cfg.semanticEvents && (rising & XINPUT_X))
        schedule_arbitrated_effect(g_map.buttonX, 1.0f, 3,
                                   now, g_cfg.nativeProbeDelayMs);
    if (!g_cfg.semanticEvents && (rising & XINPUT_Y))
        schedule_arbitrated_effect(g_map.buttonY, 1.0f, 4,
                                   now, g_cfg.nativeProbeDelayMs);
    if (!g_cfg.semanticEvents && parryRising)
        schedule_configured_effect(g_map.buttonLB, g_cfg.buttonLBGain,
                                   g_cfg.fxParry, 5, now,
                                   g_cfg.parryProbeDelayMs, ~0ull,
                                   g_cfg.parryStartTrimMs);
    if (rising & XINPUT_DPAD_UP)
        schedule_arbitrated_effect(g_map.dpadUp, 1.0f, 7, now, 0);
    if (rising & XINPUT_DPAD_DOWN)
        schedule_arbitrated_effect(g_map.dpadDown, 1.0f, 8, now, 0);
    if (!g_cfg.semanticEvents && sheatheHoodRising)
        schedule_configured_effect(g_map.dpadLeft, 1.0f,
                                   g_cfg.fxSheatheHood, 9, now,
                                   g_cfg.dpadLeftProbeDelayMs);
    if (rising & XINPUT_DPAD_RIGHT)
        schedule_arbitrated_effect(g_map.dpadRight, 1.0f, 10, now, 0);

    // Add a light impact only when the configured sprint/lunge action starts
    // (rather than stops) the sprint latch. It overlays the sustained run
    // texture and also gives swimming acceleration the combined kick.
    bool sprintStarted = sprintRising &&
        atomic_load_u32(&g_movementMode) == 2 &&
        (g_cfg.sprintToggle ? g_sprintLatched : sprintDown);
    if (sprintStarted && !g_cfg.semanticEvents) {
        u32 flags = character_flags_for_input(now);
        if (flags & CS_WATER_MASK) {
            schedule_configured_effect(
                g_map.waterLunge, g_cfg.waterLungeGain,
                g_cfg.fxWaterLunge, 210, now, g_cfg.stateProbeDelayMs,
                ~0ull, g_cfg.stateEffectStartTrimMs);
            atomic_fetch_add_u64(&g_waterLungeEffects, 1);
            log_simple("INPUT effect: configured water lunge + fast-swim texture");
        } else {
            schedule_configured_effect(
                g_map.sprintKick, g_cfg.sprintKickGain,
                g_cfg.fxSprintKick, SOURCE_SPRINT_KICK, now,
                g_cfg.dodgeProbeDelayMs,
                ~0ull, g_cfg.parryStartTrimMs);
        }
    }

    u8 oldLT = havePrevious ? previous.Gamepad.bLeftTrigger : 0;
    u8 oldRT = havePrevious ? previous.Gamepad.bRightTrigger : 0;
    if (current.Gamepad.bLeftTrigger >= (u8)g_cfg.triggerThreshold &&
        oldLT < (u8)g_cfg.triggerThreshold)
        schedule_arbitrated_effect(g_map.leftTrigger, 1.0f, 11,
                                   now, g_cfg.nativeProbeDelayMs);
    if (current.Gamepad.bRightTrigger >= (u8)g_cfg.triggerThreshold &&
        oldRT < (u8)g_cfg.triggerThreshold)
        schedule_arbitrated_effect(g_map.rightTrigger, 1.0f, 12,
                                   now, g_cfg.nativeProbeDelayMs);
}


static const char* v8_mode_name(u32 mode) {
    switch (mode) {
    case V8_MODE_MENU: return "MENU";
    case V8_MODE_CHARACTER: return "CHARACTER";
    case V8_MODE_WATER: return "WATER";
    case V8_MODE_HELM: return "HELM";
    default: return "UNKNOWN";
    }
}

static void v8_set_mode(u32 mode, u64 now) {
    u32 old = atomic_load_u32(&g_v8RuntimeMode);
    if (old == mode) return;
    atomic_store_u32(&g_v8RuntimeMode, mode);
    if (mode == V8_MODE_HELM) {
        set_movement_mode_timed(0, now);
        clear_deferred_effects();
        // A stale character jump must never survive taking the wheel.
        atomic_store_u64(&g_v8SemanticAirStartTick, 0);
    }
}

static void v8_note_character_object(u64 gameObject) {
    if (gameObject) atomic_store_u64(&g_v8CharacterObject, gameObject);
}

static bool v8_character_transition_allowed(const V8SemanticEvent& ev,
                                            bool strongCharacterEvidence) {
    if (atomic_load_u32(&g_v8RuntimeMode) != V8_MODE_HELM)
        return true;

    // HELM is not sticky. A real gait or an action which Edward cannot perform
    // while steering is enough to release it immediately. Ambiguous shared
    // event IDs (jump/landing) need Edward's learned Wwise object affinity.
    if (strongCharacterEvidence) {
        atomic_fetch_add_u64(&g_v8HelmReleases, 1);
        return true;
    }
    u64 characterObject = atomic_load_u64(&g_v8CharacterObject);
    if (ev.gameObject && characterObject && ev.gameObject == characterObject) {
        atomic_fetch_add_u64(&g_v8HelmReleases, 1);
        return true;
    }
    atomic_fetch_add_u64(&g_v8HelmAmbiguousIgnored, 1);
    return false;
}

static bool v8_enter_character_from_event(const V8SemanticEvent& ev, u64 now,
                                          bool strongCharacterEvidence) {
    if (!v8_character_transition_allowed(ev, strongCharacterEvidence))
        return false;
    v8_note_character_object(ev.gameObject);
    if (g_cfg.semanticMenu && atomic_load_u32(&g_menuMode))
        leave_menu_mode(now);
    v8_set_mode(V8_MODE_CHARACTER, now);
    atomic_store_u64(&g_v8LastGameplayEventTick, now);
    return true;
}

static void v8_enter_character_domain(u64 now) {
    if (g_cfg.semanticMenu && atomic_load_u32(&g_menuMode))
        leave_menu_mode(now);
    v8_set_mode(V8_MODE_CHARACTER, now);
    atomic_store_u64(&g_v8LastGameplayEventTick, now);
}

static void v8_enter_water_domain(u64 now) {
    r8_reset_foot_phase();
    if (g_cfg.semanticMenu && atomic_load_u32(&g_menuMode))
        leave_menu_mode(now);
    v8_set_mode(V8_MODE_WATER, now);
    atomic_store_u64(&g_v8LastGameplayEventTick, now);
    atomic_store_u64(&g_v8SemanticAirStartTick, 0);
}

static void v8_enter_menu_domain(u64 now) {
    r8_reset_foot_phase();
    if (!g_cfg.semanticMenu) return;
    if (!atomic_load_u32(&g_menuMode))
        enter_menu_mode(now, 0, false, false);
    else
        g_menuLastInputTick = now;
    v8_set_mode(V8_MODE_MENU, now);
    atomic_store_u64(&g_v8LastMenuEventTick, now);
    atomic_store_u64(&g_v8SemanticAirStartTick, 0);
    atomic_fetch_add_u64(&g_v8SemanticMenuCount, 1);
}

static void r8_reset_foot_phase() {
    g_r8NextFootRight = true;
    g_r8LastGaitKind = 0;
    g_r8LastFootstepTick = 0;
    atomic_fetch_add_u64(&g_r8FootPhaseResets, 1);
}

static bool r8_take_right_foot(u32 gaitKind, u64 now) {
    bool reset = !g_r8LastFootstepTick || !g_r8LastGaitKind ||
        gaitKind != g_r8LastGaitKind ||
        (now >= g_r8LastFootstepTick &&
         now - g_r8LastFootstepTick > (u64)g_cfg.footstepResetAfterMs);
    if (reset) {
        g_r8NextFootRight = true;
        atomic_fetch_add_u64(&g_r8FootPhaseResets, 1);
    }
    bool right = g_r8NextFootRight;
    g_r8NextFootRight = !g_r8NextFootRight;
    g_r8LastGaitKind = gaitKind;
    g_r8LastFootstepTick = now;
    atomic_fetch_add_u64(right ? &g_r8RightFootContacts : &g_r8LeftFootContacts, 1);
    return right;
}

static void v8_schedule_gait_pulse(const V8SemanticEvent& ev, bool walk,
                                   u64 now) {
    i32 effect = walk ? g_map.walk : g_map.run;
    const EffectControl& control = walk ? g_cfg.fxWalk : g_cfg.fxRun;
    if (!control.enabled || effect < 0 || (u32)effect >= g_effectCount)
        return;

    bool rightFoot = r8_take_right_foot(walk ? 1u : 2u, now);
    float leftSpatial = 1.0f;
    float rightSpatial = 1.0f;
    if (g_cfg.spatialFootsteps) {
        if (rightFoot) {
            leftSpatial = g_cfg.rightFootLeftGain;
            rightSpatial = g_cfg.rightFootRightGain;
        } else {
            leftSpatial = g_cfg.leftFootLeftGain;
            rightSpatial = g_cfg.leftFootRightGain;
        }
    }

    float dynamicScale = (!walk && g_r4LandSprintActive) ?
        1.0f : movement_dynamic_scale();
    float sprintOverlay = (!walk && g_r4LandSprintActive) ?
        g_cfg.sprintContactOverlayGain : 1.0f;
    float dualSenseContactMultiplier = walk ?
        effective_dualsense_walk_haptic_multiplier() :
        (g_r4LandSprintActive ? 1.0f : g_cfg.dualSenseJogHapticMultiplier);
    float hapticGain = g_cfg.movementGain * dynamicScale *
        control.intensityMultiplier * sprintOverlay * dualSenseContactMultiplier;
    // r9 land sprint on XInput is contact-only, so do not reuse the lighter
    // DualSense overlay gain there. Make each run step 20% stronger than jog.
    float rumbleSprintMultiplier = (!walk && g_r4LandSprintActive &&
        g_cfg.xinputLandRunStepOnly && r9_xinput_class_device()) ?
        g_cfg.xinputSprintStepGain : sprintOverlay;
    float rumbleGain = g_cfg.movementGain * dynamicScale *
        g_cfg.rumbleMovementGain * control.intensityMultiplier *
        rumbleSprintMultiplier;
    if (hapticGain < 0.0f) hapticGain = 0.0f;
    if (rumbleGain < 0.0f) rumbleGain = 0.0f;
    if (hapticGain > 32.0f) hapticGain = 32.0f;
    if (rumbleGain > 16.0f) rumbleGain = 16.0f;

    u32 baseStartMs = walk ? g_cfg.semanticWalkPulseStartMs :
                             g_cfg.semanticRunPulseStartMs;
    u32 durationMs = walk ? g_cfg.semanticWalkPulseDurationMs :
                            g_cfg.semanticRunPulseDurationMs;
    u32 startMs = adjusted_milliseconds(baseStartMs, control.startTrimOffsetMs);
    u32 delayMs = adjusted_milliseconds(0, control.triggerOffsetMs);
    if (schedule_split_arbitrated_effect(effect, hapticGain, rumbleGain,
                                         walk ? 820u : 821u, now, delayMs,
                                         ev.nativeSerialBaseline,
                                         frames_from_ms(startMs),
                                         frames_from_ms(durationMs),
                                         leftSpatial, rightSpatial)) {
        atomic_fetch_add_u64(walk ? &g_v8WalkPulses : &g_v8RunPulses, 1);
        atomic_fetch_add_u64(&g_v8SemanticFallbacks, 1);
        if (!walk && g_r4LandSprintActive && g_cfg.xinputLandRunStepOnly &&
            r9_xinput_class_device())
            atomic_fetch_add_u64(&g_r9XInputRunStepContacts, 1);
    }
}

static void r4_schedule_heavy_phases(u64 now, u64 nativeSerialBaseline, u32 baseSource) {
    // r7: replay the same clean first-impact excerpt twice. The second pulse is
    // DelayAfterFirstMs start-to-start after the first (590 ms base delay by default), so
    // button-release timing cannot shift either impact. HeavyAttackSecond keeps
    // independent Enabled/Intensity/TriggerOffset/StartTrim controls.
    if (schedule_configured_effect(g_map.heavy, 1.25f, g_cfg.fxHeavyFirst,
                                   baseSource, now, g_cfg.semanticAttackProbeMs,
                                   nativeSerialBaseline,
                                   g_cfg.heavyPhase1StartMs,
                                   g_cfg.heavyPhase1DurationMs))
        atomic_fetch_add_u64(&g_r4HeavyPhase1Count, 1);
    if (schedule_configured_effect(g_map.heavy, 1.25f, g_cfg.fxHeavySecond,
                                   baseSource + 1u, now,
                                   g_cfg.semanticAttackProbeMs + g_cfg.rbHoldSecondDelayMs,
                                   nativeSerialBaseline,
                                   g_cfg.heavyPhase1StartMs,
                                   g_cfg.heavyPhase1DurationMs))
        atomic_fetch_add_u64(&g_r4HeavyPhase2Count, 1);
}

static void r4_schedule_sheathe_phases(u64 now, u64 nativeSerialBaseline, u32 baseSource) {
    // Preserve the opening texture, then replay the strong ~0.9-1.2 s source
    // region ~30% earlier than its original capture timing.
    schedule_configured_effect(g_map.weapon, 1.0f, g_cfg.fxSheatheHood,
                               baseSource, now, 0, nativeSerialBaseline,
                               0, g_cfg.sheathePhase1DurationMs);
    if (schedule_configured_effect(g_map.weapon, 1.0f, g_cfg.fxSheatheHood,
                                   baseSource + 1u, now, g_cfg.sheathePhase2DelayMs,
                                   nativeSerialBaseline,
                                   g_cfg.sheathePhase2StartMs,
                                   g_cfg.sheathePhase2DurationMs))
        atomic_fetch_add_u64(&g_r4SheathePhase2Count, 1);
}

static void r4_reset_hybrid_runtime() {
    g_r4LandSprintActive = false;
    atomic_store_u32(&g_r9LandSprintRumbleStepOnlyActive, 0);
    g_r4Underwater = false;
    g_r4WaterLungeUntil = 0;
    g_r9cWaterLungeArmTick = 0;
    g_r9cPendingWaterLungeTick = 0;
    g_r9cPendingWaterLungeNativeSerial = 0;
    g_r4AttackPressActive = false;
    g_r5AttackHeavyCommitted = false;
    g_r4AttackPressTick = 0;
    g_r4AttackPressNativeSerial = 0;
}

static void v8_handle_semantic_event(const V8SemanticEvent& ev) {
    u64 now = ev.tick ? ev.tick : atomic_load_u64(&g_workerNowTick);
    atomic_store_u32(&g_v8LastEventId, ev.id);
    atomic_store_u64(&g_v8LastEventTick, now);

    switch (ev.id) {
    case EVT_MENU_NATIVE_TICK:
        // Ubisoft already produces the correct zero-lag native menu Quad tick.
        v8_enter_menu_domain(now);
        atomic_fetch_add_u64(&g_v8SemanticNativeOnly, 1);
        break;
    case EVT_MENU_AUDIO_ONLY:
        // Proven missing-native class: do not manufacture a probe delay.
        v8_enter_menu_domain(now);
        request_menu_effect(g_map.menuTick, g_cfg.menuTickGain,
                            g_cfg.fxMenuTick, 801, now, 0,
                            ev.nativeSerialBaseline);
        atomic_fetch_add_u64(&g_v8SemanticFallbacks, 1);
        break;
    case EVT_WALK_STEP:
        // A real Edward footstep is strong evidence that helm ownership ended.
        if (!v8_enter_character_from_event(ev, now, true)) break;
        atomic_store_u64(&g_v8LastGaitTick, now);
        atomic_store_u64(&g_v8PreviousFastGaitTick, 0);
        atomic_store_u32(&g_movementScalePermille, 1000);
        set_movement_mode_timed(0, now);
        v8_schedule_gait_pulse(ev, true, now);
        break;
    case EVT_FAST_GAIT_STEP: {
        if (!v8_enter_character_from_event(ev, now, true)) break;
        atomic_store_u64(&g_v8PreviousFastGaitTick, now);
        atomic_store_u64(&g_v8LastGaitTick, now);
        // Jog remains event-pulsed. A confirmed L3 land sprint uses the stable
        // continuous run loop: 1.90 opening gain for 1.5 s, then 1.50 sustain,
        // exactly 50% above the jog baseline of 1.00.
        if (g_r4LandSprintActive) {
            atomic_store_u32(&g_movementScalePermille, 1000);
            set_movement_mode_timed(2, now);
            // r8 keeps the continuous run texture but adds a lighter spatial
            // contact overlay from the real fast-gait footstep event.
            v8_schedule_gait_pulse(ev, false, now);
        } else {
            atomic_store_u32(&g_movementScalePermille, 1000);
            set_movement_mode_timed(0, now);
            v8_schedule_gait_pulse(ev, false, now);
        }
        break;
    }
    case EVT_DODGE_ROLL:
        if (!v8_enter_character_from_event(ev, now, true)) break;
        schedule_configured_effect(g_map.dodge, 1.0f, g_cfg.fxDodge,
                                   802, now, 0, ev.nativeSerialBaseline,
                                   g_cfg.dodgeStartTrimMs,
                                   g_cfg.semanticDodgeDurationMs);
        atomic_fetch_add_u64(&g_v8SemanticFallbacks, 1);
        break;
    case EVT_LIGHT_ATTACK:
        if (!v8_enter_character_from_event(ev, now, true)) break;
        atomic_store_u64(&g_r4LastSemanticAttackTick, now);
        // Only combat/native Motion needs arbitration. 32 ms is one-to-two Quad
        // blocks instead of v8.0's 100 ms, and late native Motion still clears
        // an already-started fallback through the existing native detector.
        schedule_configured_effect(g_map.light, 1.0f, g_cfg.fxLightAttack,
                                   803, now, g_cfg.semanticAttackProbeMs,
                                   ev.nativeSerialBaseline, 0,
                                   g_cfg.semanticLightAttackDurationMs);
        atomic_fetch_add_u64(&g_v8SemanticFallbacks, 1);
        break;
    case EVT_HEAVY_ATTACK:
        if (!v8_enter_character_from_event(ev, now, true)) break;
        atomic_store_u64(&g_r4LastSemanticAttackTick, now);
        atomic_store_u64(&g_r5LastSemanticHeavyTick, now);
        if (g_r4AttackPressActive && g_r5AttackHeavyCommitted &&
            now >= g_r4AttackPressTick) {
            atomic_fetch_add_u64(&g_v8SemanticNativeOnly, 1);
            break;
        }
        r4_schedule_heavy_phases(now, ev.nativeSerialBaseline, 804);
        if (g_r4AttackPressActive && now >= g_r4AttackPressTick)
            g_r5AttackHeavyCommitted = true;
        atomic_fetch_add_u64(&g_v8SemanticFallbacks, 1);
        break;
    case EVT_NORMAL_PARRY:
        if (!v8_enter_character_from_event(ev, now, true)) break;
        // r4: real combat parry remains Ubisoft-native, but an out-of-combat
        // L1/parry action now gets the Shadows fallback. 60 ms -> 24 ms means
        // the same timing improvement applies to DualSense and motor rumble.
        if (schedule_configured_effect(g_map.parry, g_cfg.buttonLBGain,
                                       g_cfg.fxParry, 813, now,
                                       g_cfg.parryProbeDelayMs,
                                       ev.nativeSerialBaseline,
                                       g_cfg.parryStartTrimMs, 360)) {
            atomic_fetch_add_u64(&g_r4ParryFallbacks, 1);
            atomic_fetch_add_u64(&g_v8SemanticFallbacks, 1);
        }
        break;
    case EVT_PERFECT_PARRY_A:
    case EVT_PERFECT_PARRY_B:
    case EVT_PERFECT_PARRY_C:
    case EVT_PISTOL_FIRE:
        // Perfect-parry and pistol remain native-owned; r9b boosts their native Quad buffer inside the action window.
        if (!v8_enter_character_from_event(ev, now, true)) break;
        atomic_fetch_add_u64(&g_v8SemanticNativeOnly, 1);
        break;
    case EVT_PLAYER_HIT:
        // Damage can legitimately occur while Edward is still at the wheel.
        // Never use PLAYER_HIT by itself to release HELM. If already off-helm,
        // it remains a normal native-only CHARACTER state anchor.
        if (atomic_load_u32(&g_v8RuntimeMode) != V8_MODE_HELM) {
            v8_note_character_object(ev.gameObject);
            v8_enter_character_domain(now);
        }
        atomic_fetch_add_u64(&g_v8SemanticNativeOnly, 1);
        break;
    case EVT_SHEATHE:
    case EVT_UNSHEATHE:
        if (!v8_enter_character_from_event(ev, now, true)) break;
        r4_schedule_sheathe_phases(now, ev.nativeSerialBaseline, 805);
        atomic_fetch_add_u64(&g_v8SemanticFallbacks, 1);
        break;
    case EVT_HOOD_ON:
    case EVT_HOOD_OFF:
        if (!v8_enter_character_from_event(ev, now, true)) break;
        schedule_configured_effect(g_map.weapon, 1.0f, g_cfg.fxSheatheHood,
                                   806, now, 0, ev.nativeSerialBaseline);
        atomic_fetch_add_u64(&g_v8SemanticFallbacks, 1);
        break;
    case EVT_KICK:
        if (!v8_enter_character_from_event(ev, now, true)) break;
        schedule_configured_effect(g_map.parry, g_cfg.buttonLBGain,
                                   g_cfg.fxParry, 807, now, 0,
                                   ev.nativeSerialBaseline,
                                   g_cfg.parryStartTrimMs,
                                   g_cfg.semanticKickDurationMs);
        atomic_fetch_add_u64(&g_v8SemanticFallbacks, 1);
        break;
    case EVT_JUMP_START:
        // Context only in r4. The Anvil Jump/InAir hooks own takeoff and airtime
        // because they also cover forward/targeted jumps that lack this Wwise ID.
        (void)v8_enter_character_from_event(ev, now, false);
        break;
    case EVT_LANDING_A:
    case EVT_LANDING_B:
        // Context-only confirmation; canonical InAir exit owns landing timing.
        (void)v8_enter_character_from_event(ev, now, false);
        break;
    case EVT_WATER_LUNGE:
        // r9c FIX: 0x7ED9038C also occurs when choosing/changing an underwater
        // direction after being stationary. It is therefore NOT an input edge.
        // Only a recent real L3/SprintLunge press is allowed to turn it into a
        // lunge. Keep WATER ownership evidence, but never start the lunge timer
        // or feedback from this event alone.
        v8_enter_water_domain(now);
        if (r9c_water_lunge_arm_is_valid(now)) {
            r9c_accept_water_lunge_semantic(now, ev.nativeSerialBaseline);
        } else {
            // Handle the inverse ordering where Wwise posts the event just
            // before the 8 ms controller poll observes L3 rising.
            if (g_r9cPendingWaterLungeTick)
                atomic_fetch_add_u64(&g_r9cWaterLungeSemanticRejected, 1);
            g_r9cPendingWaterLungeTick = now;
            g_r9cPendingWaterLungeNativeSerial = ev.nativeSerialBaseline;
        }
        break;
    case EVT_SURFACE:
        v8_enter_water_domain(now);
        g_r4Underwater = false;
        g_r4WaterLungeUntil = 0;
        schedule_configured_effect(g_map.surface, g_cfg.surfaceGain,
                                   g_cfg.fxSurface, 810, now, 0,
                                   ev.nativeSerialBaseline,
                                   g_cfg.stateEffectStartTrimMs,
                                   g_cfg.semanticSurfaceDurationMs);
        g_lastSurfaceEffectTick = now;
        atomic_fetch_add_u64(&g_v8SemanticFallbacks, 1);
        break;
    case EVT_SWIM_SPRINT:
        v8_enter_water_domain(now);
        break;
    case EVT_DIVE_CANDIDATE:
        v8_enter_water_domain(now);
        g_r4Underwater = true;
        // Candidate remains state-only; Circle/water-state fallback is retained.
        break;
    case EVT_HELM_ENTER:
    case EVT_CANNON_BROADSIDE:
    case EVT_CANNON_MOTION:
        // Native naval output is never replaced or augmented. These are only
        // ownership anchors. HELM may later be released by real character/water/
        // menu evidence or by a long load/desync recovery; it is not sticky.
        if (ev.gameObject && (ev.id == EVT_HELM_ENTER ||
                              ev.id == EVT_CANNON_BROADSIDE ||
                              !atomic_load_u64(&g_v8HelmObject)))
            atomic_store_u64(&g_v8HelmObject, ev.gameObject);
        if (g_cfg.semanticMenu && atomic_load_u32(&g_menuMode)) leave_menu_mode(now);
        r8_reset_foot_phase();
        v8_set_mode(V8_MODE_HELM, now);
        atomic_fetch_add_u64(&g_v8SemanticNativeOnly, 1);
        break;
    default:
        break;
    }
}

static void process_v8_semantic_events(u64 now) {
    if (!g_cfg.semanticEvents || !atomic_load_u32(&g_v8PostHookInstalled)) return;
    u64 end = atomic_load_u64(&g_v8SemWrite);
    u64 read = atomic_load_u64(&g_v8SemRead);
    u64 minSeq = end > V8_SEM_RING ? end - V8_SEM_RING + 1 : 1;
    if (read + 1 < minSeq) {
        atomic_fetch_add_u64(&g_v8SemDrops, minSeq - (read + 1));
        read = minSeq - 1;
    }
    while (read < end) {
        u64 seq = read + 1;
        V8SemanticEvent& slot = g_v8SemRing[seq & (V8_SEM_RING - 1)];
        if (atomic_load_u64(&slot.commit) != seq) break;
        V8SemanticEvent copy{};
        copy.tick = slot.tick; copy.gameObject = slot.gameObject;
        copy.nativeSerialBaseline = slot.nativeSerialBaseline;
        copy.id = slot.id; copy.playingId = slot.playingId;
        v8_handle_semantic_event(copy);
        read = seq;
        atomic_fetch_add_u64(&g_v8SemProcessed, 1);
    }
    atomic_store_u64(&g_v8SemRead, read);

    // Ground locomotion is pulse-per-event now, so timeout only expires cadence
    // history; it does not fade a loop after the player has already stopped.
    u64 gait = atomic_load_u64(&g_v8LastGaitTick);
    if (gait && now >= gait && now - gait > (u64)g_cfg.semanticGaitTimeoutMs) {
        if (g_r8LastFootstepTick && now >= g_r8LastFootstepTick &&
            now - g_r8LastFootstepTick > (u64)g_cfg.footstepResetAfterMs)
            r8_reset_foot_phase();
        atomic_store_u64(&g_v8LastGaitTick, 0);
        atomic_store_u64(&g_v8PreviousFastGaitTick, 0);
        atomic_store_u32(&g_movementScalePermille, 1000);
        if (g_r4LandSprintActive) {
            g_r4LandSprintActive = false;
            atomic_store_u32(&g_r9LandSprintRumbleStepOnlyActive, 0);
            g_sprintLatched = false;
            set_movement_mode_timed(0, now);
        }
    }
    if (g_r4Underwater && g_r4WaterLungeUntil && now >= g_r4WaterLungeUntil) {
        g_r4WaterLungeUntil = 0;
        g_sprintLatched = false;
        set_movement_mode_timed(0, now);
    }

    // r9c pending-event bridge for worker ordering: a true semantic lunge can
    // arrive immediately before the next controller poll sees L3. Give that
    // inverse ordering 96 ms, then reject it as direction-only if no L3 edge
    // materializes. The longer arm window only applies after a real L3 press.
    if (g_r9cPendingWaterLungeTick && now >= g_r9cPendingWaterLungeTick &&
        now - g_r9cPendingWaterLungeTick >
            (u64)R9C_WATER_LUNGE_PREINPUT_TOLERANCE_MS) {
        g_r9cPendingWaterLungeTick = 0;
        g_r9cPendingWaterLungeNativeSerial = 0;
        atomic_fetch_add_u64(&g_r9cWaterLungeSemanticRejected, 1);
    }
    if (g_r9cWaterLungeArmTick && now >= g_r9cWaterLungeArmTick &&
        now - g_r9cWaterLungeArmTick > (u64)g_cfg.waterLungeInputGateMs) {
        g_r9cWaterLungeArmTick = 0;
    }

    // Safety net for a missed landing/shared jump event. A stale semantic jump
    // can never turn the next ordinary contact into a 10+ second "long fall".
    u64 airStart = atomic_load_u64(&g_v8SemanticAirStartTick);
    if (airStart && now >= airStart && now - airStart > (u64)g_cfg.semanticAirTimeoutMs) {
        atomic_store_u64(&g_v8SemanticAirStartTick, 0);
        atomic_fetch_add_u64(&g_v8SemanticAirTimeouts, 1);
    }
}

static void reset_v8_semantic_state(u64 now) {
    r4_reset_hybrid_runtime();
    r8_reset_foot_phase();
    atomic_store_u32(&g_v8RuntimeMode, V8_MODE_UNKNOWN);
    atomic_store_u32(&g_v8LastEventId, 0);
    atomic_store_u64(&g_v8LastEventTick, 0);
    atomic_store_u64(&g_v8LastMenuEventTick, 0);
    atomic_store_u64(&g_v8LastGameplayEventTick, 0);
    atomic_store_u64(&g_v8LastGaitTick, 0);
    atomic_store_u64(&g_v8PreviousFastGaitTick, 0);
    atomic_store_u64(&g_v8SemanticAirStartTick, 0);
    atomic_store_u64(&g_v8LastLandingTick, 0);
    atomic_store_u64(&g_v8CharacterObject, 0);
    atomic_store_u64(&g_v8HelmObject, 0);
    atomic_store_u64(&g_v8SemRead, atomic_load_u64(&g_v8SemWrite));
    reset_menu_mode();
    cancel_automatic_runtime(now, true);
    g_gameplayResumeGuardUntil = now + g_cfg.menuTransitionGuardMs;
}

static void tb_effect_control(TextBuf& b, const char* name,
                              const EffectControl& control) {
    tb_char(b, ' '); tb_str(b, name); tb_char(b, '=');
    tb_u64(b, control.enabled); tb_char(b, '/');
    tb_float3(b, control.intensityMultiplier); tb_char(b, '/');
    tb_i64(b, control.triggerOffsetMs); tb_char(b, '/');
    tb_i64(b, control.startTrimOffsetMs);
}

static void log_input_bindings() {
    TextBuf b{};
    SYSTEMTIME st{};
    if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    tb_char(b, '['); tb_time(b, st); tb_str(b, "] INPUTCFG");
    tb_str(b, " jump="); tb_str(b, input_binding_name(g_cfg.jumpBinding));
    tb_str(b, " dodgeDive=");
    tb_str(b, input_binding_name(g_cfg.dodgeDiveBinding));
    tb_str(b, " parry="); tb_str(b, input_binding_name(g_cfg.parryBinding));
    tb_str(b, " attack="); tb_str(b, input_binding_name(g_cfg.attackBinding));
    tb_str(b, " sprintLunge=");
    tb_str(b, input_binding_name(g_cfg.sprintLungeBinding));
    tb_str(b, " sheatheHood=");
    tb_str(b, input_binding_name(g_cfg.sheatheHoodBinding));
    tb_str(b, " invalidFallbacks="); tb_u64(b, g_cfg.invalidInputBindings);
    tb_endline(b);
    append_log(b.bytes, b.len);
}

static void log_effect_controls() {
    SYSTEMTIME st{};
    if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    {
        TextBuf b{};
        tb_char(b, '['); tb_time(b, st);
        tb_str(b, "] EFFECTCFG enabled/intensity/triggerOffsetMs/startTrimOffsetMs");
        tb_effect_control(b, "walk", g_cfg.fxWalk);
        tb_effect_control(b, "run", g_cfg.fxRun);
        tb_effect_control(b, "sprintKick", g_cfg.fxSprintKick);
        tb_effect_control(b, "jump", g_cfg.fxJump);
        tb_endline(b);
        append_log(b.bytes, b.len);
    }
    {
        TextBuf b{};
        tb_char(b, '['); tb_time(b, st); tb_str(b, "] EFFECTCFG state");
        tb_effect_control(b, "landS", g_cfg.fxLandingShort);
        tb_effect_control(b, "landN", g_cfg.fxLandingNormal);
        tb_effect_control(b, "landL", g_cfg.fxLandingLong);
        tb_effect_control(b, "hay", g_cfg.fxHaystack);
        tb_effect_control(b, "dodge", g_cfg.fxDodge);
        tb_effect_control(b, "dirDodge", g_cfg.fxDirectionalDodge);
        tb_effect_control(b, "dive", g_cfg.fxDive);
        tb_effect_control(b, "surface", g_cfg.fxSurface);
        tb_effect_control(b, "swimUp", g_cfg.fxWaterJump);
        tb_effect_control(b, "waterLunge", g_cfg.fxWaterLunge);
        tb_endline(b);
        append_log(b.bytes, b.len);
    }
    {
        TextBuf b{};
        tb_char(b, '['); tb_time(b, st); tb_str(b, "] EFFECTCFG combat-menu");
        tb_effect_control(b, "parry", g_cfg.fxParry);
        tb_effect_control(b, "light", g_cfg.fxLightAttack);
        tb_effect_control(b, "heavy1", g_cfg.fxHeavyFirst);
        tb_effect_control(b, "heavy2", g_cfg.fxHeavySecond);
        tb_effect_control(b, "sheathe", g_cfg.fxSheatheHood);
        tb_effect_control(b, "menuTick", g_cfg.fxMenuTick);
        tb_effect_control(b, "menuOpen", g_cfg.fxMenuOpen);
        tb_effect_control(b, "menuShoulder", g_cfg.fxMenuShoulder);
        tb_endline(b);
        append_log(b.bytes, b.len);
    }
}

static void write_status() {
    TextBuf b{};
    SYSTEMTIME st{};
    if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    u64 now = g_api.GetTickCount64 ? g_api.GetTickCount64() :
              atomic_load_u64(&g_workerNowTick);
    tb_char(b, '['); tb_time(b, st); tb_str(b, "] STATUS enabled=");
    tb_u64(b, atomic_load_u32(&g_enabled)); tb_str(b, " input=");
    tb_u64(b, atomic_load_u32(&g_inputEnabled));
    tb_str(b, " semHook="); tb_u64(b, atomic_load_u32(&g_v8PostHookInstalled));
    tb_str(b, " semMode="); tb_str(b, v8_mode_name(atomic_load_u32(&g_v8RuntimeMode)));
    tb_str(b, " semLast=0x"); tb_hex(b, atomic_load_u32(&g_v8LastEventId), 8);
    tb_str(b, " semEvents="); tb_u64(b, atomic_load_u64(&g_v8SemProcessed));
    tb_str(b, " semDrops="); tb_u64(b, atomic_load_u64(&g_v8SemDrops));
    tb_str(b, " semAirMs=");
    { u64 a=atomic_load_u64(&g_v8SemanticAirStartTick); tb_u64(b, a&&now>=a ? now-a : 0); }
    tb_str(b, " semObj=C/H=0x"); tb_hex(b, atomic_load_u64(&g_v8CharacterObject));
    tb_str(b, "/0x"); tb_hex(b, atomic_load_u64(&g_v8HelmObject));
    tb_str(b, " semPulse=W/R="); tb_u64(b, atomic_load_u64(&g_v8WalkPulses));
    tb_char(b, '/'); tb_u64(b, atomic_load_u64(&g_v8RunPulses));
    tb_str(b, " semHelm=release/ambigIgnore="); tb_u64(b, atomic_load_u64(&g_v8HelmReleases));
    tb_char(b, '/'); tb_u64(b, atomic_load_u64(&g_v8HelmAmbiguousIgnored));
    tb_str(b, " semAirTO="); tb_u64(b, atomic_load_u64(&g_v8SemanticAirTimeouts));
    tb_str(b, " r4=run/under/lungeRemain/lightRaw/heavyRaw/parryFB/heavy12/sheath2=");
    tb_u64(b, g_r4LandSprintActive ? 1u : 0u); tb_char(b, '/');
    tb_u64(b, g_r4Underwater ? 1u : 0u); tb_char(b, '/');
    tb_u64(b, g_r4WaterLungeUntil > now ? g_r4WaterLungeUntil - now : 0); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r4RawLightFallbacks)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r4RawHeavyFallbacks)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r4ParryFallbacks)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r4HeavyPhase1Count)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r4HeavyPhase2Count)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r4SheathePhase2Count));
    tb_str(b, " r5=heavyCommit/waterReacq/climbCarry/climbCatch=");
    tb_u64(b, atomic_load_u64(&g_r5HeavyHoldCommits)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r5WaterStateReacquires)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r5ClimbAirCarries)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r5ClimbCatchImpacts));
    tb_str(b, " r6=parkourBind/directCandidate/land/climbImpact/travelMs/boostP=");
    tb_u64(b, atomic_load_u64(&g_r6HeldParkourDirectBinds)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r6HeldParkourCandidateBinds)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r6ParkourLandings)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r6ParkourClimbImpacts)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r6LastParkourTravelMs)); tb_char(b, '/');
    tb_u64(b, atomic_load_u32(&g_r6LastParkourBoostPermille));
    tb_str(b, " r7=autoWaterBind/entryFx=");
    tb_u64(b, atomic_load_u64(&g_r7AutoWaterDirectBinds)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r7AutoWaterEntryEffects));
    tb_str(b, " r8=footR/footL/resets/next=");
    tb_u64(b, atomic_load_u64(&g_r8RightFootContacts)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r8LeftFootContacts)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r8FootPhaseResets)); tb_char(b, '/');
    tb_str(b, g_r8NextFootRight ? "R" : "L");
    tb_str(b, " r9=xiRunSteps/suppressedTicks/stepOnly=");
    tb_u64(b, atomic_load_u64(&g_r9XInputRunStepContacts)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r9ContinuousRunSuppressedTicks)); tb_char(b, '/');
    tb_u64(b, g_cfg.xinputLandRunStepOnly);
    tb_str(b, " r9b=nativeCombatArms/boostBlocks/boostFrames/activeMs/gain/lastEvt=");
    tb_u64(b, atomic_load_u64(&g_nativeCombatBoostArms)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_nativeCombatBoostBlocks)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_nativeCombatBoostFrames)); tb_char(b, '/');
    { u64 until=atomic_load_u64(&g_nativeCombatBoostUntil); tb_u64(b, until>now ? until-now : 0); }
    tb_char(b, '/'); tb_float3(b, g_cfg.nativeCombatHapticGain); tb_str(b, "/0x");
    tb_hex(b, atomic_load_u32(&g_nativeCombatBoostLastEvent), 8);
    tb_str(b, " r9c=waterL3Arms/semAccept/semReject/pending/gateMs=");
    tb_u64(b, atomic_load_u64(&g_r9cWaterLungeInputArms)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r9cWaterLungeSemanticAccepted)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r9cWaterLungeSemanticRejected)); tb_char(b, '/');
    tb_u64(b, g_r9cPendingWaterLungeTick ? 1u : 0u); tb_char(b, '/');
    tb_u64(b, g_cfg.waterLungeInputGateMs);
    tb_str(b, " xiCaps=jumpLand/light/ms=");
    tb_u64(b, atomic_load_u64(&g_r9cXInputJumpLandingCaps)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r9cXInputLightAttackCaps)); tb_char(b, '/');
    tb_u64(b, g_cfg.xinputJumpLandingMaxDurationMs); tb_char(b, '/');
    tb_u64(b, g_cfg.xinputLightAttackMaxDurationMs);
    tb_str(b, " r9d=zeroBarrier/zeroTicks/gapMs=");
    tb_u64(b, atomic_load_u64(&g_r9dXInputZeroBarriers)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_r9dXInputZeroTicks)); tb_char(b, '/');
    tb_u64(b, g_cfg.xinputActionZeroGapMs);
    tb_str(b, " r9f=dsHapticG/jl/landMove/menuShoulder/menuTick/xiMenu=");
    tb_float3(b, g_cfg.dualSenseBridgeHapticGain); tb_char(b, '/');
    tb_float3(b, g_cfg.dualSenseJumpLandingHapticGain); tb_char(b, '/');
    tb_float3(b, g_cfg.dualSenseLandMovementHapticGain); tb_char(b, '/');
    tb_float3(b, g_cfg.dualSenseMenuShoulderGain); tb_char(b, '/');
    tb_float3(b, g_cfg.dualSenseMenuTickGain); tb_char(b, '/');
    tb_float3(b, g_cfg.xinputMenuShoulderGain);
    tb_str(b, " r9fMenuNative=arms/blocks/frames/activeMs=");
    tb_u64(b, atomic_load_u64(&g_nativeMenuTickBoostArms)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_nativeMenuTickBoostBlocks)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_nativeMenuTickBoostFrames)); tb_char(b, '/');
    { u64 until=atomic_load_u64(&g_nativeMenuTickBoostUntil); tb_u64(b, until>now ? until-now : 0); }
    tb_str(b, " nativeHaptics=");
    tb_u64(b, g_cfg.nativeHaptics); tb_str(b, " conventionalRumble=");
    tb_u64(b, g_cfg.conventionalRumble); tb_str(b, " hook=");
    tb_u64(b, atomic_load_u32(&g_hookInstalled)); tb_str(b, " flushCalls=");
    tb_u64(b, atomic_load_u64(&g_flushCalls)); tb_str(b, " injectedBlocks=");
    tb_u64(b, atomic_load_u64(&g_injectedBlocks)); tb_str(b, " injectedFrames=");
    tb_u64(b, atomic_load_u64(&g_injectedFrames)); tb_str(b, " maxFrames=");
    tb_u64(b, atomic_load_u32(&g_lastMaxFrames)); tb_str(b, " movement=");
    tb_u64(b, atomic_load_u32(&g_movementMode)); tb_str(b, " sink=0x");
    tb_hex(b, atomic_load_u64(&g_primarySink)); tb_str(b, " audioTid=");
    tb_u64(b, atomic_load_u32(&g_flushThreadId)); tb_str(b, " queued=");
    tb_u64(b, atomic_load_u64(&g_commandsQueued)); tb_str(b, " dropped=");
    tb_u64(b, atomic_load_u64(&g_commandsDropped)); tb_str(b, " rumbleQueued=");
    tb_u64(b, atomic_load_u64(&g_rumbleCommandsQueued)); tb_str(b, " rumbleDropped=");
    tb_u64(b, atomic_load_u64(&g_rumbleCommandsDropped)); tb_str(b, " invalid=");
    tb_u64(b, atomic_load_u64(&g_invalidSinkFrames)); tb_str(b, " controller=");
    tb_u64(b, atomic_load_u32(&g_controllerConnected)); tb_str(b, " inputBackend=");
    tb_u64(b, atomic_load_u32(&g_inputBackend)); tb_str(b, " rumbleBackend=");
    tb_u64(b, atomic_load_u32(&g_rumbleBackend)); tb_str(b, " vid=0x");
    tb_hex(b, atomic_load_u32(&g_activeVendor), 4); tb_str(b, " pid=0x");
    tb_hex(b, atomic_load_u32(&g_activeProduct), 4); tb_str(b, " dualSense=");
    tb_u64(b, atomic_load_u32(&g_activeDualSense)); tb_str(b, " deviceStatus=0x");
    tb_hex(b, atomic_load_u32(&g_activeDeviceStatus)); tb_str(b, " rumbleWrites=");
    tb_u64(b, atomic_load_u64(&g_rumbleWrites)); tb_str(b, " rumbleGI=");
    tb_u64(b, atomic_load_u64(&g_gameInputRumbleWrites)); tb_str(b, " rumbleXI=");
    tb_u64(b, atomic_load_u64(&g_xinputRumbleWrites)); tb_str(b, " rumbleLow=");
    tb_u64(b, atomic_load_u32(&g_lastRumbleLow)); tb_str(b, " rumbleHigh=");
    tb_u64(b, atomic_load_u32(&g_lastRumbleHigh));
    tb_str(b, " xinputRunPulse=enabled/active/count=");
    tb_u64(b, g_cfg.xinputRunPulseEnabled); tb_char(b, '/');
    tb_u64(b, atomic_load_u32(&g_xinputRunPulseActive)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_xinputRunPulses));
    tb_str(b, " movementTune=walkHapticUser/effective/jog/pulseIntervalMs/pulseDurationMs/pulseGain=");
    tb_float3(b, g_cfg.dualSenseWalkHapticMultiplier); tb_char(b, '/');
    tb_float3(b, effective_dualsense_walk_haptic_multiplier()); tb_char(b, '/');
    tb_float3(b, g_cfg.dualSenseJogHapticMultiplier); tb_char(b, '/');
    tb_u64(b, g_cfg.xinputRunPulseIntervalMs); tb_char(b, '/');
    tb_u64(b, g_cfg.xinputRunPulseDurationMs); tb_char(b, '/');
    tb_float3(b, g_cfg.xinputRunPulseGain);
    tb_str(b, " inputPackets=");
    tb_u64(b, atomic_load_u64(&g_controllerPackets)); tb_str(b, " buttons=0x");
    tb_hex(b, atomic_load_u32(&g_lastControllerButtons), 4); tb_str(b, " rawGI=0x");
    tb_hex(b, atomic_load_u32(&g_lastGameInputButtons), 8);
    tb_str(b, " rawController=count/mask/touch/edges=");
    tb_u64(b, atomic_load_u32(&g_rawControllerButtonCount)); tb_char(b, '/');
    tb_hex(b, atomic_load_u64(&g_rawControllerButtons)); tb_char(b, '/');
    tb_u64(b, atomic_load_u32(&g_rawTouchpadDown)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_rawTouchpadEdges)); tb_str(b, " recovery=sinkRebind/flushResume/applied=");
    tb_u64(b, atomic_load_u64(&g_sinkRebinds)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_flushResumes)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_reloadRecoveries));
    tb_str(b, " recoveryDomain=reset/preserve=");
    tb_u64(b, atomic_load_u64(&g_recoveryDomainResets)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_recoveryDomainPreserves)); tb_endline(b);
    append_log(b.bytes, b.len);

    TextBuf state{};
    u64 rawTotal = 0;
    u64 acceptedTotal = 0;
    for (u32 i = 0; i < GSE_COUNT; ++i) {
        rawTotal += atomic_load_u64(&g_stateRaw[i]);
        acceptedTotal += atomic_load_u64(&g_stateAccepted[i]);
    }
    tb_char(state, '['); tb_time(state, st); tb_str(state, "] STATE hooks=");
    tb_u64(state, atomic_load_u32(&g_stateHooksInstalled)); tb_str(state, " failed=");
    tb_u64(state, atomic_load_u32(&g_stateHookFailures)); tb_str(state, " playerCtx=0x");
    tb_hex(state, atomic_load_u64(&g_playerContext)); tb_str(state, " flags=0x");
    tb_hex(state, atomic_load_u32(&g_characterStateFlags)); tb_str(state, " binds=");
    tb_u64(state, atomic_load_u64(&g_playerBindCount)); tb_str(state, " raw/acceptedTotal=");
    tb_u64(state, rawTotal); tb_char(state, '/'); tb_u64(state, acceptedTotal);
    const char* eventNames[GSE_COUNT] = {
        " airIn=", " airOut=", " jump=", " fall=", " ledgeIn=",
        " ledgeOut=", " transition=", " climbIn=", " climbOut=",
        " swimIn=", " swimOut=", " shallowIn=", " shallowOut=",
        " hayIn=", " hayOut="
    };
    for (u32 i = 0; i < GSE_COUNT; ++i) {
        tb_str(state, eventNames[i]);
        tb_u64(state, atomic_load_u64(&g_stateRaw[i])); tb_char(state, '/');
        tb_u64(state, atomic_load_u64(&g_stateAccepted[i]));
    }
    tb_str(state, " pendingLand="); tb_u64(state, g_landingPending ? 1u : 0u);
    tb_str(state, " pendingSurface="); tb_u64(state, g_surfacePending ? 1u : 0u);
    tb_str(state, " airMs=last/max=");
    tb_u64(state, atomic_load_u64(&g_lastAirborneDurationMs)); tb_char(state, '/');
    tb_u64(state, atomic_load_u64(&g_maxAirborneDurationMs));
    tb_str(state, " shortAirRejected=");
    tb_u64(state, atomic_load_u64(&g_shortAirborneRejected));
    tb_str(state, " landing=ground/hay/class/gainP=");
    tb_u64(state, atomic_load_u64(&g_groundLandingEffects)); tb_char(state, '/');
    tb_u64(state, atomic_load_u64(&g_haystackLandingEffects)); tb_char(state, '/');
    tb_u64(state, atomic_load_u32(&g_lastLandingCategory)); tb_char(state, '/');
    tb_u64(state, atomic_load_u32(&g_lastLandingGainPermille));
    tb_str(state, " jumpGate=climb/noInput/noDir=");
    tb_u64(state, atomic_load_u64(&g_confirmedClimbJumps)); tb_char(state, '/');
    tb_u64(state, atomic_load_u64(&g_suppressedJumpWithoutInput)); tb_char(state, '/');
    tb_u64(state, atomic_load_u64(&g_suppressedClimbJumpDirection));
    tb_str(state, " takeoffMove=");
    tb_u64(state, atomic_load_u32(&g_lastTakeoffMovementMode));
    tb_str(state, " jumpDown/stickSq/lastJumpStickSq=");
    tb_u64(state, atomic_load_u32(&g_currentJumpInputDown)); tb_char(state, '/');
    tb_u64(state, atomic_load_u32(&g_currentLeftStickMagnitudeSq)); tb_char(state, '/');
    tb_u64(state, atomic_load_u32(&g_lastJumpStickMagnitudeSq));
    tb_str(state, " climbDodgeSuppressed=");
    tb_u64(state, atomic_load_u64(&g_suppressedClimbDodge));
    tb_str(state, " airDiveArmed="); tb_u64(state, atomic_load_u64(&g_armedAirDive));
    tb_str(state, " hybridJump=input/water/fallback/cancel/pending=");
    tb_u64(state, atomic_load_u64(&g_inputJumpEffects)); tb_char(state, '/');
    tb_u64(state, atomic_load_u64(&g_waterJumpEffects)); tb_char(state, '/');
    tb_u64(state, atomic_load_u64(&g_inputFallbackLandings)); tb_char(state, '/');
    tb_u64(state, atomic_load_u64(&g_inputFallbackCancels)); tb_char(state, '/');
    tb_u64(state, g_inputLandingPending ? 1u : 0u);
    tb_str(state, " waterLunge=");
    tb_u64(state, atomic_load_u64(&g_waterLungeEffects));
    tb_str(state, " dodge=plain/dir=");
    tb_u64(state, atomic_load_u64(&g_groundDodgeEffects)); tb_char(state, '/');
    tb_u64(state, atomic_load_u64(&g_directionalDodgeEffects));
    tb_str(state, " staleAirRecovery=");
    tb_u64(state, atomic_load_u64(&g_staleAirFlagRecoveries));
    tb_str(state, " moveScalePermille=");
    tb_u64(state, atomic_load_u32(&g_movementScalePermille));
    u64 runStart = atomic_load_u64(&g_runStartTick);
    tb_str(state, " runAgeMs=");
    tb_u64(state, runStart && now >= runStart ? now - runStart : 0);
    tb_endline(state);
    append_log(state.bytes, state.len);

    TextBuf native{};
    u64 hapticUntil = atomic_load_u64(&g_nativeHapticUntil);
    u64 rumbleUntil = atomic_load_u64(&g_nativeRumbleUntil);
    u32 deferredActive = 0;
    for (u32 i = 0; i < DEFERRED_EFFECT_COUNT; ++i)
        if (g_deferredEffects[i].active) ++deferredActive;
    tb_char(native, '['); tb_time(native, st); tb_str(native, "] NATIVE arb=");
    tb_u64(native, g_cfg.nativeArbitration); tb_str(native, " hRemainMs=");
    tb_u64(native, hapticUntil > now ? hapticUntil - now : 0); tb_str(native, " rRemainMs=");
    tb_u64(native, rumbleUntil > now ? rumbleUntil - now : 0); tb_str(native, " hDetect=");
    tb_u64(native, atomic_load_u64(&g_nativeHapticDetections)); tb_str(native, " rDetect=");
    tb_u64(native, atomic_load_u64(&g_nativeRumbleDetections)); tb_str(native, " gameXI=");
    tb_u64(native, atomic_load_u64(&g_gameNativeXInputWrites)); tb_str(native, " gameGI=");
    tb_u64(native, atomic_load_u64(&g_gameNativeGameInputWrites)); tb_str(native, " xObs=");
    tb_u64(native, atomic_load_u32(&g_gameXInputObserverInstalled)); tb_char(native, '/');
    tb_u64(native, atomic_load_u32(&g_gameXInputObserverFailures)); tb_str(native, " giObs=");
    tb_u64(native, atomic_load_u32(&g_gameInputObserversInstalled)); tb_char(native, '/');
    tb_u64(native, atomic_load_u32(&g_gameInputObserverFailures)); tb_str(native, " suppressedEff/H/R=");
    tb_u64(native, atomic_load_u64(&g_nativeSuppressedEffects)); tb_char(native, '/');
    tb_u64(native, atomic_load_u64(&g_nativeSuppressedHapticBlocks)); tb_char(native, '/');
    tb_u64(native, atomic_load_u64(&g_nativeSuppressedRumbleTicks)); tb_str(native, " lastPeakE6=");
    tb_u64(native, atomic_load_u32(&g_lastNativePeakMillion)); tb_str(native, " lastMeanE6=");
    tb_u64(native, atomic_load_u32(&g_lastNativeMeanMillion)); tb_str(native, " lastRumblePermille=");
    tb_u64(native, atomic_load_u32(&g_lastNativeRumblePermille)); tb_str(native, " deferred=q/drop/active=");
    tb_u64(native, atomic_load_u64(&g_deferredQueued)); tb_char(native, '/');
    tb_u64(native, atomic_load_u64(&g_deferredDropped)); tb_char(native, '/');
    tb_u64(native, deferredActive); tb_str(native, " serial=");
    tb_u64(native, atomic_load_u64(&g_nativeFeedbackSerial));
    tb_str(native, " menu=mode/sessions/pulses/moveClose/nativeTabArm/rawOpen/shoulder/domainSwitch=");
    tb_u64(native, atomic_load_u32(&g_menuMode)); tb_char(native, '/');
    tb_u64(native, atomic_load_u64(&g_menuSessions)); tb_char(native, '/');
    tb_u64(native, atomic_load_u64(&g_menuPulseRequests)); tb_char(native, '/');
    tb_u64(native, atomic_load_u64(&g_menuMovementCloses)); tb_char(native, '/');
    tb_u64(native, atomic_load_u64(&g_menuNativeTabArms)); tb_char(native, '/');
    tb_u64(native, atomic_load_u64(&g_menuRawTouchpadOpens)); tb_char(native, '/');
    tb_u64(native, atomic_load_u64(&g_menuShoulderRequests)); tb_char(native, '/');
    tb_u64(native, atomic_load_u64(&g_menuDomainSwitches)); tb_endline(native);
    append_log(native.bytes, native.len);

    TextBuf perf{};
    tb_char(perf, '['); tb_time(perf, st); tb_str(perf, "] PERF pollMs/hz=");
    tb_u64(perf, g_cfg.pollMs); tb_char(perf, '/');
    tb_u64(perf, g_cfg.pollMs ? 1000u / g_cfg.pollMs : 0u);
    tb_str(perf, " workerGapUs=max/gt16/gt25/gt50=");
    tb_u64(perf, atomic_load_u64(&g_perfWorkerMaxGapUs)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfWorkerGapGt16Ms)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfWorkerGapGt25Ms)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfWorkerGapGt50Ms));
    tb_str(perf, " giUs=max/gt4/gt8/gt16=");
    tb_u64(perf, atomic_load_u64(&g_perfGameInputMaxUs)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfGameInputGt4Ms)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfGameInputGt8Ms)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfGameInputGt16Ms));
    tb_str(perf, " rumbleUs=max/gt4/gt8/gt16=");
    tb_u64(perf, atomic_load_u64(&g_perfRumbleWriteMaxUs)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfRumbleWriteGt4Ms)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfRumbleWriteGt8Ms)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfRumbleWriteGt16Ms));
    tb_str(perf, " audioUs=max/gt1/gt2/gt5=");
    tb_u64(perf, atomic_load_u64(&g_perfAudioHookMaxUs)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfAudioHookGt1Ms)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfAudioHookGt2Ms)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfAudioHookGt5Ms));
    tb_str(perf, " giCache=id/status/rawCount=");
    tb_u64(perf, atomic_load_u64(&g_perfDeviceIdentityRefreshes)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfDeviceStatusRefreshes)); tb_char(perf, '/');
    tb_u64(perf, atomic_load_u64(&g_perfRawButtonCountQueries));
    tb_str(perf, " rumbleCadenceSkip=");
    tb_u64(perf, atomic_load_u64(&g_perfRumbleCadenceSkips));
    tb_str(perf, " sinkRebind="); tb_u64(perf, atomic_load_u64(&g_sinkRebinds));
    tb_endline(perf);
    append_log(perf.bytes, perf.len);

    log_input_bindings();
    log_effect_controls();
}

static void log_r9k_output_status(u64 now) {
    TextBuf b{}; SYSTEMTIME st{}; if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
    tb_char(b,'[');tb_time(b,st);tb_str(b,"] R9K OUTPUT profile=");
    tb_str(b,r9k_profile_name(atomic_load_u32(&g_r9kGameProfile)));
    tb_str(b," baselineNativeQuad=");tb_u64(b, r9k_ubisoft_profile()?0u:1u);
    tb_str(b," ubiFallback=cfg/active/nativeRecent=");tb_u64(b,g_cfg.ubisoftDirectDualSenseFallback);tb_char(b,'/');
    tb_u64(b,atomic_load_u32(&g_r9kDirectActive));tb_char(b,'/');tb_u64(b,r9k_native_haptic_recent(now)?1u:0u);
    tb_str(b," ds=current/physical=");tb_u64(b,atomic_load_u32(&g_activeDualSense));tb_char(b,'/');
    tb_u64(b,atomic_load_u32(&g_r9kPhysicalDualSense));
    tb_str(b," direct=openTry/openFail/writeFail/blocks/nonZero/frames/device/peakP=");
    tb_u64(b,atomic_load_u64(&g_r9kDirectOpenAttempts));tb_char(b,'/');tb_u64(b,atomic_load_u64(&g_r9kDirectOpenFailures));tb_char(b,'/');
    tb_u64(b,atomic_load_u64(&g_r9kDirectWriteFailures));tb_char(b,'/');tb_u64(b,atomic_load_u64(&g_r9kDirectBlocks));tb_char(b,'/');
    tb_u64(b,atomic_load_u64(&g_r9kDirectNonZeroBlocks));tb_char(b,'/');tb_u64(b,atomic_load_u64(&g_r9kDirectFrames));tb_char(b,'/');
    tb_u64(b,atomic_load_u32(&g_r9kDirectDeviceId));tb_char(b,'/');tb_u64(b,atomic_load_u32(&g_r9kDirectPeakPermille));
    tb_str(b," observedNativeWhileDirect=");tb_u64(b,atomic_load_u64(&g_r9kNativeObservedWhileDirect));
    tb_endline(b);append_log(b.bytes,b.len);
}

static bool hotkey_pressed(i32 key, bool& wasDown) {
    if (!g_api.GetAsyncKeyState) return false;
    bool down = (g_api.GetAsyncKeyState(key) & (SHORT)0x8000) != 0;
    bool pressed = down && !wasDown;
    wasDown = down;
    return pressed;
}

static DWORD WINABI worker_thread(void*) {
    if (!init_api()) return 0;
    get_game_info();
    if (g_api.DeleteFileW) g_api.DeleteFileW(kLogPath);
    else {
        HANDLE reset = g_api.CreateFileW(kLogPath, GENERIC_WRITE, FILE_SHARE_READ,
                                         nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (reset && reset != invalid_handle_value()) g_api.CloseHandle(reset);
    }

    load_config();
    {
        TextBuf b{};
        SYSTEMTIME st{};
        if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
        tb_char(b, '['); tb_time(b, st);
        tb_str(b, "] ACBFHapticsBridge v8-r9l-v2.3.7.4-1.06-1.07-compat starting gameBase=0x"); tb_hex(b, (u64)g_gameBase);
        tb_str(b, " gameSize=0x"); tb_hex(b, g_gameSize); tb_str(b, " flushRVA=0x");
        tb_hex(b, g_cfg.flushRva); tb_str(b, " masterUser/effective="); tb_float3(b, g_cfg.masterGain);
        tb_char(b, '/'); tb_float3(b, effective_master_gain());
        tb_str(b, " movement="); tb_float3(b, g_cfg.movementGain);
        tb_str(b, " oneShotUser/effective="); tb_float3(b, g_cfg.oneShotGain);
        tb_char(b, '/'); tb_float3(b, effective_one_shot_gain());
        tb_str(b, " nativeHaptics="); tb_u64(b, g_cfg.nativeHaptics);
        tb_str(b, " conventionalRumble="); tb_u64(b, g_cfg.conventionalRumble);
        tb_str(b, " preferGameInput="); tb_u64(b, g_cfg.preferGameInput);
        tb_str(b, " pollMs="); tb_u64(b, g_cfg.pollMs);
        tb_str(b, " giRefreshMs=id/status="); tb_u64(b, g_cfg.gameInputDeviceRefreshMs);
        tb_char(b, '/'); tb_u64(b, g_cfg.gameInputStatusRefreshMs);
        tb_str(b, " rumbleWriteMs="); tb_u64(b, g_cfg.rumbleWriteIntervalMs);
        tb_str(b, " ubiDirectFallback/delayMs="); tb_u64(b, g_cfg.ubisoftDirectDualSenseFallback);
        tb_char(b, '/'); tb_u64(b, g_cfg.ubisoftDirectDualSenseFallbackDelayMs);
        tb_endline(b);
        append_log(b.bytes, b.len);
    }
    {
        TextBuf b{};
        SYSTEMTIME st{};
        if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
        tb_char(b, '['); tb_time(b, st); tb_str(b, "] v8-r9l / v2.3.7.4 1.06+1.07 target discovery; r9k output behavior retained legacyWalkLoopMs=");
        tb_u64(b, g_cfg.walkLoopStartMs); tb_char(b, '-'); tb_u64(b, g_cfg.walkLoopEndMs);
        tb_str(b, " runLoopMs=");
        tb_u64(b, g_cfg.runLoopStartMs); tb_char(b, '-'); tb_u64(b, g_cfg.runLoopEndMs);
        tb_str(b, " parryGain="); tb_float3(b, g_cfg.buttonLBGain);
        tb_str(b, " sprintKickGain="); tb_float3(b, g_cfg.sprintKickGain);
        tb_str(b, " minAirMs="); tb_u64(b, g_cfg.minimumAirborneMs);
        tb_str(b, " landAirMs="); tb_u64(b, g_cfg.landingShortAirMs);
        tb_char(b, '/'); tb_u64(b, g_cfg.landingLongAirMs);
        tb_char(b, '/'); tb_u64(b, g_cfg.landingMaxAirMs);
        tb_str(b, " landGains=min/short/normal/long="); tb_float3(b, g_cfg.landingMinimumGain);
        tb_char(b, '/'); tb_float3(b, g_cfg.landingShortGain);
        tb_char(b, '/'); tb_float3(b, g_cfg.landingGain);
        tb_char(b, '/'); tb_float3(b, g_cfg.landingLongGain);
        tb_str(b, " jumpWindowMs="); tb_u64(b, g_cfg.jumpInputWindowMs);
        tb_str(b, " climbStick="); tb_i64(b, g_cfg.climbJumpStickThreshold);
        tb_str(b, " climbAirCarryMs="); tb_u64(b, g_cfg.climbAirCarryMs);
        tb_str(b, " landDelayMs="); tb_u64(b, g_cfg.landingDelayMs);
        tb_str(b, " stateProbeMs="); tb_u64(b, g_cfg.stateProbeDelayMs);
        tb_str(b, " nativeHold/probeMs="); tb_u64(b, g_cfg.nativeHoldMs);
        tb_char(b, '/'); tb_u64(b, g_cfg.nativeProbeDelayMs);
        tb_str(b, " rbHoldMs="); tb_u64(b, g_cfg.rbHoldThresholdMs);
        tb_str(b, " rbSecondMs="); tb_u64(b, g_cfg.rbHoldSecondDelayMs);
        tb_str(b, " gainModel/normalizedBase/master/oneShot="); tb_u64(b, g_cfg.normalizedGainBaseline);
        tb_char(b, '/'); tb_float3(b, NORMALIZED_MASTER_BASELINE); tb_char(b, '/'); tb_float3(b, NORMALIZED_ONESHOT_BASELINE);
        tb_str(b, " parkourDistance=enabled/refMs/cooldown/maxBoost="); tb_u64(b, g_cfg.parkourDistanceScaling);
        tb_char(b, '/'); tb_u64(b, g_cfg.parkourDistanceReferenceMs); tb_char(b, '/'); tb_u64(b, g_cfg.parkourLandingCooldownMs);
        tb_char(b, '/'); tb_float3(b, g_cfg.parkourDistanceMaxBoost);
        tb_str(b, " autoWater=enabled/moveThreshold/evidenceMs=");
        tb_u64(b, g_cfg.autoWaterEntryReacquire); tb_char(b, '/');
        tb_u64(b, g_cfg.autoWaterMovementThreshold); tb_char(b, '/');
        tb_u64(b, g_cfg.autoWaterEvidenceWindowMs);
        tb_str(b, " trimsMs=state/dodge/dir/parry=");
        tb_u64(b, g_cfg.stateEffectStartTrimMs); tb_char(b, '/');
        tb_u64(b, g_cfg.dodgeStartTrimMs); tb_char(b, '/');
        tb_u64(b, g_cfg.directionalDodgeStartTrimMs); tb_char(b, '/');
        tb_u64(b, g_cfg.parryStartTrimMs);
        tb_str(b, " probesMs=dodge/dir/parry/attack/menu=");
        tb_u64(b, g_cfg.dodgeProbeDelayMs); tb_char(b, '/');
        tb_u64(b, g_cfg.directionalDodgeProbeDelayMs); tb_char(b, '/');
        tb_u64(b, g_cfg.parryProbeDelayMs); tb_char(b, '/');
        tb_u64(b, g_cfg.attackProbeDelayMs); tb_char(b, '/');
        tb_u64(b, g_cfg.menuProbeDelayMs);
        tb_str(b, " menuGain=tick/shoulder=");
        tb_float3(b, g_cfg.menuTickGain); tb_char(b, '/');
        tb_float3(b, g_cfg.menuShoulderGain);
        tb_str(b, " dsTune=global/jumpLand/landMove/menuShoulder/menuTick/xiMenu=");
        tb_float3(b, g_cfg.dualSenseBridgeHapticGain); tb_char(b, '/');
        tb_float3(b, g_cfg.dualSenseJumpLandingHapticGain); tb_char(b, '/');
        tb_float3(b, g_cfg.dualSenseLandMovementHapticGain); tb_char(b, '/');
        tb_float3(b, g_cfg.dualSenseMenuShoulderGain); tb_char(b, '/');
        tb_float3(b, g_cfg.dualSenseMenuTickGain); tb_char(b, '/');
        tb_float3(b, g_cfg.xinputMenuShoulderGain);
        tb_str(b, " dsMenuNativeWindowMs="); tb_u64(b, g_cfg.dualSenseMenuTickNativeWindowMs);
        tb_str(b, " dsWalkHaptic=user/effective/norm jog=");
        tb_float3(b, g_cfg.dualSenseWalkHapticMultiplier); tb_char(b, '/');
        tb_float3(b, effective_dualsense_walk_haptic_multiplier()); tb_char(b, '/');
        tb_u64(b, g_cfg.normalizedWalkHapticBaseline); tb_char(b, '/');
        tb_float3(b, g_cfg.dualSenseJogHapticMultiplier);
        tb_str(b, " nativeCombatBoost=on/gain/windowMs=");
        tb_u64(b, g_cfg.nativeCombatBoost); tb_char(b, '/');
        tb_float3(b, g_cfg.nativeCombatHapticGain); tb_char(b, '/');
        tb_u64(b, g_cfg.nativeCombatBoostWindowMs);
        tb_str(b, " spatialFeet=on/reset/R(L,R)/L(L,R)/sprintOverlay=");
        tb_u64(b, g_cfg.spatialFootsteps); tb_char(b, '/');
        tb_u64(b, g_cfg.footstepResetAfterMs); tb_char(b, '/');
        tb_float3(b, g_cfg.rightFootLeftGain); tb_char(b, ','); tb_float3(b, g_cfg.rightFootRightGain); tb_char(b, '/');
        tb_float3(b, g_cfg.leftFootLeftGain); tb_char(b, ','); tb_float3(b, g_cfg.leftFootRightGain); tb_char(b, '/');
        tb_float3(b, g_cfg.sprintContactOverlayGain);
        tb_str(b, " xiLandRun=stepOnly/stepGain=");
        tb_u64(b, g_cfg.xinputLandRunStepOnly); tb_char(b, '/');
        tb_float3(b, g_cfg.xinputSprintStepGain);
        tb_str(b, " xiActionCaps=jumpLand/light/zeroGapMs=");
        tb_u64(b, g_cfg.xinputJumpLandingMaxDurationMs); tb_char(b, '/');
        tb_u64(b, g_cfg.xinputLightAttackMaxDurationMs); tb_char(b, '/');
        tb_u64(b, g_cfg.xinputActionZeroGapMs);
        tb_str(b, " xiRunPulse=on/interval/duration/gain=");
        tb_u64(b, g_cfg.xinputRunPulseEnabled); tb_char(b, '/');
        tb_u64(b, g_cfg.xinputRunPulseIntervalMs); tb_char(b, '/');
        tb_u64(b, g_cfg.xinputRunPulseDurationMs); tb_char(b, '/');
        tb_float3(b, g_cfg.xinputRunPulseGain);
        tb_str(b, " rawTouchIndex=");
        tb_u64(b, g_cfg.menuRawTouchpadButtonIndex);
        tb_str(b, " recoveryMs=rebind/resume=");
        tb_u64(b, g_cfg.sinkRebindAfterMs); tb_char(b, '/');
        tb_u64(b, g_cfg.reloadSilenceResetMs);
        tb_str(b, " inputLandMs="); tb_u64(b, g_cfg.inputLandingFallbackMs);
        tb_endline(b);
        append_log(b.bytes, b.len);
    }
    log_input_bindings();
    {
        TextBuf b{}; SYSTEMTIME st{}; if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
        tb_char(b,'['); tb_time(b,st); tb_str(b,"] V8R9KCFG semanticEvents/menu/gaitHistory/attackProbe/oneShotProbe/airTimeout=");
        tb_u64(b,g_cfg.semanticEvents); tb_char(b,'/'); tb_u64(b,g_cfg.semanticMenu); tb_char(b,'/');
        tb_u64(b,g_cfg.semanticGaitTimeoutMs); tb_char(b,'/'); tb_u64(b,g_cfg.semanticAttackProbeMs); tb_char(b,'/');
        tb_u64(b,g_cfg.semanticOneShotProbeMs); tb_char(b,'/'); tb_u64(b,g_cfg.semanticAirTimeoutMs);
        tb_str(b," gaitPulseMs=walkStart/dur/runStart/dur=");
        tb_u64(b,g_cfg.semanticWalkPulseStartMs); tb_char(b,'/'); tb_u64(b,g_cfg.semanticWalkPulseDurationMs); tb_char(b,'/');
        tb_u64(b,g_cfg.semanticRunPulseStartMs); tb_char(b,'/'); tb_u64(b,g_cfg.semanticRunPulseDurationMs);
        tb_endline(b); append_log(b.bytes,b.len);
    }
    if (g_cfg.invalidInputBindings)
        log_simple("WARNING one or more InputBindings names were invalid; tested defaults were used (see INPUTCFG)");
    log_effect_controls();
    log_simple("Verified target: Steam TU 1.0.6 SHA-256 8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140");
    log_simple("Verified target: voices38 SHA-256 19920f34bb2fac814023ee3c27f0ceca1872e80a2aafb37c092c419fff77cc0d");
    log_simple("Verified target: attached TU 1.0.7 SHA-256 614dab4a20a5d5c6256792e1daa6d05669c97a751079b10df1725d6965ad766d");
    log_simple("Verified target: Ubisoft Connect SHA-256 e553a964550a9a5d7baef578510974404d6ec40ee89ea228a1323a00c380601c");
    log_simple("Verified target: Ubisoft+ SHA-256 b7a2c38212b1e92b4bd5399d29ceb44965a55b3d83110719cfb8053ad1bb3c75");

    if (!g_gameBase || !g_gameSize) {
        log_simple("ERROR ACBlackFlag.exe module was not found");
        return 0;
    }
    if (!load_haptics_pack()) return 0;
    resolve_mappings();
    bool haveGameInput = init_gameinput();
    bool haveXInput = init_xinput();
    if (g_cfg.nativeHaptics && !install_flush_hook()) return 0;
    if (!g_cfg.nativeHaptics) log_simple("Native haptic injection disabled by INI");
    if (!install_v8_post_event_hook()) return 0;
    install_gameplay_state_hooks();
    log_simple("v8-r9l / v2.3.7.4 routing: v2.3.7.3/r9k gameplay, native-Quad haptics and conventional-rumble behavior retained; 1.06 and 1.07 use discovered verified targets; Ubisoft Connect/Ubisoft+ may use a guarded 48kHz/4-channel DualSense fallback only when meaningful native Quad haptics are absent; XInputSetState import resolution is dynamic with verified-profile fallback");
    if (haveXInput) install_game_xinput_observer();

    XINPUT_STATE previous{};
    bool havePrevious = false;
    bool previousRawTouchpad = false;
    u32 previousBackend = 0;
    bool f9Down = false, f10Down = false, f11Down = false, f12Down = false;
    u64 lastRumbleTick = g_api.GetTickCount64 ? g_api.GetTickCount64() : 0;
    u64 recoverySeen = atomic_load_u64(&g_recoverySerial);
    atomic_store_u64(&g_workerNowTick, lastRumbleTick);
    g_r9kWorkerStartTick = lastRumbleTick;
    u64 lastWorkerPerfTick = perf_counter_now();

    while (atomic_load_u32(&g_running)) {
        u64 workerPerfTick = perf_counter_now();
        if (lastWorkerPerfTick && workerPerfTick)
            perf_record_worker_gap(perf_elapsed_us(lastWorkerPerfTick, workerPerfTick));
        if (workerPerfTick) lastWorkerPerfTick = workerPerfTick;

        u64 loopNow = g_api.GetTickCount64 ? g_api.GetTickCount64() :
                      atomic_load_u64(&g_workerNowTick) + g_cfg.pollMs;
        atomic_store_u64(&g_workerNowTick, loopNow);

        u64 recoverySerial = atomic_load_u64(&g_recoverySerial);
        if (recoverySerial != recoverySeen) {
            recoverySeen = recoverySerial;
            u32 recoveryCause = atomic_exchange_u32(&g_recoveryCause, 0);
            havePrevious = false;
            previousRawTouchpad = false;
            atomic_fetch_add_u64(&g_reloadRecoveries, 1);
            if (recoveryCause & 1u) {
                // Long Wwise silence/resume corresponds to loads, desync/reload
                // transitions, and other cases where control ownership may really
                // have changed. Reacquire from UNKNOWN instead of keeping HELM.
                reset_v8_semantic_state(loopNow);
                atomic_fetch_add_u64(&g_recoveryDomainResets, 1);
                log_simple("RECOVERY long Wwise silence/resume; semantic/menu/game domain reset to UNKNOWN");
            } else {
                // A sink pointer rebound alone is an output-layer event. Preserve
                // current MENU/CHARACTER/WATER/HELM ownership and learned objects.
                clear_deferred_effects();
                set_movement_mode_timed(0, loopNow);
                reset_rb_hold_state();
                g_r4AttackPressActive = false;
                atomic_fetch_add_u64(&g_recoveryDomainPreserves, 1);
                log_simple("RECOVERY Wwise sink rebound; output playback reset, semantic gameplay domain preserved");
            }
        }

        if (hotkey_pressed(VK_F9, f9Down) && atomic_load_u32(&g_enabled)) {
            schedule_configured_effect(g_map.dodge, 1.0f, g_cfg.fxDodge,
                                       108, loopNow, 0, ~0ull,
                                       g_cfg.dodgeStartTrimMs,
                                       g_cfg.semanticDodgeDurationMs);
            log_simple("F9 preview: v8-r9l semantic dodge trim/duration");
        }
        if (hotkey_pressed(VK_F10, f10Down)) { write_status(); log_r9k_output_status(loopNow); }
        if (hotkey_pressed(VK_F11, f11Down)) {
            u32 next = atomic_load_u32(&g_enabled) ? 0u : 1u;
            atomic_store_u32(&g_enabled, next);
            if (!next) {
                reset_v8_semantic_state(loopNow);
                r9k_direct_shutdown();
                log_simple("F11 bridge disabled; all plugin playback and semantic/menu/game states cleared");
            } else {
                // Re-enable from a clean UNKNOWN domain. The next exact menu,
                // character, water, helm or gait event reconstructs live state.
                reset_v8_semantic_state(loopNow);
                log_simple("F11 bridge enabled; semantic router, menu domain, airtime, gait and actor/state caches reloaded");
            }
        }
        if (hotkey_pressed(VK_F12, f12Down)) {
            u32 next = atomic_load_u32(&g_inputEnabled) ? 0u : 1u;
            atomic_store_u32(&g_inputEnabled, next);
            havePrevious = false;
            previousRawTouchpad = false;
            if (!next) {
                reset_menu_mode();
                cancel_automatic_runtime(loopNow, true);
            } else {
                g_gameInputDeviceIdentityTick = 0;
                g_gameplayResumeGuardUntil =
                    loopNow + g_cfg.menuTransitionGuardMs;
            }
            log_simple(next ? "F12 input polling/bridge enabled" :
                              "F12 input polling suspended; semantic/native routing remains active");
        }

        process_v8_semantic_events(loopNow);

        XINPUT_STATE current{};
        bool connected = false;
        u32 backend = 0;
        if (atomic_load_u32(&g_inputEnabled)) {
            if (g_cfg.preferGameInput) {
                if (haveGameInput) {
                    u64 perfStart = perf_counter_now();
                    connected = poll_gameinput(current);
                    perf_record_gameinput(perf_elapsed_us(perfStart, perf_counter_now()));
                    if (connected) backend = 1;
                }
                if (!connected && haveXInput && g_xinputGetState &&
                    g_xinputGetState(g_cfg.controllerIndex, &current) == ERROR_SUCCESS)
                    backend = 2, connected = true;
            } else {
                if (haveXInput && g_xinputGetState &&
                    g_xinputGetState(g_cfg.controllerIndex, &current) == ERROR_SUCCESS)
                    backend = 2, connected = true;
                if (!connected && haveGameInput) {
                    u64 perfStart = perf_counter_now();
                    connected = poll_gameinput(current);
                    perf_record_gameinput(perf_elapsed_us(perfStart, perf_counter_now()));
                    if (connected) backend = 1;
                }
            }

            atomic_store_u32(&g_inputBackend, backend);
            if (connected) {
                if (backend != previousBackend) {
                    havePrevious = false;
                    previousRawTouchpad = false;
                    reset_rb_hold_state();
                }
                if (backend == 2) {
                    atomic_store_u32(&g_lastGameInputButtons, 0);
                    atomic_store_u32(&g_rawControllerButtonCount, 0);
                    atomic_store_u64(&g_rawControllerButtons, 0);
                    atomic_store_u32(&g_rawTouchpadDown, 0);
                    atomic_store_u32(&g_activeVendor, 0);
                    atomic_store_u32(&g_activeProduct, 0);
                    atomic_store_u32(&g_activeDualSense, 0);
                    atomic_store_u32(&g_activeDeviceStatus, 0x7);
                    g_gameInputDeviceIdentityTick = 0;
                    g_gameInputRawButtonCountValid = false;
                }
                atomic_store_u32(&g_controllerConnected, 1);
                atomic_fetch_add_u64(&g_controllerPackets, 1);
                atomic_store_u32(&g_lastControllerButtons, current.Gamepad.wButtons);
                bool rawTouchpadDown = backend == 1 &&
                    atomic_load_u32(&g_rawTouchpadDown) != 0;
                bool rawTouchpadRising = rawTouchpadDown && !previousRawTouchpad;
                process_controller_edges(current, previous, havePrevious,
                                         rawTouchpadRising);
                previous = current;
                previousRawTouchpad = rawTouchpadDown;
                previousBackend = backend;
                havePrevious = true;
            } else {
                atomic_store_u32(&g_controllerConnected, 0);
                atomic_store_u32(&g_lastControllerButtons, 0);
                atomic_store_u32(&g_lastGameInputButtons, 0);
                atomic_store_u32(&g_rawControllerButtonCount, 0);
                atomic_store_u64(&g_rawControllerButtons, 0);
                atomic_store_u32(&g_rawTouchpadDown, 0);
                atomic_store_u32(&g_activeDeviceStatus, 0);
                atomic_store_u32(&g_activeVendor, 0);
                atomic_store_u32(&g_activeProduct, 0);
                atomic_store_u32(&g_activeDualSense, 0);
                if (!g_cfg.semanticEvents) atomic_store_u32(&g_movementMode, 0);
                atomic_store_u32(&g_currentJumpInputDown, 0);
                atomic_store_u32(&g_currentLeftStickMagnitudeSq, 0);
                atomic_store_u32(&g_lastJumpStickMagnitudeSq, 0);
                g_sprintLatched = false;
                r4_reset_hybrid_runtime();
                reset_rb_hold_state();
                reset_menu_mode();
                cancel_input_landing_fallback(false);
                clear_deferred_effects();
                havePrevious = false;
                previousRawTouchpad = false;
                previousBackend = 0;
                // Force immediate device metadata reacquisition after a reconnect.
                g_gameInputDeviceIdentityTick = 0;
            }
        }

        u64 now = g_api.GetTickCount64 ? g_api.GetTickCount64() : lastRumbleTick + g_cfg.pollMs;
        atomic_store_u64(&g_workerNowTick, now);
        if (atomic_load_u32(&g_menuMode) || now < g_gameplayResumeGuardUntil ||
            !atomic_load_u32(&g_enabled) ||
            !atomic_load_u32(&g_inputEnabled)) {
            synchronize_state_event_cursor();
        } else {
            process_gameplay_state_signals(now);
        }
        process_deferred_effects(now);
        r9k_service_ubisoft_direct_haptics(now);
        u64 elapsed = lastRumbleTick ? now - lastRumbleTick : g_cfg.pollMs;
        lastRumbleTick = now;
        if (elapsed < 1) elapsed = 1;
        if (elapsed > 50) elapsed = 50;
        RumbleSample rumble = render_rumble((u32)elapsed * 48u, now);
        u64 rumblePerfStart = perf_counter_now();
        write_rumble_output(rumble);
        perf_record_rumble_write(
            perf_elapsed_us(rumblePerfStart, perf_counter_now()));

        g_api.Sleep(g_cfg.pollMs);
    }
    atomic_store_u32(&g_movementMode, 0);
    r9k_direct_shutdown();
    r4_reset_hybrid_runtime();
    reset_rb_hold_state();
    reset_menu_mode();
    cancel_input_landing_fallback(false);
    clear_deferred_effects();
    atomic_fetch_add_u32(&g_cancelGeneration, 1);
    RumbleSample stop{};
    stop.timelineActive = false;
    g_rumbleOutputWasActive = true;
    write_rumble_output(stop);
    if (g_gameInputDevice) { com_release(g_gameInputDevice); g_gameInputDevice = nullptr; }
    if (g_gameInput) { com_release(g_gameInput); g_gameInput = nullptr; }
    log_simple("Worker ending");
    return 0;
}

extern "C" BOOL WINABI DllMain(void*, DWORD reason, void*) {
    if (reason == DLL_PROCESS_ATTACH) {
        get_game_info();
        if (init_bootstrap_api() && g_api.CreateThread) {
            DWORD tid = 0;
            g_api.CreateThread(nullptr, 0, &worker_thread, nullptr, 0, &tid);
        }
    } else if (reason == DLL_PROCESS_DETACH) {
        atomic_store_u32(&g_running, 0);
    }
    // Referencing the relocation anchor prevents aggressive link-time removal.
    if (!g_relocAnchor) return 0;
    return 1;
}
