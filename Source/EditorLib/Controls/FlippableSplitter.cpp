#include "FlippableSplitter.h"

#include <qevent.h>
#include <QMenu>

void FlippableSplitter::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = new QMenu();
    menu->addAction("Flip Orientation", [=](bool) {
        if (this->orientation() == Qt::Horizontal)
            this->setOrientation(Qt::Vertical);
        else
            this->setOrientation(Qt::Horizontal);
    });
    menu->exec(QPoint(event->globalX(), event->globalY()));
}