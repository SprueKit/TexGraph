#pragma once

#include "ViewManager.h"

namespace Urho3D
{
    class Context;
}

namespace SprueEditor
{

/// Base class for a view that is orthographic (UV/Graph)
/// Primarily handles viewport control
class OrthographicView : public ViewBase
{
    URHO3D_OBJECT(OrthographicView, ViewBase);
public:
    OrthographicView(Urho3D::Context*, DocumentBase* document);
    virtual ~OrthographicView();
};

}