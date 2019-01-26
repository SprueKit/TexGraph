#include "DataGridWidget.h"

#include <EditorLib/ApplicationCore.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/DocumentBase.h>

#include <QBoxLayout>

#include <algorithm>
#include <iterator>
#include <set>

DataGridWidget::DataGridWidget()
{
    stack_ = new QStackedLayout(this);
    table_ = new QTableWidget();

    stack_->addWidget(table_);

    connect(table_, &QTableWidget::itemChanged, this, &DataGridWidget::DataChanged);
}

DataGridWidget::~DataGridWidget()
{

}

void DataGridWidget::Link(Selectron* sel)
{
    connect(sel, &Selectron::SelectionChanged, this, &DataGridWidget::SelectionChanged);
    connect(sel, &Selectron::DataChanged, this, &DataGridWidget::SelectionDataChanged);
}

void DataGridWidget::ActiveDocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc)
{
}

void DataGridWidget::SelectionChanged(void* source, Selectron* sel)
{
    // If we're not visible than don't do this.
    if (hidden_)
        return;

    dataSources_.clear();
    table_->clearContents();
    table_->setRowCount(0);

    if (sel->GetSelectedCount() == 0)
        return SetToNoSelection();


    table_->blockSignals(true);

    filtered_.clear();
    std::set<QString> working;
    appearanceOrder_.clear();

    for (unsigned i = 0; i < sel->GetSelectedCount(); ++i)
    {
        working.clear();
        auto object = sel->Selected(i);
        
        dataSources_.push_back(object);
        auto names = object->EnumerateFields();
        for (auto name : names)
        {
            if (i == 0)
            {
                filtered_.insert(name);
                appearanceOrder_.push_back(name);
            }
            else
                working.insert(name);
        }

        if (i != 0)
        {
            std::set<QString> newCurrent;
            std::set_intersection(filtered_.begin(), filtered_.end(), working.begin(), working.end(), std::inserter(newCurrent, newCurrent.begin()));
            filtered_ = newCurrent;
        }
    }

    if (!filtered_.empty())
    {
        {
            table_->setColumnCount(filtered_.size());
            int c = 0;
            for (unsigned col = 0; col < appearanceOrder_.size(); ++col)
            {
                QString column = appearanceOrder_[col];
                if (filtered_.find(column) == filtered_.end())
                    continue;
                table_->setHorizontalHeaderItem(c, new QTableWidgetItem(column));
                ++c;
            }
        }

        for (unsigned r = 0; r < dataSources_.size(); ++r)
        {
            auto object = sel->Selected(r);
            
            table_->insertRow(r);
            int c = 0;
            for (unsigned col = 0; col < appearanceOrder_.size(); ++col)
            {
                QString column = appearanceOrder_[col];
                // Was filtered out
                if (filtered_.find(column) == filtered_.end())
                    continue;
                //if (auto widget = handler->CreateWidget(object, column))
                //{
                //    table_->setCellWidget(r, c, widget);
                //}
                //else
                {
                    QTableWidgetItem* item = new QTableWidgetItem(object->ToText(column));
                    if (object->CanSetText(column))
                        item->setFlags(item->flags() | Qt::ItemIsEditable);
                    table_->setItem(r, c, item);
                }
                ++c;
            }
        }
        table_->blockSignals(false);
        return;
    }
    // No one above returned, so set to incompatible
    table_->blockSignals(false);
    SetToIncompatible();
}

void DataGridWidget::SelectionDataChanged(void* source, Selectron* sel, unsigned hash)
{
    // If we're not visible than don't do this.
    if (hidden_)
        return;

    if (source == this)
        return;
    SelectionChanged(source, sel);
}

void DataGridWidget::SetToIncompatible()
{
    stack_->setCurrentIndex(1);
}

void DataGridWidget::SetToNoSelection()
{
    stack_->setCurrentIndex(0);
}

void DataGridWidget::DataChanged(QTableWidgetItem* item)
{
    if (auto object = dataSources_[item->row()])
    {
        int c = 0;
        for (unsigned col = 0; col < appearanceOrder_.size(); ++col)
        {
            QString column = appearanceOrder_[col];
            // Was filtered out
            if (filtered_.find(column) == filtered_.end())
                continue;
            if (c == item->column())
            {
                object->FromText(column, item->text());
                break;
            }
            ++c;
        }
    }
}

void DataGridWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    hidden_ = false;
    // Force an update
    SelectionChanged(0x0, GetSelectron());
}

void DataGridWidget::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    hidden_ = true;
}