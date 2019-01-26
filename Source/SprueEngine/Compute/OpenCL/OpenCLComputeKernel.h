#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/Compute/ComputeKernel.h>

namespace cl
{
    class Kernel;
}

namespace SprueEngine
{
    class OpenCLComputeDevice;
    class OpenCLComputeShader;

    /// Kernel object from an OpenCL program.
    class SPRUE OpenCLComputeKernel : public ComputeKernel
    {
        NOCOPYDEF(OpenCLComputeKernel);
    public:
        OpenCLComputeKernel(const std::string& name, OpenCLComputeShader* shader, OpenCLComputeDevice* device);
        virtual ~OpenCLComputeKernel();

        cl::Kernel* GetKernel() { return kernel_; }

        virtual void Bind(ComputeBuffer* buffer, unsigned index) override;
        virtual void Execute(unsigned x, unsigned y, unsigned z) override;
        virtual void SetArg(unsigned index, void* value, unsigned sz) override;
        virtual bool IsExecutable() const override { return kernel_ != 0x0; }

    protected:
        cl::Kernel* kernel_ = 0x0;
    };

}