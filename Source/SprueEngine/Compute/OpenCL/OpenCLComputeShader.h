#pragma once

#include <SprueEngine/Compute/ComputeShader.h>

namespace cl
{
    class Kernel;
    class Program;
}

namespace SprueEngine
{
    class OpenCLComputeDevice;
    class ComputeKernel;

    /// OpenCL compute program. Potentially contains many kernels.
    class SPRUE OpenCLComputeShader : public ComputeShader
    {
        friend class OpenCLComputeKernel;

        NOCOPYDEF(OpenCLComputeShader);
        BASECLASSDEF(OpenCLComputeShader, ComputeShader);
    public:
        /// Construct for the given compute device with an identifying name.
        OpenCLComputeShader(const std::string& name, OpenCLComputeDevice*);
        /// Destruct and free.
        virtual ~OpenCLComputeShader();

        /// Compiles the shader from the given sources (concatenated) and preprocessor definitions.
        virtual bool CompileShader(const std::vector<std::string>& sources, const std::string& defines = std::string()) override;
        /// Compiles the shader from the given source code and preprocessor definitions.
        virtual bool CompileShader(const std::string& source, const std::string& defines = std::string()) override;
        /// Gets a kernel (if it exists) by name from the compiled shader.
        virtual ComputeKernel* GetKernel(const std::string& name) override;

        /// Returns the underlying OpenCL program object.
        cl::Program* GetProgram() { return program_; }

    private:
        /// OpenCL compute program that is wrapped.
        cl::Program* program_ = 0x0;
    };

}