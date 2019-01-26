#include "IDService.h"

#include "Core/Context.h"

namespace SprueEngine
{

IDService::IDService(Context* context) :
    ids_(UINT_MAX)
{
    context->RegisterService(this);
}

IDService::~IDService()
{

}

unsigned IDService::GetID() const
{
    unsigned ret;
    ids_.CreateID(ret);
    return ret;
}

void IDService::ReleaseID(unsigned id)
{
    ids_.DestroyID(id);
}

}