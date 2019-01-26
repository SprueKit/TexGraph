#pragma once

#include <SprueEngine/Math/Color.h>
#include <SprueEngine/BlockMap.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Math/Color.h>
#include <Urho3D/Math/Matrix3.h>
#include <Urho3D/Math/Matrix3x4.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Quaternion.h>

#include <algorithm>
#include <functional>
#include <map>
#include <memory>

namespace Urho3D
{
    class CustomGeometry;
    class DebugRenderer;
    class Node;
    class Texture2D;
}

namespace SprueEngine
{
    class IMeshable;
    class MeshData;
    class MeshResource;
}

namespace SprueEditor
{
    /// Recovers the name of a variable from the given node's scene
    std::string RecoverVariableName(Urho3D::Node*, const Urho3D::StringHash& hash);
    /// Stores a variable name for the given node in its scene.
    void StoreVariableName(Urho3D::Node*, const std::string& name);
    /// Adds a variable to the given node and stores the name in the node's scene.
    void AddUrhoVariable(Urho3D::Node*, const std::string& name, Urho3D::Variant var);

    inline Urho3D::Vector2 ToUrhoVec(const SprueEngine::Vec2& vec) { return Urho3D::Vector2(vec.x, vec.y); }
    inline Urho3D::Vector3 ToUrhoVec(const SprueEngine::Vec3& vec) { return Urho3D::Vector3(vec.x, vec.y, vec.z); }
    inline Urho3D::Color ToUrhoColor(const SprueEngine::RGBA& vec) { return Urho3D::Color(vec.r, vec.g, vec.b, vec.a); }
    inline Urho3D::Matrix3x4 ToUrhoMat3x3(const SprueEngine::Mat3x3& mat) { return Urho3D::Matrix3(&mat.v[0][0]); }
    inline Urho3D::Matrix3x4 ToUrhoMat3x4(const SprueEngine::Mat3x4& mat) { return Urho3D::Matrix3x4(&mat.v[0][0]); }

    inline SprueEngine::Vec2 ToSprueVec(const Urho3D::Vector2& vec) { return SprueEngine::Vec2(vec.x_, vec.y_); }
    inline SprueEngine::Vec3 ToSprueVec(const Urho3D::Vector3& vec) { return SprueEngine::Vec3(vec.x_, vec.y_, vec.z_); }
    inline SprueEngine::RGBA ToSprueColor(const Urho3D::Color& vec) { return SprueEngine::RGBA(vec.r_, vec.g_, vec.b_, vec.a_); }

    /// Fills the given Urho3D::CustomGeometry with data to match the provided SprueEngine::MeshData
    void ConvertGeometry(Urho3D::CustomGeometry* holder, SprueEngine::MeshData* meshData);
    void ConvertGeometry(Urho3D::CustomGeometry* holder, SprueEngine::MeshResource* meshData);

    void CreateMesh(Urho3D::CustomGeometry* holder, const SprueEngine::IMeshable* meshData);

    Urho3D::Texture2D* CreateTexture(Urho3D::Context* context, const char* name, std::shared_ptr<SprueEngine::FilterableBlockMap<SprueEngine::RGBA> > image, bool sRGB);
    Urho3D::Texture2D* CreateTexture(Urho3D::Context* context, const char* name, SprueEngine::FilterableBlockMap<SprueEngine::RGBA>* image, bool sRGB);

    void DrawPie(Urho3D::DebugRenderer*, float angle, const Urho3D::Vector3& center, const Urho3D::Vector3& normal, float radius, const Urho3D::Color& color, int steps, bool depthTest);

    /// Helper class for dealing with Urho3D event subscribtions to lambdas
    class UrhoEventHandler : public Urho3D::Object
    {
        URHO3D_OBJECT(Object, UrhoEventHandler);
    public:
        UrhoEventHandler(Urho3D::Context* context) : Urho3D::Object(context) { }

        typedef std::function<void(Urho3D::StringHash, Urho3D::VariantMap&)> EVENT_SUBSCRIPTION;

        void Subscribe(Urho3D::Object* from, Urho3D::StringHash eventID, EVENT_SUBSCRIPTION func);
        void Unsubscribe(Urho3D::Object* from, Urho3D::StringHash eventID);

        std::map<Urho3D::StringHash, EVENT_SUBSCRIPTION> subscribers_;

    protected:
        void BaseEventHandler(Urho3D::StringHash eventID, Urho3D::VariantMap& data);
    };

}