#include "GPGPU.h"

namespace SprueEngine
{

ComputeDevice* CreateComputeDevice()
{
    #if defined(CPU_COMPUTE)
        return 0x0;
    #elif defined(DX11_COMPUTE)
        return 0x0;
    #elif defined(OGL_COMPUTE)
        return 0x0;
    #else
        OpenCLComputeDevice* ret = new OpenCLComputeDevice();
        ret->Initialize();
        return ret;
    #endif
}

}