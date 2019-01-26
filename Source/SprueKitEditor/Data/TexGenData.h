#pragma once

#include <SprueEngine/StringHash.h>

#include <map>
#include <vector>

#include <QIcon>
#include <QColor>

namespace SprueEditor
{
    class TexGenData
    {
    public:
        struct NodeNameData
        {
            const char* first;
            const char* second;
            const char* typeName_;
        };
        typedef std::pair<SprueEngine::StringHash, NodeNameData > NodeNameRecord;
        static const std::map<SprueEngine::StringHash, NodeNameData > NodeNames;

        static const std::map<SprueEngine::StringHash, QColor> TitleColors;

        static std::vector<NodeNameRecord> GetSortedNodeRecords(const char* groupName);
        static QColor GetColorOf(const char* typeName);

        static QIcon GetIconFor(const char* typeName);

        static void DumpToXML(const char* file);
    };
}