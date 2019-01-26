#pragma once

#include <EditorLib/editorlib_global.h>

#include <QLineEdit>

/// Specialization of QLineEdit that on focus selects all contents and responds to the enter/return key
/// to emit the SearchPerformed() signal.
class EDITORLIB_EXPORT SearchLineEdit : public QLineEdit
{
    Q_OBJECT;
public:
    SearchLineEdit();
    virtual ~SearchLineEdit();

signals:
    void SearchPerformed();

protected:
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void focusInEvent(QFocusEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent*) override;

    bool justGotFocus_ = false;
};