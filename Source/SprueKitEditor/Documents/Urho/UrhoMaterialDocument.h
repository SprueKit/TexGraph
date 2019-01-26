#pragma once

#include "BaseUrhoDocument.h"

#include <Urho3D/Graphics/Material.h>

namespace UrhoEditor
{

    class UrhoMaterialDocument : public BaseUrhoDocument
    {
    public:
    protected:
        Urho3D::SharedPtr<Urho3D::Material> material_;
    };

}