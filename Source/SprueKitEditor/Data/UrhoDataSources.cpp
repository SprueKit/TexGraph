#include "UrhoDataSources.h"

#include "../Commands/Urho/UrhoPropertyCmd.h"
#include "../SprueKitEditor.h"
#include "../GlobalAccess.h"

#include <EditorLib/Selectron.h>

namespace UrhoEditor
{
    std::map<Urho3D::StringHash, const char*> SerializableDataSource::UrhoIcons =
    {
        { Urho3D::StringHash("Scene"), ":/Images/urho/StaticNode.png" },
        { Urho3D::StringHash("Node"), ":/Images/urho/StaticNode.png" },
        { Urho3D::StringHash("Octree"), ":/Images/urho/NodeGrid.png" },
        { Urho3D::StringHash("DebugRenderer"), ":/Images/urho/NodeGrid.png" },
        { Urho3D::StringHash("PhysicsWorld"), ":/Images/urho/NodeGrid.png" },
        { Urho3D::StringHash("PhysicsWorld2D"), ":/Images/urho/NodeGrid.png" },


        { Urho3D::StringHash("Terrain"), ":/Images/urho/Terrain.png" },
        { Urho3D::StringHash("SplinePath"), ":/Images/urho/SplinePath.png" },
        { Urho3D::StringHash("ScriptInstance"), ":/Images/urho/Script.png" },
        { Urho3D::StringHash("AngelScriptInstance"), ":/Images/urho/Script.png" },
        { Urho3D::StringHash("LuaScriptInstance"), ":/Images/urho/Script.png" },

        { Urho3D::StringHash("Camera"), ":/Images/urho/Camera.png" },
        { Urho3D::StringHash("Light"), ":/Images/urho/Light.png" },
        { Urho3D::StringHash("Zone"), ":/Images/urho/Octree.png" },
        { Urho3D::StringHash("AnimationController"), ":/Images/urho/Timer.png" },
        { Urho3D::StringHash("NavigationMesh"), ":/Images/urho/NavMesh.png" },
        { Urho3D::StringHash("RibbonTrail"), ":/Images/urho/SplinePath.png" },
        { Urho3D::StringHash("AnimatedModel"), ":/Images/urho/AnimatedMesh.png" },
        { Urho3D::StringHash("StaticModel"), ":/Images/urho/Octree.png" },
        { Urho3D::StringHash("StaticModelGroup"), ":/Images/urho/Octree.png" },
        { Urho3D::StringHash("BillboardSet"), ":/Images/urho/Billboard.png" },
        { Urho3D::StringHash("CustomGeometry"), ":/Images/urho/Octree.png" },
        { Urho3D::StringHash("Skybox"), ":/Images/urho/StaticModel.png" },
        { Urho3D::StringHash("ParticleEmitter"), ":/Images/urho/ParticleEffect.png" },
        { Urho3D::StringHash("ParticleEmitter2D"), ":/Images/urho/ParticleEffect.png" },
        { Urho3D::StringHash("DecalSet"), ":/Images/urho/Billboard.png" },


        { Urho3D::StringHash("SoundSource"),   ":/Images/urho/SoundSource.png" },
        { Urho3D::StringHash("SoundSource3D"), ":/Images/urho/SoundSource.png" },
        { Urho3D::StringHash("SoundListener"), ":/Images/urho/SoundListener.png" },

        { Urho3D::StringHash("NetworkPriority"), ":/Images/urho/Network.png" },

        { Urho3D::StringHash("Text3D"), ":/Images/urho/Text.png" },

        { Urho3D::StringHash("Navigable"), ":/Images/urho/NavMesh.png" },
        { Urho3D::StringHash("NavigationMesh"), ":/Images/urho/NavMesh.png" },
        { Urho3D::StringHash("DynamicNavigationMesh"), ":/Images/urho/NavMesh.png" },
        { Urho3D::StringHash("CrowdAgent"), ":/Images/urho/CrowdAgent.png" },
        { Urho3D::StringHash("CrowdManager"), ":/Images/urho/Crowdmanager.png" },
        { Urho3D::StringHash("OffMeshConnection"), ":/Images/urho/connection.png" },
        { Urho3D::StringHash("NavArea"), ":/Images/urho/WindowBox.png" },
        { Urho3D::StringHash("Obstacle"), ":/Images/urho/Obstacle.png" },

        { Urho3D::StringHash("RigidBody"), ":/Images/urho/RigidBody.png" },
        { Urho3D::StringHash("CollisionShape"), ":/Images/urho/CollisionShape.png" },
        { Urho3D::StringHash("CollisionShape2D"), ":/Images/urho/CollisionShape.png" },
        { Urho3D::StringHash("RigidBody2D"), ":/Images/urho/RigidBody.png" },
        { Urho3D::StringHash("Constraint"), ":/Images/urho/Joint.png" },

        { Urho3D::StringHash("StaticSprite2D"), ":/Images/urho/Billboard.png" },
        { Urho3D::StringHash("AnimatedSprite2D"), ":/Images/urho/Billboard.png" },
        { Urho3D::StringHash("TileMap2D"), ":/Images/urho/Terrain.png" },

        { Urho3D::StringHash("CollisionBox2D"), ":/Images/urho/CollisionShape.png" },
        { Urho3D::StringHash("CollisionChain2D"), ":/Images/urho/CollisionShape.png" },
        { Urho3D::StringHash("CollisionCircle2D"), ":/Images/urho/CollisionShape.png" },
        { Urho3D::StringHash("CollisionPolygon2D"), ":/Images/urho/CollisionShape.png" },
        { Urho3D::StringHash("CollisionEdge2D"), ":/Images/urho/CollisionShape.png" },
        { Urho3D::StringHash("ConstraintDistance2D"), ":/Images/urho/Joint.png" },
        { Urho3D::StringHash("ConstraintFriction2D"), ":/Images/urho/Joint.png" },
        { Urho3D::StringHash("ConstraintGear2D"), ":/Images/urho/Joint.png" },
        { Urho3D::StringHash("ConstraintMotor2D"), ":/Images/urho/Joint.png" },
        { Urho3D::StringHash("ConstraintMouse2D"), ":/Images/urho/Joint.png" },
        { Urho3D::StringHash("ConstraintPrismatic2D"), ":/Images/urho/Joint.png" },
        { Urho3D::StringHash("ConstraintPulley2D"), ":/Images/urho/Joint.png" },
        { Urho3D::StringHash("ConstraintRevolute2D"), ":/Images/urho/Joint.png" },
        { Urho3D::StringHash("ConstraintRope2D"), ":/Images/urho/Joint.png" },
        { Urho3D::StringHash("ConstraintWeld2D"), ":/Images/urho/Joint.png" },
        { Urho3D::StringHash("ConstraintWheel2D"), ":/Images/urho/Joint.png" },

        { Urho3D::StringHash("UIElement"), ":/Images/urho/UIElement.png" },
        { Urho3D::StringHash("BorderImage"), ":/Images/urho/UIBorderImage.png" },
        { Urho3D::StringHash("Button"), ":/Images/urho/UIButton.png" },
        { Urho3D::StringHash("CheckBox"), ":/Images/urho/UICheckbox.png" },
        { Urho3D::StringHash("Cursor"), ":/Images/urho/UICursor.png" },
        { Urho3D::StringHash("DropDownList"), ":/Images/urho/UIDropDownList.png" },
        { Urho3D::StringHash("LineEdit"), ":/Images/urho/UILineEdit.png" },
        { Urho3D::StringHash("ListView"), ":/Images/urho/UIListView.png" },
        { Urho3D::StringHash("Menu"), ":/Images/urho/UIDropDownList.png" },
        { Urho3D::StringHash("ScrollBar"), ":/Images/urho/UIScrollBar.png" },
        { Urho3D::StringHash("ScrollView"), ":/Images/urho/UIScrollView.png" },
        { Urho3D::StringHash("Slider"), ":/Images/urho/UISlider.png" },
        { Urho3D::StringHash("Sprite"), ":/Images/urho/UISprite.png" },
        { Urho3D::StringHash("Text"), ":/Images/urho/UIText.png" },
        { Urho3D::StringHash("ToolTip"), ":/Images/urho/UIText.png" },
        { Urho3D::StringHash("View3D"), ":/Images/urho/UIWindow.png" },
        { Urho3D::StringHash("Window"), ":/Images/urho/UIWindow.png" },
        { Urho3D::StringHash("UI"), ":/Images/urho/UIRoot.png" },
        { Urho3D::StringHash("UIRoot"), ":/Images/urho/UIRoot.png" },
    };


    SerializableDataSource::SerializableDataSource(Urho3D::Serializable* serial)
    {
        serial_ = serial;
    }

    void SerializableDataSource::Destroy()
    {
        serial_.Reset();
    }

    bool SerializableDataSource::equals(const DataSource* rhs) const
    {
        if (const SerializableDataSource* other = dynamic_cast<const SerializableDataSource*>(rhs))
            return other->serial_ == serial_;
        return false;
    }

    QString SerializableDataSource::GetName() const
    {
        return "<not nameable>";
    }

    QString SerializableDataSource::GetTypeName() const
    {
        if (serial_)
            return serial_->GetTypeName().CString();
        return "<invalid type - null>";
    }

    std::vector<QString> SerializableDataSource::EnumerateFields()
    {
        std::vector<QString> ret;
        if (auto editable = GetObject())
        {
            auto attrInfo = editable->GetAttributes();
            if (!attrInfo)
                return ret;

            for (auto item : *attrInfo)
            {
                if (item.mode_ & Urho3D::AM_NOEDIT)
                    continue;
                ret.push_back(item.name_.CString());
            }
        }
        return ret;
    }

    bool SerializableDataSource::CanShow(const QString& column)
    {
        return true;
    }

    bool SerializableDataSource::CanSetText(const QString& column)
    {
        return true;
    }

    void SerializableDataSource::FromText(const QString& column, const QString& text)
    {

    }

    QString SerializableDataSource::ToText(const QString& column)
    {
        return QString("< invalid >");
    }

    QVariant SerializableDataSource::ToVariant(const QString& column)
    {
        return QVariant();
    }

    void SerializableDataSource::FromVariant(const QString& column, const QVariant& var)
    {

    }

    QString NodeDataSource::GetName() const
    {
        if (GetNode() && !GetNode()->GetName().Empty())
            return GetNode()->GetName().CString();
        return "<unnamed>";
    }

    QString ComponentDataSource::GetName() const
    {
        return "<not nameable>";
    }
    
    QString UIDataSource::GetName() const
    {
        if (GetElement() && !GetElement()->GetName().Empty())
            return GetElement()->GetName().CString();
        return "<unnamed>";
    }
 
}