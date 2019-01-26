#pragma once

#include <EditorLib/Selectron.h>

#include <QWidget>
#include <QStackedLayout>
#include <QTableWidget>

#include <vector>
#include <set>
#include <string>
#include <memory>

class DocumentBase;

/// Control that displays the values of selected objects in a datagrid format. 
/// Enables relatively easy comparisons of values and editing to match/differ.
class DataGridWidget : public QWidget, public SelectronLinked
{
    Q_OBJECT
public:
    /// Construct.
    DataGridWidget();
    /// Destruct.
    virtual ~DataGridWidget();

    /// Setup signals/slots for selection changes.
    virtual void Link(Selectron* sel) override;

private slots:
    /// Flush.
    void ActiveDocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc);
    /// Redertimine the grid layout.
    void SelectionChanged(void* source, Selectron* sel);
    /// Update changes to the data of selected objects.
    void SelectionDataChanged(void* source, Selectron* sel, unsigned hash);

    /// Update properties from text cell changes.
    void DataChanged(QTableWidgetItem* item);

protected:
    virtual void showEvent(QShowEvent*) Q_DECL_OVERRIDE;
    virtual void hideEvent(QHideEvent*) Q_DECL_OVERRIDE;

private:
    /// Sets the layout stack to display the "Incompatible selections" page.
    void SetToIncompatible();
    /// Sets the layout stack to display the "Nothing selected" page.
    void SetToNoSelection();

    /// Only contains 3 pages, "nothing selected," "Incompatible objects selected", and the datatable itself
    QStackedLayout* stack_;
    /// Table-grid used for cell display.
    QTableWidget* table_;
    /// List of the columns in left-to-right order, based on the order in which their names were first encountered.
    std::vector<QString> appearanceOrder_;
    /// List of the columns to be displayed that have passed filtering.
    std::set<QString> filtered_;
    /// List of the datasources used for each row of the datagrid.
    std::vector< std::shared_ptr<DataSource> > dataSources_;
    bool hidden_ = true;
};