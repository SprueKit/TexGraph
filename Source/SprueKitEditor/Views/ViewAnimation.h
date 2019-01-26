#pragma once

#include <Urho3D/Scene/Node.h>

namespace SprueEditor
{

    /// View animation is used to animate viewport camera behavior for smooth scrolling.
    /// ANY user input into the view will halt the animation.
    class ViewAnimation
    {
    public:
        ViewAnimation(Urho3D::SharedPtr<Urho3D::Node> animateNode) :
            animatingNode_(animateNode)
        {

        }

        virtual ~ViewAnimation()
        {

        }

        virtual void Update(float td) = 0;

    protected:
        Urho3D::SharedPtr<Urho3D::Node> animatingNode_;
    };

}