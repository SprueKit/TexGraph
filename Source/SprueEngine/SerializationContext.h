#pragma once

#include <string>
#include <vector>

namespace SprueEngine
{
    class IEditable;
    class TypeProperty;

    struct SerializationContext
    {
        struct PathError
        {
            IEditable* editable_;
            TypeProperty* property_;
            std::string path_;
        };
        std::string relativePath_;
        bool isClone_ = false;
        mutable std::vector< PathError > pathErrors_;
    };

}