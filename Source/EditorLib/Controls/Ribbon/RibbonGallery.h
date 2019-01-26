#pragma once

#include <EditorLib/editorlib_global.h>

#include <QListWidget>
#include <QWidget>

/// Widget for handling gallery type widgets in the ribbon.
class EDITORLIB_EXPORT RibbonGallery : public QWidget
{
    Q_OBJECT;
public:
    /// Construct, optionally sets up the list RibbonGallery for search functionality.
    RibbonGallery(bool searchable = false);
    /// Destruct.
    virtual ~RibbonGallery();

    /// Adds an item to both lists (uses clone to fill the popup-list) 
    void AddItem(QListWidgetItem* item);

    /// Gets the QListWidget that is displayed in the actual ribbon.
    QListWidget* GetRibbonList() { return displayList_; }
    /// Gets the QListWidget that is displayed in the popup window.
    QListWidget* GetPopupList() { return popupList_; }

protected:
    /// Override if special searching functionality is required.
    virtual void UpdateSearch(const QString& searchText);

    private slots:
    /// Spoof arrow key events are sent to scroll the list using the scroll buttons.
    void OnScrollUp(bool);
    /// Spoof arrow key events are sent to scroll the list using the scroll buttons.
    void OnScrollDown(bool);

private:
    /// This list widget is in this actual widget.
    QListWidget* displayList_;
    /// This list widget is not.
    QListWidget* popupList_;
};