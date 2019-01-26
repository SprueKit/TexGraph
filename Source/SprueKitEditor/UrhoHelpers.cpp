#include "UrhoHelpers.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Math/Vector4.h>

#include <SprueEngine/IMeshable.h>
#include <SprueEngine/FString.h>
#include <SprueEngine/Geometry/MeshData.h>
#include <SprueEngine/Meshing/Octree.h>
#include <SprueEngine/Core/SceneObject.h>

using namespace Urho3D;
using namespace SprueEngine;

namespace SprueEditor
{

    std::string RecoverVariableName(Urho3D::Node* node, const Urho3D::StringHash& hash)
    {
        if (Urho3D::Scene* scene = node->GetScene())
            return scene->GetVarName(hash).CString();
        return std::string();
    }
    
    void StoreVariableName(Urho3D::Node* node, const std::string& name)
    {
        if (Urho3D::Scene* scene = node->GetScene())
        {
            // Only add the variable name if it doesn't already exist
            if (scene->GetVarName(name.c_str()).Empty())
            {
                Urho3D::String names = scene->GetVarNamesAttr();
                scene->SetVarNamesAttr(FString("%1;%2;", names.CString(), name).c_str());
            }
        }
    }

    void AddUrhoVariable(Urho3D::Node* node, const std::string& name, Urho3D::Variant var)
    {
        auto map = node->GetVars();
        map[name.c_str()] = var;
        StoreVariableName(node, name);
    }

    void ConvertGeometry(CustomGeometry* holder, MeshData* meshData)
    {
        if (meshData->GetIndexBuffer().size() > 0)
        {
            holder->SetNumGeometries(1);
            holder->BeginGeometry(0, Urho3D::PrimitiveType::TRIANGLE_LIST);
            holder->SetViewMask(0x80000000);
            holder->SetOccludee(false);
            holder->SetMaterial(holder->GetContext()->GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/Matcap.xml"));

            if (meshData->GetNormalBuffer().empty())
                meshData->CalculateNormals();
            if (meshData->GetTangentBuffer().empty())
                meshData->CalculateTangents();

            for (unsigned i = 0; i < meshData->GetIndexBuffer().size(); ++i)
            {
                unsigned index = meshData->indexBuffer_[i];
                holder->DefineVertex(ToUrhoVec(meshData->positionBuffer_[index]));
                if (meshData->uvBuffer_.size() > 0)
                    holder->DefineTexCoord(ToUrhoVec(meshData->uvBuffer_[index]));
                else
                    holder->DefineTexCoord(Urho3D::Vector2());
                if (meshData->normalBuffer_.size() > 0)
                    holder->DefineNormal(ToUrhoVec(meshData->normalBuffer_[index]));
                if (meshData->tangentBuffer_.size() > 0)
                {
                    Vec4 vec = meshData->tangentBuffer_[index];
                    holder->DefineTangent(Vector4(vec.x, vec.y, vec.z, vec.w));
                }
                //if (meshData->colorBuffer_.size() > 0)
                //    holder->DefineColor(ToUrhoColor(meshData->colorBuffer_[index]));
            }

            holder->Commit();
        }
    }

    void ConvertGeometry(Urho3D::CustomGeometry* holder, SprueEngine::MeshResource* meshResource)
    {
        if (!meshResource)
        {
            holder->SetNumGeometries(0);
            return;
        }
        holder->SetNumGeometries(meshResource->GetMeshCount());
        for (unsigned i = 0; i < meshResource->GetMeshCount(); ++i)
        {
            auto meshData = meshResource->GetMesh(i);

            holder->BeginGeometry(i, Urho3D::PrimitiveType::TRIANGLE_LIST);
            holder->SetViewMask(0x80000000);
            holder->SetOccludee(false);
            holder->SetMaterial(i, holder->GetContext()->GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/Matcap.xml"));
            if (meshData->IsTwoSided())
            {
                auto clone = holder->GetMaterial(i)->Clone();
                clone->SetCullMode(Urho3D::CullMode::CULL_NONE);
                holder->SetMaterial(i, clone);
            }

            // Calculate normals and tangents if they are not present
            if (meshData->GetNormalBuffer().empty())
                meshData->CalculateNormals();
            if (meshData->GetTangentBuffer().empty())
                meshData->CalculateTangents();

            for (unsigned i = 0; i < meshData->GetIndexBuffer().size(); ++i)
            {
                unsigned index = meshData->indexBuffer_[i];
                holder->DefineVertex(ToUrhoVec(meshData->positionBuffer_[index]));
                if (meshData->uvBuffer_.size() > 0)
                    holder->DefineTexCoord(ToUrhoVec(meshData->uvBuffer_[index]));
                else
                    holder->DefineTexCoord(Urho3D::Vector2());
                if (meshData->normalBuffer_.size() > 0)
                    holder->DefineNormal(ToUrhoVec(meshData->normalBuffer_[index]));
                if (meshData->tangentBuffer_.size() > 0)
                {
                    Vec4 vec = meshData->tangentBuffer_[index];
                    holder->DefineTangent(Vector4(vec.x, vec.y, vec.z, vec.w));
                }
                if (meshData->colorBuffer_.size() > 0)
                    holder->DefineColor(ToUrhoColor(meshData->colorBuffer_[index]));
            }
            holder->Commit();
        }
    }

    void CreateMesh(CustomGeometry* holder, const IMeshable* meshData)
    {
        int size = 64;
        SprueEngine::BoundingBox bnds = ((SceneObject*)meshData)->GetWorldBounds().MinimalEnclosingAABB();

        for (; size < bnds.Size().MaxElement(); size *= 2);

        OctreeNode* node = BuildOctree(meshData, Vec3(-size / 2, -size / 2, -size / 2), size, 0.75f);

        SprueEngine::VertexBuffer vertBuffer;
        SprueEngine::IndexBuffer indexBuffer;
        GenerateMeshFromOctree(node, vertBuffer, indexBuffer);

        DestroyOctree(node);

        if (indexBuffer.size() > 0)
        {
            MeshData tempMeshData;
            tempMeshData.indexBuffer_ = indexBuffer;
            tempMeshData.positionBuffer_.reserve(vertBuffer.size());
            int idx = 0;
            for (auto vert : vertBuffer)
            {
                tempMeshData.positionBuffer_.push_back(vert.Position);
                tempMeshData.normalBuffer_.push_back(vert.Normal);
                tempMeshData.colorBuffer_.push_back(RGBA(
                    idx % 3 == 0 ? 1.0f : 0.0f, 
                    idx % 3 == 1 ? 1.0f : 0.0f,
                    idx % 3 == 2 ? 1.0f : 0.0f));
                ++idx;
            }

            ConvertGeometry(holder, &tempMeshData);
        }
    }


    Urho3D::Texture2D* CreateTexture(Urho3D::Context* context, const char* name, std::shared_ptr<SprueEngine::FilterableBlockMap<SprueEngine::RGBA> > bitmap, bool sRGB)
    {
        return CreateTexture(context, name, bitmap.get(), sRGB);
    }

    Urho3D::Texture2D* CreateTexture(Urho3D::Context* context, const char* name, SprueEngine::FilterableBlockMap<SprueEngine::RGBA>* bitmap, bool sRGB)
    {
        ResourceCache* cache = context->GetSubsystem<ResourceCache>();

        bool created = false;
        Urho3D::Texture2D* texture = cache->GetResource<Urho3D::Texture2D>(name, false);
        if (texture == 0x0)
        {
            texture = new Urho3D::Texture2D(context);
            created = true;
        }

        Urho3D::Image* image = new Urho3D::Image(context);
        image->SetSize(bitmap->getWidth(), bitmap->getHeight(), 4);
        for (unsigned y = 0; y < bitmap->getHeight(); ++y)
            for (unsigned x = 0; x < bitmap->getWidth(); ++x)  
                image->SetPixel(x, y, ToUrhoColor(bitmap->get(x, y).Clip()));
        texture->SetSRGB(sRGB);
        texture->SetData(image, true);
        delete image;
        texture->SetName(name);

        // If we created the texture then be sure to store it
        if (created)
            cache->AddManualResource(texture);
        return texture;
    }

    void DrawPie(Urho3D::DebugRenderer* debugRenderer, float angle, const Urho3D::Vector3& center, const Urho3D::Vector3& normal, float radius, const Urho3D::Color& color, int steps, bool depthTest)
    {
        Quaternion orientation;
        orientation.FromRotationTo(Vector3::UP, normal.Normalized());

        Vector3 p = orientation * Vector3(radius, 0, 0) + center;
        unsigned uintColor = color.ToUInt();
        Vector3 right = p.CrossProduct(normal);

        Vector3 last;
        for (int i = 0; i <= steps; i++)
        {
            Vector3 vt;
            int degrees = ((float)i) / (((float)steps) * 360.0f);
            vt = right * cos(((angle) / steps)*i);
            vt += p * sin(((angle) / steps)*i);
            vt += center;
            if (i > 0)
                debugRenderer->AddTriangle(center, last, vt, color);
            last = vt;
        }
    }

    void UrhoEventHandler::Subscribe(Urho3D::Object* from, Urho3D::StringHash eventID, EVENT_SUBSCRIPTION func)
    {
        if (from)
            SubscribeToEvent(from, eventID, URHO3D_HANDLER(UrhoEventHandler, BaseEventHandler));
        else
            SubscribeToEvent(eventID, URHO3D_HANDLER(UrhoEventHandler, BaseEventHandler));
        _ASSERT(subscribers_.find(eventID) == subscribers_.end());
        subscribers_[eventID] = func;
    }

    void UrhoEventHandler::Unsubscribe(Urho3D::Object* from, Urho3D::StringHash eventID)
    {
        if (from)
            UnsubscribeFromEvent(from, eventID);
        else
            UnsubscribeFromEvent(eventID);
    }

    void UrhoEventHandler::BaseEventHandler(Urho3D::StringHash eventID, Urho3D::VariantMap& data)
    {
        auto found = subscribers_.find(eventID);
        if (found != subscribers_.end())
            found->second(eventID, data);
    }
}