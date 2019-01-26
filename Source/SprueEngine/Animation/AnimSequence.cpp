#include "AnimSequence.h"

#include <SprueEngine/Core/Context.h>

namespace SprueEngine
{

    AnimSequence::AnimSequence()
    {

    }

    AnimSequence::~AnimSequence()
    {

    }

    void AnimSequence::Register(Context* context)
    {
        context->RegisterFactory<AnimSequence>("AnimSequence", "A collection of animation tracks and keyframes");
        REGISTER_PROPERTY_CONST_SET(AnimSequence, std::string, GetName, SetName, "", "Name", "Identifer for this animation sequence", PS_Default);
        REGISTER_PROPERTY(AnimSequence, unsigned, GetFlags, SetFlags, 0, "Flags", "Special markers for API and batch tool usage to indicate the purposes of this animation sequence", PS_Flags);
        REGISTER_PROPERTY(AnimSequence, float, GetDuration, SetDuration, 0.0f, "Duration", "Determines the total length of the animation", PS_Default);
        REGISTER_PROPERTY_MEMORY(AnimSequence, bool, offsetof(AnimSequence, loops_), false, "Looping", "Enables looping the animation continuously", PS_Default);
    }

}