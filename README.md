# ACBFHapticsBridge
AC Black Flag Resynced – Shadows Haptics Bridge  Adds Assassin’s Creed Shadows–derived haptic feedback and conventional rumble to Assassin’s Creed Black Flag Resynced. Supports X-input/Dinput , Xbox 360/DS4/DS5 Controllers wired or wirelessly , native or emulated.


AC Black Flag Resynced – Shadows Haptics Bridge

Adds Assassin’s Creed Shadows–derived haptic feedback and conventional rumble to Assassin’s Creed Black Flag Resynced.
The mod supports native DualSense haptics (wired-native) and wirelessly through DSX(+ dlc), as well as ordinary rumble for Xbox 360, DS4, XInput-emulated and other compatible controllers. 

Works by hooking into the game exe and using the wwise motion native pipeline that Anvil Engine has to trigger the haptic events so it should technically work with other emulation software like dsx , ds4windows or viiper releases or completely native dinput and x-input.

Confirmed compatible: Steam version, Title Update 1.0.6  ( will update Ubisoft/ Epic versions  in the next day/days-need the files/sha256 hash/logs from those versions)


If you use the Ubisoft Connect or Epic version, please provide your ACBlackFlag.exe SHA-256 hash and the generated ACBFHapticsBridge log(pressing f10 while in menus and once again while in gameplay then close game) so support can be added. Thanks!


DONATING ( not obligated to, but apreciated nonetheless! :D)

If you liked this mod and  wanna buy me a coffee, show your apreciation/ leave a tip , you can donate via these  wallets , multicoin, preferably BSC Network , ETH Network (BNB , USDC , or whatever you would like) ( Not obligated to , just helps pay bills , motivate me :D and encourages me to pursue my endeavours)
 
0xFfd40Cf24B1484f8567181E1c550eA494967f36f (metamask multicoin wallet)

DYgtA98yJ7yMf7rg8Mx65jiybeL4FyqpKGoED3VW2MuG --- Solana Network adress

TL2oF5L8UNVLS58BmmsynndjY3nZAkTnid --Tron Network adress

bc1qjl3kv3s7l2wrmx68vhdmuqnfcj6hl6650krufx  --Bitcoin Native Segwit adress

Paypal profile donate not working for my country and cant leave the direct link here as that would mean doxxing myself :(


AAAANDDDDDDDDDD THIS IS MY FIRST MOD!! Glad it works for me at least!! :D ( please report any bugs or stuff so i can work on them if i can)


What the ASI mod does (v2.1)

ACBFHapticsBridge adds Shadows-derived feedback to Black Flag Resynced:
Full 48 kHz stereo DualSense haptics through the game’s native Wwise haptic path.
Conventional two-motor rumble for Xbox, DS4, DSX Xbox 360 emulation, and compatible GameInput controllers.
Walking and running textures.
Sprint-start and water-lunge impacts.
Jump takeoff and airtime-based landing feedback.
Climbing-jump, swimming, diving, water-entry and surface effects.
Ground and directional dodge effects.
Parry, light attack and two-stage heavy-attack fallbacks.
Hood and sheathe/unsheathe feedback.
Short feedback for inventory/map menu navigation.
Detection of Black Flag’s existing native feedback to prevent doubled effects.
Native wired DualSense also works normally with Steam Input enabled.

V2.1 Changelog
Compared with v6-r5-menu-state-recovery(v2.0)

R7 includes the subsequent controller movement work from r6 plus the new r7 gameplay additions.

NEW — Kick Haptic

Added dedicated kick detection:

Hold R2 + press Square

New KickModifier input binding
New KickAction input binding
Activates only from the Square press edge while R2 is already held
Holding Square first and then pressing R2 does not incorrectly trigger it
Holding Square does not continuously retrigger the effect
Disabled during inferred menu state
Uses native-output arbitration to prevent duplicated feedback
Dedicated 120 ms anti-double-trigger cooldown
Separate [Effect.Kick] configuration section
Independently configurable intensity and timing
DualSense Walking Feedback Increased

The original r5 walking level was considered too subtle on DualSense.

Controller-movement tuning introduced a dedicated DualSense-only walking multiplier.

r6:

DualSense Haptic Walking = 2x original r5 level

r7:

DualSense Haptic Walking = 4x original r5 level

This amplification affects DualSense/Wwise native haptics only.

It does not make conventional Xbox/XInput walking rumble four times stronger.

This allows DualSense footsteps to remain clearly perceptible without making ordinary rumble excessive.

Improved Conventional Running Feedback

Retains the controller movement tuning introduced after r5.

Conventional/XInput running now receives a dedicated movement pulse system so sustained running remains clearly noticeable on normal two-motor controllers.

Current tuning uses short repeated running pulses rather than relying entirely on the DualSense-oriented haptic waveform behavior.

This makes running more distinct from walking on conventional controllers.

Preserved Menu-State Recovery

All menu-state recovery improvements from v6-r5-menu-state-recovery are retained.

This includes recovery after:

Opening/closing menus
Controller/menu state changes
Temporary player-state loss
Returning from menus to gameplay
Rebinding Edward's gameplay-state context after interruption

Movement feedback resumes only after gameplay movement has been confirmed, helping prevent false walking/running effects while a menu is open.

Native Haptic Arbitration Retained

The bridge continues to watch Black Flag's existing:

Quad Audio haptics
GameInput rumble
XInput rumble

When the game already generates suitable native feedback, the bridge suppresses the corresponding fallback effect rather than stacking another effect on top of it.

This is especially useful for combat actions such as attacks, clashes and parries.

Movement Tuning Retained

R7 keeps the tuned movement behavior introduced during the r6 development branch:

Stable walking loop
Stable running loop
Smooth movement fade
Stronger opening section when starting to run
Sprint-start impact
Separate walk/run thresholds
Sprint toggle support
Movement suppression while menus are active
Gameplay-State Haptics Retained

The state-based traversal system remains active for:

Jumping
Falling
Short landings
Normal landings
Long landings
Haystack landings
Water entry
Water surface
Swimming
Swim-up
Water lunge
Climbing-related traversal

Landing strength continues to scale according to measured airtime.

Per-Effect Configuration Retained

Each automatic effect remains independently configurable where applicable.

Users can adjust:

Enable/disable state
Intensity
Trigger timing
Waveform start trim
Cooldowns
Movement loop timing

This allows the bridge to be tuned without recompiling the ASI.

R7 Effect Summary

R7 currently provides contextual feedback for:

Movement

Walking
Running
Sprint start

Parkour

Jump
Short landing
Normal landing
Long landing
Haystack landing
Ground dodge
Directional dodge

Water

Dive / water entry
Surface
Swim up
Water lunge / fast-swim start

Combat

Parry
Light attack
Heavy attack first impact
Heavy attack second impact
Kick

Character / Equipment

Weapon sheathe / unsheathe
Shared hood/action transition

Menus

Menu open
Menu navigation ticks
Menu/tab interaction

The bridge keeps Black Flag's native feedback whenever possible and fills in actions where the original game provides little or no controller response.
Keyboard/ASI hotkeys

Keyboard key	ASI function
F9	Plays the Dodge test effect through DualSense haptics and conventional rumble. Useful to know if the mod works or not.
F10	Writes diagnostic information to
ACBFHapticsBridge-v6.log
, including controller backend, game state, effects, rumble activity, and r5 movement tuning.( will require this in case of bugfixes)
F11	Enables/disables the entire mod. Disabling immediately stops all ASI-added haptics and rumble.
F12	Enables/disables only automatic gameplay and menu effects. The ASI stays loaded, native game feedback remains active, and F9 still works. Useful for comparisons and ship gameplay.(basicaly disables haptic feedback/rumble by mod while keeping the asi loaded for testing/jackdaw steering. Will update in future the jackdaw state ship event)

Default controller bindings

INI action	Default input	What it controls
Jump
Cross
Ground jump, directional climbing jump, swim-up and jump/landing state correlation
DodgeDive
Circle
Ground dodge, directional roll, airborne dive request and water dive
Parry
L1
Parry fallback when the game produces no native effect
Attack
R1
Tap/release light attack and held two-stage heavy attack
SprintLunge
L3
Sprint latch, sprint-start kick and water lunge/fast swimming
SheatheHood
DPadLeft
Hood toggle and sheathe/unsheathe feedback
The left stick remains responsible for:
Walking and running strength.
Directional-dodge detection.
Climbing-jump direction.
Menu navigation.
Landing, haystack entry, water entry and water surface effects are primarily triggered by detected character states rather than a remappable button.
Changing controller inputs

Open
ACBFHapticsBridge-v6.ini
beside
ACBlackFlag.exe
and find:


[InputBindings]
Jump=Cross
DodgeDive=Circle
Parry=L1
Attack=R1
SprintLunge=L3
SheatheHood=DPadLeft

Change the value after
=
to match the corresponding in-game controller assignment.
For example, if you change Jump to Triangle in Black Flag:


[InputBindings]
Jump=Triangle
DodgeDive=Circle
Parry=L1
Attack=R1
SprintLunge=L3
SheatheHood=DPadLeft

This moves all relevant Jump processing to Triangle, including:
Immediate takeoff feedback.
Jump-state correlation.
Climbing-jump detection.
Swim-up feedback.
The guarded landing cycle.
Cross will no longer trigger provisional jump feedback.
Restart the game after changing the INI. Settings are only loaded during startup.
Available input names
PlayStation value	Xbox alias	Physical position
Cross
A
Bottom face button
Circle
B
Right face button
Square
X
Left face button
Triangle
Y
Top face button
L1
LB
Left shoulder
R1
RB
Right shoulder
L2
LT
Left trigger
R2
RT
Right trigger
L3
LS
Left-stick click
R3
RS
Right-stick click
DPadUp
Same	D-pad Up
DPadDown
Same	D-pad Down
DPadLeft
Same	D-pad Left
DPadRight
Same	D-pad Right
None
Disabled
or
Off
No direct input
Values are case-insensitive. Spaces, underscores and hyphens are ignored.
Important: use
Cross
for the PlayStation cross/X button. Writing
X
means the Xbox X position, which corresponds to PlayStation Square.
The following cannot be used as gameplay action bindings:
Touchpad
PS/Guide button
Controller paddles
Keyboard keys
Analog-stick directions
Options/Start
Share/Create/View
Menu controls are handled separately.
Do not give multiple action entries the same physical button unless you intentionally want both actions to trigger. The mod does not automatically resolve duplicate assignments.
Press
F10
after launching. The log should contain something similar to:


INPUTCFG jump=Triangle dodgeDive=Circle parry=L1 attack=R1 sprintLunge=L3 sheatheHood=DPadLeft invalidFallbacks=0

If a name is invalid, that action falls back to its tested default and
invalidFallbacks
increases.
Changing individual effects

Every automatic effect has its own section:


[Effect.Jump]
Enabled=1
IntensityMultiplier=1.00
TriggerOffsetMs=0
StartTrimOffsetMs=0
CooldownMs=300

The common settings mean:
Setting	Function
Enabled=1
Effect enabled
Enabled=0
Effect disabled
IntensityMultiplier=0.50
Half strength
IntensityMultiplier=1.00
Default strength
IntensityMultiplier=1.25
25% stronger
IntensityMultiplier=2.00
Double strength
TriggerOffsetMs
Adds or removes delay after the input/state is detected
StartTrimOffsetMs
Changes where playback begins inside the captured waveform
IntensityMultiplier
changes both DualSense haptics and conventional rumble.
For timing:
Negative
TriggerOffsetMs=-20
makes the effect earlier, but never before the input/state was detected.
Positive
TriggerOffsetMs=20
makes it later.
Positive
StartTrimOffsetMs=20
skips more of the waveform’s beginning, producing a faster impact.
Negative
StartTrimOffsetMs=-20
restores more of the original lead-in.
These settings never change game or animation speed.
Examples
Disable walking feedback:


[Effect.Walk]
Enabled=0

Reduce parry strength by 30%:


[Effect.Parry]
Enabled=1
IntensityMultiplier=0.70

Make the ground-dodge impact start faster:


[Effect.GroundDodge]
TriggerOffsetMs=-20
StartTrimOffsetMs=25

Disable only the second heavy-attack impact:


[Effect.HeavyAttackSecond]
Enabled=0

Make menu navigation softer:


[Effect.MenuTick]
IntensityMultiplier=0.60

Special effect settings

Some sections include additional controls:
Section	Additional controls
[Effect.Walk]
LoopStartMs
,
LoopEndMs
,
FadeMs
[Effect.Run]
Loop points, fade and
OpeningDurationMs
[Effect.Jump]
CooldownMs
Landing sections	Separate short, normal and long intensity/timing controls
[Effect.WaterDive]
Input and game-state cooldowns
[Effect.MenuTick]
Direction-repeat delay and interval
[Effect.HeavyAttackFirst]
HoldThresholdMs
[Effect.HeavyAttackSecond]
DelayAfterFirstMs
Example heavy-attack timing:


[Effect.HeavyAttackFirst]
HoldThresholdMs=320

[Effect.HeavyAttackSecond]
DelayAfterFirstMs=640

Global settings

Output type


[Output]
NativeHaptics=1
ConventionalRumble=1
PreferGameInput=1
RumbleOnDualSense=1

NativeHaptics=0
disables full DualSense waveform injection.
ConventionalRumble=0
disables ordinary low/high-motor vibration.
RumbleOnDualSense=0
leaves only the full native DualSense haptic layer on detected DualSense devices.
Keep both enabled for the normal hybrid configuration.
Overall intensity


[Playback]
MasterGain=1.00
MovementGain=1.00
OneShotGain=1.00

MasterGain
changes everything.
MovementGain
changes walk/run textures.
OneShotGain
changes jump, landing, dodge, attack, water and menu impacts.
Conventional rumble


[Rumble]
Gain=1.00
MovementGain=2.00
LowMotorScale=3.00
HighMotorScale=1.60
Deadzone=0.008

These only control conversion of the Shadows waveforms into conventional controller motors.
Movement


[Movement]
WalkThreshold=11000
RunThreshold=24500
SprintToggle=1

Lower thresholds make walking/running activate with less stick movement.
SprintToggle=1
means press SprintLunge once.
SprintToggle=0
means hold the configured SprintLunge input.
Menus


[Menus]
Enabled=1
TickGain=0.28
OpenGain=0.28

Setting
Enabled=0
disables ASI-added map/inventory menu feedback without disabling gameplay effects.
Important INI warning

Use
[InputBindings]
for controller buttons.
The lower
[Buttons]
section contains advanced waveform names such as:


BEffect=dodge
LBEffect=parry
RBEffect=light_attack_single

Those are not controller-remapping fields. Changing them selects which captured HFX waveform an action uses. Normally, leave
[Buttons]
,
[Hook]
, and the advanced
[GameplayState]
addresses at their supplied values.
Future updates will include (hopefully) better haptics and rumble effects captured from AC Shadows.

Not perfect but perfectible. DEadDRop out!
