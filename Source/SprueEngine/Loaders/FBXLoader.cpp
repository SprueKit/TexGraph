#include "FBXLoader.h"

#include "../FString.h"
#include "../GeneralUtility.h"
#include "../Animation/LegacySequence.h"
#include "../Geometry/MeshData.h"
#include "../Geometry/Skeleton.h"
#include "../Logging.h"
#include "../Resource.h"

#include <fbxsdk.h>
#include <fbxsdk/utils/fbxgeometryconverter.h>

namespace SprueEngine
{

const std::string FBXLoader::resourceURI_("Mesh");
const StringHash  FBXLoader::typeHash_("Mesh");

Mat4x4 ConvertFbxMatrix(const FbxMatrix& matrix)
{
    auto mat = matrix.Transpose();
    return Mat4x4(mat.Buffer()->Buffer());
}

std::string FBXLoader::GetResourceURIRoot() const
{
    return resourceURI_;
}

StringHash FBXLoader::GetResourceTypeID() const
{
    return typeHash_;
}

std::shared_ptr<Resource> FBXLoader::LoadResource(const char* path) const
{
    FbxManager* lSdkManager = FbxManager::Create();

    FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
    lSdkManager->SetIOSettings(ios);

    lSdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_MATERIAL, true);
    lSdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_TEXTURE, true);
    lSdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_LINK, false);
    //lSdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_SHAPE, false);
    lSdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_GOBO, false);
    lSdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_ANIMATION, true);
    lSdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

    FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
    bool lImportStatus = lImporter->Initialize(path, -1, lSdkManager->GetIOSettings());

    if (!lImportStatus) {
        SPRUE_LOG_ERROR(FString("Call to FbxImporter::Initialize() failed.\nError returned: %1\n\n", lImporter->GetStatus().GetErrorString()).str());
        lImporter->Destroy();
        ios->Destroy();
        lSdkManager->Destroy();
        lImporter->Destroy();
        return 0x0;
    }

    FbxScene* scene = FbxScene::Create(lSdkManager, "myScene");
    if (!lImporter->Import(scene) || !scene)
    {
        SPRUE_LOG_ERROR(FString("Failed import of FbxScene: %1", path).str());
        lImporter->Destroy();
        ios->Destroy();
        lSdkManager->Destroy();
        if (scene)
            scene->Destroy();
        return 0x0;
    }

    // Convert into our axis system
    FbxAxisSystem dxStyleAxes(FbxAxisSystem::eOpenGL);
    dxStyleAxes.ConvertScene(scene);

    // Convert any quads/polys into triangles
    FbxGeometryConverter geoConverter(lSdkManager);
    geoConverter.Triangulate(scene, true);

    Skeleton* skeleton = ReadSkeleton(scene->GetRootNode());

    SPRUE_LOG_INFO(FString("Processing FBX Scene: %1", scene->GetName()).str());
    std::vector<MeshData*> meshes;
    for (unsigned i = 0; i < scene->GetNodeCount(); ++i)
    {
        FbxNode* node = scene->GetNode(i);
        if (FbxMarker* marker = node->GetMarker())
        {
            if (marker->GetType() == FbxMarker::eStandard) // Standard marker for whatever purpose
            {

            }
            else if (marker->GetType() == FbxMarker::eEffectorIK) // IK effector marker
            {

            }
        }
        else if (auto mesh = node->GetMesh())
        {
            SPRUE_LOG_INFO(FString("Processing FBX Mesh: %1", node->GetName()).str());

            Mat4x4 transform = ConvertFbxMatrix(node->EvaluateGlobalTransform());
            /// Maps Control-Point-Index -> Vertex-Index for duplicates, vertex buffer can safely be assumed to be valid
            std::multimap<unsigned, unsigned> controlPointsTable;

            int polyCount = mesh->GetPolygonCount();
            std::vector<Vertex> vertices;
            std::vector<unsigned> indices;
            vertices.reserve(mesh->GetControlPointsCount());
            indices.reserve(polyCount * 3);

            unsigned vertexIndex = 0;
            for (int poly = 0; poly < polyCount; ++poly)
            {
                for (int vert = 0; vert < mesh->GetPolygonSize(poly); ++vert, ++vertexIndex)
                {
                    unsigned vidx = vertices.size();
                    //if (vert == 3)
                    //{
                    //    indices.push_back(indices[indices.size()-3]);
                    //    indices.push_back(indices[indices.size()-1]);
                    //}
                    indices.push_back(vidx);
                    vertices.push_back(Vertex());
                    //if (vert == 2)
                    //    std::swap(indices[indices.size() - 3], indices[indices.size() - 1]);

                    int vertIndex = mesh->GetPolygonVertex(poly, vert);
                    controlPointsTable.insert(std::make_pair(vertIndex, vidx));

                    /// Position
                    FbxVector4 position = mesh->GetControlPointAt(vertIndex);
                    vertices[vidx].position_.Set(position.mData[0], position.mData[1], position.mData[2]);
                    vertices[vidx].position_ = transform * vertices[vidx].position_;

                    // Normals
                    int normalCt = mesh->GetElementNormalCount();
                    if (normalCt)
                        vertices[vidx].normal_ = ReadNormal(vertIndex, vertexIndex, mesh);
                    
                    // UV coordinates
                    for (int uvLayer = 0; uvLayer < mesh->GetElementUVCount() && uvLayer < 2; ++uvLayer)
                    {
                        int uvIndex = mesh->GetTextureUVIndex(poly, vert);
                        if (uvLayer == 0)
                            vertices[vidx].uv_ = ReadUV(vertIndex, uvIndex, uvLayer, mesh);
                        else
                        {
                            // set as lightmap UV (UV1)
                        }
                    }
                }
            }

#if 0
            // Bone weights, only bother with them if we have a skeleton
            if (skeleton)
            {
                const int boneSets = mesh->GetDeformerCount(FbxDeformer::eSkin);
                for (unsigned i = 0; i < boneSets; ++i)
                {
                    int clusterCount = ((FbxSkin*)mesh->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();
                    for (int clusterIdx = 0; clusterIdx < clusterCount; ++clusterIdx)
                    {
                        FbxCluster* cluster = ((FbxSkin*)mesh->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(clusterIdx);

                        int boneIndex = 0;
                        if (Joint* found = skeleton->GetByUserData(cluster->GetLink()))
                            boneIndex = skeleton->IndexOf(found);
                        else
                            continue;

                        const int indexCount = cluster->GetControlPointIndicesCount();
                        int* indices = cluster->GetControlPointIndices();
                        double* weights = cluster->GetControlPointWeights();

                        for (int k = 0; k < indexCount; k++)
                        {
                            const int ctrlPoint = indices[k];
                            const float weight = (float)weights[k];

                            // Map control point index onto propert duplicate vertices
                            auto range = controlPointsTable.equal_range(ctrlPoint);
                            for (auto duplicate = range.first; duplicate  != range.second; ++duplicate)
                                vertices[duplicate->second].boneWeights_.AddBoneWeight(boneIndex, weight);
                        }
                    }
                }
            }

            /// Morph targets
            int morphTargetCt = mesh->GetDeformerCount(FbxDeformer::eBlendShape);
            for (int morphTargetIndex = 0; morphTargetIndex < morphTargetCt; ++morphTargetIndex)
            {
                FbxBlendShape* morphTarget = (FbxBlendShape*)mesh->GetDeformer(morphTargetIndex, FbxDeformer::eBlendShape);
                int channelCt = morphTarget->GetBlendShapeChannelCount();

                for (int channelIdx = 0; channelIdx < channelCt; ++channelIdx)
                {
                    FbxBlendShapeChannel* channel = morphTarget->GetBlendShapeChannel(channelIdx);
                    int targetShapeCount = channel->GetTargetShapeCount();
                    for (int targetShapeIdx = 0; targetShapeIdx < targetShapeCount; ++targetShapeIdx)
                    {
                        FbxShape* shape = channel->GetTargetShape(targetShapeIdx);
                        int* indices = shape->GetControlPointIndices();
                        if (indices)
                        {
                            
                        }
                    }
                }
            }
#endif

            // If we have geometry than push back a mesh
            if (vertices.size() && indices.size())
            {
                if (MeshData* mesh = ToMeshData(node->GetName(), vertices, indices))
                {
                    mesh->CalculateBounds();
                    meshes.push_back(mesh);
                }
            }
        }
    }

#if 0
    // Animation
    std::vector<LegacySequence*> sequences;
    if (skeleton)
    {
        const int srcObjects = scene->GetSrcObjectCount();
        for (int i = 0; i < srcObjects; ++i)
        {
            if (FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>(i))
            {
                LegacySequence* sequence = new LegacySequence();
                sequence->SetName(animStack->GetName());
                sequence->GetTimelines().resize(skeleton->GetAllJoints().size());

                if (FbxTakeInfo* take = scene->GetTakeInfo(animStack->GetName()))
                {
                    FbxTime start = take->mLocalTimeSpan.GetStart();
                    FbxTime end = take->mLocalTimeSpan.GetStop();
                    const int takeLength = end.GetFrameCount(FbxTime::eFrames30) - start.GetFrameCount(FbxTime::eFrames30);

                    for (FbxLongLong frame = start.GetFrameCount(FbxTime::eFrames30); frame <= end.GetFrameCount(FbxTime::eFrames30); ++frame)
                    {
                        FbxTime curTime;
                        curTime.SetFrame(frame, FbxTime::eFrames30);
                        for (unsigned jointIdx = 0; jointIdx < skeleton->GetAllJoints().size(); ++jointIdx)
                        {
                            Joint* joint = skeleton->GetAllJoints()[jointIdx];
                            FbxNode* jointNode = (FbxNode*)joint->GetUserData();
                            FbxMatrix transformOffset = jointNode->EvaluateGlobalTransform(curTime);

                            sequence->GetTimelines()[jointIdx].jointIndex_ = jointIdx;
                            LegacyKeyframe key;
                            
                            FbxVector4 pos, scl, shear; 
                            fbxsdk_2015_1::FbxQuaternion rot; 
                            double junk;
                            transformOffset.GetElements(pos, rot, shear, scl, junk);
                            
                            key.position_.Set(pos[0], pos[1], pos[2]);
                            key.rotation_.Set(rot[0], rot[1], rot[2], rot[3]);
                            key.scale_.Set(scl[0], scl[1], scl[2]);

                            sequence->GetTimelines()[jointIdx].frames_.push_back(key);
                        }
                    }
                }

                sequences.push_back(sequence);
            }
        }
    }
#endif

    scene->Destroy(true);
    lImporter->Destroy();
    ios->Destroy();
    lSdkManager->Destroy();

    //TODO add the LegacySequence vector to the mesh resource.
    std::shared_ptr<MeshResource> meshRes = std::make_shared<MeshResource>();
    meshRes->GetMeshes() = meshes;
    if (skeleton)
        meshRes->SetSkeleton(skeleton);

    return meshRes;
}

bool FBXLoader::CanLoad(const char* str) const
{
    return EndsWith(ToLower(str), ".fbx");
}

Vec3 FBXLoader::ReadNormal(int ctrlPoint, unsigned vertIndex, FbxMesh* mesh) const
{
    Vec3 outNorm;
    FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
    switch (vertexNormal->GetMappingMode())
    {
    case FbxGeometryElement::eByControlPoint:
        switch (vertexNormal->GetReferenceMode())
        {
        case FbxGeometryElement::eDirect: {
                auto vec = vertexNormal->GetDirectArray().GetAt(ctrlPoint);
                outNorm.x = vec.mData[0];
                outNorm.y = vec.mData[1];
                outNorm.z = vec.mData[2];
            } break;
        case FbxGeometryElement::eIndexToDirect: {
                int index = vertexNormal->GetIndexArray().GetAt(ctrlPoint);
                auto vec = vertexNormal->GetDirectArray().GetAt(index);
                outNorm.x = vec.mData[0];
                outNorm.y = vec.mData[1];
                outNorm.z = vec.mData[2];
            } break;
        default: {
                SPRUE_LOG_ERROR("Error in FbxMesh vertex normal");
                return outNorm;
            }
        }
        break;

    case FbxGeometryElement::eByPolygonVertex:
        switch (vertexNormal->GetReferenceMode())
        {
        case FbxGeometryElement::eDirect: {
                auto vec = vertexNormal->GetDirectArray().GetAt(vertIndex);
                outNorm.x = vec.mData[0];
                outNorm.y = vec.mData[1];
                outNorm.z = vec.mData[2];
            } break;
        case FbxGeometryElement::eIndexToDirect: {
                int index = vertexNormal->GetIndexArray().GetAt(vertIndex);
                auto vec = vertexNormal->GetDirectArray().GetAt(index);
                outNorm.x = vec.mData[0];
                outNorm.y = vec.mData[1];
                outNorm.z = vec.mData[2];
            } break;
        default: {
                SPRUE_LOG_ERROR("Error in FbxMesh vertex normal");
                return outNorm;
            }
        }
        break;
    }
    return outNorm;
}

Vec2 FBXLoader::ReadUV(int ctrlPoint, unsigned uvIndex, int layer, FbxMesh* mesh) const
{
    Vec2 outUV;
    if (layer >= 2 || mesh->GetElementUVCount() <= layer)
    {
        SPRUE_LOG_ERROR("Error in FbxMesh UV layers");
        return outUV;
    }

    FbxGeometryElementUV* vertexUV = mesh->GetElementUV(layer);
    switch (vertexUV->GetMappingMode())
    {
    case FbxGeometryElement::eByControlPoint:
        switch (vertexUV->GetReferenceMode())
        {
        case FbxGeometryElement::eDirect: {
                outUV.x = vertexUV->GetDirectArray().GetAt(ctrlPoint).mData[0];
                outUV.y = vertexUV->GetDirectArray().GetAt(ctrlPoint).mData[1];
            } break;

        case FbxGeometryElement::eIndexToDirect: {
                int index = vertexUV->GetIndexArray().GetAt(ctrlPoint);
                outUV.x = vertexUV->GetDirectArray().GetAt(index).mData[0];
                outUV.y = vertexUV->GetDirectArray().GetAt(index).mData[1];
            } break;
        default: {
                SPRUE_LOG_ERROR("Error in FbxMesh UV coordinates");
                return outUV;
            }
        }
        break;
    case FbxGeometryElement::eByPolygonVertex:
        switch (vertexUV->GetReferenceMode())
        {
        case FbxGeometryElement::eDirect:
        case FbxGeometryElement::eIndexToDirect: {
                outUV.x = vertexUV->GetDirectArray().GetAt(uvIndex).mData[0];
                outUV.y = vertexUV->GetDirectArray().GetAt(uvIndex).mData[1];
            } break;
        default: {
                SPRUE_LOG_ERROR("Error in FbxMesh UV coordinates");
                return outUV;
            }
        } break;
    }
    return outUV;
}

Skeleton* FBXLoader::ReadSkeleton(fbxsdk::FbxNode* rootNode) const
{
    int boneIndex = 0;
    Skeleton* skeleton = new Skeleton();
    for (int childIndex = 0; childIndex < rootNode->GetChildCount(); ++childIndex)
    {
        FbxNode* curNode = rootNode->GetChild(childIndex);
        BuildSkeletonRecurse(skeleton, curNode, 0x0);
    }

    if (skeleton->GetAllJoints().size())
        return skeleton;

    delete skeleton;
    return 0x0;
}

void FBXLoader::BuildSkeletonRecurse(Skeleton* skeleton, fbxsdk::FbxNode* node, Joint* currentJoint) const
{
    if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
    {
        do {
            FbxSkeleton* skel = node->GetSkeleton();
            if (skel->GetSkeletonType() == FbxSkeleton::eEffector)
                break;

            Joint* joint = new Joint();
            joint->SetUserData(node);
            joint->SetName(node->GetName());

            if (currentJoint)
                currentJoint->AddChild(joint);
            else
                skeleton->AddJoint(joint);

            auto matrix = node->EvaluateGlobalTransform();
            joint->SetModelSpaceTransform(ConvertFbxMatrix(matrix).Float3x4Part());

            currentJoint = joint;

        } while (false);
    }

    for (int i = 0; i < node->GetChildCount(); ++i)
        BuildSkeletonRecurse(skeleton, node->GetChild(i), currentJoint);
}

MeshData* FBXLoader::ToMeshData(const char* name, std::vector<Vertex>& vertices, std::vector<unsigned>& indices) const
{
    if (vertices.size() > 0)
    {
        MeshData* ret = new MeshData();
        ret->SetName(name);
        ret->indexBuffer_ = indices;
        ret->positionBuffer_.reserve(vertices.size());
        ret->normalBuffer_.reserve(vertices.size());
        ret->uvBuffer_.reserve(vertices.size());
        for (unsigned i = 0; i < vertices.size(); ++i)
        {
            auto vertData = vertices[i];
            ret->positionBuffer_.push_back(vertData.position_);
            ret->normalBuffer_.push_back(vertData.normal_);
            ret->uvBuffer_.push_back(vertData.uv_);
        }
        return ret;
    }
    return 0x0;
}

void FBXWriteNormals(FbxGeometryElementNormal* layer, const Vec3* data, unsigned ct)
{
    layer->SetReferenceMode(FbxGeometryElement::eDirect);
    for (unsigned elemIdx = 0; elemIdx < ct; ++elemIdx)
        layer->GetDirectArray().Add(FbxVector4(data[elemIdx].x, data[elemIdx].y, data[elemIdx].z));
}

void FBXWriteColor(FbxGeometryElementVertexColor* layer, const RGBA* data, unsigned ct)
{
    layer->SetReferenceMode(FbxGeometryElement::eDirect);
    for (unsigned elemIdx = 0; elemIdx < ct; ++elemIdx)
        layer->GetDirectArray().Add(FbxColor(data[elemIdx].r, data[elemIdx].g, data[elemIdx].b, data[elemIdx].a));
}

void FBXWriteUV(FbxGeometryElementUV* layer, const Vec2* data, unsigned ct)
{
    layer->SetReferenceMode(FbxGeometryElement::eDirect);
    for (unsigned elemIdx = 0; elemIdx < ct; ++elemIdx)
        layer->GetDirectArray().Add(FbxVector2(data[elemIdx].x, data[elemIdx].y));
}

void AddMeshToScene(FbxScene* scene, const MeshData* meshData)
{
    auto he = meshData->BuildHalfEdgeMesh();
    FbxMesh* mesh = FbxMesh::Create(scene, meshData->GetName().c_str());
    FbxArray<FbxVector4> controlPoints;
    
    mesh->SetControlPointCount(meshData->positionBuffer_.size());
    for (unsigned i = 0; i < meshData->positionBuffer_.size(); ++i)
    {
        auto& buffer = meshData->positionBuffer_;
        mesh->SetControlPointAt(FbxVector4(buffer[i].x, buffer[i].y, buffer[i].z, 0.0f), i);
    }
    // write vertex positions
    //for (unsigned i = 0; i < meshData->indexBuffer_.size(); i += 3)
    //{
    //    unsigned indices[3] = {
    //        meshData->indexBuffer_[i],
    //        meshData->indexBuffer_[i + 1],
    //        meshData->indexBuffer_[i + 2]
    //    };
    //
    //    SprueEngine::Vec3 positions[3] = {
    //        meshData->positionBuffer_[indices[0]],
    //        meshData->positionBuffer_[indices[1]],
    //        meshData->positionBuffer_[indices[2]]
    //    };
    //
    //    controlPoints.Add(FbxVector4(positions[0].x, positions[0].y, positions[0].z, 0.0f));
    //    controlPoints.Add(FbxVector4(positions[0].y, positions[0].y, positions[0].y, 0.0f));
    //    controlPoints.Add(FbxVector4(positions[0].z, positions[0].z, positions[0].z, 0.0f));
    //}

    // Write indices
    for (unsigned i = 0; i < meshData->indexBuffer_.size(); i += 3)
    {
        unsigned indices[3] = {
            meshData->indexBuffer_[i],
            meshData->indexBuffer_[i + 1],
            meshData->indexBuffer_[i + 2]
        };

        mesh->BeginPolygon(-1, -1, false);
        for (unsigned v = 0; v < 3; ++v)
            mesh->AddPolygon(indices[v]);
        mesh->EndPolygon();
    }

    if (meshData->GetNormalBuffer().size())
    {
        FbxGeometryElementNormal* normals = mesh->CreateElementNormal();
        normals->SetMappingMode(FbxGeometryElement::eByControlPoint);
        FBXWriteNormals(normals, meshData->normalBuffer_.data(), meshData->normalBuffer_.size());
    }

    if (meshData->GetUVBuffer().size())
    {
        FbxGeometryElementUV* uv = mesh->CreateElementUV(0x0);
        uv->SetMappingMode(FbxGeometryElement::eByControlPoint);
        FBXWriteUV(uv, meshData->uvBuffer_.data(), meshData->uvBuffer_.size());
    }

    if (meshData->GetColorBuffer().size())
    {
        FbxGeometryElementVertexColor* colors = mesh->CreateElementVertexColor();
        colors->SetMappingMode(FbxGeometryElement::eByControlPoint);
        FBXWriteColor(colors, meshData->colorBuffer_.data(), meshData->colorBuffer_.size());
    }

    //TODO: tangents? binormals?

    //TODO: write skeleton

    //TODO: bone weights

    //TODO: morph targets

    FbxNode* lNode = FbxNode::Create(scene, meshData->GetName().c_str());
    lNode->SetNodeAttribute(mesh);
    lNode->SetShadingMode(FbxNode::eTextureShading);
}

void FBXLoader::SaveModel(const std::vector<MeshData*>& meshData, const char* fileName)
{
    FbxManager* lSdkManager = FbxManager::Create();

    FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
    lSdkManager->SetIOSettings(ios);

    FbxExporter* lExporter = FbxExporter::Create(lSdkManager, "");
    bool lExportStatus = lExporter->Initialize(fileName, -1, lSdkManager->GetIOSettings());
    if (!lExportStatus)
    {
        SPRUE_LOG_ERROR("Failed to prepare export of FBX file");
        lExporter->Destroy();
        ios->Destroy();
        lSdkManager->Destroy();
        return;
    }

    FbxScene* lScene = FbxScene::Create(lSdkManager, fileName);

    for (unsigned i = 0; i < meshData.size(); ++i)
    {
        if (meshData[i]->GetPositionBuffer().empty())
            continue;
        AddMeshToScene(lScene, meshData[i]);
    }

    if (!lExporter->Export(lScene))
    {
        SPRUE_LOG_ERROR("Faile dto export FBX file");
    }

    lScene->Destroy();
    lExporter->Destroy();
    ios->Destroy();
    lSdkManager->Destroy();
}

}