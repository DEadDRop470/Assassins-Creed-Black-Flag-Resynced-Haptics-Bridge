/*
  ACBFHapticsBridge v7-r7-kick-walk4x
  Source-level reconstruction of the R7-specific changes.

  IMPORTANT:
  - This is NOT the lost original full C++ source for the whole ASI.
  - The R7 package was created as a narrow binary patch on top of the compiled
    v6-r6-controller-movement-tuning runtime.
  - This file reconstructs the behavior added/changed by R7, while the exact
    injected machine code is preserved in r7_kick_stub.S.
*/

#include <cstdint>

namespace acbf::r7 {

static constexpr const char* kBuildLabel = "v7-r7-kick-walk4x";
static constexpr std::uint8_t kDefaultTriggerThreshold = 160;
static constexpr float kDefaultParryGain = 0.45f;
static constexpr float kDualSenseWalkMultiplier = 4.00f;
static constexpr std::uint32_t kKickCooldownMs = 120;

// Existing R6 semantic/config state, shown only as the fields R7 depends on.
struct PadState {
    bool squareDown{};
    std::uint8_t rightTrigger{};
};

struct KickConfig {
    bool enabled{true};
    std::uint8_t triggerThreshold{kDefaultTriggerThreshold};
    float intensityMultiplier{1.0f};
    std::int32_t triggerOffsetMs{0};
    std::int32_t startTrimOffsetMs{0};
    std::uint32_t cooldownMs{kKickCooldownMs};
};

struct RuntimeState {
    bool prevSquareDown{};
    std::uint64_t kickNextAllowedMs{};
};

// Conceptual existing R6 scheduler interface.
struct EffectScheduler {
    virtual ~EffectScheduler() = default;
    virtual void queueParryFallback(float gain,
                                    std::int32_t triggerOffsetMs,
                                    std::int32_t startTrimOffsetMs,
                                    bool useNativeArbitration) = 0;
};

// R7 gameplay kick chord: R2 must already be held when the Square press edge
// occurs. Square-alone and Square-first-then-R2 are rejected.
inline void ProcessKickChord(const PadState& pad,
                             const KickConfig& cfg,
                             RuntimeState& state,
                             EffectScheduler& scheduler,
                             bool menuConsumed,
                             bool gameplayInputEnabled,
                             std::uint64_t nowMs,
                             float lbEffectGain = kDefaultParryGain)
{
    const bool modifierDown = pad.rightTrigger >= cfg.triggerThreshold;
    const bool actionDown = pad.squareDown;
    const bool actionEdge = actionDown && !state.prevSquareDown;

    if (!menuConsumed &&
        gameplayInputEnabled &&
        cfg.enabled &&
        modifierDown &&
        actionEdge &&
        nowMs >= state.kickNextAllowedMs)
    {
        scheduler.queueParryFallback(
            lbEffectGain * cfg.intensityMultiplier,
            cfg.triggerOffsetMs,
            cfg.startTrimOffsetMs,
            true /* native-output arbitration */);

        state.kickNextAllowedMs = nowMs + cfg.cooldownMs;
    }

    state.prevSquareDown = actionDown;
}

// R7 keeps R6 movement gains/loop timing unchanged. Only the native 48 kHz
// DualSense walking path receives the extra 4.00x output-specific multiplier.
inline float ApplyNativeDualSenseWalkMultiplier(float r6NativeWalkSample)
{
    return r6NativeWalkSample * kDualSenseWalkMultiplier;
}

// Conventional rumble deliberately does NOT receive the 4x native multiplier.
inline float ApplyConventionalWalkMultiplier(float r6RumbleWalkValue)
{
    return r6RumbleWalkValue;
}

} // namespace acbf::r7
