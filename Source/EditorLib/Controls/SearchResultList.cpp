#include "SearchResultList.h"

#include <EditorLib/InternalNames.h>

#include <QHeaderView>

SearchResultList::SearchResultList()
{
    setObjectName(SEARCH_RESULTS_CONTROL);
    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

    setColumnCount(3);
    verticalHeader()->setVisible(false);

    QStringList headers;
    headers.push_back("Found");
    headers.push_back("Source");
    headers.push_back("Hits");
    setHorizontalHeaderLabels(headers);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

    connect(this, &QTableWidget::itemDoubleClicked, this, &SearchResultList::ItemDoubleClicked);
}

void SearchResultList::SetResults(const SearchResultVector& results)
{
    clearContents();
    results_ = results;
    unsigned row = 0;
    setRowCount(results_.size());
    for (auto result : results)
    {
        auto textItem = new QTableWidgetItem(result->text_);
        auto sourceItem = new QTableWidgetItem(result->source_);
        auto hitItem = new QTableWidgetItem(QString::number(result->hitCount_));

        auto flags = textItem->flags();
        flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);

        textItem->setFlags(flags);
        sourceItem->setFlags(flags);
        hitItem->setFlags(flags);

        setItem(row, 0, textItem);
        setItem(row, 1, sourceItem);
        setItem(row, 2, hitItem);
        ++row;
    }
}

void SearchResultList::ItemDoubleClicked(QTableWidgetItem* item)
{
    if (item->row() < results_.size())
        results_[item->row()]->GoTo();
}