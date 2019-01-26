#pragma once

#include <SprueEngine/ClassDef.h>

#include <SprueEngine/Geometry/MeshData.h>
#include <vector>
#include <memory>

namespace SprueEngine
{

class SPRUE ModelImporter
{
    NOCOPYDEF(ModelImporter);
public:
    virtual std::vector<std::shared_ptr<MeshData> > Import(const char* file) const = 0;
};

}