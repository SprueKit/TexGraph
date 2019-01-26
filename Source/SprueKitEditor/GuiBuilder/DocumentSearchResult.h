#pragma once

#include <EditorLib/Search/ISearchable.h>
#include <EditorLib/DocumentBase.h>

namespace SprueEditor
{

    class DocumentSearchResult : public SearchResult
    {
    public:
        DocumentSearchResult(std::shared_ptr<DataSource> dataSource, std::shared_ptr<DocumentBase> document);

        virtual void GoTo() override;

    private:
        std::shared_ptr<DocumentBase> document_;
    };

}