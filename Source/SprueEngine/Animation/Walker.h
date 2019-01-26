#pragma once

#include <SprueEngine/ClassDef.h>

#include <math.h>

namespace SprueEngine
{

    // P = period
    // Ri = relative phase
    // 

    inline float CalculateSupportDuration(float aStroke, float aBodySpeed) { return aStroke / aBodySpeed; }
    inline float CalculateDutyFactor(float aSupportDuration, float aPeriod) { return aSupportDuration / aPeriod; }
    inline float CalculateTransferDuration(float aTransferTrajectoryArcLength, float aLegSpeed) { return aTransferTrajectoryArcLength / aLegSpeed; }
    inline float CalculatePeriod(float aSupportDuration, float aTransferDuration) { return aSupportDuration + aTransferDuration; }

    // legState = (legaState^0 + t) mod P

    inline float CalculateLegStateTime(float aLastLegStateTime, float aDeltaTime, float aPeriod) { return ::fmodf((aLastLegStateTime + aDeltaTime), aPeriod); }
    inline float CalculateInitialLegStateTime(float aRelativePhase, float aPeriod) { return aRelativePhase * aPeriod; }

    class SPRUE Walker
    {
    public:
    };

}