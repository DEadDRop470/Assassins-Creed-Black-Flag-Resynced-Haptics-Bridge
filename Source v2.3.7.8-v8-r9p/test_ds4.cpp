// Executes the shipping polling/observer/output routines with a GameInput v0 mock.
#include "src/main.cpp"
extern "C" int puts(const char*);
extern "C" void abort();

struct MockDevice {
    void** vtable; u32 refs = 1, releases = 0;
    GameInputDeviceInfoPrefixV0 info{};
    u32 status = 1, infoCalls = 0, statusCalls = 0, writes = 0;
    float low = 0, high = 0;
};
struct MockReading {
    void** vtable; u32 refs = 1, releases = 0;
    MockDevice* device{};
    GameInputGamepadStateV0 state{};
    u32 kind = 0x40000, rawCount = 15, rawQueries = 0, deviceCalls = 0;
    u64 rawMask = 1ull << 13;
    bool valid = true;
};
struct MockInput {
    void** vtable;
    MockReading* gamepad{}; MockReading* raw{};
    u32 calls = 0, rawCalls = 0, badFilter = 0;
    bool fail = false, rawFail = false, combinedOnly = false;
    MockReading* combinedOverride = nullptr;
};
static MockDevice ds4{}, ds4b{}, ds5{}, xbox{}, unknown{};
static MockReading normalized{}, raw{};
static MockInput input{};
static void* devTable[11]{};
static void* xboxTable[11]{};
static void* readingTable[23]{};
static void* inputTable[5]{};
static u32 protectCalls = 0, writableExecCalls = 0, xiWrites = 0;
static float xiLow = 0, xiHigh = 0;
static u64 mockTick = 1000;
static usize WINABI mock_query(LPCVOID p, CompatMemoryInfo* m, usize n) {
    if (!p || n != sizeof(*m)) return 0;
    *m = {}; m->BaseAddress = (void*)((u64)p & ~0xFFFull);
    m->RegionSize = 4096; m->State = MEM_COMMIT; m->Protect = 0x20;
    return sizeof(*m);
}
static BOOL WINABI mock_protect(void*, usize n, DWORD mode, DWORD* old) {
    if (n != 8) abort();
    ++protectCalls; if (mode == PAGE_EXECUTE_READWRITE) ++writableExecCalls;
    *old = 0x20; return 1;
}
static DWORD WINABI mock_tid() { return 7; }
static u64 WINABI mock_tick() { return mockTick; }
static u32 FASTABI release_device(void* p) {
    auto d = (MockDevice*)p; ++d->releases; return --d->refs;
}
static u32 FASTABI release_reading(void* p) {
    auto r = (MockReading*)p; ++r->releases; return --r->refs;
}
static const GameInputDeviceInfoPrefixV0* FASTABI info_device(void* p) {
    auto d = (MockDevice*)p; ++d->infoCalls; return &d->info;
}
static u32 FASTABI status_device(void* p) {
    auto d = (MockDevice*)p; ++d->statusCalls; return d->status;
}
static void FASTABI rumble_device(void* p, const GameInputRumbleParamsV0* r) {
    auto d = (MockDevice*)p; ++d->writes; d->low = r ? r->lowFrequency : 0;
    d->high = r ? r->highFrequency : 0;
}
static u32 FASTABI kind_reading(void* p) { return ((MockReading*)p)->kind; }
static void FASTABI device_reading(void* p, void** out) {
    auto r = (MockReading*)p; ++r->deviceCalls; *out = r->device;
    if (r->device) ++r->device->refs;
}
static u8 FASTABI gamepad_reading(void* p, GameInputGamepadStateV0* out) {
    auto r = (MockReading*)p; *out = r->state; return r->valid ? 1 : 0;
}
static u32 FASTABI count_reading(void* p) {
    auto r = (MockReading*)p;
    if (r->device->info.vendorId == 0x054C &&
        !is_dualsense_product(r->device->info.vendorId, r->device->info.productId)) abort();
    ++r->rawQueries; return r->rawCount;
}
static u32 FASTABI buttons_reading(void* p, u32 n, u8* out) {
    auto r = (MockReading*)p;
    if (r->device->info.vendorId == 0x054C &&
        !is_dualsense_product(r->device->info.vendorId, r->device->info.productId)) abort();
    u32 copied = n < r->rawCount ? n : r->rawCount;
    for (u32 i = 0; i < copied; ++i) out[i] = (r->rawMask >> i) & 1;
    return copied;
}
static i32 FASTABI current_reading(void* p, u32 kind, void* device, void** out) {
    auto in = (MockInput*)p; ++in->calls; *out = nullptr;
    MockReading* r = nullptr;
    if (kind == 0x40004 && !device) { if (in->fail) return -1; r = in->combinedOverride ? in->combinedOverride : in->gamepad; }
    else if (kind == 0x40000 && !device) { if (in->fail || in->combinedOnly) return -1; r = in->gamepad; }
    else if (kind == 4 && device == in->gamepad->device) {
        ++in->rawCalls; if (in->rawFail) return -1; r = in->raw;
    } else { ++in->badFilter; return -1; }
    if (!r) return -1;
    ++r->refs; *out = r; return 0;
}
static DWORD WINABI xi_rumble(DWORD, XINPUT_VIBRATION* v) {
    ++xiWrites; xiLow = (float)v->wLeftMotorSpeed / 65535.0f;
    xiHigh = (float)v->wRightMotorSpeed / 65535.0f; return 0;
}
static bool expect(bool yes, const char* name) {
    puts(yes ? name : "FAIL assertion"); if (!yes) puts(name); return yes;
}
static void select(MockDevice& d) {
    normalized.device = &d; raw.device = &d; normalized.valid = true;
    normalized.kind = 0x40000; normalized.state = {};
    input.gamepad = &normalized; input.raw = &raw; input.fail = input.rawFail = false;
    normalized.state.buttons = 0x00000004 | 0x00000100 | 0x00000400;
    normalized.state.leftTrigger = 0.5f; normalized.state.rightTrigger = 1.0f;
    normalized.state.leftThumbstickX = -1.0f; normalized.state.rightThumbstickY = 1.0f;
    atomic_store_u64(&g_workerNowTick, mockTick);
}
int main() {
    g_api.VirtualQuery = &mock_query; g_api.VirtualProtect = &mock_protect;
    g_api.GetCurrentThreadId = &mock_tid; g_api.GetTickCount64 = &mock_tick;
    devTable[2] = (void*)&release_device; devTable[3] = (void*)&info_device;
    devTable[4] = (void*)&status_device; devTable[10] = (void*)&rumble_device;
    for (u32 i = 0; i < 11; ++i) xboxTable[i] = devTable[i];
    readingTable[2] = (void*)&release_reading; readingTable[3] = (void*)&kind_reading;
    readingTable[6] = (void*)&device_reading; readingTable[10] = (void*)&count_reading;
    readingTable[11] = (void*)&buttons_reading; readingTable[22] = (void*)&gamepad_reading;
    inputTable[4] = (void*)&current_reading;
    ds4.vtable = ds4b.vtable = ds5.vtable = unknown.vtable = devTable;
    xbox.vtable = xboxTable;
    ds4.info = {8,0x054C,0x05C4}; ds4b.info = {8,0x054C,0x09CC};
    ds5.info = {8,0x054C,0x0CE6}; xbox.info = {8,0x045E,0x028E};
    unknown.info = {8,0x054C,0x1234};
    normalized.vtable = raw.vtable = readingTable; input.vtable = inputTable;
    g_gameInput = &input; g_cfg.menuRawTouchpad = 1; g_cfg.menuRawTouchpadSonyOnly = 0;
    g_cfg.nativeArbitration = 1; g_xinputSetState = &xi_rumble;
    bool ok = true; XINPUT_STATE s{};
    select(ds4);
    ok &= expect(poll_gameinput(s), "PASS physical DS4 first poll");
    ok &= expect(protectCalls == 0 && input.rawCalls == 0 && normalized.rawQueries == 0 &&
        devTable[10] == (void*)&rumble_device && !atomic_load_u32(&g_activeDualSense),
        "PASS DS4 never hooks shared rumble or reads raw Sony buttons (even SonyOnly=0)");
    ok &= expect(s.Gamepad.wButtons == (XINPUT_A|XINPUT_DPAD_LEFT|XINPUT_LEFT_SHOULDER) &&
        s.Gamepad.bLeftTrigger == 127 && s.Gamepad.bRightTrigger == 255 &&
        s.Gamepad.sThumbLX == -32768 && s.Gamepad.sThumbRY == 32767,
        "PASS normalized DS4 buttons/triggers/sticks");
    atomic_store_u32(&g_inputBackend,1); atomic_store_u32(&g_controllerConnected,1);
    RumbleSample sample{}; sample.timelineActive = true; sample.low = .25f; sample.high = .50f;
    write_rumble_output(sample);
    ok &= expect(ds4.writes == 0 && xiWrites == 1 && xiLow > .24f && xiHigh > .49f,
        "PASS unsupported DS4 GI rumble is rejected; available XInput still receives its original envelope");
    sample.timelineActive = false; write_rumble_output(sample);
    ok &= expect(ds4.low == 0 && ds4.high == 0 && xiLow == 0 && xiHigh == 0,
        "PASS successful XInput fallback stops both motors after effect");
    for (u32 i=0;i<10000;++i) {
        mockTick += 2; atomic_store_u64(&g_workerNowTick,mockTick);
        if (!poll_gameinput(s)) ok = false;
    }
    ok &= expect(ds4.refs == 2 && normalized.refs == 1 && ds4.infoCalls == 1 &&
        ds4.statusCalls <= 42 && input.rawCalls == 0 && input.badFilter == 0,
        "PASS 10000 DS4 polls: reference balance and cached metadata/status");
    sample.timelineActive = true; write_rumble_output(sample);
    select(ds4b); ok &= expect(poll_gameinput(s) && ds4.low == 0 && ds4.high == 0 && ds4.refs == 1 && ds4b.refs == 2 &&
        g_gameInputDevice == &ds4b && atomic_load_u32(&g_activeProduct) == 0x09CC,
        "PASS physical/virtual DS4 switch balances references and changes identity without GI motor calls");
    write_rumble_output(sample);
    ok &= expect(ds4b.writes == 0 && xiLow > .24f && xiHigh > .49f,
        "PASS DS4 handoff keeps XInput fallback and avoids unsupported GI dispatch");
    sample.timelineActive = false; write_rumble_output(sample);
    select(xbox); normalized.valid = false;
    ok &= expect(!poll_gameinput(s) && !g_gameInputDevice && ds4b.refs == 1 &&
        !atomic_load_u32(&g_activeVendor),
        "PASS unrelated raw-only non-Sony view cannot select an output device or retain stale identity");
    select(ds4b); ok &= expect(poll_gameinput(s) && ds4b.infoCalls == 2,
        "PASS reconnect of same pointer refreshes cleared identity");
    ds4b.status = 0; mockTick += 501; atomic_store_u64(&g_workerNowTick,mockTick);
    ok &= expect(!poll_gameinput(s) && !g_gameInputDevice && ds4b.refs == 1,
        "PASS disconnected GameInput device is rejected and released"); ds4b.status = 1;
    select(unknown); ok &= expect(poll_gameinput(s) && input.rawCalls == 0 && protectCalls == 0,
        "PASS unknown Sony pad follows conservative DS4 policy");
    select(ds5); ok &= expect(poll_gameinput(s), "PASS DualSense normalized input");
    ok &= expect(input.rawCalls == 1 && atomic_load_u32(&g_rawTouchpadDown) == 1 &&
        atomic_load_u32(&g_activeDualSense) == 1 && raw.refs == 1 &&
        devTable[10] == (void*)&gameinput_rumble_observer,
        "PASS DualSense touchpad uses same-device raw view and keeps rumble observer");
    ok &= expect(writableExecCalls > 0, "PASS pointer patch preserves executable page permission");
    u64 nativeBefore = atomic_load_u64(&g_gameNativeGameInputWrites);
    set_gameinput_rumble(.3f,.4f);
    ok &= expect(ds5.low == .3f && ds5.high == .4f &&
        atomic_load_u64(&g_gameNativeGameInputWrites) == nativeBefore,
        "PASS plugin GameInput writes do not observe themselves as native rumble");
    GameInputRumbleParamsV0 native{.4f,.5f,0,0};
    ((GameInputDeviceSetRumbleFn)devTable[10])(&ds5,&native);
    ok &= expect(ds5.low == .4f && atomic_load_u64(&g_gameNativeGameInputWrites) == nativeBefore+1,
        "PASS game native GameInput rumble is observed and forwarded once");
    atomic_store_u64(&g_nativeRumbleUntil,0);
    input.raw = &normalized; normalized.rawCount = 15; normalized.rawMask = 1ull << 13;
    ok &= expect(poll_gameinput(s) && normalized.refs == 1,
        "PASS distinct retained raw lookup returning same COM pointer is released");
    select(ds4);
    ok &= expect(poll_gameinput(s) && devTable[10] == (void*)&rumble_device &&
        atomic_load_u32(&g_gameInputObserversInstalled) == 0 &&
        atomic_load_u64(&g_gameInputObserverRetirements) == 1 && !atomic_load_u32(&g_rawTouchpadDown),
        "PASS DS4 retires previously shared DualSense observer and clears touchpad state");
    u32 oldWrites = ds5.writes; gameinput_rumble_observer(&ds5,&native);
    ok &= expect(ds5.writes == oldWrites+1, "PASS already-entered retired observer can still forward original");
    select(ds5); ok &= expect(poll_gameinput(s) && devTable[10] == (void*)&rumble_device,
        "PASS retired DS4-shared slot is never hooked again");
    input.rawFail = true; ok &= expect(poll_gameinput(s) && !atomic_load_u32(&g_rawTouchpadDown) && raw.refs == 1,
        "PASS missing DualSense raw view does not invalidate normalized input");
    select(xbox); ok &= expect(poll_gameinput(s) && !atomic_load_u32(&g_activeDualSense) &&
        xboxTable[10] == (void*)&gameinput_rumble_observer,
        "PASS virtual Xbox polling and native observer retained");
    input.fail = true; ok &= expect(!poll_gameinput(s) && !g_gameInputDevice && xbox.refs == 1,
        "PASS failed primary read releases cached device before XInput fallback");
    g_lastRumblePhysicalWriteTick = 555; poll_gameinput(s);
    ok &= expect(g_lastRumblePhysicalWriteTick == 555,
        "PASS repeated absent GameInput does not break XInput output cadence");
    ok &= expect(normalized.refs == 1 && raw.refs == 1 && ds4.refs == 1 && ds4b.refs == 1 &&
        ds5.refs == 1 && xbox.refs == 1 && unknown.refs == 1 && input.badFilter == 0,
        "PASS final COM reference balance and validated normalized/legacy filters");
    input.fail = false; input.combinedOnly = true;
    select(ds5); input.combinedOnly = true;
    ok &= expect(poll_gameinput(s) && atomic_load_u32(&g_gameInputNormalizedReady) == 1 &&
        atomic_load_u32(&g_gameInputReadRoute) == 1,
        "PASS legacy combined-only DualSense stream restores working input and haptic identity");
    atomic_store_u32(&g_activeVendor,0); atomic_store_u32(&g_activeProduct,0);
    atomic_store_u32(&g_activeDualSense,0); u32 infoCalls = ds5.infoCalls;
    ok &= expect(poll_gameinput(s) && atomic_load_u32(&g_activeDualSense) == 1 &&
        atomic_load_u32(&g_activeVendor) == 0x054C && ds5.infoCalls == infoCalls,
        "PASS same cached GameInput object restores Sony identity after XInput fallback");
    raw.valid = false; raw.device = &xbox; input.combinedOverride = &raw;
    input.combinedOnly = false;
    ok &= expect(poll_gameinput(s) && g_gameInputDevice == &ds5 &&
        atomic_load_u32(&g_gameInputReadRoute) == 2,
        "PASS unrelated combined raw update falls back to the actual normalized gamepad");
    input.combinedOverride = nullptr;
    select(ds4); normalized.valid = false;
    ok &= expect(poll_gameinput(s) && !atomic_load_u32(&g_gameInputNormalizedReady) &&
        atomic_load_u32(&g_gameInputReadRoute) == 3 && g_gameInputDevice == &ds4 &&
        !s.Gamepad.wButtons && !s.Gamepad.bLeftTrigger && devTable[10] == (void*)&rumble_device,
        "PASS raw-only physical DS4 remains a safe feedback target without invented button state");
    atomic_store_u32(&g_inputBackend,1); sample.timelineActive = true;
    atomic_store_u64(&g_nativeRumbleUntil,0); write_rumble_output(sample);
    ok &= expect(ds4.writes == 0 && xiLow > .24f && xiHigh > .49f,
        "PASS raw-only DS4 cannot falsely count a void GI call as motor delivery");
    sample.timelineActive = false; write_rumble_output(sample);
    clear_gameinput_device(mockTick);
    ok &= expect(normalized.refs == 1 && raw.refs == 1 && ds4.refs == 1 && ds5.refs == 1 && xbox.refs == 1,
        "PASS legacy fallback and feedback-only reference balance");
    puts(ok ? "FINAL PASS DS4/controller regression harness" : "FINAL FAIL DS4/controller regression harness");
    return ok ? 0 : 1;
}
