#pragma once

#if defined(CPU_COMPUTE)        // Purely on the CPU, abstract now has cost
    // NOT IMPLEMENTED
    #include <SprueEngine/Compute/CPU/CPUComputeBuffer.h>
    #include <SprueEngine/Compute/CPU/CPUComputeDevice.h>
    #include <SprueEngine/Compute/CPU/CPUComputeShader.h>
#elif defined(DX11_COMPUTE)     // Use DX11 DirectCompute
    // NOT IMPLEMENTED
    #include <SprueEngine/Compute/D3D11/D3D11ComputeBuffer.h>
    #include <SprueEngine/Compute/D3D11/D3D11ComputeDevice.h>
    #include <SprueEngine/Compute/D3D11/D3D11ComputeShader.h>
#elif defined(OGL_COMPUTE)      // Use OpenGL's compute shader (and SSBO)
    // NOT IMPLEMENTED
    #include <SprueEngine/Compute/OGL/OGLComputeBuffer.h>
    #include <SprueEngine/Compute/OGL/OGLComputeDevice.h>
    #include <SprueEngine/Compute/OGL/OGLComputeShader.h>
#else //defined(OCL_COMPUTE)    // Use OpenCL's GPGPU compute kernels, prefered
    #include <SprueEngine/Compute/OpenCL/OpenCLComputeBuffer.h>
    #include <SprueEngine/Compute/OpenCL/OpenCLComputeDevice.h>
    #include <SprueEngine/Compute/OpenCL/OpenCLComputeShader.h>
#endif

namespace SprueEngine
{

    ComputeDevice* CreateComputeDevice();

}