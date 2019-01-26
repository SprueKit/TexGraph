#pragma once

#include <SprueEngine/ClassDef.h>

#include <string>
#include <vector>

namespace SprueEngine
{
    class ComputeBuffer;
    class ComputeDevice;
    class ComputeKernel;

    class SPRUE ComputeShader
    {
        NOCOPYDEF(ComputeShader);
    public:
        ComputeShader(const std::string& name, ComputeDevice* device) : 
            device_(device), name_(name) 
        { 
        }

        virtual ~ComputeShader() { }

        virtual bool CompileShader(const std::vector<std::string>& sources, const std::string& defines = std::string()) = 0;
        virtual bool CompileShader(const std::string& source, const std::string& defines = std::string()) = 0;
        virtual ComputeKernel* GetKernel(const std::string& name) = 0;
        virtual bool IsCompiled() const { return isCompiled_; }

        const std::string& GetName() const { return name_; }

    protected:
        bool isCompiled_;
        std::string name_;
        ComputeDevice* device_;
    };

}