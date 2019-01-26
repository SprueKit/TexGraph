#pragma once

#include <SprueEngine/ClassDef.h>

#include <string>
#include <vector>

namespace SprueEngine
{

    class ComputeBuffer;
    class ComputeShader;

    class SPRUE ComputeDevice
    {
        NOCOPYDEF(ComputeDevice);
    public:
        ComputeDevice() { }
        virtual ~ComputeDevice() { }

        /// Device must be initialized after construction, will return false if initialization failed.
        virtual bool Initialize() = 0;
        virtual bool IsValid() const = 0;
        virtual void Finish() = 0;
        virtual void Barrier() = 0;

        /// Construct an arbitrary data buffer
        virtual ComputeBuffer* CreateBuffer(const std::string& name, unsigned size, unsigned bufferType) = 0;
        /// Construct an Image2D
        virtual ComputeBuffer* CreateBuffer(const std::string& name, unsigned width, unsigned height, unsigned bufferType) = 0;
        /// Construct an Image3D
        virtual ComputeBuffer* CreateBuffer(const std::string& name, unsigned width, unsigned height, unsigned depth, unsigned bufferType) = 0;
        /// Construct a shader (not prepared or compiled)
        virtual ComputeShader* CreateShader(const std::string& name) = 0;
    };

}