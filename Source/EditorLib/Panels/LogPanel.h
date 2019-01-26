#pragma once

#include <EditorLib/editorlib_global.h>

#include <QWidget>

class QTextBrowser;
class SearchLineEdit;

class EDITORLIB_EXPORT LogPanel : public QWidget
{
    Q_OBJECT;
public:
    LogPanel();
    virtual ~LogPanel();

public slots:
    void MessageAdded(const QString& src, const QString& msg, const QString& level);
    void OnSearch();

private:
    QTextBrowser* browser_;
    SearchLineEdit* searchEdit_;
};