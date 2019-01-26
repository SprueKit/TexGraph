#include "UrhoResourceCache.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Core/StringUtils.h>

using namespace Urho3D;

namespace UrhoEditor
{

    UrhoResourceCache::UrhoResourceCache(Urho3D::Context* context) :
        context_(context)
    {
        
    }

    UrhoResourceCache::~UrhoResourceCache()
    {

    }

    void UrhoResourceCache::GenerateUI(QPainter*, const QSize& size)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
        ImGui::SetNextWindowSize(ImVec2(size.width(), size.height()), ImGuiSetCond_Always);
        ImGui::Begin("RESOURCE_CACHE", 0x0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

        if (auto cache = context_->GetSubsystem<ResourceCache>())
        {
            ImGui::Columns(5, "Resources", true);
            ImGui::Text("Resource");
            ImGui::NextColumn();
            ImGui::Text("Use"); // TODO: add a sort button for the individual resources
            ImGui::NextColumn();
            ImGui::Text("Ct");  // TODO: add a sort button for the individual resources
            ImGui::NextColumn();
            ImGui::Text("Max");
            ImGui::NextColumn();
            ImGui::Text("Total");
            ImGui::NextColumn();
            ImGui::Separator();

            unsigned totalResourceCt = 0;
            unsigned long long totalLargest = 0;
            unsigned long long totalAverage = 0;
            unsigned long long totalUse = cache->GetTotalMemoryUse();

            auto resourceGroups = cache->GetAllResources();
            for (auto resourceGrp : resourceGroups)
            {
                const unsigned resourceCt = resourceGrp.second_.resources_.Size();
                unsigned long long average = 0;
                if (resourceCt > 0)
                    average = resourceGrp.second_.memoryUse_ / resourceCt;
                else
                    average = 0;
                unsigned long long largest = 0;
                for (HashMap<StringHash, SharedPtr<Resource> >::ConstIterator resIt = resourceGrp.second_.resources_.Begin(); resIt != resourceGrp.second_.resources_.End(); ++resIt)
                {
                    if (resIt->second_->GetMemoryUse() > largest)
                        largest = resIt->second_->GetMemoryUse();
                    if (largest > totalLargest)
                        totalLargest = largest;
                }

                const String countString(resourceGrp.second_.resources_.Size());
                const String memUseString = GetFileSizeString(average);
                const String memMaxString = GetFileSizeString(largest);
                const String memBudgetString = GetFileSizeString(resourceGrp.second_.memoryBudget_);
                const String memTotalString = GetFileSizeString(resourceGrp.second_.memoryUse_);
                const String resTypeName = context_->GetTypeName(resourceGrp.first_);

            // Name column
                bool isExpanded = ImGui::TreeNode(resTypeName.CString());
                ImGui::NextColumn();
                
            // Size column
                ImGui::Text(memUseString.CString());
                ImGui::NextColumn();

            // Ct column
                ImGui::Text(countString.CString());
                ImGui::NextColumn();
                
            // Max column
                ImGui::Text(memMaxString.CString());
                ImGui::NextColumn();

            // Total column
                ImGui::Text(memTotalString.CString());
                ImGui::NextColumn();

                if (isExpanded)
                {
                    //TODO: collect and sort here

                    for (auto res : resourceGrp.second_.resources_)
                    {
                        auto name = res.second_->GetName();
                        unsigned size = res.second_->GetMemoryUse();
                        auto fileSize = GetFileSizeString(size);

                    // Name column
                        ImGui::TreeNodeEx(name.CString(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
                        ImGui::NextColumn();
                        // TODO? show a thumbnail from OS filesystem thumbnail provider?

                    // Size column, show the size of this resource
                        ImGui::Text(fileSize.CString());
                        ImGui::NextColumn();

                    // Ct column, show reference count in in the "Ct" column
                        ImGui::Text("%ix", res.second_->Refs());
                        ImGui::NextColumn();

                    // Max column, nothing to show
                        ImGui::NextColumn();
                    // Total column, nothing to show
                        ImGui::NextColumn();
                    }
                    ImGui::TreePop();
                }
            }
        }

        ImGui::End();
    }

    void UrhoResourceCache::BuildStyles()
    {
        QImGui::BuildStyles();
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = { 0, 0, 0, 0.7f };
    }
}