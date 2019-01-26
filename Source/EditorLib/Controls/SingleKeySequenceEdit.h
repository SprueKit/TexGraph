#pragma once

#include <qkeysequence>
#include <qkeysequenceedit>

/// Specialized version of QKeySequenceEdit that only permits a single shortcut to be entered instead of multiple
class SingleKeySequenceEdit : public QKeySequenceEdit
{
public:
    SingleKeySequenceEdit(QKeySequence& sequence);
    virtual ~SingleKeySequenceEdit() { }

protected:
    void keyPressEvent(QKeyEvent *pEvent) override;
};