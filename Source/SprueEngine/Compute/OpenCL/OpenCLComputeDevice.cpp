#include "OpenCLComputeDevice.h"

#include "ErrorCodes.h"
#include "OpenCLComputeBuffer.h"
#include "OpenCLComputeShader.h"

#include <SprueEngine/FString.h>
#include <SprueEngine/Logging.h>

#include <CL/cl.hpp>
#include <CL/cl.h>

namespace SprueEngine
{

OpenCLComputeDevice::OpenCLComputeDevice()
{

}
    
OpenCLComputeDevice::~OpenCLComputeDevice()
{
    if (commandQueue_)
        delete commandQueue_;
    if (context_)
        delete context_;
    commandQueue_ = 0x0;
    context_ = 0x0;
}

bool OpenCLComputeDevice::Initialize()
{
    SPRUE_LOG_DEBUG("Initializing OpenCL");
    cl_int errCode = CL_SUCCESS;

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform plat;
    std::string chosenPlat;
    for (auto &p : platforms) {
        std::string platver = p.getInfo<CL_PLATFORM_VERSION>();
        if (platver.find("OpenCL 2.") != std::string::npos) {
            plat = p;
            chosenPlat = platver;
            SPRUE_LOG_INFO(FString("Selecting %1 for OpenCL platform", platver).c_str());
        }
    }

    if (plat() == 0 && platforms.size() > 0)
    {
        plat = platforms.front();
        std::string platver = plat.getInfo<CL_PLATFORM_VERSION>();
        SPRUE_LOG_WARNING(FString("Falling back to %1 for OpenCL platform", platver).str());
    }
        
    if (plat() == 0)  {
        SPRUE_LOG_ERROR("No OpenCL platform found.");
        return false;
    }

    std::vector<cl::Device> devices;
    plat.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (devices.empty())
    {
        plat.getDevices(CL_DEVICE_TYPE_CPU, &devices);
        SPRUE_LOG_WARNING(FString("Falling back to CPU mode: %1", chosenPlat).c_str());
    }
    if (devices.empty())
    {
        SPRUE_LOG_ERROR(FString("Could not acquire an OpenCL device for: %1", chosenPlat).c_str());
        return false;
    }
    //cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)plat(), 0 };

    context_ = new cl::Context(devices.front());

    devices_ = context_->getInfo<CL_CONTEXT_DEVICES>();
    if (devices_.size() == 0)
    {
        SPRUE_LOG_ERROR("Unable to acquire any OpenCL devices");
        return false;
    }
        
    device_ = devices_.front();
    commandQueue_ = new cl::CommandQueue(*context_, devices_[0], 0, &errCode);
    if (errCode != CL_SUCCESS)
    {
        SPRUE_LOG_ERROR(FString("%1: Unable to construct OpenCL command queue", TranslateOpenCLErrorCode(errCode)).str());
        return false;
    }

    std::string platformName = plat.getInfo<CL_PLATFORM_NAME>();
    std::string platformVer = plat.getInfo<CL_PLATFORM_VERSION>();
    std::string deviceName = device_.getInfo<CL_DEVICE_NAME>();
    //SPRUE_LOG_INFO(FString("%3 initialized: %1, %2", platformName, deviceName, platformVer).str());

    return true;
}

void OpenCLComputeDevice::Finish()
{
    if (commandQueue_->finish() != CL_SUCCESS)
        SPRUE_LOG_ERROR("Failed to 'finish' OpenCL command queue");
}

void OpenCLComputeDevice::Barrier()
{
    if (::clEnqueueBarrier((*commandQueue_)()) != CL_SUCCESS)
        SPRUE_LOG_ERROR("Failed to encqueue OpenCL barrier");
}

ComputeBuffer* OpenCLComputeDevice::CreateBuffer(const std::string& name, unsigned size, unsigned bufferType)
{
    return new OpenCLComputeBuffer(name, this, size, bufferType);
}

ComputeBuffer* OpenCLComputeDevice::CreateBuffer(const std::string& name, unsigned width, unsigned height, unsigned bufferType)
{
    return new OpenCLComputeBuffer(name, this, width, height, bufferType);
}

ComputeBuffer* OpenCLComputeDevice::CreateBuffer(const std::string& name, unsigned width, unsigned height, unsigned depth, unsigned bufferType)
{
    return new OpenCLComputeBuffer(name, this, width, height, depth, bufferType);
}

ComputeShader* OpenCLComputeDevice::CreateShader(const std::string& name)
{
    return new OpenCLComputeShader(name, this);
}

}