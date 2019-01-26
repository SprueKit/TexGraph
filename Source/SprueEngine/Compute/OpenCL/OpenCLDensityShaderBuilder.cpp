#include "OpenCLDensityShaderBuilder.h"

#include <SprueEngine/Core/SpinalPiece.h>
#include <SprueEngine/FString.h>

namespace SprueEngine
{

static const char* OpenCLDensityMainCode = "float DensityFunc(float3 pos, const global float* shapeData, const global float4* transformData) {\r\nint paramIndex = 0; int transformIndex = 0;\r\n%1\r\nreturn density;\r\n}\r\n";

OpenCLDensityShaderBuilder::OpenCLDensityShaderBuilder(bool writeCode, bool writeData) :
    writeCode_(writeCode),
    writeData_(writeData)
{

}

std::string OpenCLDensityShaderBuilder::BuildShader(SprueModel* model)
{
    buffer_.Clear();
    transforms_.Clear();
    code_.clear();
    model->VisitParentFirst(this);
    Finish();
    return code_;
}

bool OpenCLDensityShaderBuilder::ShouldNotVisitChildren(SceneObject* object)
{
    return object->IsDisabled();
}

bool OpenCLDensityShaderBuilder::Visit(SpruePiece* child)
{
    if (auto simplePiece = dynamic_cast<SimplePiece*>(child))
    {
        if (writeCode_)
        {
            if (code_.length() > 0)
            {
                switch (child->GetMode())
                {
                case SM_Additive:
                    code_ += FString("density = CSGAdd(density, %1);\r\n", simplePiece->GetDensityHandler()->ToString()).str();
                    break;
                case SM_Subtractive:
                    code_ += FString("density = CSGSubtract(density, %1);\r\n", simplePiece->GetDensityHandler()->ToString()).str();
                    break;
                case SM_Intersection:
                    code_ += FString("density = CSGIntersect(density, %1);\r\n", simplePiece->GetDensityHandler()->ToString()).str();
                    break;
                }
            }
            else
                code_ = FString("float density = %1;\r\n", simplePiece->GetDensityHandler()->ToString()).str();
        }

        if (writeData_)
        {
            simplePiece->GetDensityHandler()->WriteParameters(&buffer_);
            transforms_.Write(&child->GetWorldTransform().Inverted(), sizeof(Mat3x4));
        }
    }
    else if (auto spinalPiece = dynamic_cast<SpinalPiece*>(child))
    {
        if (writeCode_)
        {
            if (code_.length() > 0)
            {
                switch (spinalPiece->GetMode())
                {
                case SM_Additive:
                    code_ += "density = CSGAdd(density, Segment(pos, shapeData, transformData, &paramIndex, &transformIndex));\r\n";
                    break;
                case SM_Subtractive:
                    code_ += "density = CSGSubtract(density, Segment(pos, shapeData, transformData, &paramIndex, &transformIndex));\r\n";
                    break;
                case SM_Intersection:
                    code_ += "density = CSGIntersect(density, Segment(pos, shapeData, transformData, &paramIndex, &transformIndex));\r\n";
                    break;
                }
            }
            else
                code_ = "float density = Segment(pos, shapeData, transformData, &paramIndex, &transformIndex);\r\n";
        }

        if (writeData_)
        {
            spinalPiece->WriteParameters(&buffer_);
            transforms_.Write(&spinalPiece->GetWorldTransform().Inverted(), sizeof(Mat3x4));
        }
    }
    return true;
}

void OpenCLDensityShaderBuilder::Finish()
{
    code_ = FString(OpenCLDensityMainCode, code_).str();
}

}