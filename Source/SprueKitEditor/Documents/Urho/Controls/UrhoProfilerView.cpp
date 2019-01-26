#include "UrhoProfilerView.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Profiler.h>

using namespace Urho3D;

namespace UrhoEditor
{

    UrhoProfilerView::UrhoProfilerView(Urho3D::Context* context) :
        context_(context)
    {
        
    }

    UrhoProfilerView::~UrhoProfilerView()
    {

    }

    void UrhoProfilerView::GenerateUI(QPainter*, const QSize& size)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
        ImGui::SetNextWindowSize(ImVec2(size.width(), size.height()), ImGuiSetCond_Always);
        ImGui::Begin("PROFILER_WINDOW", 0x0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

        if (Profiler* profiler = context_->GetSubsystem<Profiler>())
        {
            if (auto blk = profiler->GetRootBlock())
            {
                ImGui::Columns(6, "Profiler Columns", true);
                
                // Initialize column bias
                if (IsFirstRun())
                {
                    ImGui::SetColumnOffset(5, ImGui::GetColumnOffset(5) + 30);
                    ImGui::SetColumnOffset(4, ImGui::GetColumnOffset(4) + 30);
                    ImGui::SetColumnOffset(3, ImGui::GetColumnOffset(3) + 30);
                    ImGui::SetColumnOffset(2, ImGui::GetColumnOffset(2) + 30);
                    ImGui::SetColumnOffset(1, ImGui::GetColumnOffset(1) + 30);
                }
                ImGui::Text("Profiler Block");
                ImGui::NextColumn();
                ImGui::Text("Ct");
                ImGui::NextColumn();
                ImGui::Text("Avg.");
                ImGui::NextColumn();
                ImGui::Text("Max");
                ImGui::NextColumn();
                ImGui::Text("Frame");
                ImGui::NextColumn();
                ImGui::Text("Total");
                ImGui::NextColumn();
                ImGui::Separator();
                PrintBlock(blk, 0);
            }
        }

        ImGui::End();
    }

    void UrhoProfilerView::PrintBlock(const Urho3D::ProfilerBlock* block, int depth)
    {
        bool  anyChildrenToShow = block->children_.Size();
        ////?? This results in poor usability from nodes appearing and disappearing
        //??for (auto child : block->children_)
        //??    anyChildrenToShow |= child->frameTime_ > 0;

        if (anyChildrenToShow)
        {
            if (depth == 0) // don't display the 'root' block
            {
                for (unsigned i = 0; i < block->children_.Size(); ++i)
                    PrintBlock(block->children_[i], depth + 1);
            }
            else
            {
                bool expanded = ImGui::TreeNode(block->name_);
                ImGui::NextColumn();
                WriteBlockInfo(block);

                if (expanded)
                {
                    for (unsigned i = 0; i < block->children_.Size(); ++i)
                        PrintBlock(block->children_[i], depth + 1);
                    ImGui::TreePop();
                }
            }
        }
        else
        {
            ImGui::TreeNodeEx(block->name_, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
            ImGui::NextColumn();
            WriteBlockInfo(block);
        }
    }

    void UrhoProfilerView::WriteBlockInfo(const Urho3D::ProfilerBlock* block)
    {
        
        float avg = (block->intervalCount_ ? block->intervalTime_ / block->intervalCount_ : 0.0f) / 1000.0f;
        float max = block->intervalMaxTime_ / 1000.0f;
        float frame = block->intervalTime_ / std::max<unsigned>(block->intervalCount_, 1) / 1000.0f;
        float all = block->intervalTime_ / 1000.0f;

        ImGui::Text("%i", block->frameCount_);
        ImGui::NextColumn();
        ImGui::Text("%.3f", avg);
        ImGui::NextColumn();
        ImGui::Text("%.3f", max);
        ImGui::NextColumn();
        ImGui::Text("%.3f", frame);
        ImGui::NextColumn();
        ImGui::Text("%.3f", all);
        ImGui::NextColumn();
    }

    void UrhoProfilerView::BuildStyles()
    {
        QImGui::BuildStyles();
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = { 0, 0, 0, 0.7f };
    }
}