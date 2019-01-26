#pragma once

#include <qwidget.h>

#include <qboxlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <string>

/// Combines a text editor and browse button for selecting folder or file paths
class PathPickerWidget : public QWidget
{
    Q_OBJECT
public:
    PathPickerWidget(bool folder, bool includeDelButton = false);
    virtual ~PathPickerWidget();

    void SetPath(const std::string& text);
    std::string GetPath() const;

    void SetFilter(const std::string& filter) { filter_ = filter; }
    std::string GetFilter() const { return filter_; }

    public slots:
    void TextChanged(const QString&);
    void BrowseButtonClick(bool);

signals:
    void PathChanged(const PathPickerWidget* widget, const char* newPath);
    void Deleted(const PathPickerWidget* widget);

private:
    std::string lastValidPath_;
    std::string filter_;
    QHBoxLayout* layout_;
    QLineEdit* pathText_;
    QPushButton* browseButton_;
    bool forFolder_;
};