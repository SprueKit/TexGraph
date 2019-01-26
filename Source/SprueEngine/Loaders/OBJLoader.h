#pragma once

#include <SprueEngine/ResourceLoader.h>

#include <vector>

namespace SprueEngine
{

class MeshData;

class SPRUE OBJLoader : public ResourceLoader
{
public:
    virtual std::string GetResourceURIRoot() const override;
    virtual StringHash GetResourceTypeID() const override;
    virtual std::shared_ptr<Resource> LoadResource(const char*) const override;
    virtual bool CanLoad(const char*) const override;

    static void SaveModel(const std::vector<MeshData*>& meshData, const char* fileName);

private:
    static const std::string resourceURI_;
    static const StringHash typeHash_;
};

}