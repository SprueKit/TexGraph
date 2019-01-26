#include "SearchLineEdit.h"

#include <qevent.h>

SearchLineEdit::SearchLineEdit()
{

}

SearchLineEdit::~SearchLineEdit()
{

}

void SearchLineEdit::focusInEvent(QFocusEvent* event)
{
    QLineEdit::focusInEvent(event);
    selectAll();
    justGotFocus_ = true;
}

void SearchLineEdit::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key::Key_Enter || event->key() == Qt::Key::Key_Return)
    {
        emit SearchPerformed();
        event->accept();
    }
    else
        QLineEdit::keyReleaseEvent(event);
}

void SearchLineEdit::mousePressEvent(QMouseEvent* event)
{
    if (!hasFocus() || justGotFocus_)
    {
        event->accept();
        selectAll();
        justGotFocus_ = false;
    }
    else
        QLineEdit::mousePressEvent(event);
}

void SearchLineEdit::mouseReleaseEvent(QMouseEvent* event)
{
    if (!hasFocus())
        event->accept();
    else
        QLineEdit::mouseReleaseEvent(event);
}