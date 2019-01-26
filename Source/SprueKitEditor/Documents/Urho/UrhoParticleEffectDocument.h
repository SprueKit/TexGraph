#pragma once

#include "BaseUrhoDocument.h"

#include <Urho3D/Graphics/ParticleEffect.h>
#ifdef URHO3D_URHO2D
    #include <Urho3D/Urho2D/ParticleEffect2D.h>
#endif

namespace UrhoEditor
{

    template<class T>
    class UrhoParticleEffectDocument : public BaseUrhoDocument
    {
    public:

        Urho3D::SharedPtr<T> GetEffect() { return effect_; }

    protected:
        Urho3D::SharedPtr<T> effect_;
    };

/// Possible to compile without Urho2D support.
#ifdef URHO3D_URHO2D
    class UrhoParticleEffectDocument2D : public UrhoParticleEffectDocument<Urho3D::ParticleEffect2D>
    {
    public:
    
    };
#endif

    class UrhoParticleEffectDocument3D : public UrhoParticleEffectDocument<Urho3D::ParticleEffect>
    {
    public:
    
    };
}