#pragma once

#include <SprueEngine/StringHash.h>

namespace SprueEditor
{
    class BaseTypeEditor;

    /// Factory base type for constructing custom fields in the property list
    class CustomPropertyFactory
    {
    public:
        virtual BaseTypeEditor* Construct(const SprueEngine::StringHash& forProperty, std::string& propertyName) = 0;
    };

}