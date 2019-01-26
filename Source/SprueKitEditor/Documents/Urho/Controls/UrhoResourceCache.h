#pragma once

#include <EditorLib/IM/QImGui.h>

namespace Urho3D
{
    class Context;
    class ResourceCache;
}

namespace UrhoEditor
{

    class UrhoResourceCache : public QImGui
    {
    public:
        UrhoResourceCache(Urho3D::Context* context);
        virtual ~UrhoResourceCache();

    protected:
        virtual void GenerateUI(QPainter*, const QSize& size) override;

        virtual void BuildStyles() override;

        Urho3D::Context* context_;
    };

}