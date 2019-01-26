#include "OpenCLComputeBuffer.h"

#include "ErrorCodes.h"
#include "OpenCLComputeDevice.h"
#include "OpenCLComputeKernel.h"

#include "../../FString.h"
#include "../../Logging.h"
#include "../../Math/MathDef.h"

namespace SprueEngine
{

OpenCLComputeBuffer::OpenCLComputeBuffer(const std::string& name, OpenCLComputeDevice* device, unsigned size, unsigned settings) :
    base(name, device, size, settings)
{
    unsigned memFlags = (settings & (CBS_Read | CBS_Write) ? CL_MEM_READ_WRITE : (settings & CBS_Write ? CL_MEM_WRITE_ONLY : CL_MEM_READ_ONLY));
    buffer_ = new cl::Buffer(device->GetContext(), memFlags, size);
    if (!buffer_ || !(*buffer_)())
    {
        SPRUE_LOG_ERROR("Failed to construct OpenCL buffer");
        if (buffer_)
            delete buffer_;
        buffer_ = 0x0;
    }
}

OpenCLComputeBuffer::OpenCLComputeBuffer(const std::string& name, OpenCLComputeDevice* device, unsigned width, unsigned height, unsigned settings) :
    base(name, device, width, height, settings)
{
    unsigned memFlags = (settings & (CBS_Read | CBS_Write) ? CL_MEM_READ_WRITE : (settings & CBS_Write ? CL_MEM_WRITE_ONLY : CL_MEM_READ_ONLY));
    cl::ImageFormat format = DetermineImageFormat(settings);
    image2D_ = new cl::Image2D(device->GetContext(), memFlags, format, width, height);
    if (!image2D_ || !(*image2D_)())
    {
        SPRUE_LOG_ERROR("Failed to construct 2D image OpenCL buffer");
        if (image2D_)
            delete image2D_;
        image2D_ = 0x0;
    }
}

OpenCLComputeBuffer::OpenCLComputeBuffer(const std::string& name, OpenCLComputeDevice* device, unsigned width, unsigned height, unsigned depth, unsigned settings) :
    base(name, device, width, height, depth, settings)
{
    unsigned memFlags = (settings & (CBS_Read | CBS_Write) ? CL_MEM_READ_WRITE : (settings & CBS_Write ? CL_MEM_WRITE_ONLY : CL_MEM_READ_ONLY));
    cl::ImageFormat format = DetermineImageFormat(settings);
    image3D_ = new cl::Image3D(device->GetContext(), memFlags, format, width, height, depth);
    if (!image3D_ || !(*image3D_)())
    {
        SPRUE_LOG_ERROR(FString("Failed to construct 3D image OpenCL buffer: %1", name_).str());
        if (image3D_)
            delete image3D_;
        image3D_ = 0x0;
    }
}

OpenCLComputeBuffer::~OpenCLComputeBuffer()
{
    if (buffer_)
    {
        delete buffer_;
    }
    if (image2D_)
        delete image2D_;
    if (image3D_)
        delete image3D_;
    buffer_ = 0x0;
    image2D_ = 0x0;
    image3D_ = 0x0;
}

void OpenCLComputeBuffer::SetData(void* data, unsigned offset, unsigned len)
{
    if (!buffer_ && !image2D_ && !image3D_)
    {
        SPRUE_LOG_ERROR(FString("Attempted to write data to an uninitialized OpenCL buffer: %1", name_).str());
        return;
    }

    cl_int errCode = CL_SUCCESS;
    OpenCLComputeDevice* device = ((OpenCLComputeDevice*)device_);
    if (buffer_)
        errCode = device->GetQueue().enqueueWriteBuffer(*buffer_, true, offset, len, data);
    else if (image2D_)
    {        
        cl::size_t<3> reg;
        reg[0] = width_;
        reg[1] = height_;
        reg[2] = 0;
        errCode = device->GetQueue().enqueueWriteImage(*image2D_, true, cl::size_t<3>(), reg, elementSize_ * width_, 0, data);
    }
    else if (image3D_)
    {
        cl::size_t<3> reg;
        reg[0] = width_;
        reg[1] = height_;
        reg[2] = depth_;
        errCode = device->GetQueue().enqueueWriteImage(*image3D_, true, cl::size_t<3>(), reg, elementSize_ * width_, (elementSize_ * width_) * height_, data);
    }

    if (errCode != CL_SUCCESS)
        SPRUE_LOG_ERROR(FString("%2: Failed to write buffer: %1", name_, TranslateOpenCLErrorCode(errCode)).str());
}

void OpenCLComputeBuffer::ReadData(void* data, unsigned offset, unsigned len)
{
    if (!buffer_ && !image2D_ && !image3D_)
    {
        SPRUE_LOG_ERROR(FString("Attempted to read data from an uninitialized OpenCL buffer: %1", name_).str());
        return;
    }

    cl_int errCode = CL_SUCCESS;
    OpenCLComputeDevice* device = ((OpenCLComputeDevice*)device_);
    if (buffer_)
        errCode = device->GetQueue().enqueueReadBuffer(*buffer_, true, offset, len, data);
    else if (image2D_)
    {
        cl::size_t<3> reg;
        reg[0] = width_;
        reg[1] = height_;
        reg[2] = 0;
        errCode = device->GetQueue().enqueueReadImage(*image2D_, true, cl::size_t<3>(), reg, elementSize_ * width_, 0, data);
    }
    else if (image3D_)
    {
        cl::size_t<3> reg;
        reg[0] = width_;
        reg[1] = height_;
        reg[2] = depth_;
        errCode = device->GetQueue().enqueueReadImage(*image3D_, true, cl::size_t<3>(), reg, elementSize_ * width_, (elementSize_ * width_) * height_, data);
    }

    if (errCode != CL_SUCCESS)
        SPRUE_LOG_ERROR(FString("Failed to read buffer: %1", name_).str());
}

void OpenCLComputeBuffer::Bind(ComputeKernel* shader, unsigned index)
{
    if (!shader)
    {
        SPRUE_LOG_ERROR(FString("Attempted to bind OpenCL buffer %1 to a null kernel", name_));
        return;
    }
    if (!buffer_ && !image2D_ && !image3D_)
    {
        SPRUE_LOG_ERROR(FString("Attempted to bind an uninitialized OpenCL buffer: %1", name_).str());
        return;
    }

    OpenCLComputeKernel* clKernel = (OpenCLComputeKernel*)shader;
    
    cl_int errCode = CL_SUCCESS;
    if (buffer_)
        errCode = clKernel->GetKernel()->setArg(index, *buffer_);
    else if (image2D_)
        errCode = clKernel->GetKernel()->setArg(index, *image2D_);
    else if (image3D_)
        errCode = clKernel->GetKernel()->setArg(index, *image3D_);

    if (errCode != CL_SUCCESS)
        SPRUE_LOG_ERROR(FString("%3: Failed to bind buffer to kernel: %1, shader: %2", name_, clKernel->GetName(), TranslateOpenCLErrorCode(errCode)).str());
}

cl::ImageFormat OpenCLComputeBuffer::DetermineImageFormat(unsigned settings)
{
    cl::ImageFormat format;
    if (settings & CBS_FloatData)
    {
        if (settings & CBS_RGB)
        {
            format = cl::ImageFormat(CL_RGB, CL_FLOAT);
            elementSize_ = sizeof(float) * 3;
        }
        else if (settings & CBS_RGBA)
        {
            format = cl::ImageFormat(CL_RGBA, CL_FLOAT);
            elementSize_ = sizeof(float)*4;
        }
        else if (settings & CBS_Grayscale)
        {
            format = cl::ImageFormat(CL_R, CL_FLOAT);
            elementSize_ = sizeof(float);
        }
    }
    else
    {
        if (settings & CBS_RGB)
        {
            format = cl::ImageFormat(CL_RGB, CL_UNORM_INT8);
            elementSize_ = sizeof(unsigned char) * 3;
        }
        else if (settings & CBS_RGBA)
        {
            format = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);
            elementSize_ = sizeof(unsigned char) * 4;
        }
        else if (settings & CBS_Grayscale)
        {
            format = cl::ImageFormat(CL_A, CL_UNORM_INT8);
            elementSize_ = sizeof(unsigned char);
        }
    }
    return format;
}

}