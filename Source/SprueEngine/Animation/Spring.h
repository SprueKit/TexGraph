#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

namespace SprueEngine
{

class SPRUE Spring
{
public:
    template<typename T> 
    bool SpringGreaterComp(const T& lhs, const T& rhs) { return lhs > rhs; }

    template<>
    bool SpringGreaterComp<Vec2>(const Vec2& lhs, const Vec2& rhs) { return lhs.LengthSq() > rhs.LengthSq(); }

    template<>
    bool SpringGreaterComp<Vec3>(const Vec3& lhs, const Vec3& rhs) { return lhs.LengthSq() > rhs.LengthSq(); }

    /// Generically compute a spring
    template<typename T>
    T ComputeSpring(const T& target, const T& current, T& previousVelocity, float k = 0.3f, float dampen = 0.3f)
    {
        T force = previousVelocity * (1.0f - dampen) + ((target - current) * k);
        previousVelocity = force;
        return current + force;
    }

    template<>
    Quat ComputeSpring<Quat>(const Quat& target, const Quat& current, Quat& previousVelocity, float k, float dampen)
    {
        Quat force = previousVelocity * (1.0f - dampen) * (target * current.Conjugated() * k);
        previousVelocity = force;
        return current * force;
    }

    /// Like above, but only uses spring to arrive and cannot overshoot the target
    template<typename T>
    T ComputeArrivalSpring(const T& target, const T& current, T& previousVelocity, float k = 0.3f, float dampen = 0.3f)
    {
        T direction = target - current;
        T force = previousVelocity * (1.0f - dampen) + ((direction) * k);
        T anticipated = current + force;
        T anticipatedDirection = anticipated - current;
        if (SpringGreaterComp(anticipatedDirection, direction))
        {
            previousVelocity = direction;
            return target;
        }
        previousVelocity = force;
        return anticipated;
    }

    void Confirm()
    {
        Vec3 vel(0, 0, 0);
        Vec3 value = ComputeSpring(Vec3(0, 0, 0), Vec3(1, 1, 1), vel);

        float fVel = 0.0f;
        float fVal = ComputeSpring<float>(0.0f, 1.0f, fVel);
    }
};


}