#include "MultiGizmo.h"

#include <EditorLib/Commands/CompoundCommand.h>
#include <EditorLib/DocumentBase.h>
#include <EditorLib/Selectron.h>

#include <sstream>

using namespace Urho3D;

namespace SprueEditor
{

    MultiGizmo::MultiGizmo(Urho3D::Node* holdingNode, std::vector< std::shared_ptr<Gizmo> > gizmos) :
        Gizmo(),
        wrappedGizmos_(gizmos)
    {
        for (int i = 0; i < gizmos.size(); ++i)
        {
            if (i == 0)
                gizmoCapabilities_ = gizmos[i]->GetCapabilities();
            else
                gizmoCapabilities_ = gizmoCapabilities_ & gizmos[i]->GetCapabilities();
        }
            
        std::vector<std::shared_ptr<DataSource> > srcs;
        for (auto giz : gizmos)
            srcs.push_back(giz->GetEditObject());
        dataSource_.reset(new MultiDataSource(srcs));
        PrepareCenter();
    }

    MultiGizmo::~MultiGizmo()
    {

    }

    void MultiGizmo::Construct()
    {
        Gizmo::Construct();
        SetForTranslation();
    }

    void MultiGizmo::NotifyDataChanged(void* src, Selectron* sel)
    {
        for (unsigned i = 0; i < wrappedGizmos_.size(); ++i)
            wrappedGizmos_[i]->NotifyDataChanged(src, sel);
    }

    void MultiGizmo::RefreshValue()
    {
        for (auto giz : wrappedGizmos_)
            giz->RefreshValue();
        PrepareCenter();
        Gizmo::RefreshValue();
    }

    Matrix3x4 MultiGizmo::GetTransform()
    {
        return Matrix3x4(centerPosition_, Quaternion(), Vector3(1,1,1));
    }

    void MultiGizmo::ApplyTransform(const Matrix3x4& transform)
    {
        Vector3 pos;
        Quaternion rot;
        Vector3 scl;
        transform.Decompose(pos, rot, scl);
        for (auto giz : wrappedGizmos_)
        {
            Matrix3x4 trans = giz->GetTransform();
            Vector3 thisPos;
            Quaternion thisRot;
            Vector3 thisScl;
            trans.Decompose(thisPos, thisRot, thisScl);
            thisPos += (pos - centerPosition_);
            //thisRot = (thisRot * rot).Normalized();
            
            giz->ApplyTransform(Matrix3x4(thisPos, rot, scl));
        }
        PrepareCenter();
    }

    void MultiGizmo::PrepareCenter()
    {
        centerPosition_ = Vector3();
        for (auto giz : wrappedGizmos_)
        {
            Matrix3x4 mat = giz->GetTransform();
            centerPosition_ += mat.Translation();
        }
        centerPosition_ /= wrappedGizmos_.size();
    }

    void MultiGizmo::RecordTransforms()
    {
        recordedTransforms_.clear();
        for (int i = 0; i < wrappedGizmos_.size(); ++i)
            recordedTransforms_.push_back(wrappedGizmos_[i]->GetTransform());
    }

    void MultiGizmo::PushUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix)
    {
        std::vector<SmartCommand*> cmds;
        std::stringstream ss;
        ss << "Transform ";
        for (int i = 0; i < wrappedGizmos_.size(); ++i)
        {
            if (auto undo = wrappedGizmos_[i]->CreateUndo(document, recordedTransforms_[i]))
            {
                cmds.push_back(undo);
                if (cmds.size() > 0)
                    ss << ", ";
                ss << wrappedGizmos_[i]->GetEditObject()->GetName().toStdString();
            }
        }
        if (cmds.size() > 0)
            document->GetUndoStack()->Push(new CompoundCommand(QString::fromStdString(ss.str()), cmds));
    }

    SmartCommand* MultiGizmo::CreateUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix)
    {
        return 0x0;
    }

    bool MultiGizmo::Equal(Gizmo* rhs) const
    {
        if (auto other = dynamic_cast<MultiGizmo*>(rhs))
        {
            if (wrappedGizmos_.size() != other->wrappedGizmos_.size())
                return false;
            for (unsigned i = 0; i < wrappedGizmos_.size(); ++i)
            {
                if (!wrappedGizmos_[i]->Equal(other->wrappedGizmos_[i].get()))
                    return false;
            }
            return true;
        }
        return false;
    }
}