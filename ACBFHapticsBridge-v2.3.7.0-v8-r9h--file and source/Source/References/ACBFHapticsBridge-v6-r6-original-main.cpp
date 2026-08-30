// ACBFHapticsBridge v6-r6 controller-specific movement-tuning build
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

static u8* g_gameBase{};
static u32 g_gameSize{};
static volatile u32 g_running = 1;
static volatile u32 g_enabled = 1;
static volatile u32 g_inputEnabled = 1;

static constexpr WCHAR kIniPath[] = L".\\ACBFHapticsBridge-v6.ini";
static constexpr WCHAR kLogPath[] = L".\\ACBFHapticsBridge-v6.log";

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
    u32 nativeHaptics = 1;
    u32 conventionalRumble = 1;
    u32 preferGameInput = 1;
    u32 rumbleOnDualSense = 1;
    u32 controllerIndex = 0;
    u32 pollMs = 8;
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
    u32 inputJumpFallback = 1;
    u32 inputLandingFallbackMs = 2000;
    u32 inputLandingTimeoutMs = 8000;
    u32 stateEffectStartTrimMs = 236;
    i32 climbJumpStickThreshold = 4500;
    u32 landingShortAirMs = 900;
    u32 landingLongAirMs = 2500;
    u32 landingMaxAirMs = 6000;
    u32 nativeArbitration = 1;
    u32 nativeHoldMs = 220;
    u32 nativeProbeDelayMs = 220;
    u32 dodgeProbeDelayMs = 20;
    u32 directionalDodgeProbeDelayMs = 20;
    u32 parryProbeDelayMs = 60;
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
    u32 runOpeningDurationMs = 5000;
    u32 jumpCooldownMs = 300;
    u32 landingCooldownMs = 500;
    u32 haystackCooldownMs = 700;
    u32 diveStateCooldownMs = 900;
    u32 diveInputCooldownMs = 250;
    u32 surfaceCooldownMs = 700;
    u32 waterJumpCooldownMs = 250;
    u32 menuRepeatDelayMs = 360;
    u32 menuRepeatIntervalMs = 180;
    u32 xinputRunPulseEnabled = 1;
    u32 xinputRunPulseIntervalMs = 300;
    u32 xinputRunPulseDurationMs = 75;
    u32 rbHoldThresholdMs = 320;
    u32 rbHoldSecondDelayMs = 640;
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
    float dualSenseWalkHapticMultiplier = 2.0f;
    float runBaseGain = 1.25f;
    float runOpeningGain = 1.50f;
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
    float landingShortGain = 0.55f;
    float landingGain = 0.70f;
    float landingLongGain = 1.00f;
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
    EffectControl fxParry{};
    EffectControl fxLightAttack{};
    EffectControl fxHeavyFirst{};
    EffectControl fxHeavySecond{};
    EffectControl fxSheatheHood{};
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

// These device fields are declared before the rumble renderer because r6
// selects its spaced running envelope from the currently presented device.
static volatile u32 g_inputBackend{}; // 0=none, 1=GameInput, 2=XInput
static volatile u32 g_activeVendor{};
static volatile u32 g_activeProduct{};
static volatile u32 g_activeDualSense{};

static void load_config() {
    g_cfg.enabled = (u32)read_ini_i32(L"General", L"Enabled", (i32)g_cfg.enabled);
    g_cfg.inputBridge = (u32)read_ini_i32(L"General", L"InputBridge", (i32)g_cfg.inputBridge);
    g_cfg.nativeHaptics = (u32)read_ini_i32(
        L"Output", L"NativeHaptics", (i32)g_cfg.nativeHaptics);
    g_cfg.conventionalRumble = (u32)read_ini_i32(
        L"Output", L"ConventionalRumble", (i32)g_cfg.conventionalRumble);
    g_cfg.preferGameInput = (u32)read_ini_i32(
        L"Output", L"PreferGameInput", (i32)g_cfg.preferGameInput);
    g_cfg.rumbleOnDualSense = (u32)read_ini_i32(
        L"Output", L"RumbleOnDualSense", (i32)g_cfg.rumbleOnDualSense);
    g_cfg.controllerIndex = (u32)read_ini_i32(L"General", L"ControllerIndex", (i32)g_cfg.controllerIndex);
    g_cfg.pollMs = (u32)read_ini_i32(L"General", L"PollMs", (i32)g_cfg.pollMs);
    if (g_cfg.pollMs < 2) g_cfg.pollMs = 2;
    if (g_cfg.pollMs > 100) g_cfg.pollMs = 100;
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
    if (g_cfg.inputLandingFallbackMs < 250) g_cfg.inputLandingFallbackMs = 250;
    if (g_cfg.inputLandingFallbackMs > 10000) g_cfg.inputLandingFallbackMs = 10000;
    if (g_cfg.inputLandingTimeoutMs < g_cfg.inputLandingFallbackMs)
        g_cfg.inputLandingTimeoutMs = g_cfg.inputLandingFallbackMs;
    if (g_cfg.inputLandingTimeoutMs > 60000) g_cfg.inputLandingTimeoutMs = 60000;
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
    g_cfg.dualSenseWalkHapticMultiplier = read_ini_float(
        L"Effect.Walk", L"DualSenseHapticMultiplier",
        g_cfg.dualSenseWalkHapticMultiplier);
    g_cfg.runBaseGain = read_ini_float(L"Movement", L"RunBaseGain", g_cfg.runBaseGain);
    g_cfg.runOpeningGain = read_ini_float(
        L"Movement", L"RunOpeningGain", g_cfg.runOpeningGain);
    g_cfg.rumbleGain = read_ini_float(L"Rumble", L"Gain", g_cfg.rumbleGain);
    g_cfg.rumbleMovementGain = read_ini_float(
        L"Rumble", L"MovementGain", g_cfg.rumbleMovementGain);
    g_cfg.rumbleLowScale = read_ini_float(L"Rumble", L"LowMotorScale", g_cfg.rumbleLowScale);
    g_cfg.rumbleHighScale = read_ini_float(L"Rumble", L"HighMotorScale", g_cfg.rumbleHighScale);
    g_cfg.rumbleDeadzone = read_ini_float(L"Rumble", L"Deadzone", g_cfg.rumbleDeadzone);
    g_cfg.xinputRunPulseGain = read_ini_float(
        L"Rumble", L"XInputRunPulseGain", g_cfg.xinputRunPulseGain);
    g_cfg.buttonLBGain = read_ini_float(L"Buttons", L"LBEffectGain", g_cfg.buttonLBGain);
    g_cfg.buttonRBGain = read_ini_float(L"Buttons", L"RBEffectGain", g_cfg.buttonRBGain);
    g_cfg.rbHoldGain = read_ini_float(L"Buttons", L"RBHoldGain", g_cfg.rbHoldGain);
    g_cfg.sprintKickGain = read_ini_float(
        L"Movement", L"SprintKickGain", g_cfg.sprintKickGain);
    g_cfg.jumpGain = read_ini_float(L"GameplayState", L"JumpGain", g_cfg.jumpGain);
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
    if (g_cfg.dualSenseWalkHapticMultiplier > 4.0f)
        g_cfg.dualSenseWalkHapticMultiplier = 4.0f;
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
        g_cfg.rbHoldSecondDelayMs, 3000);

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

static constexpr u32 COMMAND_RING_SIZE = 64;
struct TriggerCommand {
    volatile u32 ready{};
    i32 effect{-1};
    float gain{1.0f};
    u32 source{};
    u32 startFrame{};
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
                                u32 startFrame = 0) {
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
    cmd.source = source;
    cmd.startFrame = bounded_start_frame(effect, startFrame);
    cmd.generation = atomic_load_u32(&g_cancelGeneration);
    atomic_store_u32(&cmd.ready, write + 1);
    atomic_store_u32(&g_commandWrite, write + 1);
    atomic_fetch_add_u64(&g_commandsQueued, 1);
    return true;
}

static bool queue_rumble_effect(i32 effect, float gain, u32 source,
                                u32 startFrame = 0) {
    if (!g_cfg.conventionalRumble || effect < 0 || (u32)effect >= g_effectCount) return false;
    u32 write = atomic_load_u32(&g_rumbleCommandWrite);
    u32 read = atomic_load_u32(&g_rumbleCommandRead);
    if (write - read >= COMMAND_RING_SIZE) {
        atomic_fetch_add_u64(&g_rumbleCommandsDropped, 1);
        return false;
    }
    TriggerCommand& cmd = g_rumbleCommands[write & (COMMAND_RING_SIZE - 1)];
    cmd.effect = effect;
    cmd.gain = gain;
    cmd.source = source;
    cmd.startFrame = bounded_start_frame(effect, startFrame);
    cmd.generation = atomic_load_u32(&g_cancelGeneration);
    atomic_store_u32(&cmd.ready, write + 1);
    atomic_store_u32(&g_rumbleCommandWrite, write + 1);
    atomic_fetch_add_u64(&g_rumbleCommandsQueued, 1);
    return true;
}

static bool queue_effect(i32 effect, float gain, u32 source,
                         u32 startFrame = 0) {
    bool haptic = g_cfg.nativeHaptics ?
        queue_haptic_effect(effect, gain, source, startFrame) : false;
    bool rumble = queue_rumble_effect(effect, gain, source, startFrame);
    return haptic || rumble;
}

static constexpr u32 DEFERRED_EFFECT_COUNT = 32;
struct DeferredEffect {
    bool active{};
    i32 effect{-1};
    float gain{1.0f};
    u32 source{};
    u32 startFrame{};
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
                                       u32 startFrame = 0) {
    if (!atomic_load_u32(&g_enabled) || !atomic_load_u32(&g_inputEnabled))
        return false;
    if (effect < 0 || (u32)effect >= g_effectCount) return false;
    u64 nativeSerial = atomic_load_u64(&g_nativeFeedbackSerial);
    if (nativeSerialBaseline == ~0ull) nativeSerialBaseline = nativeSerial;
    if (native_feedback_active(now) || nativeSerial != nativeSerialBaseline) {
        atomic_fetch_add_u64(&g_nativeSuppressedEffects, 1);
        return false;
    }
    if (!g_cfg.nativeArbitration || !delayMs) {
        return queue_effect(effect, gain, source, startFrame);
    }
    for (u32 i = 0; i < DEFERRED_EFFECT_COUNT; ++i) {
        if (g_deferredEffects[i].active) continue;
        g_deferredEffects[i].effect = effect;
        g_deferredEffects[i].gain = gain;
        g_deferredEffects[i].source = source;
        g_deferredEffects[i].startFrame = bounded_start_frame(effect, startFrame);
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
                                       u32 calibratedStartTrimMs = 0) {
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
                                      frames_from_ms(startTrimMs));
}

static void process_deferred_effects(u64 now) {
    for (u32 i = 0; i < DEFERRED_EFFECT_COUNT; ++i) {
        DeferredEffect& item = g_deferredEffects[i];
        if (!item.active || now < item.dueTick) continue;
        i32 effect = item.effect;
        float gain = item.gain;
        u32 source = item.source;
        u32 startFrame = item.startFrame;
        u64 nativeSerialBaseline = item.nativeSerialBaseline;
        item = {};
        if (native_feedback_active(now) ||
            atomic_load_u64(&g_nativeFeedbackSerial) != nativeSerialBaseline) {
            atomic_fetch_add_u64(&g_nativeSuppressedEffects, 1);
            continue;
        }
        queue_effect(effect, gain, source, startFrame);
    }
}

static constexpr u32 MAX_VOICES = 8;
struct PlaybackVoice {
    i32 effect{-1};
    u64 position{};
    float gain{1.0f};
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
        g_voices[i].gain = 1.0f;
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
        g_rumbleVoices[i].gain = 1.0f;
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
}

static void start_voice(i32 effect, float gain, u32 startFrame, u32 source) {
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
    v.gain = gain;
    v.serial = ++g_voiceSerial;
    v.source = source;
}

static void start_rumble_voice(i32 effect, float gain, u32 startFrame,
                               u32 source) {
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
    v.gain = gain;
    v.serial = ++g_rumbleVoiceSerial;
    v.source = source;
}

static void drain_commands() {
    for (;;) {
        u32 read = atomic_load_u32(&g_commandRead);
        TriggerCommand& cmd = g_commands[read & (COMMAND_RING_SIZE - 1)];
        if (atomic_load_u32(&cmd.ready) != read + 1) break;
        if (cmd.generation == atomic_load_u32(&g_cancelGeneration))
            start_voice(cmd.effect, cmd.gain, cmd.startFrame, cmd.source);
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
                               cmd.source);
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
                                  float gain, float& low, float& high) {
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
    float candidateLow = (sum / (float)measured) * g_cfg.rumbleLowScale * gain;
    float candidateHigh = peak * g_cfg.rumbleHighScale * gain;
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
    bool pulsedXInputRun = desiredEffect == g_map.run &&
        use_xinput_run_pulses(desiredMode);

    u32 activeVoices = 0;
    const float oneShotGain = g_cfg.masterGain * g_cfg.oneShotGain * g_cfg.rumbleGain;
    for (u32 i = 0; i < MAX_VOICES; ++i) {
        PlaybackVoice& voice = g_rumbleVoices[i];
        if (voice.effect < 0 || (u32)voice.effect >= g_effectCount) continue;
        const Effect& effect = g_effects[voice.effect];
        if (voice.position >= effect.frames) { voice.effect = -1; continue; }
        // The initial parry-derived sprint kick would bridge the first two
        // pulse gaps and make Xbox running feel continuous again. Its native
        // haptic copy remains intact; only the XInput motor copy yields to the
        // new gait envelope.
        if (!(pulsedXInputRun && voice.source == SOURCE_SPRINT_KICK))
            measure_rumble_window(effect, voice.position, frames, false, 0,
                                  effect.frames, oneShotGain * voice.gain,
                                  result.low, result.high);
        u64 remaining = effect.frames - voice.position;
        voice.position += (u64)frames < remaining ? frames : remaining;
        if (voice.position >= effect.frames) voice.effect = -1;
        else ++activeVoices;
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
        float gain = g_cfg.masterGain * g_cfg.movementGain *
                     movement_dynamic_scale() * g_cfg.rumbleMovementGain *
                     g_cfg.rumbleGain;
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
static volatile u64 g_flushCalls{};
static volatile u64 g_injectedBlocks{};
static volatile u64 g_injectedFrames{};
static volatile u64 g_invalidSinkFrames{};
static volatile u32 g_lastMaxFrames{};
static volatile u32 g_flushThreadId{};
static u32 g_audioCancelSeen{};

static bool detect_native_haptic_buffer(const float* left, const float* right,
                                        u32 validFrames, u64 now) {
    if (!g_cfg.nativeArbitration || !left || !right || !validFrames) return false;
    float peak = 0.0f;
    float sum = 0.0f;
    for (u32 i = 0; i < validFrames; ++i) {
        float a = absf(left[i]);
        float b = absf(right[i]);
        sum += a + b;
        if (a > peak) peak = a;
        if (b > peak) peak = b;
    }
    float mean = sum / (float)(validFrames * 2u);
    atomic_store_u32(&g_lastNativePeakMillion,
                     (u32)(clampf(peak, 0.0f, 4.0f) * 1000000.0f));
    atomic_store_u32(&g_lastNativeMeanMillion,
                     (u32)(clampf(mean, 0.0f, 4.0f) * 1000000.0f));
    // The mean guard rejects stale sub-audible residue.  The high-peak branch
    // still catches very short impact impulses that occupy only a few frames.
    bool detected =
        (peak >= g_cfg.nativeHapticPeakThreshold &&
         mean >= g_cfg.nativeHapticMeanThreshold) ||
        peak >= g_cfg.nativeHapticPeakThreshold * 4.0f;
    if (detected) {
        atomic_extend_until(&g_nativeHapticUntil, now + g_cfg.nativeHoldMs);
        atomic_fetch_add_u64(&g_nativeHapticDetections, 1);
        atomic_fetch_add_u64(&g_nativeFeedbackSerial, 1);
    }
    return detected;
}

static bool mix_one_shots(float* left, float* right, u32 frames) {
    bool active = false;
    const float baseGain = g_cfg.masterGain * g_cfg.oneShotGain;
    for (u32 vindex = 0; vindex < MAX_VOICES; ++vindex) {
        PlaybackVoice& voice = g_voices[vindex];
        if (voice.effect < 0 || (u32)voice.effect >= g_effectCount) continue;
        const Effect& effect = g_effects[voice.effect];
        u64 pos = voice.position;
        if (pos >= effect.frames) { voice.effect = -1; continue; }
        active = true;
        float gain = baseGain * voice.gain;
        u32 count = frames;
        u64 remain = effect.frames - pos;
        if ((u64)count > remain) count = (u32)remain;
        const float* samples = effect.interleaved + pos * 2ull;
        for (u32 i = 0; i < count; ++i) {
            left[i] += samples[i * 2u] * gain;
            right[i] += samples[i * 2u + 1u] * gain;
        }
        voice.position += count;
        if (voice.position >= effect.frames) voice.effect = -1;
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
        g_cfg.dualSenseWalkHapticMultiplier : 1.0f;
    float target = desiredMode && desiredEffect == g_movement.effect ?
        g_cfg.movementGain * movement_dynamic_scale() * hapticOutputScale :
        0.0f;
    float configuredFadeMs = g_movement.effect == g_map.run ?
        g_cfg.runFadeMs : g_cfg.walkFadeMs;
    u32 fadeFrames = (u32)(48000.0f * configuredFadeMs / 1000.0f);
    if (fadeFrames > loopFrames / 4ull) fadeFrames = (u32)(loopFrames / 4ull);
    float maxScale = g_cfg.walkGain * g_cfg.fxWalk.intensityMultiplier *
        g_cfg.dualSenseWalkHapticMultiplier;
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

        float gain = g_movement.gain * g_cfg.masterGain;
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
    detect_native_haptic_buffer(left, right, validFrames, now);

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

using FlushFn = void (FASTABI*)(void*);
static FlushFn g_originalFlush{};
static void* g_flushTrampoline{};
static volatile u32 g_hookInstalled{};

static void FASTABI flush_hook(void* self) {
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
    bool rebound = false;
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
            rebound = true;
            atomic_store_u64(&g_primarySinkLastTick, now);
            atomic_fetch_add_u64(&g_sinkRebinds, 1);
        } else {
            primary = atomic_load_u64(&g_primarySink);
        }
    }
    if (sink && sink == primary) {
        atomic_store_u64(&g_primarySinkLastTick, now);
    }
    if (resumed || rebound) {
        if (resumed) atomic_fetch_add_u64(&g_flushResumes, 1);
        // The worker clears actor/menu inference after observing this serial.
        // Advancing the playback generation here prevents stale pre-load
        // commands from reaching a newly-created Wwise sink first.
        atomic_fetch_add_u32(&g_cancelGeneration, 1);
        atomic_fetch_add_u64(&g_recoverySerial, 1);
    }
    if (sink && sink == primary) inject_sink_block(self);
    if (g_originalFlush) g_originalFlush(self);
}

static bool bytes_equal(const u8* a, const u8* b, u32 count) {
    for (u32 i = 0; i < count; ++i) if (a[i] != b[i]) return false;
    return true;
}

static bool install_flush_hook() {
    if (!g_gameBase || !g_cfg.flushRva || g_cfg.flushRva + 16 > g_gameSize) {
        log_simple("ERROR configured flush RVA is outside ACBlackFlag.exe");
        return false;
    }
    u8* target = g_gameBase + g_cfg.flushRva;
    // Exact prologue of AkQuadAudioHapticsSink's frame-final flush in the
    // Black Flag Resynced Steam TU 1.0.6 executable (SHA-256 8d522381...).
    static const u8 signature[16] = {
        0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57,
        0x48,0x83,0xEC,0x20,0x0F,0xB7,0x79,0x42
    };
    if (!bytes_equal(target, signature, 16)) {
        log_simple("ERROR QuadAudioHaptics flush signature mismatch; no code was patched");
        return false;
    }

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
    log_simple("Installed Black Flag native Quad Audio Haptics flush injector at RVA 0x04F78610");
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
    if (newBinding) {
        atomic_store_u64(&g_playerContext, actor);
        atomic_store_u32(&g_characterStateFlags, 0);
        atomic_fetch_add_u64(&g_playerBindCount, 1);
    }
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
        bool explicitPlayerAction =
            (event == GSE_JUMP || event == GSE_SWIM_ENTER) &&
            bindMask && bind_input_is_recent(now, bindMask, event);
        if (bindMask && (stale || explicitPlayerAction) &&
            bind_input_is_recent(now, bindMask, event)) {
            newBinding = true;
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
        if ((inputBit == BIND_JUMP && event == GSE_JUMP) ||
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

static void install_gameplay_state_hooks() {
    if (!g_cfg.gameplayStateHooks) {
        log_simple("Gameplay state hooks disabled by INI; button fallbacks remain active");
        return;
    }
    // Slot RVAs and original-function RVAs are for the exact fingerprint
    // logged at startup.  Each pair is verified independently.
    patch_state_vtable(0x09151C18ull, 0x015273A0ull, &state_air_enter, &g_origAirEnter, "InAir enter");
    patch_state_vtable(0x09151C20ull, 0x017FC510ull, &state_air_exit, &g_origAirExit, "InAir exit");
    patch_state_vtable(0x09152898ull, 0x05E7F6C0ull, &state_jump_spot, &g_origJumpOnSpot, "JumpOnSpot enter");
    patch_state_vtable(0x09152848ull, 0x06B118E0ull, &state_free_jump, &g_origFreeJump, "FreeJump enter");
    patch_state_vtable(0x0914E4E8ull, 0x02750BF0ull, &state_targeted_jump, &g_origTargetedJump, "TargetedJump enter");
    patch_state_vtable(0x0914E3F8ull, 0x01FCD920ull, &state_freefall, &g_origFreefall, "Freefall enter");
    patch_state_vtable(0x09151F38ull, 0x06B01790ull, &state_ledge_fall_enter, &g_origLedgeFallEnter, "FallFromLedge enter");
    patch_state_vtable(0x09151F40ull, 0x06B01A30ull, &state_ledge_fall_exit, &g_origLedgeFallExit, "FallFromLedge exit");
    patch_state_vtable(0x0914E218ull, 0x06AFDB60ull, &state_transition_exit, &g_origTransitionExit, "TransitionExit enter");
    patch_state_vtable(0x09147A38ull, 0x0152D180ull, &state_climb_a_enter, &g_origClimbAEnter, "Climb A enter");
    patch_state_vtable(0x09147A40ull, 0x0102CC10ull, &state_climb_a_exit, &g_origClimbAExit, "Climb A exit");
    patch_state_vtable(0x0920DC90ull, 0x06A98A70ull, &state_climb_b_enter, &g_origClimbBEnter, "Climb B enter");
    patch_state_vtable(0x0920DC98ull, 0x06A99E00ull, &state_climb_b_exit, &g_origClimbBExit, "Climb B exit");
    patch_state_vtable(0x09159718ull, 0x027D3CF0ull, &state_swim_a_enter, &g_origSwimAEnter, "Swimming A enter");
    patch_state_vtable(0x09159720ull, 0x018A3D00ull, &state_swim_a_exit, &g_origSwimAExit, "Swimming A exit");
    patch_state_vtable(0x091A3928ull, 0x08065C00ull, &state_swim_b_enter, &g_origSwimBEnter, "Swimming B enter");
    patch_state_vtable(0x091A3930ull, 0x08066540ull, &state_swim_b_exit, &g_origSwimBExit, "Swimming B exit");
    patch_state_vtable(0x0915AB98ull, 0x06C9ED00ull, &state_shallow_enter, &g_origShallowEnter, "ShallowSwim enter");
    patch_state_vtable(0x0915ABA0ull, 0x06C9FAC0ull, &state_shallow_exit, &g_origShallowExit, "ShallowSwim exit");
    patch_state_vtable(0x09145208ull, 0x01B6C060ull, &state_haystack_enter, &g_origHaystackEnter, "HayStack enter");
    patch_state_vtable(0x09145210ull, 0x054F1850ull, &state_haystack_exit, &g_origHaystackExit, "HayStack exit");

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
static volatile u32 g_controllerConnected{};
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

static void install_game_xinput_observer() {
    // ACBlackFlag.exe imports XInputSetState through this exact IAT slot.
    // Patching the slot observes only the game's writes; the bridge calls the
    // resolved export directly, so its own conventional-rumble output cannot
    // be misclassified as native feedback.
    static constexpr u64 kXInputSetStateIatRva = 0x1D0119E0ull;
    if (!g_cfg.nativeArbitration || !g_xinputSetState || !g_gameBase ||
        kXInputSetStateIatRva + sizeof(u64) > g_gameSize) return;
    u64* slot = (u64*)(g_gameBase + kXInputSetStateIatRva);
    if (*slot != (u64)g_xinputSetState) {
        atomic_fetch_add_u32(&g_gameXInputObserverFailures, 1);
        log_simple("WARNING native XInput observer IAT verification failed; leaving slot untouched");
        return;
    }
    DWORD oldProtect = 0;
    if (!g_api.VirtualProtect(slot, sizeof(u64), PAGE_READWRITE, &oldProtect)) {
        atomic_fetch_add_u32(&g_gameXInputObserverFailures, 1);
        log_simple("WARNING native XInput observer could not change IAT protection");
        return;
    }
    g_gameXInputSetStateOriginal = api_cast<XInputSetStateFn>((void*)*slot);
    *slot = (u64)&game_xinput_set_state_observer;
    DWORD ignored = 0;
    g_api.VirtualProtect(slot, sizeof(u64), oldProtect, &ignored);
    atomic_store_u32(&g_gameXInputObserverInstalled, 1);
    log_simple("Installed verified game XInputSetState native-rumble observer at IAT RVA 0x1D0119E0");
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

static void update_gameinput_device(void* device) {
    if (!device) return;
    if (device == g_gameInputDevice) {
        com_release(device);
    } else {
        if (g_gameInputDevice) com_release(g_gameInputDevice);
        g_gameInputDevice = device; // GetDevice supplies the retained reference.
    }
    GameInputDeviceGetInfoFn getInfo = api_cast<GameInputDeviceGetInfoFn>(
        com_method(g_gameInputDevice, 3));
    GameInputDeviceGetStatusFn getStatus = api_cast<GameInputDeviceGetStatusFn>(
        com_method(g_gameInputDevice, 4));
    const GameInputDeviceInfoPrefixV0* info = getInfo ? getInfo(g_gameInputDevice) : nullptr;
    u16 vendor = info ? info->vendorId : 0;
    u16 product = info ? info->productId : 0;
    atomic_store_u32(&g_activeVendor, vendor);
    atomic_store_u32(&g_activeProduct, product);
    atomic_store_u32(&g_activeDualSense, is_dualsense_product(vendor, product) ? 1u : 0u);
    atomic_store_u32(&g_activeDeviceStatus, getStatus ? getStatus(g_gameInputDevice) : 0u);
    install_gameinput_rumble_observer(g_gameInputDevice);
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
    GameInputReadingGetDeviceFn getDevice = api_cast<GameInputReadingGetDeviceFn>(
        com_method(reading, 6));
    void* device = nullptr;
    if (getDevice) getDevice(reading, &device);
    if (device) update_gameinput_device(device);

    u32 rawCount = 0;
    u64 rawMask = 0;
    bool rawTouchpad = false;
    if (g_cfg.menuRawTouchpad) {
        GameInputReadingGetControllerButtonCountFn getButtonCount =
            api_cast<GameInputReadingGetControllerButtonCountFn>(
                com_method(reading, 10));
        GameInputReadingGetControllerButtonStateFn getButtonState =
            api_cast<GameInputReadingGetControllerButtonStateFn>(
                com_method(reading, 11));
        rawCount = getButtonCount ? getButtonCount(reading) : 0;
        u32 request = rawCount > 64u ? 64u : rawCount;
        u8 raw[64]{};
        u32 copied = request && getButtonState ?
            getButtonState(reading, request, raw) : 0;
        if (copied < request) request = copied;
        for (u32 i = 0; i < request; ++i)
            if (raw[i]) rawMask |= 1ull << i;
        bool allowedVendor = !g_cfg.menuRawTouchpadSonyOnly ||
            atomic_load_u32(&g_activeVendor) == 0x054Cu;
        u32 index = g_cfg.menuRawTouchpadButtonIndex;
        rawTouchpad = allowedVendor && index < request && raw[index] != 0;
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
    if (stateFlags & (CS_AIR_MASK | CS_CLIMB_MASK | CS_HAYSTACK)) {
        // Never replay ground contacts while airborne or climbing.  This is
        // the state-aware guard that stick input alone could not provide.
        set_movement_mode_timed(0, now);
    } else if (stateFlags & CS_WATER_MASK) {
        // Preserve the configured swim-acceleration texture and start kick,
        // but do not turn ordinary swimming stick motion into footsteps.
        set_movement_mode_timed(sprinting && magnitudeSquared >= (u64)(run * run) ? 2u : 0u, now);
    } else if (sprinting && magnitudeSquared >= (u64)(run * run)) {
        set_movement_mode_timed(2, now);
    } else if (magnitudeSquared >= (u64)(walk * walk)) {
        set_movement_mode_timed(1, now);
    } else {
        set_movement_mode_timed(0, now);
    }
}

static u64 g_stateSeen[GSE_COUNT]{};
static bool g_hadAirborne{};
static bool g_airborneConfirmedJump{};
static bool g_airborneLongFall{};
static bool g_landingPending{};
static bool g_surfacePending{};
static u64 g_airborneStartTick{};
static u64 g_lastAirborneEndTick{};
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
static volatile u32 g_currentLeftStickMagnitudeSq{};
static volatile u32 g_lastJumpStickMagnitudeSq{};
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
    if (durationMs <= (u64)g_cfg.landingShortAirMs) {
        category = 1;
        return g_cfg.landingShortGain;
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
        g_airborneStartTick = now;
        g_airborneConfirmedJump = false;
        g_airborneLongFall = false;
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
    for (u32 event = 0; event < GSE_COUNT; ++event) {
        u64 sequence = atomic_load_u64(&g_stateAccepted[event]);
        if (sequence == g_stateSeen[event]) continue;
        g_stateSeen[event] = sequence;
        switch (event) {
        case GSE_AIR_ENTER:
            begin_airborne_cycle(now, event);
            break;
        case GSE_AIR_EXIT:
            arm_ground_landing(now);
            break;
        case GSE_JUMP:
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
            // Grabbing or traversing a climb surface is not a ground landing.
            clear_airborne_cycle();
            cancel_input_landing_fallback(true);
            break;
        case GSE_SWIM_ENTER:
            if (g_airborneStartTick && now >= g_airborneStartTick)
                remember_air_duration(now - g_airborneStartTick);
            g_surfacePending = false;
            clear_airborne_cycle();
            cancel_input_landing_fallback(true);
            if (effect_cooldown_elapsed(now, g_lastDiveEffectTick,
                                        g_cfg.diveStateCooldownMs)) {
                schedule_configured_effect(g_map.dive, g_cfg.diveGain,
                                           g_cfg.fxDive, 203, now,
                                           g_cfg.stateProbeDelayMs, ~0ull,
                                           g_cfg.stateEffectStartTrimMs);
                g_lastDiveEffectTick = now;
                log_simple("STATE effect: water entry / dive splash");
            }
            break;
        case GSE_SWIM_EXIT:
            g_surfacePending = true;
            g_surfaceDueTick = now + g_cfg.surfaceDelayMs;
            break;
        case GSE_SHALLOW_ENTER:
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
        } else if (flags & (CS_CLIMB_MASK | CS_WATER_MASK | CS_HAYSTACK)) {
            cancel_input_landing_fallback(true);
            clear_airborne_cycle();
        } else {
            g_landingPending = false;
            if (effect_cooldown_elapsed(now, g_lastLandingEffectTick,
                                        g_cfg.landingCooldownMs)) {
                u32 category = 1;
                float gain = landing_gain_for_airtime(g_pendingLandingAirMs,
                                                       category);
                schedule_configured_effect(
                    g_map.landing, gain, landing_control_for_category(category),
                    202, now, g_cfg.stateProbeDelayMs, ~0ull,
                    g_cfg.stateEffectStartTrimMs);
                g_lastLandingEffectTick = now;
                atomic_fetch_add_u64(&g_groundLandingEffects, 1);
                atomic_store_u32(&g_lastLandingCategory, category);
                atomic_store_u32(&g_lastLandingGainPermille,
                                 (u32)(gain * 1000.0f + 0.5f));
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
        if (flags & (CS_CLIMB_MASK | CS_WATER_MASK | CS_HAYSTACK)) {
            cancel_input_landing_fallback(true);
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
            schedule_configured_effect(
                g_map.landing, gain, landing_control_for_category(category),
                209, now, g_cfg.stateProbeDelayMs, ~0ull,
                g_cfg.stateEffectStartTrimMs);
            g_lastLandingEffectTick = now;
            atomic_fetch_add_u64(&g_inputFallbackLandings, 1);
            atomic_store_u32(&g_lastLandingCategory, category);
            atomic_store_u32(&g_lastLandingGainPermille,
                             (u32)(gain * 1000.0f + 0.5f));
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
                                u64 now, u32 delayMs);

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
                                u64 now, u32 delayMs) {
    if (!g_cfg.menuFeedback) return;
    atomic_fetch_add_u64(&g_menuPulseRequests, 1);
    schedule_configured_effect(effect, gain, control, source, now, delayMs);
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

    if (!active && g_cfg.menuNativeTabArm) {
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

    if (rising & XINPUT_B) {
        g_menuExitArmedTick = now;
        g_menuExitMovementTick = 0;
    }

    i64 x = current.Gamepad.sThumbLX;
    i64 y = current.Gamepad.sThumbLY;
    u64 magnitudeSquared = (u64)(x * x + y * y);
    i64 walk = g_cfg.walkThreshold;
    if (walk < 0) walk = -walk;
    if (g_menuExitArmedTick &&
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
    atomic_store_u32(&g_currentJumpInputDown, jumpDown ? 1u : 0u);
    atomic_store_u32(&g_currentLeftStickMagnitudeSq, stickMagnitudeSq);
    if (jumpRising)
        atomic_store_u32(&g_lastJumpStickMagnitudeSq, stickMagnitudeSq);
    set_movement_from_stick(current, sprintRising, sprintDown);
    process_attack_mapping(attackDown, attackRising, attackFalling, now);
    if (jumpRising) {
        atomic_store_u64(&g_lastJumpInputTick, now);
        try_bind_recent_state_candidate(now, BIND_JUMP);
        handle_jump_input(now, stickMagnitudeSq);
    }
    if (dodgeDiveRising) {
        atomic_store_u64(&g_lastDodgeDiveInputTick, now);
        try_bind_recent_state_candidate(now, BIND_DODGE_DIVE);
        process_contextual_dodge_dive(now, stickMagnitudeSq);
    }
    if (rising & XINPUT_X)
        schedule_arbitrated_effect(g_map.buttonX, 1.0f, 3,
                                   now, g_cfg.nativeProbeDelayMs);
    if (rising & XINPUT_Y)
        schedule_arbitrated_effect(g_map.buttonY, 1.0f, 4,
                                   now, g_cfg.nativeProbeDelayMs);
    if (parryRising)
        schedule_configured_effect(g_map.buttonLB, g_cfg.buttonLBGain,
                                   g_cfg.fxParry, 5, now,
                                   g_cfg.parryProbeDelayMs, ~0ull,
                                   g_cfg.parryStartTrimMs);
    if (rising & XINPUT_DPAD_UP)
        schedule_arbitrated_effect(g_map.dpadUp, 1.0f, 7, now, 0);
    if (rising & XINPUT_DPAD_DOWN)
        schedule_arbitrated_effect(g_map.dpadDown, 1.0f, 8, now, 0);
    if (sheatheHoodRising)
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
    if (sprintStarted) {
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
    tb_u64(b, atomic_load_u32(&g_inputEnabled)); tb_str(b, " nativeHaptics=");
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
    tb_str(b, " movementTune=walkHapticX/pulseIntervalMs/pulseDurationMs/pulseGain=");
    tb_float3(b, g_cfg.dualSenseWalkHapticMultiplier); tb_char(b, '/');
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
    tb_u64(b, atomic_load_u64(&g_reloadRecoveries)); tb_endline(b);
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
    log_input_bindings();
    log_effect_controls();
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
        tb_str(b, "] ACBFHapticsBridge v6-r6-controller-movement-tuning starting gameBase=0x"); tb_hex(b, (u64)g_gameBase);
        tb_str(b, " gameSize=0x"); tb_hex(b, g_gameSize); tb_str(b, " flushRVA=0x");
        tb_hex(b, g_cfg.flushRva); tb_str(b, " master="); tb_float3(b, g_cfg.masterGain);
        tb_str(b, " movement="); tb_float3(b, g_cfg.movementGain);
        tb_str(b, " oneShot="); tb_float3(b, g_cfg.oneShotGain);
        tb_str(b, " nativeHaptics="); tb_u64(b, g_cfg.nativeHaptics);
        tb_str(b, " conventionalRumble="); tb_u64(b, g_cfg.conventionalRumble);
        tb_str(b, " preferGameInput="); tb_u64(b, g_cfg.preferGameInput);
        tb_endline(b);
        append_log(b.bytes, b.len);
    }
    {
        TextBuf b{};
        SYSTEMTIME st{};
        if (g_api.GetLocalTime) g_api.GetLocalTime(&st);
        tb_char(b, '['); tb_time(b, st); tb_str(b, "] v6-r6 tuning walkLoopMs=");
        tb_u64(b, g_cfg.walkLoopStartMs); tb_char(b, '-'); tb_u64(b, g_cfg.walkLoopEndMs);
        tb_str(b, " runLoopMs=");
        tb_u64(b, g_cfg.runLoopStartMs); tb_char(b, '-'); tb_u64(b, g_cfg.runLoopEndMs);
        tb_str(b, " parryGain="); tb_float3(b, g_cfg.buttonLBGain);
        tb_str(b, " sprintKickGain="); tb_float3(b, g_cfg.sprintKickGain);
        tb_str(b, " minAirMs="); tb_u64(b, g_cfg.minimumAirborneMs);
        tb_str(b, " landAirMs="); tb_u64(b, g_cfg.landingShortAirMs);
        tb_char(b, '/'); tb_u64(b, g_cfg.landingLongAirMs);
        tb_char(b, '/'); tb_u64(b, g_cfg.landingMaxAirMs);
        tb_str(b, " landGains="); tb_float3(b, g_cfg.landingShortGain);
        tb_char(b, '/'); tb_float3(b, g_cfg.landingGain);
        tb_char(b, '/'); tb_float3(b, g_cfg.landingLongGain);
        tb_str(b, " jumpWindowMs="); tb_u64(b, g_cfg.jumpInputWindowMs);
        tb_str(b, " climbStick="); tb_i64(b, g_cfg.climbJumpStickThreshold);
        tb_str(b, " landDelayMs="); tb_u64(b, g_cfg.landingDelayMs);
        tb_str(b, " stateProbeMs="); tb_u64(b, g_cfg.stateProbeDelayMs);
        tb_str(b, " nativeHold/probeMs="); tb_u64(b, g_cfg.nativeHoldMs);
        tb_char(b, '/'); tb_u64(b, g_cfg.nativeProbeDelayMs);
        tb_str(b, " rbHoldMs="); tb_u64(b, g_cfg.rbHoldThresholdMs);
        tb_str(b, " rbSecondMs="); tb_u64(b, g_cfg.rbHoldSecondDelayMs);
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
        tb_str(b, " dsWalkHapticX=");
        tb_float3(b, g_cfg.dualSenseWalkHapticMultiplier);
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
    if (g_cfg.invalidInputBindings)
        log_simple("WARNING one or more InputBindings names were invalid; tested defaults were used (see INPUTCFG)");
    log_effect_controls();
    log_simple("Target fingerprint: ACBlackFlag.exe SHA-256 8d52238155c9491f329c0b78af2d00ee67ab5e03946eea83e12b061b64b23140 (Steam TU 1.0.6)");

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
    install_gameplay_state_hooks();
    if (haveXInput) install_game_xinput_observer();

    XINPUT_STATE previous{};
    bool havePrevious = false;
    bool previousRawTouchpad = false;
    u32 previousBackend = 0;
    bool f9Down = false, f10Down = false, f11Down = false, f12Down = false;
    u64 lastRumbleTick = g_api.GetTickCount64 ? g_api.GetTickCount64() : 0;
    u64 recoverySeen = atomic_load_u64(&g_recoverySerial);
    atomic_store_u64(&g_workerNowTick, lastRumbleTick);

    while (atomic_load_u32(&g_running)) {
        u64 loopNow = g_api.GetTickCount64 ? g_api.GetTickCount64() :
                      atomic_load_u64(&g_workerNowTick) + g_cfg.pollMs;
        atomic_store_u64(&g_workerNowTick, loopNow);

        u64 recoverySerial = atomic_load_u64(&g_recoverySerial);
        if (recoverySerial != recoverySeen) {
            recoverySeen = recoverySerial;
            reset_menu_mode();
            cancel_automatic_runtime(loopNow, true);
            g_gameplayResumeGuardUntil = loopNow + g_cfg.menuTransitionGuardMs;
            havePrevious = false;
            previousRawTouchpad = false;
            atomic_fetch_add_u64(&g_reloadRecoveries, 1);
            log_simple("RECOVERY Wwise sink resumed/rebound; controller, actor, menu, and playback state reset");
        }

        if (hotkey_pressed(VK_F9, f9Down) && atomic_load_u32(&g_enabled)) {
            queue_effect(g_map.dodge, 1.0f, 108);
            log_simple("F9 preview: dodge");
        }
        if (hotkey_pressed(VK_F10, f10Down)) write_status();
        if (hotkey_pressed(VK_F11, f11Down)) {
            u32 next = atomic_load_u32(&g_enabled) ? 0u : 1u;
            atomic_store_u32(&g_enabled, next);
            if (!next) {
                reset_menu_mode();
                cancel_automatic_runtime(loopNow, true);
                log_simple("F11 bridge disabled; all plugin playback cancelled");
            } else {
                g_gameplayResumeGuardUntil =
                    loopNow + g_cfg.menuTransitionGuardMs;
                log_simple("F11 bridge enabled");
            }
        }
        if (hotkey_pressed(VK_F12, f12Down)) {
            u32 next = atomic_load_u32(&g_inputEnabled) ? 0u : 1u;
            atomic_store_u32(&g_inputEnabled, next);
            if (!next) {
                reset_menu_mode();
                cancel_automatic_runtime(loopNow, true);
            } else {
                g_gameplayResumeGuardUntil =
                    loopNow + g_cfg.menuTransitionGuardMs;
            }
            log_simple(next ? "F12 input bridge enabled" : "F12 input bridge disabled");
        }

        XINPUT_STATE current{};
        bool connected = false;
        u32 backend = 0;
        if (g_cfg.preferGameInput) {
            if (haveGameInput) { connected = poll_gameinput(current); if (connected) backend = 1; }
            if (!connected && haveXInput && g_xinputGetState &&
                g_xinputGetState(g_cfg.controllerIndex, &current) == ERROR_SUCCESS) backend = 2, connected = true;
        } else {
            if (haveXInput && g_xinputGetState &&
                g_xinputGetState(g_cfg.controllerIndex, &current) == ERROR_SUCCESS) backend = 2, connected = true;
            if (!connected && haveGameInput) { connected = poll_gameinput(current); if (connected) backend = 1; }
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
            atomic_store_u32(&g_movementMode, 0);
            atomic_store_u32(&g_currentJumpInputDown, 0);
            atomic_store_u32(&g_currentLeftStickMagnitudeSq, 0);
            atomic_store_u32(&g_lastJumpStickMagnitudeSq, 0);
            g_sprintLatched = false;
            reset_rb_hold_state();
            reset_menu_mode();
            cancel_input_landing_fallback(false);
            clear_deferred_effects();
            havePrevious = false;
            previousRawTouchpad = false;
            previousBackend = 0;
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
        u64 elapsed = lastRumbleTick ? now - lastRumbleTick : g_cfg.pollMs;
        lastRumbleTick = now;
        if (elapsed < 1) elapsed = 1;
        if (elapsed > 50) elapsed = 50;
        RumbleSample rumble = render_rumble((u32)elapsed * 48u, now);
        write_rumble_output(rumble);

        g_api.Sleep(g_cfg.pollMs);
    }
    atomic_store_u32(&g_movementMode, 0);
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
