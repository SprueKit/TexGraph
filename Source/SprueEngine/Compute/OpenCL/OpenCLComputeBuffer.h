#pragma once

#include <SprueEngine/Compute/ComputeBuffer.h>
#include <CL/cl.hpp>

namespace SprueEngine
{

    class OpenCLComputeDevice;

    class OpenCLComputeBuffer : public ComputeBuffer
    {
        NOCOPYDEF(OpenCLComputeBuffer);
        BASECLASSDEF(OpenCLComputeBuffer, ComputeBuffer);
    public:
        /// Construct a buffer object for the given compute device.
        OpenCLComputeBuffer(const std::string& name, OpenCLComputeDevice*, unsigned size, unsigned settings);
        /// Construct a 2d image object for the given compute device.
        OpenCLComputeBuffer(const std::string& name, OpenCLComputeDevice*, unsigned width, unsigned height, unsigned settings);
        /// Construct a 3d image object for the given compute device.
        OpenCLComputeBuffer(const std::string& name, OpenCLComputeDevice*, unsigned width, unsigned height, unsigned depth, unsigned settings);
        /// Destruct and release.
        virtual ~OpenCLComputeBuffer();

        /// Sets the data of the backing buffer.
        virtual void SetData(void* data, unsigned offset, unsigned len) override;
        /// Reads data from the buffer.
        virtual void ReadData(void* data, unsigned offset, unsigned len) override;

        /// Binds the buffer to a given shader with a target binding index.
        virtual void Bind(ComputeKernel* shader, unsigned index) override;

    private:
        /// Internal function for translating generic buffer settings into a cl::ImageFormat.
        cl::ImageFormat DetermineImageFormat(unsigned settings);

        /// Associated OpenCL buffer (if initialized as a buffer, null otherwise).
        cl::Buffer* buffer_ = 0x0;
        /// Associated OpenCL 2d image (if initialized as a 2d image, null otherwise).
        cl::Image2D* image2D_ = 0x0;
        /// Associated OpenCL 3d image (if initialized as a 3d image, null otherwise).
        cl::Image3D* image3D_ = 0x0;
    };

}