/* Copyright (c) 2012, STANISLAW ADASZEWSKI
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of STANISLAW ADASZEWSKI nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL STANISLAW ADASZEWSKI BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "qnodeseditor.h"

#include <qapplication.h>
#include <qgraphicsview.h>
#include <QGraphicsScene>
#include <qevent.h>
#include <qkeyeventtransition.h>
#include <QGraphicsSceneMouseEvent>

#include "qneport.h"
#include "qneconnection.h"
#include "qneblock.h"

#include <qrubberband.h>

QNodesEditor::QNodesEditor(QObject *parent) :
    QObject(parent), rubber_(0x0)
{
	conn_ = 0;
}

void QNodesEditor::install(QGraphicsScene *s, QGraphicsView* view)
{
	s->installEventFilter(this);
	scene_ = s;
    view_ = view;
}

void QNodesEditor::haltConnection()
{
    if (conn_)
    {
        scene_->removeItem(conn_);
        delete conn_;
        conn_ = 0;
    }
}

QGraphicsItem* QNodesEditor::itemAt(const QPointF &pos)
{
	QList<QGraphicsItem*> items = scene_->items(QRectF(pos - QPointF(1,1), QSize(3,3)));

	foreach(QGraphicsItem *item, items)
		if (item->type() > QGraphicsItem::UserType)
			return item;

	return 0;
}

bool QNodesEditor::eventFilter(QObject *o, QEvent *e)
{
	QGraphicsSceneMouseEvent *me = (QGraphicsSceneMouseEvent*) e;
	switch ((int) e->type())
	{
	case QEvent::GraphicsSceneMousePress:
	{
		switch ((int) me->button())
		{
		case Qt::LeftButton:
		{
            if (me->modifiers() & Qt::KeyboardModifier::ShiftModifier)
            {
                startRubber_ = me->scenePos();
                rubber_ = new QRubberBand(QRubberBand::Shape::Rectangle, view_);
                QPointF local = view_->mapFromScene(startRubber_);
                rubber_->setGeometry(local.x(), local.y(), 1, 1);
                rubber_->show();
                view_->setDragMode(QGraphicsView::DragMode::RubberBandDrag);
                me->accept();
                return true;
            }

			QGraphicsItem *item = itemAt(me->scenePos());
			if (item && item->type() == QNEPort::Type)
			{
                conn_ = new QNEConnection(0);
                scene_->addItem(conn_);
				conn_->setPort1((QNEPort*) item);
				conn_->setPos1(item->scenePos());
				conn_->setPos2(me->scenePos());
				conn_->updatePath();

				return true;
			}
			break;
		}
        }
	}
	case QEvent::GraphicsSceneMouseMove:
	{
        if (view_->dragMode() == QGraphicsView::DragMode::RubberBandDrag)
        {
            QPainterPath path;

            QPointF newPos = me->scenePos();
            QPointF startPt(std::min(newPos.x(), startRubber_.x()), std::min(newPos.y(), startRubber_.y()));
            QPointF endPt(std::max(newPos.x(), startRubber_.x()), std::max(newPos.y(), startRubber_.y()));
            
            path.addRect(startPt.x(), startPt.y(), endPt.x() - startPt.x(), endPt.y() - startPt.y());
            
            startPt = view_->mapFromScene(startPt);
            endPt = view_->mapFromScene(endPt);
            
            rubber_->setGeometry(startPt.x(), startPt.y(), endPt.x() - startPt.x(), endPt.y() - startPt.y());
            rubber_->repaint();
            scene_->setSelectionArea(path, Qt::ItemSelectionMode::IntersectsItemBoundingRect);
            return true;
        }
		if (conn_)
		{
			conn_->setPos2(me->scenePos());
			conn_->updatePath();
			return true;
		}
		break;
	}
	case QEvent::GraphicsSceneMouseRelease:
	{
        if (me->button() == Qt::LeftButton && view_->dragMode() == QGraphicsView::DragMode::RubberBandDrag)
        {
            view_->setDragMode(QGraphicsView::DragMode::NoDrag);
            if (rubber_)
                delete rubber_;
            rubber_ = 0x0;
            me->accept();
            return true;
        }
		if (conn_ && me->button() == Qt::LeftButton)
		{
			QGraphicsItem *item = itemAt(me->scenePos());
			if (item && item->type() == QNEPort::Type)
			{
				QNEPort *port1 = conn_->port1();
				QNEPort *port2 = (QNEPort*) item;

				if (port1->block() != port2->block() && port1->isOutput() != port2->isOutput() && !port1->isConnected(port2))
				{
                    bool allowed = true;
                    emit AllowConnect(allowed, port1, port2);
                    if (allowed && !objectSelfFormsConnections_)
                    {
					    conn_->setPos2(port2->scenePos());
					    conn_->setPort2(port2);
					    conn_->updatePath();
					    conn_ = 0;
					    return true;
                    }
				}
			}

            scene_->removeItem(conn_);
			delete conn_;
			conn_ = 0;
			return true;
		}
		break;
	}
	}
	return QObject::eventFilter(o, e);
}