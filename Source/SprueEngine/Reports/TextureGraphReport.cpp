#include "TextureGraphReport.h"

#include "HTMLReport.h"

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Graph/Graph.h>
#include <SprueEngine/Graph/GraphNode.h>
#include <SprueEngine/Graph/GraphSocket.h>
#include <SprueEngine/FileBuffer.h>
#include <SprueEngine/FString.h>
#include <SprueEngine/Reports/HTMLReport.h>
#include <SprueEngine/Logging.h>
#include <SprueEngine/TextureGen/TextureNode.h>

#include <SprueEngine/MathGeoLib/Time/Clock.h>

namespace SprueEngine
{

    TextureGraphReport::TextureGraphReport(const std::string& title, const std::string& path, const std::vector<std::string>& files)
    {
        HTMLReport report(path, title);
        MakeTOC(&report, files);
        for (auto file : files)
        {
            if (Graph* graph = GraphFromFile(file))
            {
                MakeHTMLReport(&report, file, graph, false);
                delete graph;
            }
        }
    }

    TextureGraphReport::TextureGraphReport(const std::string& title, const std::string& path, const std::string& file)
    {
        if (Graph* graph = GraphFromFile(file))
        {
            HTMLReport report(path, title);
            MakeHTMLReport(&report, file, graph, true);
        }
    }

    TextureGraphReport::~TextureGraphReport()
    {

    }

    void TextureGraphReport::MakeHTMLReport(HTMLReport* report, const std::string& reportTitle, Graph* graph, bool detailed)
    {
        // Print the report title
        report->Header(1);
        report->Anchor(FString("%1", FileName(reportTitle)));
        report->Link(reportTitle);
        report->Text(FileName(reportTitle));
        report->PopTag();
        report->PopTag();

        report->Header(2);
        report->Text("Texture Outputs");
        report->PopTag();

        report->Table();
        report->Tr();
        math::tick_t totalGenerationTime = 0;
        for (auto output : graph->GetNodesByType<TextureOutputNode>())
        {
            report->Td();
            report->P();
            report->Text(output->name.empty() ? "Unnamed output" : output->name);
            report->PopTag();
            auto startTime = Clock::Tick();
            // TODO! generate the PNG image
            auto blockmap = output->GetPreview();
            report->ImgEmbedded(blockmap.get());
            auto endTime = Clock::Tick();
            auto elapsedTime = Clock::TicksInBetween(endTime, startTime);
            totalGenerationTime += elapsedTime;
            report->P();
            report->Text(FString("%1 seconds", Clock::TicksToSecondsF(elapsedTime)));
            report->PopTag();
            report->PopTag();
        }
        report->PopTag();
        report->PopTag();

        report->P();
        report->Text(FString("Total generation time: %1 seconds", Clock::TicksToSecondsF(totalGenerationTime)));
        report->PopTag();

        report->Anchor("basic_info");
        report->Header(2);
        report->Text("Basic Information");
        report->PopTag();

        report->P();
        report->Bold("Number of Nodes");
        report->Text(": ");
        report->Text(FString("%1", (int)graph->GetNodes().size()));
        report->PopTag();

        report->P();
        report->Bold("Number of Entry Points");
        report->Text(": ");
        report->Text(FString("%1", (int)graph->GetEntryNodes().size()));
        report->PopTag();

        report->P();
        report->Bold("Number of Connections");
        report->Text(": ");
        report->Text(FString("%1", (int)graph->GetConnectionCount()));
        report->PopTag();

        if (!detailed)
            return;

        report->Anchor("nodes_used");
        report->Header(2);
        report->Text("Nodes Used");
        report->PopTag();

        std::map<std::string, int> nodeCts;
        for (auto node : graph->GetNodes())
        {
            auto found = nodeCts.find(node->GetTypeName());
            if (found != nodeCts.end())
                found->second += 1;
            else
                nodeCts.insert(std::make_pair(node->GetTypeName(), 1));
        }

        std::vector< std::pair<std::string, int> > flippedNodes;
        for (auto item : nodeCts)
            flippedNodes.push_back(item);

        std::sort(flippedNodes.begin(), flippedNodes.end(), [](const std::pair<std::string, int>& lhs, const std::pair<std::string, int>& rhs) { return lhs.second < rhs.second; });

        report->Table();
        report->Tr();
        report->Th(); report->Text("#"); report->PopTag();
        report->Th(); report->Text("Node Type"); report->PopTag();
        report->PopTag();
        for (auto item = flippedNodes.rbegin(); item != flippedNodes.rend(); ++item)
        {
            report->Tr();
            report->Td();
            report->Text(FString("%1", (int)item->second));
            report->PopTag();
            report->Td();
            report->Text(item->first.c_str());
            report->PopTag();
            report->PopTag();
        }
        report->PopTag();

        report->Anchor("external_res");
        report->Header(2);
        report->Text("External Resources");
        report->PopTag();

        std::map<std::string, int> resourceCounts;
        std::map<std::string, std::string> resourceTypes;

        for (auto node : graph->GetNodes())
        {
            VariantVector properties = node->GetProperties();
            for (auto item : properties)
            {
                if (item.getType() == VT_ResourceHandle)
                {
                    auto resHandle = item.getResourceHandle();
                    if (!resHandle.Name.empty())
                    {
                        auto foundCount = resourceCounts.find(resHandle.Name);
                        if (foundCount != resourceCounts.end())
                            foundCount->second += 1;
                        else
                            resourceCounts[resHandle.Name] = 1;
                        resourceTypes[resHandle.Name] = Context::GetInstance()->GetHashName(resHandle.Type);
                    }
                }
            }
        }

        std::vector< std::pair<std::string, int> > flippedResources;
        for (auto item : resourceCounts)
            flippedResources.push_back(item);

        std::sort(flippedResources.begin(), flippedResources.end(), [](const std::pair<std::string, int>& lhs, const std::pair<std::string, int>& rhs) { return lhs.second < rhs.second; });

        if (!flippedResources.empty())
        {
            report->Table();
            report->Tr();
            report->Th(); report->Text("#"); report->PopTag();
            report->Th(); report->Text("File Path"); report->PopTag();
            report->Th(); report->Text("Resource Type"); report->PopTag();
            report->PopTag();
            for (auto item = flippedResources.rbegin(); item != flippedResources.rend(); ++item)
            {
                report->Tr();
                report->Td();
                report->Text(FString("%1", (int)item->second));
                report->PopTag();
                report->Td();
                report->Text(item->first.c_str());
                report->PopTag();
                report->Td();
                report->Text(resourceTypes[item->first].c_str());
                report->PopTag();
                report->PopTag();
            }
            report->PopTag();
        }
        else
        {
            report->P();
            report->Text("No external resources used");
            report->PopTag();
        }
    }

    Graph* TextureGraphReport::GraphFromFile(const std::string& filePath)
    {
        SprueEngine::SerializationContext ctx;
        ctx.relativePath_ = FolderOf(filePath);

        Graph* graph = 0x0;
        if (EndsWith(filePath, ".xml"))
        {
            tinyxml2::XMLDocument doc;
            tinyxml2::XMLError errCode = doc.LoadFile(filePath.c_str());
            if (errCode == tinyxml2::XMLError::XML_SUCCESS)
            {
                if (tinyxml2::XMLElement* root = doc.FirstChildElement("graph"))
                {
                    graph = new Graph();
                    graph->Deserialize(root, ctx);
                    if (ctx.pathErrors_.empty())
                        return graph;
                    else
                    {
                        SPRUE_LOG_ERROR(FString("Unable to resolve all file paths in file for report: %1", filePath));
                        delete graph;
                    }
                }
                if (graph)
                    delete graph;
            }
            else
            {
                SPRUE_LOG_ERROR(FString("Unable to open file for report: %1", filePath));
            }
        }
        else if (EndsWith(filePath, ".texg"))
        {
            FileBuffer buffer(filePath.c_str(), true, true);
            if (buffer.ReadFileID().compare("TEXG") == 0)
            {
                // Read the string hash, had to write the hash in order to work with Clone() correctly.
                buffer.ReadStringHash();
                graph = new Graph();
                graph->Deserialize(&buffer, ctx);

                if (ctx.pathErrors_.empty())
                    return graph;
                else
                {
                    SPRUE_LOG_ERROR(FString("Unable to resolve all file paths in file for report: %1", filePath));
                    delete graph;
                }

                return graph;
            }
        }

        return graph;
    }

    void TextureGraphReport::MakeTOC(HTMLReport* report, const std::vector<std::string>& files)
    {
        report->Header(1);
        report->Text("Contents");
        report->PopTag();

        report->UL();
        for (auto file : files)
        {
            if (Graph* graph = GraphFromFile(file))
            {
                report->LI();
                report->Link(FString("#%1", FileName(file)));
                report->Text(FileName(file));
                report->PopTag();
                report->PopTag();
                delete graph;
            }
        }
        report->PopTag();
    }
}