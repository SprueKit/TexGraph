#pragma once

#include <SprueEngine/ClassDef.h>

#include <string>
#include <vector>

namespace SprueEngine
{
    class Graph;
    class HTMLReport;

    /// Constructs a report that contains information on:
    ///     graph nodes used (with optional counts),
    ///     Writes images of all graphs into a table (based on texture unit)
    ///     External graph references
    ///         Externals referenced by a graph
    ///         Graphs referenced by other graphs
    class SPRUE TextureGraphReport
    {
    public:
        TextureGraphReport(const std::string& reportTitle, const std::string& path, const std::vector<std::string>& files);
        TextureGraphReport(const std::string& reportTitle, const std::string& path, const std::string& file);
        virtual ~TextureGraphReport();

    protected:
        void MakeHTMLReport(HTMLReport* report, const std::string& reportTitle, Graph* graph, bool detailed);
        void MakeTOC(HTMLReport* report, const std::vector<std::string>& files);
        Graph* GraphFromFile(const std::string& filePath);
    };

}