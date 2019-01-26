#pragma once

#include <SprueEngine/ClassDef.h>

#include <string>

namespace SprueEngine
{
    class ComputeDevice;
    class ComputeBuffer;

    class SPRUE ComputeKernel
    {
        NOCOPYDEF(ComputeKernel);
    public:
        ComputeKernel(const std::string& name, ComputeDevice* device) :
            name_(name), device_(device)
        {

        }

        virtual ~ComputeKernel() { }

        virtual void Bind(ComputeBuffer* buffer, unsigned index) = 0;
        virtual void Execute(unsigned x, unsigned y, unsigned z) = 0;
        virtual void SetArg(unsigned index, void* value, unsigned sz) = 0;
        virtual bool IsExecutable() const = 0;

        template<typename T>
        void SetArg(unsigned index, const T& value) { SetArg(index, &value, sizeof(T)); }

        const std::string& GetName() const { return name_; }

    protected:
        ComputeDevice* device_;
        std::string name_;
    };

}