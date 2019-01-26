#include "TextureCreateRibbonBuilder.h"

#include "../../SprueKitEditor.h"
#include "../../InternalNames.h"

#include <EditorLib/Controls/Ribbon/Ribbon.h>
#include <EditorLib/Controls/Ribbon/RibbonButton.h>
#include <EditorLib/Controls/Ribbon/RibbonGallery.h>
#include <EditorLib/Controls/Ribbon/RibbonPage.h>
#include <EditorLib/Controls/Ribbon/RibbonSection.h>

#include "TextureDocument.h"
#include "../../GuiBuilder/Ribbons/RibbonDocumentPage.h"

#include "../../Data/TexGenData.h"

#include <EditorLib/Commands/RegisteredAction.h>

#include <QAction>
#include <string>

namespace SprueEditor
{
    class TextureCreateNodeListItem : public QListWidgetItem
    {
    public:
        TextureCreateNodeListItem(const SprueEngine::StringHash& typeHash, const std::string& name)
        {
            displayName_ = name;
            creationHash_ = typeHash;
            setItemData();
        }

        std::string displayName_;
        QIcon displayIcon_;
        SprueEngine::StringHash creationHash_;
        std::string savedData_; // for prefabs|favorites

        void setItemData()
        {
            setText(displayName_.c_str());
            setData(Qt::UserRole, creationHash_.value_);
            setData(Qt::ItemDataRole::AccessibleTextRole, "TEXTURE_GRAPH_NODE");
        }
    };

    void TextureCreateRibbonBuilder::Build(SprueKitEditor* editor)
    {
        auto page = new RibbonDocumentPage(new RibbonDocumentPageType<TextureDocument>(), "Create");
        page->SetActive(false);
        editor->GetRibbon()->AddPage(page);

        // Generators
        {
            RibbonSection* section = new RibbonSection("Generators");
            RibbonGallery* gallery = new RibbonGallery();
            section->AddWidgets(gallery);
            page->AddSection(section);

            auto nodes = TexGenData::GetSortedNodeRecords("Generator");
            for (auto node : nodes)
            {
                QIcon icon = TexGenData::GetIconFor(node.second.typeName_);

                auto dispitem = new TextureCreateNodeListItem(node.first, node.second.first);
                dispitem->setIcon(icon);
                auto popitem = new TextureCreateNodeListItem(node.first, node.second.first);
                popitem->setIcon(icon);
                gallery->GetRibbonList()->addItem(dispitem);
                gallery->GetPopupList()->addItem(popitem);
            }            
        }

        // Filters
        {
            RibbonSection* section = new RibbonSection("Filters");
            RibbonGallery* gallery = new RibbonGallery();
            section->AddWidgets(gallery);
            page->AddSection(section);

            auto nodes = TexGenData::GetSortedNodeRecords("Filter");
            //auto nextnodes = TexGenData::GetSortedNodeRecords("Color");
            //nodes.insert(nodes.end(), nextnodes.begin(), nextnodes.end());
            for (auto node : nodes)
            {
                QIcon icon = TexGenData::GetIconFor(node.second.typeName_);

                auto dispitem = new TextureCreateNodeListItem(node.first, node.second.first);
                dispitem->setIcon(icon);
                auto popitem = new TextureCreateNodeListItem(node.first, node.second.first);
                popitem->setIcon(icon);
                gallery->GetRibbonList()->addItem(dispitem);
                gallery->GetPopupList()->addItem(popitem);
            }
        }

        // Colors
        {
            RibbonSection* section = new RibbonSection("Colors");
            RibbonGallery* gallery = new RibbonGallery();
            section->AddWidgets(gallery);
            page->AddSection(section);

            auto nodes = TexGenData::GetSortedNodeRecords("Color");
            for (auto node : nodes)
            {
                QIcon icon = TexGenData::GetIconFor(node.second.typeName_);

                auto dispitem = new TextureCreateNodeListItem(node.first, node.second.first);
                dispitem->setIcon(icon);
                auto popitem = new TextureCreateNodeListItem(node.first, node.second.first);
                popitem->setIcon(icon);
                gallery->GetRibbonList()->addItem(dispitem);
                gallery->GetPopupList()->addItem(popitem);
            }
        }

        // Other
        {
            RibbonSection* section = new RibbonSection("Other");
            RibbonGallery* gallery = new RibbonGallery();
            section->AddWidgets(gallery);
            page->AddSection(section);

            auto nodes = TexGenData::GetSortedNodeRecords("Math");
            auto nextnodes = TexGenData::GetSortedNodeRecords("Normal Maps");
            nodes.insert(nodes.end(), nextnodes.begin(), nextnodes.end());
            nextnodes = TexGenData::GetSortedNodeRecords("Bakers");
            nodes.insert(nodes.end(), nextnodes.begin(), nextnodes.end());
            nextnodes = TexGenData::GetSortedNodeRecords("SPECIAL");
            nodes.insert(nodes.end(), nextnodes.begin(), nextnodes.end());
            for (auto node : nodes)
            {
                QIcon icon = TexGenData::GetIconFor(node.second.typeName_);

                auto dispitem = new TextureCreateNodeListItem(node.first, node.second.first);
                dispitem->setIcon(icon);
                auto popitem = new TextureCreateNodeListItem(node.first, node.second.first);
                popitem->setIcon(icon);
                gallery->GetRibbonList()->addItem(dispitem);
                gallery->GetPopupList()->addItem(popitem);
            }
        }
    }

}