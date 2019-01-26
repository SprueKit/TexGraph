#pragma once

#include <SprueEngine/IContextService.h>

#include <SprueEngine/Libs/MakeID.h>

namespace SprueEngine
{

class Context;

class SPRUE IDService : public IContextService
{
public:
    IDService(Context* context);
    ~IDService();

    unsigned GetID() const;
    void ReleaseID(unsigned id);

private:
    mutable MakeID ids_;
};

}