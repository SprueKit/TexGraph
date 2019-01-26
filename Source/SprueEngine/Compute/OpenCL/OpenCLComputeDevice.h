#pragma once

#include <SprueEngine/Compute/ComputeDevice.h>

#include <CL/cl.hpp>

namespace SprueEngine
{

/// Implements an OpenCL device (CPU or GPU) for heterogenous compute processing.
class OpenCLComputeDevice : public ComputeDevice
{
public:
    /// Construct.
    OpenCLComputeDevice();
    /// Destruct.
    virtual ~OpenCLComputeDevice();

    /// Initializes the OpenCL device.
    virtual bool Initialize() override;
    /// Returns true if the device is in a valid state fo ruse.
    virtual bool IsValid() const { return context_ != 0x0 && commandQueue_ != 0x0; }
    /// Forces all pending commands to finish before resuming execution.
    virtual void Finish() override;
    virtual void Barrier() override;

    /// Construct a buffer for this device with the given identifier.
    virtual ComputeBuffer* CreateBuffer(const std::string& name, unsigned size, unsigned bufferType) override;
    /// Construct a 2d image for this device with the given identifier.
    virtual ComputeBuffer* CreateBuffer(const std::string& name, unsigned width, unsigned height, unsigned bufferType) override;
    /// Construct a 3d image for this device with the given identifier.
    virtual ComputeBuffer* CreateBuffer(const std::string& name, unsigned width, unsigned height, unsigned depth, unsigned bufferType) override;
    /// Construct a shader for this device with the given identifier.
    virtual ComputeShader* CreateShader(const std::string& name) override;

    /// Returns the command queue for OpenCL execution.
    cl::CommandQueue& GetQueue() { return *commandQueue_; }
    /// Returns the context for OpenCL execution.
    cl::Context& GetContext() { return *context_; }
    /// Returns all available devices.
    std::vector<cl::Device>& GetDevices() { return devices_; }
    /// Returns the device for this compute device instance.
    cl::Device& GetDevice() { return device_; }
    /// Returns all available devices.
    std::vector<cl::Device> GetDeviceVector() { std::vector<cl::Device> dev; dev.push_back(device_); return dev; }

private:
    std::vector<cl::Device> devices_; // This is bullshit to make the API happy
    cl::Device device_;
    cl::Context* context_ = 0x0;
    cl::CommandQueue* commandQueue_ = 0x0;
};

}