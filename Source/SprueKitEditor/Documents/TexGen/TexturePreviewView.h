#pragma once

#include "../../Views/PerspectiveView.h"

namespace Urho3D
{
    class Text;
}

namespace SprueEditor
{
    class TextureDocument;

    class TexturePreviewView : public PerspectiveView
    {
    public:
        TexturePreviewView(TextureDocument* doc, Urho3D::Context*);
        virtual ~TexturePreviewView();

        void UpdateMaterial();

        virtual void Activate() override;
        virtual void Deactivate() override;
        virtual void SelectionChanged(Selectron* newSelection) override;
        virtual void VisuallyChanged(Selectron* object) override;
        virtual void RenderUpdate() override;
        virtual void GizmoUpdated(std::shared_ptr<Gizmo> giz) override;

        Urho3D::SharedPtr<Urho3D::Node> previewNode_;
    };

}