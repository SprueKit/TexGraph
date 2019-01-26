#include "OpenCLComputeKernel.h"

#include "../ComputeBuffer.h"

#include "ErrorCodes.h"
#include "OpenCLComputeDevice.h"
#include "OpenCLComputeShader.h"

#include "../../FString.h"
#include "../../Logging.h"

#include <CL/cl.hpp>

namespace SprueEngine
{

    OpenCLComputeKernel::OpenCLComputeKernel(const std::string& name, OpenCLComputeShader* shader, OpenCLComputeDevice* device) :
        ComputeKernel(name, device)
    {
        int errCode = 0;
        kernel_ = new cl::Kernel(*(shader->program_), name_.c_str(), &errCode);
        if (errCode != CL_SUCCESS)
        {
            SPRUE_LOG_ERROR(FString("%2: Failed to construct OpenCL kernel: %1", name_, TranslateOpenCLErrorCode(errCode)).str());
            delete kernel_;
            kernel_ = 0x0;
        }
    }

    OpenCLComputeKernel::~OpenCLComputeKernel()
    {
        if (kernel_)
            delete kernel_;
    }

    void OpenCLComputeKernel::Bind(ComputeBuffer* buffer, unsigned index)
    {
        buffer->Bind(this, index);
    }

    void OpenCLComputeKernel::Execute(unsigned x, unsigned y, unsigned z)
    {
        if (kernel_)
        {
            OpenCLComputeDevice* device = (OpenCLComputeDevice*)device_;
            cl_int errCode = device->GetQueue().enqueueNDRangeKernel(*kernel_, cl::NDRange(), cl::NDRange(x, y, z));
            if (errCode != CL_SUCCESS)
                SPRUE_LOG_ERROR(FString("%2: Failed to queue/execute OpenCL kernel: %1", name_, TranslateOpenCLErrorCode(errCode)).str());
        }
        else
        {
            SPRUE_LOG_ERROR(FString("Attempted to execute uncompiled OpenCL shader: %1", name_).str());
        }
    }

    void OpenCLComputeKernel::SetArg(unsigned index, void* value, unsigned sz)
    {
        if (kernel_)
            kernel_->setArg(index, sz, value);
    }
}