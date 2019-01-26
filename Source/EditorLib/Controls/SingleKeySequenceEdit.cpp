#include "SingleKeySequenceEdit.h"


SingleKeySequenceEdit::SingleKeySequenceEdit(QKeySequence& sequence) :
    QKeySequenceEdit(sequence)
{
}

void SingleKeySequenceEdit::keyPressEvent(QKeyEvent *pEvent)
{
    QKeySequenceEdit::keyPressEvent(pEvent);
    QKeySequence seq(QKeySequence::fromString(keySequence().toString().split(", ").first()));
    setKeySequence(seq);
    emit keySequenceChanged(seq);
}