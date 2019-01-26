#include "DocumentFauxButton.h"

#include <EditorLib/DocumentBase.h>

#include <qevent.h>
#include <QBrush>
#include <QPainter>
#include <QPen>


#define ACTIVE_BG_COLOR QColor("#3daee9")
#define INACTIVE_BG_COLOR QColor("#31363b")
#define DIRTY_BORDER QColor("#BA1111")

DocumentFauxButton::DocumentFauxButton(DocumentBase* document, QWidget* parent) :
    QWidget(parent),
    document_(document)
{

}

void DocumentFauxButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (dirty_)
        painter.setPen(QPen(QBrush(DIRTY_BORDER), 2));

    if (current_)
        painter.setBrush(ACTIVE_BG_COLOR);
    else
        painter.setBrush(INACTIVE_BG_COLOR);

    painter.drawRoundedRect(0, 0, width(), height(), 5.0f, 5.0f);
    QFont font("Arial", 10);
    QFontMetrics metrics(font);
    painter.setFont(font);
    painter.setPen(QPen(QBrush(Qt::white), 2));
    painter.drawText(10, height() / 2 + metrics.height() / 2, document_->GetFileName());
}

void DocumentFauxButton::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MouseButton::LeftButton)
        hasMouseDown_ = true;
}

void DocumentFauxButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (hasMouseDown_ && event->button() == Qt::MouseButton::LeftButton)
        emit Clicked();
}

void DocumentFauxButton::enterEvent(QEvent* event)
{
    hasMouseDown_ = false;
}

void DocumentFauxButton::SetDirty(bool state)
{
    dirty_ = state;
    repaint();
}

void DocumentFauxButton::SetCurrent(bool isCurrent)
{
    current_ = isCurrent;
    repaint();
}