#pragma once

#include <EditorLib/editorlib_global.h>

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>

#include <map>
#include <string>

/// Baseclass for a dialog that uses autocomplete and a list to search through items as actions or such
class EDITORLIB_EXPORT FastCommandDlg : public QDialog
{

    Q_OBJECT;
protected:
    FastCommandDlg();

public:
    virtual ~FastCommandDlg();

    virtual void keyReleaseEvent(QKeyEvent* event) override;

protected:
    virtual void Prepare();
    virtual void Fill(QStringList&) = 0;
    virtual void Selected(const QString& text) = 0;
    virtual bool eventFilter(QObject* src, QEvent* event) Q_DECL_OVERRIDE;

private:
    class SearchLineEdit;
    class FastCommandListWidget;

protected:
    std::string title_;
    QLineEdit* search_ = 0x0;
    QListWidget* list_ = 0x0;
    QCompleter* completer_ = 0x0;
    int lastSelStart_ = -1;
};