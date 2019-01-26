#pragma once

#include "../Gizmo.h"

#include <SprueEngine/IHaveGizmos.h>

namespace SprueEditor
{

    class SprueGizmo : public Gizmo
    {
    public:
        SprueGizmo(std::shared_ptr<DataSource> dataSource, std::shared_ptr<SprueEngine::Gizmo> gizmo);
        virtual ~SprueGizmo();

        virtual void Construct() override;

        std::shared_ptr<SprueEngine::Gizmo> GetGizmo() { return gizmo_; }

        virtual std::shared_ptr<DataSource> GetEditObject() { return dataSource_; }
        virtual void RefreshValue() override;
        virtual Urho3D::Matrix3x4 GetTransform() override;
        virtual void ApplyTransform(const Urho3D::Matrix3x4& transform) override;

        virtual void PushUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix) override;
        virtual SmartCommand* CreateUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix) override;

        virtual void PaintGizmo(Urho3D::Camera* camera, Urho3D::DebugRenderer* debugRender) override;

        virtual bool Equal(Gizmo* rhs) const override;
        virtual bool ExecuteCmd(unsigned cmd) override { return gizmo_->ExecuteCommand(cmd, SprueEngine::Variant()); }

    protected:
        std::shared_ptr<SprueEngine::Gizmo> gizmo_;
        std::shared_ptr<DataSource> dataSource_;
        /// The edit transform does not contain the application of snapping
        Urho3D::Matrix3x4 editTransform_;
    };

}