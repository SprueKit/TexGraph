#pragma once

#include <EditorLib/IM/QImGui.h>

namespace Urho3D
{
    class Context;
    class ProfilerBlock;
}

namespace UrhoEditor
{

    class UrhoProfilerView : public QImGui
    {
    public:
        UrhoProfilerView(Urho3D::Context* context);
        virtual ~UrhoProfilerView();

    protected:
        virtual void GenerateUI(QPainter*, const QSize& size) override;

        virtual void BuildStyles() override;

        /// Recursively outputs profiler blocks
        void PrintBlock(const Urho3D::ProfilerBlock*, int depth);
        /// Writes the frame info.
        void WriteBlockInfo(const Urho3D::ProfilerBlock*);

        Urho3D::Context* context_;
    };

}