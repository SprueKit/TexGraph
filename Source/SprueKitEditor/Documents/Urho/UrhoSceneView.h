#pragma once

#include "../../Views/PerspectiveView.h"

#include "BaseUrhoDocument.h"

namespace UrhoEditor
{

    class UrhoSceneView : public SprueEditor::PerspectiveView
    {
    public:
        UrhoSceneView(Urho3D::Context* context, BaseUrhoDocument* document);
        virtual ~UrhoSceneView();

        virtual void Activate() override;
        virtual void Deactivate() override;
        virtual void SelectionChanged(Selectron* newSelection) override;
        virtual void VisuallyChanged(Selectron* object) override;
        virtual void RenderUpdate() override;
        virtual void GizmoUpdated(std::shared_ptr<SprueEditor::Gizmo> giz) override;

    protected:
        BaseUrhoDocument* document_ = 0x0;
    };

}