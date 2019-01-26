#pragma once

#include <SprueEngine/ClassDef.h>

#include <string>
#include <vector>

namespace SprueEngine
{
    class ComputeDevice;
    class ComputeKernel;

    enum ComputeBufferSettings
    {
        CBS_Read = 1,
        CBS_Write = 1 << 1,
        CBS_Image2D = 1 << 2,
        CBS_Image3D = 1 << 3,
        CBS_RGB = 1 << 4,
        CBS_Grayscale = 1 << 5,
        CBS_RGBA = 1 << 6,
        CBS_FloatData = 1 << 7
    };

    class SPRUE ComputeBuffer
    {
        NOCOPYDEF(ComputeBuffer);
    public:
        /// Construct for data buffer
        ComputeBuffer(const std::string& name, ComputeDevice* device, unsigned size, unsigned settings) : 
            device_(device), name_(name), width_(size), height_(0), depth_(0), elementSize_(0)
        {
        }
        
        /// Construct for 2D image buffer
        ComputeBuffer(const std::string& name, ComputeDevice* device, unsigned width, unsigned height, unsigned settings) : 
            device_(device), name_(name), width_(width), height_(height), depth_(0), elementSize_(0)
        { 
        }
        
        /// Construct for 3D image buffer
        ComputeBuffer(const std::string& name, ComputeDevice* device, unsigned width, unsigned height, unsigned depth, unsigned settings) : 
            device_(device), name_(name), width_(width), height_(height), depth_(depth), elementSize_(0)
        { 
        }

        virtual ~ComputeBuffer() { }

        void SetData(void* data, unsigned len) { SetData(data, 0, len); }
        virtual void SetData(void* data, unsigned offset, unsigned len) = 0;

        void ReadData(void* data, unsigned len) { ReadData(data, 0, len); }
        virtual void ReadData(void* data, unsigned offset, unsigned len) = 0;

        virtual void Bind(ComputeKernel* kernel, unsigned index) = 0;

        const std::string& GetName() const { return name_; }

        /// Read a typed buffer.
        template<typename T>
        void SetData(T* data, unsigned ct) { SetData((void*)data, 0, sizeof(T) * ct); }

        /// Read a typed buffer.
        template<typename T>
        void ReadData(T* data, unsigned ct) { ReadData((void*)data, 0, sizeof(T) * ct); }

        /// Utility function for reading just the first value in a buffer.
        template<typename T>
        void ReadFirstvalue(T* data) { ReadData((void*)data, 0, sizeof(T)); }

        /// Utility function for reading the last value in a buffer, mostly just for reading sums.
        template<typename T>
        void ReadLastValue(T* data) { ReadData((void*)data, sizeof(T) * ct - sizeof(T), sizeof(T)); }

        template<typename T>
        void Read(std::vector<T>& data) {
            const size_t size = width_ / sizeof(T);
            data.resize(size);
            ReadData((void*)data.data(), size);
        }

    protected:
        std::string name_;
        ComputeDevice* device_;
        unsigned width_ = 0;
        unsigned height_ = 0;
        unsigned depth_ = 0;
        unsigned elementSize_ = 0;
    };

}