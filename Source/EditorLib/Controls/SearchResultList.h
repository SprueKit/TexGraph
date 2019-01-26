#pragma once

#include <QTableWidget>

#include <EditorLib/Search/ISearchable.h>
#include <EditorLib/Controls/ISignificantControl.h>

/// Displays search results
class SearchResultList : public QTableWidget, public ISignificantControl
{
    Q_OBJECT
public:
    SearchResultList();

    void SetResults(const SearchResultVector& results);

private slots:
    void ItemDoubleClicked(QTableWidgetItem* item);
private:
    std::vector< std::shared_ptr<SearchResult> > results_;
};