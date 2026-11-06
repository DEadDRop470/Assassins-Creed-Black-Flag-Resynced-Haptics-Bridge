// DS4 transport only. The r9m effects, waveform reduction and gains live in
// main.cpp unchanged. Protocol facts: Linux hid-playstation and SDL PS4 HID.
// All storage belongs to the worker. No GameInput vtable/raw-button calls.
#pragma once

struct Ds4Guid { u32 a; u16 b, c; u8 d[8]; };
struct Ds4Interface { u32 size; Ds4Guid guid; u32 flags; u64 reserved; };
struct Ds4Attributes { u32 size; u16 vendor, product, version, padding; };
struct Ds4Caps {
    u16 usage, page, inputBytes, outputBytes, featureBytes, reserved[17];
    u16 counts[10];
};
struct Ds4Overlapped { u64 internal, internalHigh, offset; HANDLE event; };
struct alignas(8) Ds4Detail { u32 size; WCHAR path[1024]; };
static_assert(sizeof(Ds4Guid) == 16 && sizeof(Ds4Interface) == 32, "SetupAPI x64 ABI");
static_assert(sizeof(Ds4Attributes) == 12 && sizeof(Ds4Caps) == 64, "HID ABI");
static_assert(sizeof(Ds4Overlapped) == 32, "OVERLAPPED x64 ABI");
#if !defined(_MSC_VER)
static_assert(__builtin_offsetof(Ds4Detail, path) == 4, "Unicode device path offset");
#endif

using Ds4GetGuidFn = void (WINABI*)(Ds4Guid*);
using Ds4GetAttrsFn = u8 (WINABI*)(HANDLE, Ds4Attributes*);
using Ds4GetPreparsedFn = u8 (WINABI*)(HANDLE, void**);
using Ds4FreePreparsedFn = u8 (WINABI*)(void*);
using Ds4GetCapsFn = i32 (WINABI*)(void*, Ds4Caps*);
using Ds4GetClassFn = HANDLE (WINABI*)(const Ds4Guid*, const WCHAR*, HANDLE, DWORD);
using Ds4EnumFn = BOOL (WINABI*)(HANDLE, void*, const Ds4Guid*, DWORD, Ds4Interface*);
using Ds4GetDetailFn = BOOL (WINABI*)(HANDLE, Ds4Interface*, void*, DWORD, DWORD*, void*);
using Ds4DestroyListFn = BOOL (WINABI*)(HANDLE);
using Ds4CreateEventFn = HANDLE (WINABI*)(void*, BOOL, BOOL, const WCHAR*);
using Ds4ResetEventFn = BOOL (WINABI*)(HANDLE);
using Ds4LastErrorFn = DWORD (WINABI*)();
using Ds4ResultFn = BOOL (WINABI*)(HANDLE, Ds4Overlapped*, DWORD*, BOOL);
using Ds4CancelFn = BOOL (WINABI*)(HANDLE, Ds4Overlapped*);
struct Ds4Api {
    Ds4GetGuidFn guid; Ds4GetAttrsFn attrs; Ds4GetPreparsedFn preparsed;
    Ds4FreePreparsedFn freePreparsed; Ds4GetCapsFn caps;
    Ds4GetClassFn getClass; Ds4EnumFn enumerate; Ds4GetDetailFn detail;
    Ds4DestroyListFn destroyList; Ds4CreateEventFn createEvent;
    Ds4ResetEventFn resetEvent; Ds4LastErrorFn lastError;
    Ds4ResultFn result; Ds4CancelFn cancel;
};
static Ds4Api g_ds4Api{};
static bool g_ds4ApiReady{};
static constexpr u32 DS4_HID_BUFFER = 1024;
static constexpr u32 DS4_HID_SLOTS = 4;
static constexpr DWORD DS4_IO_PENDING = 997, DS4_IO_INCOMPLETE = 996;
static constexpr DWORD DS4_FILE_OVERLAPPED = 0x40000000u;
struct Ds4HidDevice {
    HANDLE file{};
    Ds4Overlapped read{}, write{};
    u8 input[DS4_HID_BUFFER]{}, output[DS4_HID_BUFFER]{};
    u16 vendor{}, product{}, inputBytes{}, outputBytes{};
    bool bluetooth{}, retired{}, closing{}, readPending{}, writePending{};
    bool ready{}, touch{}, dirty{}, desiredActive{}, sentActive{};
    u8 desiredLow{}, desiredHigh{}, sentLow{}, sentHigh{};
    u64 openTick{}, inputTick{}, writeTick{};
    u32 generation{};
    XINPUT_STATE state{};
};
static Ds4HidDevice g_ds4Hid[DS4_HID_SLOTS]{};
static Ds4HidDevice* g_ds4Active{};
static u64 g_ds4ScanDue{};
static u32 g_ds4NextGeneration{};
static volatile u64 g_ds4HidScans{}, g_ds4HidOpens{}, g_ds4HidOpenFailures{};
static volatile u64 g_ds4HidInputReports{}, g_ds4HidInvalidReports{};
static volatile u64 g_ds4HidSubmitted{}, g_ds4HidCompleted{}, g_ds4HidWriteErrors{};
static volatile u64 g_ds4HidReadErrors{}, g_ds4HidBusyCoalesces{};
static volatile u32 g_ds4HidLastError{};
static bool g_ds4InputReady{};

static bool is_ds4_product(u32 vendor, u32 product) {
    return vendor == 0x054Cu &&
        (product == 0x05C4u || product == 0x09CCu || product == 0x0BA0u);
}

static u32 ds4_crc32_update(u32 crc, u8 byte) {
    crc ^= byte;
    for (u32 bit = 0; bit < 8; ++bit)
        crc = (crc >> 1) ^ ((crc & 1u) ? 0xEDB88320u : 0u);
    return crc;
}
static u32 ds4_report_crc(u8 seed, const u8* data, u32 count) {
    u32 crc = ds4_crc32_update(0xFFFFFFFFu, seed);
    for (u32 i = 0; i < count; ++i) crc = ds4_crc32_update(crc, data[i]);
    return ~crc;
}
static u32 ds4_read_le32(const u8* p) {
    return (u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
}
static u8 ds4_motor_byte(float level) {
    return (u8)(clampf(level, 0.0f, 1.0f) * 255.0f + 0.5f);
}
static u32 ds4_make_output(u8* dst, u32 capacity, bool bluetooth, u8 low, u8 high) {
    const u32 length = bluetooth ? 78u : 32u;
    if (capacity < length) return 0;
    memset(dst, 0, capacity); // Windows requires the descriptor's full output length.
    dst[0] = bluetooth ? 0x11u : 0x05u;
    const u32 common = bluetooth ? 3u : 1u;
    if (bluetooth) dst[1] = 0xC4u; // HID + CRC; 4 ms input interval.
    dst[common] = 0x01u; // Motors only: no LED, blink, speaker or audio changes.
    dst[common + 3u] = high; // small/right motor
    dst[common + 4u] = low;  // large/left motor
    if (bluetooth) {
        u32 crc = ds4_report_crc(0xA2u, dst, 74u);
        for (u32 i = 0; i < 4; ++i) dst[74u + i] = (u8)(crc >> (8u * i));
    }
    return length;
}
static i16 ds4_axis(u8 value, bool invert) {
    // Map the centre exactly to zero and both ends to XInput's full range.
    i32 axis = value < 128u ? ((i32)value - 128) * 256 :
        (((i32)value - 128) * 32767) / 127;
    if (invert) axis = axis == -32768 ? 32767 : -axis;
    return (i16)axis;
}
static bool ds4_decode_input(const u8* bytes, u32 count, bool bluetooth,
                             u16 product, XINPUT_STATE& state, bool& touch) {
    u32 offset = 0;
    if (!bytes || count < 10u) return false;
    if (bytes[0] == 0x01u && (bluetooth || count == 64u)) offset = 1u;
    else if (bluetooth && bytes[0] == 0x11u && count >= 78u) {
        if (!(bytes[1] & 0x80u) ||
            ds4_report_crc(0xA1u, bytes, 74u) != ds4_read_le32(bytes + 74u)) return false;
        offset = 3u;
    } else return false;
    if (count < offset + 9u) return false;
    if (product == 0x0BA0u && count >= offset + 31u &&
        (bytes[offset + 30u] & 0x04u)) return false; // adapter has no paired pad
    const u8* p = bytes + offset;
    XINPUT_STATE decoded{};
    decoded.Gamepad.sThumbLX = ds4_axis(p[0], false);
    decoded.Gamepad.sThumbLY = ds4_axis(p[1], true);
    decoded.Gamepad.sThumbRX = ds4_axis(p[2], false);
    decoded.Gamepad.sThumbRY = ds4_axis(p[3], true);
    static constexpr WORD hats[9] = {
        XINPUT_DPAD_UP, XINPUT_DPAD_UP | XINPUT_DPAD_RIGHT, XINPUT_DPAD_RIGHT,
        XINPUT_DPAD_RIGHT | XINPUT_DPAD_DOWN, XINPUT_DPAD_DOWN,
        XINPUT_DPAD_DOWN | XINPUT_DPAD_LEFT, XINPUT_DPAD_LEFT,
        XINPUT_DPAD_LEFT | XINPUT_DPAD_UP, 0
    };
    u8 hat = p[4] & 15u;
    decoded.Gamepad.wButtons = hats[hat < 8u ? hat : 8u];
    if (p[4] & 0x10u) decoded.Gamepad.wButtons |= XINPUT_X;
    if (p[4] & 0x20u) decoded.Gamepad.wButtons |= XINPUT_A;
    if (p[4] & 0x40u) decoded.Gamepad.wButtons |= XINPUT_B;
    if (p[4] & 0x80u) decoded.Gamepad.wButtons |= XINPUT_Y;
    if (p[5] & 0x01u) decoded.Gamepad.wButtons |= XINPUT_LEFT_SHOULDER;
    if (p[5] & 0x02u) decoded.Gamepad.wButtons |= XINPUT_RIGHT_SHOULDER;
    if (p[5] & 0x10u) decoded.Gamepad.wButtons |= XINPUT_BACK;
    if (p[5] & 0x20u) decoded.Gamepad.wButtons |= XINPUT_START;
    if (p[5] & 0x40u) decoded.Gamepad.wButtons |= XINPUT_LEFT_THUMB;
    if (p[5] & 0x80u) decoded.Gamepad.wButtons |= XINPUT_RIGHT_THUMB;
    decoded.Gamepad.bLeftTrigger = p[7];
    decoded.Gamepad.bRightTrigger = p[8];
    touch = (p[6] & 0x02u) != 0;
    decoded.dwPacketNumber = state.dwPacketNumber + 1u;
    state = decoded;
    return true;
}

static bool ds4_caps_transport(const Ds4Caps& caps, bool& bluetooth) {
    if (caps.page != 1u || (caps.usage != 4u && caps.usage != 5u) ||
        caps.inputBytes > DS4_HID_BUFFER || caps.outputBytes > DS4_HID_BUFFER) return false;
    if (caps.inputBytes == 64u && caps.outputBytes == 32u) bluetooth = false;
    else if (caps.inputBytes >= 78u && caps.outputBytes >= 78u) bluetooth = true;
    else return false;
    return true;
}

static void ds4_hid_error(const char* operation, DWORD error) {
    atomic_store_u32(&g_ds4HidLastError, error);
    TextBuf b{}; tb_str(b, "R9P DS4 HID "); tb_str(b, operation);
    tb_str(b, " error="); tb_u64(b, error); tb_endline(b); append_log(b.bytes, b.len);
}
static bool init_ds4_hid() {
    if (!g_api.LoadLibraryW) return false;
    u8* hid = (u8*)g_api.LoadLibraryW(L"hid.dll");
    u8* setup = (u8*)g_api.LoadLibraryW(L"setupapi.dll");
    u8* kernel = find_module_ascii("kernel32.dll");
    if (!hid || !setup || !kernel) return false;
#define DS4_RESOLVE(member, type, module, name) \
    g_ds4Api.member = api_cast<type>(resolve_export(module, name))
    DS4_RESOLVE(guid, Ds4GetGuidFn, hid, "HidD_GetHidGuid");
    DS4_RESOLVE(attrs, Ds4GetAttrsFn, hid, "HidD_GetAttributes");
    DS4_RESOLVE(preparsed, Ds4GetPreparsedFn, hid, "HidD_GetPreparsedData");
    DS4_RESOLVE(freePreparsed, Ds4FreePreparsedFn, hid, "HidD_FreePreparsedData");
    DS4_RESOLVE(caps, Ds4GetCapsFn, hid, "HidP_GetCaps");
    DS4_RESOLVE(getClass, Ds4GetClassFn, setup, "SetupDiGetClassDevsW");
    DS4_RESOLVE(enumerate, Ds4EnumFn, setup, "SetupDiEnumDeviceInterfaces");
    DS4_RESOLVE(detail, Ds4GetDetailFn, setup, "SetupDiGetDeviceInterfaceDetailW");
    DS4_RESOLVE(destroyList, Ds4DestroyListFn, setup, "SetupDiDestroyDeviceInfoList");
    DS4_RESOLVE(createEvent, Ds4CreateEventFn, kernel, "CreateEventW");
    DS4_RESOLVE(resetEvent, Ds4ResetEventFn, kernel, "ResetEvent");
    DS4_RESOLVE(lastError, Ds4LastErrorFn, kernel, "GetLastError");
    DS4_RESOLVE(result, Ds4ResultFn, kernel, "GetOverlappedResult");
    DS4_RESOLVE(cancel, Ds4CancelFn, kernel, "CancelIoEx");
#undef DS4_RESOLVE
    g_ds4ApiReady = g_ds4Api.guid && g_ds4Api.attrs && g_ds4Api.preparsed &&
        g_ds4Api.freePreparsed && g_ds4Api.caps && g_ds4Api.getClass &&
        g_ds4Api.enumerate && g_ds4Api.detail && g_ds4Api.destroyList &&
        g_ds4Api.createEvent && g_ds4Api.resetEvent && g_ds4Api.lastError &&
        g_ds4Api.result && g_ds4Api.cancel;
    log_simple(g_ds4ApiReady ? "R9P DS4 HID backend ready; r9m mixer and effect tuning retained" :
                              "WARNING R9P DS4 HID APIs unavailable");
    return g_ds4ApiReady;
}
static void ds4_hid_retire(Ds4HidDevice& device) {
    if (device.retired || !device.file) return;
    device.retired = true; device.dirty = false; device.ready = false;
    if (g_ds4Active == &device) g_ds4Active = nullptr;
    // Cancel only this bridge's handle. Storage/events remain live until both
    // operations have completed; CancelIoEx itself is not completion.
    if (device.readPending) g_ds4Api.cancel(device.file, &device.read);
    if (device.writePending) g_ds4Api.cancel(device.file, &device.write);
}
static void ds4_hid_complete_write(Ds4HidDevice& device) {
    atomic_fetch_add_u64(&g_ds4HidCompleted, 1);
    if (atomic_load_u64(&g_ds4HidCompleted) == 1u)
        log_simple("R9P DS4 HID first motor report completed (Windows transport acknowledgement)");
    device.sentActive = device.sentLow != 0u || device.sentHigh != 0u;
}
static bool ds4_hid_submit(Ds4HidDevice& device, u64 now) {
    if (!device.file || device.retired || !device.dirty) return false;
    if (device.writePending) { atomic_fetch_add_u64(&g_ds4HidBusyCoalesces, 1); return true; }
    HANDLE event = device.write.event;
    device.write = {}; device.write.event = event;
    g_ds4Api.resetEvent(event);
    if (!ds4_make_output(device.output, device.outputBytes, device.bluetooth,
                         device.desiredLow, device.desiredHigh)) return false;
    device.sentLow = device.desiredLow; device.sentHigh = device.desiredHigh;
    device.dirty = false; device.writeTick = now;
    DWORD bytes = 0;
    BOOL success = g_api.WriteFile(device.file, device.output, device.outputBytes,
                                   &bytes, &device.write);
    DWORD error = success ? 0u : g_ds4Api.lastError();
    if (success && bytes == device.outputBytes) {
        atomic_fetch_add_u64(&g_ds4HidSubmitted, 1);
        ds4_hid_complete_write(device);
        return true;
    }
    if (!success && error == DS4_IO_PENDING) {
        device.writePending = true;
        atomic_fetch_add_u64(&g_ds4HidSubmitted, 1);
        return true;
    }
    atomic_fetch_add_u64(&g_ds4HidWriteErrors, 1);
    ds4_hid_error("write", success ? 13u : error);
    ds4_hid_retire(device);
    return false;
}
static bool set_ds4_hid_rumble(float low, float high) {
    if (!g_ds4Active || g_ds4Active->retired) return false;
    Ds4HidDevice& device = *g_ds4Active;
    device.desiredLow = ds4_motor_byte(low); device.desiredHigh = ds4_motor_byte(high);
    device.desiredActive = device.desiredLow != 0u || device.desiredHigh != 0u;
    device.dirty = true;
    return ds4_hid_submit(device, atomic_load_u64(&g_workerNowTick));
}
static void ds4_hid_accept_input(Ds4HidDevice& device, DWORD bytes, u64 now) {
    if (bytes > device.inputBytes ||
        !ds4_decode_input(device.input, bytes, device.bluetooth, device.product,
                           device.state, device.touch)) {
        atomic_fetch_add_u64(&g_ds4HidInvalidReports, 1);
        if (device.product == 0x0BA0u && bytes >= 32u && (device.input[31] & 4u))
            device.ready = false;
        return;
    }
    device.ready = true; device.inputTick = now;
    atomic_fetch_add_u64(&g_ds4HidInputReports, 1);
}
static void ds4_hid_service(u64 now) {
    if (!g_ds4ApiReady) return;
    for (u32 index = 0; index < DS4_HID_SLOTS; ++index) {
        Ds4HidDevice& d = g_ds4Hid[index];
        if (!d.file) continue;
        if (d.writePending) {
            DWORD bytes = 0;
            BOOL success = g_ds4Api.result(d.file, &d.write, &bytes, 0);
            DWORD error = success ? 0u : g_ds4Api.lastError();
            if (success || error != DS4_IO_INCOMPLETE) {
                d.writePending = false;
                if (!d.retired) {
                    if (success && bytes == d.outputBytes) ds4_hid_complete_write(d);
                    else {
                        atomic_fetch_add_u64(&g_ds4HidWriteErrors, 1);
                        ds4_hid_error("write completion", success ? 13u : error);
                        ds4_hid_retire(d);
                    }
                }
            } else if (!d.retired && now >= d.writeTick && now - d.writeTick >= 1000u) {
                atomic_fetch_add_u64(&g_ds4HidWriteErrors, 1);
                ds4_hid_error("write timeout", 1460u); ds4_hid_retire(d);
            }
        }
        // Drain at most eight ready reports per iteration, keeping a busy HID
        // device from monopolizing the game's haptic worker.
        for (u32 reads = 0; reads < 8u; ++reads) {
            DWORD bytes = 0;
            if (d.readPending) {
                BOOL success = g_ds4Api.result(d.file, &d.read, &bytes, 0);
                DWORD error = success ? 0u : g_ds4Api.lastError();
                if (!success && error == DS4_IO_INCOMPLETE) break;
                d.readPending = false;
                if (!d.retired) {
                    if (success) ds4_hid_accept_input(d, bytes, now);
                    else {
                        atomic_fetch_add_u64(&g_ds4HidReadErrors, 1);
                        ds4_hid_error("read completion", error); ds4_hid_retire(d);
                    }
                }
            }
            if (d.retired) break;
            HANDLE event = d.read.event;
            d.read = {}; d.read.event = event; g_ds4Api.resetEvent(event);
            BOOL success = g_api.ReadFile(d.file, d.input, d.inputBytes, &bytes, &d.read);
            DWORD error = success ? 0u : g_ds4Api.lastError();
            if (success) ds4_hid_accept_input(d, bytes, now);
            else if (error == DS4_IO_PENDING) { d.readPending = true; break; }
            else {
                atomic_fetch_add_u64(&g_ds4HidReadErrors, 1);
                ds4_hid_error("read", error); ds4_hid_retire(d); break;
            }
        }
        if (!d.retired && d.ready && now >= d.inputTick && now - d.inputTick > 1000u)
            d.ready = false;
        if (!d.retired && now >= d.openTick && now - d.openTick > 5000u &&
            (!d.inputTick || (now >= d.inputTick && now - d.inputTick > 5000u))) {
            ds4_hid_error("input timeout", 1460u); ds4_hid_retire(d);
        }
        if (d.retired) {
            if (!d.readPending && !d.writePending) {
                g_api.CloseHandle(d.file); g_api.CloseHandle(d.read.event);
                g_api.CloseHandle(d.write.event); d = {};
            }
        } else {
            if (d.dirty) ds4_hid_submit(d, now);
            if (d.closing && !d.writePending && !d.dirty) ds4_hid_retire(d);
        }
    }
}

static bool ds4_hid_open(u64 now, u16 desiredProduct) {
    if (!g_ds4ApiReady) return false;
    if (g_ds4Active && desiredProduct && g_ds4Active->product != desiredProduct) {
        Ds4HidDevice* previous = g_ds4Active;
        if (previous->desiredActive || previous->sentActive) {
            set_ds4_hid_rumble(0.0f, 0.0f);
            previous->closing = true; // let the final zero complete before cancellation
        } else ds4_hid_retire(*previous);
        g_ds4Active = nullptr; g_ds4ScanDue = 0;
    }
    if (g_ds4Active) return true;
    if (g_ds4ScanDue && now < g_ds4ScanDue) return false;
    g_ds4ScanDue = now + 2000u;
    atomic_fetch_add_u64(&g_ds4HidScans, 1);
    Ds4HidDevice* freeSlot = nullptr;
    for (u32 i = 0; i < DS4_HID_SLOTS; ++i) if (!g_ds4Hid[i].file) { freeSlot = &g_ds4Hid[i]; break; }
    if (!freeSlot) return false; // canceled storage cannot be reused while pending
    Ds4Guid guid{}; g_ds4Api.guid(&guid);
    HANDLE list = g_ds4Api.getClass(&guid, nullptr, nullptr, 0x12u); // present + interfaces
    if (!list || list == invalid_handle_value()) {
        ds4_hid_error("enumeration", g_ds4Api.lastError()); return false;
    }
    bool opened = false;
    for (u32 i = 0; i < 256u; ++i) {
        Ds4Interface iface{}; iface.size = sizeof(iface);
        if (!g_ds4Api.enumerate(list, nullptr, &guid, i, &iface)) break;
        DWORD required = 0;
        g_ds4Api.detail(list, &iface, nullptr, 0, &required, nullptr);
        Ds4Detail detail{}; detail.size = 8u; // sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W) on x64
        if (required < 6u || required > sizeof(detail) ||
            !g_ds4Api.detail(list, &iface, &detail, sizeof(detail), &required, nullptr)) continue;
        HANDLE probe = g_api.CreateFileW(detail.path, 0u, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                         nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (!probe || probe == invalid_handle_value()) continue;
        Ds4Attributes attributes{}; attributes.size = sizeof(attributes);
        bool match = g_ds4Api.attrs(probe, &attributes) &&
            is_ds4_product(attributes.vendor, attributes.product) &&
            (!desiredProduct || attributes.product == desiredProduct);
        Ds4Caps caps{}; void* preparsed = nullptr; bool bluetooth = false;
        if (match) {
            match = g_ds4Api.preparsed(probe, &preparsed) && preparsed;
            if (match) match = g_ds4Api.caps(preparsed, &caps) >= 0 &&
                               ds4_caps_transport(caps, bluetooth);
            if (preparsed) g_ds4Api.freePreparsed(preparsed);
        }
        g_api.CloseHandle(probe);
        if (!match) continue;
        HANDLE file = g_api.CreateFileW(detail.path, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | DS4_FILE_OVERLAPPED, nullptr);
        if (!file || file == invalid_handle_value()) {
            atomic_fetch_add_u64(&g_ds4HidOpenFailures, 1);
            ds4_hid_error("open read/write", g_ds4Api.lastError()); continue;
        }
        HANDLE readEvent = g_ds4Api.createEvent(nullptr, 1, 0, nullptr);
        HANDLE writeEvent = g_ds4Api.createEvent(nullptr, 1, 0, nullptr);
        if (!readEvent || !writeEvent) {
            DWORD error = g_ds4Api.lastError();
            if (readEvent) g_api.CloseHandle(readEvent);
            if (writeEvent) g_api.CloseHandle(writeEvent);
            g_api.CloseHandle(file); ds4_hid_error("event", error); continue;
        }
        Ds4HidDevice& d = *freeSlot; d = {};
        d.file = file; d.read.event = readEvent; d.write.event = writeEvent;
        d.vendor = attributes.vendor; d.product = attributes.product;
        d.inputBytes = caps.inputBytes; d.outputBytes = caps.outputBytes;
        d.bluetooth = bluetooth; d.openTick = now; d.generation = ++g_ds4NextGeneration;
        g_ds4Active = &d; opened = true;
        g_lastRumblePhysicalWriteTick = 0;
        atomic_fetch_add_u64(&g_ds4HidOpens, 1);
        TextBuf b{}; tb_str(b, "R9P DS4 HID opened vid=0x"); tb_hex(b, d.vendor, 4);
        tb_str(b, " pid=0x"); tb_hex(b, d.product, 4);
        tb_str(b, d.bluetooth ? " transport=Bluetooth" : " transport=USB");
        tb_str(b, " inputBytes/outputBytes="); tb_u64(b, d.inputBytes); tb_char(b, '/');
        tb_u64(b, d.outputBytes); tb_endline(b); append_log(b.bytes, b.len);
        break;
    }
    g_ds4Api.destroyList(list);
    return opened;
}

static bool poll_ds4_hid(XINPUT_STATE& state, u64 now, u16 desiredProduct) {
    g_ds4InputReady = false;
    if (!ds4_hid_open(now, desiredProduct) || !g_ds4Active) return false;
    ds4_hid_service(now);
    if (!g_ds4Active) return false;
    Ds4HidDevice& d = *g_ds4Active;
    atomic_store_u32(&g_activeVendor, d.vendor); atomic_store_u32(&g_activeProduct, d.product);
    atomic_store_u32(&g_activeDualSense, 0); atomic_store_u32(&g_activeDeviceStatus, 1);
    atomic_store_u32(&g_lastGameInputButtons, 0);
    g_ds4InputReady = d.ready;
    state = d.ready ? d.state : XINPUT_STATE{};
    u32 wasTouch = atomic_load_u32(&g_rawTouchpadDown);
    bool touch = d.ready && d.touch && g_cfg.menuRawTouchpad;
    atomic_store_u32(&g_rawTouchpadDown, touch ? 1u : 0u);
    // DS4's HID touch click has a documented bit. Do not pretend it is a
    // GameInput raw-button array (index 13 was never verified for DS4).
    atomic_store_u32(&g_rawControllerButtonCount, 0);
    atomic_store_u64(&g_rawControllerButtons, 0);
    if (touch && !wasTouch) atomic_fetch_add_u64(&g_rawTouchpadEdges, 1);
    return true;
}
static void select_ds4_hid_transport(XINPUT_STATE& state, u32& backend,
                                     bool& connected, u64 now) {
    bool candidate = backend == 0u || (backend == 1u &&
        is_ds4_product(atomic_load_u32(&g_activeVendor),
                       atomic_load_u32(&g_activeProduct)));
    if (candidate && poll_ds4_hid(state, now,
            backend == 1u ? (u16)atomic_load_u32(&g_activeProduct) : 0u))
        backend = 3u, connected = true;
    if (backend != 3u && g_ds4Active &&
        (g_ds4Active->desiredActive || g_ds4Active->sentActive))
        set_ds4_hid_rumble(0.0f, 0.0f);
}
static void log_ds4_hid_status() {
    TextBuf b{}; tb_str(b, "R9P DS4 HID active/normalized/transport=");
    tb_u64(b, g_ds4Active ? 1u : 0u); tb_char(b, '/');
    tb_u64(b, g_ds4InputReady ? 1u : 0u); tb_char(b, '/');
    tb_str(b, !g_ds4Active ? "none" : (g_ds4Active->bluetooth ? "Bluetooth" : "USB"));
    tb_str(b, " scans/opens/openFail="); tb_u64(b, atomic_load_u64(&g_ds4HidScans));
    tb_char(b, '/'); tb_u64(b, atomic_load_u64(&g_ds4HidOpens)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_ds4HidOpenFailures));
    tb_str(b, " input/invalid="); tb_u64(b, atomic_load_u64(&g_ds4HidInputReports));
    tb_char(b, '/'); tb_u64(b, atomic_load_u64(&g_ds4HidInvalidReports));
    tb_str(b, " submitted/completed/writeErr/readErr/coalesced=");
    tb_u64(b, atomic_load_u64(&g_ds4HidSubmitted)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_ds4HidCompleted)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_ds4HidWriteErrors)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_ds4HidReadErrors)); tb_char(b, '/');
    tb_u64(b, atomic_load_u64(&g_ds4HidBusyCoalesces));
    tb_str(b, " lastError="); tb_u64(b, atomic_load_u32(&g_ds4HidLastError));
    tb_str(b, " rumbleBackend:1=GI-dispatch,2=XI-success,3=DS4-HID; HID completed means Windows accepted the report");
    tb_endline(b); append_log(b.bytes, b.len);
}
static void ds4_hid_shutdown() {
    if (!g_ds4ApiReady) return;
    if (g_ds4Active && (g_ds4Active->desiredActive || g_ds4Active->sentActive))
        set_ds4_hid_rumble(0.0f, 0.0f);
    for (u32 attempt = 0; attempt < 16u; ++attempt) {
        u64 now = g_api.GetTickCount64 ? g_api.GetTickCount64() : 0;
        ds4_hid_service(now);
        if (!g_ds4Active || (!g_ds4Active->writePending && !g_ds4Active->dirty)) break;
        g_api.Sleep(2);
    }
    for (u32 i = 0; i < DS4_HID_SLOTS; ++i) ds4_hid_retire(g_ds4Hid[i]);
    ds4_hid_service(g_api.GetTickCount64 ? g_api.GetTickCount64() : 0);
}
