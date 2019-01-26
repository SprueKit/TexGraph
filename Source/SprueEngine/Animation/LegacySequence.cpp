#include "LegacySequence.h"

namespace SprueEngine
{

    bool LegacyKeyframe::IsCloseEnough(const LegacyKeyframe& rhs, float positionTolerance, float rotationTolerance, float scaleTolerance)
    {
        float posDist = (position_ - rhs.position_).Length();
        float rotDist = rotation_.AngleBetween(rhs.rotation_);
        float scaleDist = (scale_ - rhs.scale_).Length();
        
        /// If anyone is outside of the threshold than we keep this keyframe
        if (posDist > positionTolerance || rotDist > rotationTolerance || scaleDist > scaleTolerance)
            return false;
        return true;
    }

    LegacySequence::LegacySequence()
    {

    }

    LegacySequence::~LegacySequence()
    {

    }

}