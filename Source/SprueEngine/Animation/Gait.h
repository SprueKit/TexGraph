#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <vector>

namespace SprueEngine
{

// Notes: from Steve Rotenburg's Locomotion (2005)
//  Support = foot on ground
//  Transfer = foot in air
//  Gait-cycle = 1 complete rep
//  Gait-period = 1 cycle
//      Gait-period = Support-duration + Transfer-duration
//  Gait-frequency = 1 / gait-period
//  Each leg 1 complete limb cycle in a gait-cycle
//  Gait-phase = 0-1 normalized time in cycle
//  DutyFactor = Support-duration / Gait-period
//  Step-phase/step-trigger = 0-1 range for LIMB's cycle, 0 == lift-off
//      Ground-contact at 1 - duty-factor

/* Phase shifts

Biped Walk:
    0.00 0.50

Quadrupeds:
    Walk: 
        0.25 0.75
        0.00 0.50
    Amble:
        0.20 0.70
        0.00 0.50
    Trot:
        0.50 0.00
        0.00 0.50
    Pace/Rack:
        0.00 0.50
        0.00 0.50
    Canter:
        0.70 0.00  | 0.60 0.00
        0.00 0.30  | 0.00 0.10
    Transverse Gallop:
        0.50 0.60
        0.00 0.10
    Rotary Gallop:
        0.60 0.50
        0.00 0.10
    Equestrian Gallop:
        0.40 0.50
        0.00 0.10
    Feline Gallop:
        0.60 0.70
        0.00 0.10
    Bound:
        0.50 0.50
        0.00 0.00
    Pronk:
        0.00 0.00
        0.00 0.00

Hexapods:
    Off-sync back to front wave: (slow)
        0.6 0.1
        0.3 0.8
        0.0 0.5
    Tripod: (fast)
        0.0 0.5
        0.5 0.0
        0.0 0.5

Octapods:
    Off-sync back to front: (slow)
        0.6 0.1
        0.4 0.9
        0.2 0.7
        0.0 0.5
    Quadrapod: (fast)
        0.5 0.0
        0.0 0.5
        0.5 0.0
        0.0 0.5

More equal:
    Off-sync back to front
        Lerp phase factor for 0.0 to (HalfNumLegs / 2) - 0.1
*/

/// Contains the data for an individual leg's pattern of movement.
struct SPRUE StepData
{
    /// Relative phase is when the leg starts to move (from a zero launch).
    float relativePhase_ = 0.0f;
    /// Relative scale of the leg's transfer responsibilities. DutyFactor = SupportDuration / Period
    float dutyFactor_ = 0.5f;
    /// Amount of time the leg must stay on the ground. SupportDuration = stride / movement-speed
    float supportDuration_ = 0.5f;
};

/// Contains the data for all of the leg's in a particular type of motion
struct SPRUE GaitData
{
    std::vector<StepData*> steppingData_;
};

struct SPRUE LegState
{
    /// Data used for this legs cycling control.
    StepData* data_;
    /// Where did the foot land?
    Vec3 takeOff_;
    /// Time in step phase.
    float stepPhase_ = 0.0f;
    /// Normalized position in step phase.
    float transferPhase_ = 0.0f;
    /// Whether we're in the step phase or not.
    bool inStepPhase_ = false;

    /// Updates the legstate for the given delta.
    void Update(float phaseDelta, float phasePeriod, float lastGaitPhase, float newGaitPhase);
};

struct SPRUE GaitState
{
    Vec3 velocity_;
    /// Current time in the gait period.
    float gaitPhase_ = 0.0f;
    /// How much time it takes to complete a full cycle of motion.
    float gaitPeriod_ = 1.0f;
    GaitData* gait_ = 0x0;
    std::vector<LegState*> legStates_;

    GaitState(GaitData* gait);
    GaitState(const GaitState& rhs);
    ~GaitState();
    void Update(float deltaTime);
};

}