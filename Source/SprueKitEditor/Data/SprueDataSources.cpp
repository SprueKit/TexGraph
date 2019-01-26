#include "SprueDataSources.h"

#include "../Commands/SprueKit/IEditablePropertyCmd.h"
#include "../SprueKitEditor.h"
#include "../GlobalAccess.h"

#include <EditorLib/Selectron.h>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Graph/Graph.h>
#include <SprueEngine/Graph/GraphNode.h>
#include <SprueEngine/IEditable.h>
#include <SprueEngine/Property.h>
#include <SprueEngine/Core/SceneObject.h>
#include <SprueEngine/FString.h>

#include <QPainter>
#include <QMenu>
#include <QAction>

using namespace SprueEngine;

namespace SprueEditor
{

    IEditableDataSource::IEditableDataSource(IEditable* editable, std::shared_ptr<DataSource> parent) :
        editable_(editable)
    {
        parent_ = parent;
    }

    void IEditableDataSource::Destroy()
    {
        if (editable_)
        {
            delete editable_;
            editable_ = 0x0;
        }
    }

    bool IEditableDataSource::equals(const DataSource* rhs) const
    {
        if (const IEditableDataSource* other = dynamic_cast<const IEditableDataSource*>(rhs))
            return other->editable_ == editable_;
        return false;
    }

    QString IEditableDataSource::GetName() const
    {
        if (editable_)
            return FString("<unnamed> %1", editable_->GetTypeName()).c_str();
        return "<invalid object - null>";
    }

    QString IEditableDataSource::GetTypeName() const
    {
        if (editable_)
            return editable_->GetTypeName();
        return "<invalid object - null>";
    }

    std::vector<QString> IEditableDataSource::EnumerateFields()
    {
        std::vector<QString> ret;
        if (auto editable = GetEditable())
        {
            auto names = editable->GetPropertyNames();
            for (auto name : names)
            {
                // Filter out secret properties so they're not seen
                auto prop = editable->FindProperty(name);
                if (prop && prop->GetFlags() & SprueEngine::PS_Secret)
                    continue;
                ret.push_back(QString(name.c_str()));
            }
        }
        return ret;
    }

    bool IEditableDataSource::CanShow(const QString& column)
    {
        if (auto editable = GetEditable())
        {
            if (auto property = editable->FindProperty(column.toStdString()))
                if (property->GetFlags() & SprueEngine::PS_Secret)
                    return false;
        }
        return true;
    }

    bool IEditableDataSource::CanSetText(const QString& column)
    {
        return true;
    }

    void IEditableDataSource::FromText(const QString& column, const QString& textValue)
    {
        if (auto editable = GetEditable())
        {
            if (auto property = editable->FindProperty(column.toStdString()))
            {
                auto var = property->Get(editable);
                SprueEngine::Variant newValue = var;
                if (var.getType() == SprueEngine::VT_Quat)
                {
                    newValue.FromString(SprueEngine::VT_Vec3, textValue.toStdString());
                    SprueEngine::Quat quat;
                    SprueEngine::Vec3 v = newValue.getVec3();
                    quat.FromEulerXYZ(v.x * DEG_TO_RAD, v.y * DEG_TO_RAD, v.z * DEG_TO_RAD);
                    newValue = quat;
                }
                else
                    newValue.FromString(var.getType(), textValue.toStdString());

                IEditablePropertyCommand* cmd = new IEditablePropertyCommand(std::dynamic_pointer_cast<IEditableDataSource>(shared_from_this()), property, var, newValue, SprueEngine::StringHash());
                Global_DocumentManager()->GetActiveDocument()->GetUndoStack()->push(cmd);
                SprueKitEditor::GetInstance()->GetObjectSelectron()->NotifyDataChanged(this, shared_from_this(), property->GetHash());
            }
        }
    }

    QString IEditableDataSource::ToText(const QString& column)
    {
        if (auto editable = GetEditable())
        {
            auto var = editable->GetProperty(column.toStdString());
            if (var.getType() == VT_Quat)
                var = var.getQuat().ToEulerXYZ() * RAD_TO_DEG;
            return QString(var.ConvertToString().c_str());
        }
        return QString("< invalid >");
    }

    QVariant IEditableDataSource::ToVariant(const QString& column)
    {
        return QVariant();
    }

    void IEditableDataSource::FromVariant(const QString& column, const QVariant& var)
    {

    }

    SceneObjectDataSource::SceneObjectDataSource(SceneObject* sceneObject) :
        IEditableDataSource(sceneObject),
        sceneObject_(sceneObject)
    {

    }

    QString SceneObjectDataSource::GetName() const
    {
        if (GetSceneObject() && !GetSceneObject()->GetName().empty())
            return GetSceneObject()->GetName().c_str();
        return "<unnamed>";
    }

    SprueEngine::SceneObject* SceneObjectDataSource::GetSceneObject() const 
    { 
        return dynamic_cast<SprueEngine::SceneObject*>(editable_); 
    }

    GraphNodeDataSource::GraphNodeDataSource(SprueEngine::GraphNode* node) :
        IEditableDataSource(node)
    {

    }

    SprueEngine::GraphNode* GraphNodeDataSource::GetNode() const 
    { 
        return dynamic_cast<SprueEngine::GraphNode*>(editable_); 
    }

    QString GraphNodeDataSource::GetName() const
    {
        if (GetNode() && !GetNode()->name.empty())
            return GetNode()->name.c_str();
        return "<unnamed>";
    }

    GraphDataSource::GraphDataSource(SprueEngine::Graph* graph) :
        IEditableDataSource(graph)
    {

    }

    QString GraphDataSource::GetName() const
    {
        return "<graph>";
    }

    SprueEngine::Graph* GraphDataSource::GetGraph() const 
    { 
        return dynamic_cast<SprueEngine::Graph*>(editable_); 
    }
}