// Production DS4 transport with Win64-ABI fixtures; no attached hardware.
#include "src/main.cpp"
extern "C" int puts(const char*);
extern "C" void abort();

static bool expect(bool yes, const char* text) {
    puts(yes ? text : "FAIL assertion"); if (!yes) puts(text); return yes;
}
static DWORD lastError = 0;
static u32 rwOpens = 0, ds5RwOpens = 0, closes = 0, writes = 0, reads = 0;
static bool failOpen = false, failWrite = false, pendingWrite = false;
static bool writeComplete = false, cancelComplete = false, cancelled = false;
static bool bluetoothCaps = false, paddedBluetooth = false, unsupportedCaps = false;
static u32 eventSerial = 300;
static u8 lastOutput[1024]{};
static const u8* inFlightBuffer = nullptr;
static DWORD inFlightBytes = 0;
static Ds4Overlapped* inFlightWrite = nullptr;
static DWORD WINABI error_code() { return lastError; }
static void WINABI hid_guid(Ds4Guid* guid) { *guid = {}; }
static HANDLE WINABI device_list(const Ds4Guid*, const WCHAR*, HANDLE, DWORD flags) {
    if (flags != 0x12u) abort();
    return (HANDLE)900;
}
static BOOL WINABI enumerate(HANDLE, void*, const Ds4Guid*, DWORD index, Ds4Interface* data) {
    if (data->size != 32) abort();
    if (index >= 2) { lastError = 259; return 0; }
    data->reserved = index; return 1;
}
static BOOL WINABI detail(HANDLE, Ds4Interface* iface, void* buffer, DWORD n, DWORD* required, void*) {
    *required = 20;
    if (!buffer) { lastError = 122; return 0; }
    auto d = (Ds4Detail*)buffer;
    if (d->size != 8 || n < 20 || (u64)d->path - (u64)d != 4u) abort();
    const WCHAR* path = iface->reserved == 0 ? L"ds5" : L"ds4";
    for (u32 i = 0; i < 4; ++i) d->path[i] = path[i];
    return 1;
}
static BOOL WINABI destroy_list(HANDLE) { return 1; }
static HANDLE WINABI create_file(const WCHAR* path, DWORD access, DWORD share, void*, DWORD how, DWORD flags, HANDLE) {
    if (access == FILE_APPEND_DATA) return invalid_handle_value(); // silence test log file
    if (how != OPEN_EXISTING || share != (FILE_SHARE_READ | FILE_SHARE_WRITE)) abort();
    bool ds5 = path[2] == L'5';
    if (!access) return ds5 ? (HANDLE)1 : (HANDLE)2;
    if (ds5) { ++ds5RwOpens; abort(); }
    if (access != (GENERIC_READ | GENERIC_WRITE) || !(flags & DS4_FILE_OVERLAPPED)) abort();
    if (failOpen) { lastError = 5; return invalid_handle_value(); }
    ++rwOpens; reads = 0; cancelled = cancelComplete = false;
    return (HANDLE)(u64)(100u + rwOpens);
}
static u8 WINABI attributes(HANDLE h, Ds4Attributes* a) {
    if (a->size != 12) abort();
    a->vendor = 0x054C; a->product = h == (HANDLE)1 ? 0x0CE6 : 0x05C4; return 1;
}
static u8 WINABI preparsed(HANDLE h, void** p) { *p = h; return 1; }
static u8 WINABI free_preparsed(void*) { return 1; }
static i32 WINABI caps(void*, Ds4Caps* c) {
    *c = {}; c->usage = unsupportedCaps ? 2 : 5; c->page = 1;
    c->inputBytes = bluetoothCaps ? 78 : 64;
    c->outputBytes = bluetoothCaps ? (paddedBluetooth ? 547 : 78) : 32;
    return 0x00110000;
}
static HANDLE WINABI event(void*, BOOL manual, BOOL initial, const WCHAR*) {
    if (!manual || initial) abort();
    return (HANDLE)(u64)++eventSerial;
}
static BOOL WINABI reset_event(HANDLE) { return 1; }
static BOOL WINABI close_handle(HANDLE) { ++closes; return 1; }
static BOOL WINABI cancel(HANDLE, Ds4Overlapped*) { cancelled = true; return 1; }
static BOOL WINABI result(HANDLE, Ds4Overlapped* ol, DWORD* bytes, BOOL wait) {
    if (wait) abort(); // the haptic worker must never block on HID I/O
    if (cancelled && cancelComplete) { lastError = 995; return 0; }
    if (ol == inFlightWrite && writeComplete) { *bytes = inFlightBytes; return 1; }
    lastError = DS4_IO_INCOMPLETE; return 0;
}
static BOOL WINABI write_file(HANDLE, const void* data, DWORD n, DWORD* bytes, void* ol) {
    if (n != (bluetoothCaps ? (paddedBluetooth ? 547u : 78u) : 32u)) abort();
    ++writes; memcpy(lastOutput, data, n);
    if (failWrite) { lastError = 1167; return 0; }
    if (pendingWrite) {
        inFlightBuffer = (const u8*)data; inFlightBytes = n;
        inFlightWrite = (Ds4Overlapped*)ol; writeComplete = false;
        lastError = DS4_IO_PENDING; return 0;
    }
    *bytes = n; return 1;
}
static BOOL WINABI read_file(HANDLE, void* data, DWORD n, DWORD* bytes, void*) {
    if (n != (bluetoothCaps ? 78u : 64u)) abort();
    if (reads++ != 0) { lastError = DS4_IO_PENDING; return 0; }
    u8* p = (u8*)data; memset(p, 0, n);
    p[0] = 1; p[1] = p[2] = p[3] = p[4] = 128;
    p[5] = 0x28; p[6] = 0x02; p[7] = 2; p[8] = 0x55; p[9] = 0xFF;
    *bytes = n; return 1;
}
static void install_fixture() {
    g_api.CreateFileW = &create_file; g_api.ReadFile = &read_file;
    g_api.WriteFile = &write_file; g_api.CloseHandle = &close_handle;
    g_ds4Api = {&hid_guid,&attributes,&preparsed,&free_preparsed,&caps,
        &device_list,&enumerate,&detail,&destroy_list,&event,&reset_event,
        &error_code,&result,&cancel}; g_ds4ApiReady = true;
}
int main() {
    bool ok = true; u8 usb[32]{}, bt[78]{};
    ok &= expect(ds4_make_output(usb,32,false,64,128) == 32 &&
        usb[0] == 5 && usb[1] == 1 && usb[4] == 128 && usb[5] == 64,
        "PASS USB report ID, motor-only validity and right/left order match primary protocol");
    bool quiet = true; for (u32 i = 6; i < 32; ++i) quiet &= usb[i] == 0;
    ok &= expect(quiet && usb[2] == 0 && usb[3] == 0,
        "PASS motor reports leave LED/blink/speaker/audio validity bits off");
    // CRC literals independently generated by Python zlib over prefix A2 +
    // the protocol's first 74 bytes, not by the production CRC routine.
    ds4_make_output(bt,78,true,64,128);
    ok &= expect(bt[0] == 0x11 && bt[1] == 0xC4 && bt[3] == 1 &&
        bt[6] == 128 && bt[7] == 64 && ds4_read_le32(bt+74) == 0x4640D2ECu,
        "PASS Bluetooth format and independently computed motor-report CRC");
    ds4_make_output(bt,78,true,0,0);
    ok &= expect(ds4_read_le32(bt+74) == 0x89FE8937u,
        "PASS independent Bluetooth zero/stop CRC vector");
    ok &= expect(ds4_make_output(bt,31,false,1,1) == 0 &&
        ds4_motor_byte(-1) == 0 && ds4_motor_byte(2) == 255,
        "PASS report bounds and motor-level clamping");
    u8 input[78]{}; input[0] = 1;
    const u8 payload[9] = {0,255,128,128,0x39,0xE3,2,85,255};
    memcpy(input+1,payload,9); XINPUT_STATE state{}; bool touch = false;
    ok &= expect(ds4_decode_input(input,64,false,0x05C4,state,touch) &&
        state.Gamepad.wButtons == (XINPUT_A|XINPUT_X|XINPUT_START|XINPUT_LEFT_SHOULDER|
            XINPUT_RIGHT_SHOULDER|XINPUT_LEFT_THUMB|XINPUT_RIGHT_THUMB) && touch &&
        state.Gamepad.bLeftTrigger == 85 && state.Gamepad.bRightTrigger == 255 &&
        state.Gamepad.sThumbLX == -32768 && state.Gamepad.sThumbLY == -32767 &&
        state.Gamepad.sThumbRX == 0 && state.Gamepad.sThumbRY == 0,
        "PASS DS4 buttons, triggers, stick orientation and touch click normalize to existing mappings");
    ok &= expect(ds4_decode_input(input,10,true,0x09CC,state,touch),
        "PASS Bluetooth minimal report supplies existing gamepad mappings");
    memset(input,0,78); input[0] = 0x11; input[1] = 0x80; memcpy(input+3,payload,9);
    const u32 inputCrc = 0xE2200438u;
    for (u32 i=0;i<4;++i) input[74+i]=(u8)(inputCrc>>(i*8));
    ok &= expect(ds4_decode_input(input,78,true,0x05C4,state,touch),
        "PASS full Bluetooth input with independent A1 CRC vector");
    input[4] ^= 1;
    ok &= expect(!ds4_decode_input(input,78,true,0x05C4,state,touch) &&
        !ds4_decode_input(input,9,true,0x05C4,state,touch),
        "PASS corrupt and truncated input are rejected before state publication");
    ok &= expect(!is_ds4_product(0x054C,0x0CE6) && !is_ds4_product(0x045E,0x028E),
        "PASS DualSense and Xbox cannot enter the DS4 transport");
    Ds4Caps descriptor{}; descriptor.usage=5;descriptor.page=1;
    descriptor.inputBytes=64;descriptor.outputBytes=32;bool mode=true;
    ok &= expect(ds4_caps_transport(descriptor,mode) && !mode,
        "PASS USB gamepad descriptor identifies transport");
    descriptor.inputBytes=78;descriptor.outputBytes=547;
    ok &= expect(ds4_caps_transport(descriptor,mode) && mode,
        "PASS Bluetooth descriptor supports Windows maximum report padding");
    descriptor.inputBytes=2048;
    ok &= expect(!ds4_caps_transport(descriptor,mode),"PASS oversized descriptors are rejected");
    install_fixture();
    atomic_store_u32(&g_activeVendor,0x054C);atomic_store_u32(&g_activeProduct,0x05C4);
    u32 backend=1;bool connected=true;atomic_store_u64(&g_workerNowTick,1000);
    select_ds4_hid_transport(state,backend,connected,1000);
    ok &= expect(backend==3 && connected && g_ds4InputReady && g_ds4Active &&
        rwOpens==1 && ds5RwOpens==0 && (state.Gamepad.wButtons & XINPUT_A) &&
        atomic_load_u32(&g_rawTouchpadDown)==1,
        "PASS the actual worker selector promotes Sony feedback-only DS4 to HID with usable input");
    u32 priorOpens=rwOpens;backend=2;select_ds4_hid_transport(state,backend,connected,1002);
    ok &= expect(backend==2 && rwOpens==priorOpens,
        "PASS existing Xbox/XInput controller remains selected");
    backend=1;atomic_store_u32(&g_activeProduct,0x0CE6);
    select_ds4_hid_transport(state,backend,connected,1004);
    ok &= expect(backend==1 && rwOpens==priorOpens,
        "PASS native/wireless DualSense remains on the existing GameInput/native haptic path");
    atomic_store_u32(&g_inputBackend,3);atomic_store_u32(&g_activeDualSense,0);
    RumbleSample sample{};sample.low=.25f;sample.high=.5f;sample.timelineActive=true;
    write_rumble_output(sample);
    ok &= expect(lastOutput[4]==128 && lastOutput[5]==64 &&
        atomic_load_u32(&g_rumbleBackend)==3 && atomic_load_u64(&g_ds4HidCompleted)==1,
        "PASS the production mixer output sends unchanged envelope to DS4 motors and counts transport completion");
    sample.timelineActive=false;write_rumble_output(sample);
    ok &= expect(lastOutput[4]==0 && lastOutput[5]==0 && !g_rumbleOutputWasActive,
        "PASS the existing end-of-effect path sends a DS4 motor stop");
    pendingWrite=true;set_ds4_hid_rumble(.25f,.5f);
    u8 saved[32]{};memcpy(saved,inFlightBuffer,32);u32 before=writes;
    set_ds4_hid_rumble(.7f,.8f);set_ds4_hid_rumble(0,0);
    ok &= expect(writes==before && !memcmp(saved,inFlightBuffer,32) && g_ds4Active->dirty,
        "PASS in-flight output storage remains intact while newer values coalesce");
    writeComplete=true;pendingWrite=false;ds4_hid_service(1010);
    ok &= expect(writes==before+1 && lastOutput[4]==0 && lastOutput[5]==0 &&
        !g_ds4Active->writePending && !g_ds4Active->dirty,
        "PASS a coalesced final zero is delivered after async completion without an active timeline");
    Ds4HidDevice* old=g_ds4Active;u32 closedBefore=closes;
    ds4_hid_retire(*old);ds4_hid_service(1012);
    ok &= expect(old->file && old->readPending && closes==closedBefore,
        "PASS cancellation does not free/reuse buffers or events before read completion");
    cancelComplete=true;ds4_hid_service(1014);
    ok &= expect(!old->file && closes==closedBefore+3,
        "PASS canceled read completion releases only this backend's handle and events");
    g_ds4ScanDue=0;failOpen=true;
    ok &= expect(!ds4_hid_open(2000,0x05C4) && atomic_load_u32(&g_ds4HidLastError)==5,
        "PASS exclusive/HidHide access failures are observable and cannot claim delivery");
    failOpen=false;g_ds4ScanDue=0;unsupportedCaps=true;before=rwOpens;
    ok &= expect(!ds4_hid_open(2001,0x05C4) && rwOpens==before,
        "PASS a non-gamepad Sony interface is never opened for motor output");
    unsupportedCaps=false;g_ds4ScanDue=0;
    ok &= expect(ds4_hid_open(2002,0x05C4),"PASS a reconnected DS4 can reopen safely");
    failWrite=true;u64 completed=atomic_load_u64(&g_ds4HidCompleted);
    ok &= expect(!set_ds4_hid_rumble(.4f,.6f) && !g_ds4Active &&
        atomic_load_u64(&g_ds4HidCompleted)==completed && atomic_load_u32(&g_ds4HidLastError)==1167,
        "PASS failed writes retire the target and never increment completed motor reports");
    failWrite=false;ds4_hid_service(2004);g_ds4ScanDue=0;
    bluetoothCaps=paddedBluetooth=true;
    ok &= expect(ds4_hid_open(3000,0x05C4) && set_ds4_hid_rumble(.25f,.5f) &&
        lastOutput[0]==0x11 && ds4_read_le32(lastOutput+74)==0x4640D2ECu,
        "PASS actual Bluetooth transport sends protocol-correct rumble with Windows descriptor padding");
    quiet=true;for(u32 i=78;i<547;++i)quiet &= lastOutput[i]==0;
    ok &= expect(quiet,"PASS Windows output padding is zero and leaves the report CRC at its protocol offset");
    pendingWrite=true;set_ds4_hid_rumble(.5f,.7f);old=g_ds4Active;
    before=writes;
    ok &= expect(!ds4_hid_open(3002,0x09CC) && !g_ds4Active && old->closing &&
        !old->retired && old->writePending && writes==before,
        "PASS device handoff keeps old async storage live until its queued motor stop completes");
    writeComplete=true;pendingWrite=false;ds4_hid_service(3004);
    ok &= expect(lastOutput[6]==0 && lastOutput[7]==0 && old->retired,
        "PASS handoff flushes the old motor stop before canceling input and releasing the handle");
    cancelComplete=true;ds4_hid_service(3006);
    puts(ok?"FINAL PASS DS4 HID transport/lifecycle harness":"FINAL FAIL DS4 HID harness");
    return ok?0:1;
}
