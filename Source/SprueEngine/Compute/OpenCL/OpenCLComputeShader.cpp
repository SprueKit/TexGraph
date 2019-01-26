#include "OpenCLComputeShader.h"

#include "ErrorCodes.h"
#include "OpenCLComputeBuffer.h"
#include "OpenCLComputeDevice.h"
#include "OpenCLComputeKernel.h"

#include "../../FString.h"
#include "../../Logging.h"

namespace SprueEngine
{

OpenCLComputeShader::OpenCLComputeShader(const std::string& name, OpenCLComputeDevice* device) :
    base(name, device)
{
    isCompiled_ = false;
}

OpenCLComputeShader::~OpenCLComputeShader()
{
    if (program_)
        delete program_;
}

bool OpenCLComputeShader::CompileShader(const std::vector<std::string>& sources, const std::string& defines)
{
    isCompiled_ = false;

    // Destroy anything we've already got
    if (program_)
        delete program_;
    program_ = 0x0;

    // Attempt to create the shader and kernel
    cl_int errCode = CL_SUCCESS;

    OpenCLComputeDevice* device = (OpenCLComputeDevice*)device_;
    cl::Program::Sources srcs;
    srcs.resize(sources.size());
    for (unsigned i = 0; i < sources.size(); ++i)
        srcs[i] = std::make_pair(sources[i].c_str(), sources[i].length());

    program_ = new cl::Program(device->GetContext(), srcs);
    if (!defines.empty())
        errCode = program_->build(device->GetDeviceVector(), FString("-cl-std=CL2.0 %1", defines).c_str());
    else
        errCode = program_->build(device->GetDeviceVector(), "-cl-std=CL2.0");
    if (errCode != CL_SUCCESS)
    {
        SPRUE_LOG_ERROR(FString("%2: Failed to generate OpenCL shader program: %1", name_, TranslateOpenCLErrorCode(errCode)).str());
        if (errCode == CL_BUILD_PROGRAM_FAILURE)
        {
            cl_build_status status = program_->getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device->GetDevice());
            if (status == CL_BUILD_ERROR)
            {
                std::string name = cl::Device::getDefault().getInfo<CL_DEVICE_NAME>();
                std::string buildlog = program_->getBuildInfo<CL_PROGRAM_BUILD_LOG>(device->GetDevice());
                SPRUE_LOG_ERROR(FString("Build log: %1\n%2", name, buildlog).str());
            }
        }
        delete program_;
        program_ = 0x0;
        return false;
    }

    isCompiled_ = true;
    return true;
}

bool OpenCLComputeShader::CompileShader(const std::string& source, const std::string& defines)
{
    std::vector<std::string> srcs;
    srcs.push_back(source);
    return CompileShader(srcs, defines);
}

ComputeKernel* OpenCLComputeShader::GetKernel(const std::string& name)
{
    if (!IsCompiled())
    {
        SPRUE_LOG_ERROR(FString("Attempting to get kernel for uncompiled compute shader: %1", name));
        return 0x0;
    }

    ComputeKernel* kernel = new OpenCLComputeKernel(name, this, (OpenCLComputeDevice*)device_);
    if (kernel->IsExecutable())
        return kernel;
    delete kernel;
    SPRUE_LOG_ERROR(FString("Kernel for compute shader: %1 is not executable", name));
    return 0x0;
}

}