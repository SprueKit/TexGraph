#pragma once

#include <SprueEngine/ClassDef.h>

namespace SprueEngine
{

class SPRUE IContextService
{
protected:
    /// Prevent unintended construction
    IContextService() { }
public:
    /// Make polymorphic
    virtual ~IContextService() { }
};

}