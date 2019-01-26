#pragma once

#include "Gizmo.h"

#include <EditorLib/Commands/SmartUndoStack.h>

#include <vector>

namespace SprueEditor
{

    class MultiGizmo : public Gizmo
    {
    public:
        MultiGizmo(Urho3D::Node* holdingNode, std::vector< std::shared_ptr<Gizmo> >);
        virtual ~MultiGizmo();

        virtual void Construct() override;
        virtual void NotifyDataChanged(void* src, Selectron* sel) override;

        virtual std::shared_ptr<DataSource> GetEditObject() override { return dataSource_; }
        virtual void RefreshValue() override;
        virtual Urho3D::Matrix3x4 GetTransform() override;
        virtual void ApplyTransform(const Urho3D::Matrix3x4& transform) override;

        virtual void RecordTransforms() override;
        virtual void PushUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix) override;
        virtual SmartCommand* CreateUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix) override;

        virtual bool Equal(Gizmo* rhs) const override;

    private:
        void PrepareCenter();

        Urho3D::Vector3 centerPosition_;
        std::vector<Urho3D::Matrix3x4> recordedTransforms_;
        std::vector< std::shared_ptr<Gizmo> > wrappedGizmos_;
        std::shared_ptr<MultiDataSource> dataSource_;
    };

}