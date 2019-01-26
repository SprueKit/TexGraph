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

#include "qneconnection.h"

#include "qneport.h"

#include <qpainter.h>
#include <QBrush>
#include <QPen>
#include <QGraphicsScene>
#include <qgraphicseffect.h>

QNEConnection::QNEConnection(QGraphicsItem *parent) : QGraphicsPathItem(parent)
{
    this->setSelected(true);
	setPen(QPen(Qt::white, 2));
	setBrush(Qt::NoBrush);
	setZValue(-1);
	m_port1 = 0;
	m_port2 = 0;
    setFlag(QGraphicsItem::ItemIsSelectable);

    //QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    //effect->setBlurRadius(5.0f);
    //effect->setColor(QColor(0, 0, 0, 128));
    //effect->setOffset(8.0f, 8.0f);
    //setGraphicsEffect(effect);
}

QNEConnection::~QNEConnection()
{
	if (m_port1)
		m_port1->connections().remove(m_port1->connections().indexOf(this));
	if (m_port2)
		m_port2->connections().remove(m_port2->connections().indexOf(this));
}

void QNEConnection::setPos1(const QPointF &p)
{
	pos1 = p;
}

void QNEConnection::setPos2(const QPointF &p)
{
	pos2 = p;
}

void QNEConnection::setPort1(QNEPort *p)
{
	m_port1 = p;

	m_port1->connections().append(this);
}

void QNEConnection::setPort2(QNEPort *p)
{
	m_port2 = p;

	m_port2->connections().append(this);
}

void QNEConnection::updatePosFromPorts()
{
	pos1 = m_port1->scenePos();
    if (m_port2)
        pos2 = m_port2->scenePos();
    else
        pos2 = pos1;
}

void QNEConnection::updatePath()
{
	QPainterPath p;

	//QPointF pos1(m_port1->scenePos());
	//QPointF pos2(m_port2->scenePos());
	p.moveTo(pos1);

	qreal dx = pos2.x() - pos1.x();
	qreal dy = pos2.y() - pos1.y();

    QPointF ctr1;
    QPointF ctr2;
    if (/*fabsf(dx) > 100 || */ pos2.x() <= pos1.x())
    {
        ctr1 = QPointF(pos1.x() + 150, pos1.y());
        ctr2 = QPointF(pos2.x() - 150, pos2.y());
    }
    else
    {
        ctr1 = QPointF(pos1.x() + dx * 0.5, pos1.y() + dy * 0.1);
        ctr2 = QPointF(pos1.x() + dx * 0.5, pos1.y() + dy * 0.9);
    }

	p.cubicTo(ctr1, ctr2, pos2);

	setPath(p);
}

QNEPort* QNEConnection::port1() const
{
	return m_port1;
}

QNEPort* QNEConnection::port2() const
{
	return m_port2;
}

void QNEConnection::save(QDataStream &ds)
{
	ds << (quint64) m_port1;
	ds << (quint64) m_port2;
}

void QNEConnection::load(QDataStream &ds, const QMap<quint64, QNEPort*> &portMap)
{
	quint64 ptr1;
	quint64 ptr2;
	ds >> ptr1;
	ds >> ptr2;

	setPort1(portMap[ptr1]);
	setPort2(portMap[ptr2]);
	updatePosFromPorts();
	updatePath();
}

void QNEConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (isSelected())
        setPen(QPen(Qt::yellow, 6));
    else
        setPen(QPen(Qt::white, 4));
    QGraphicsPathItem::paint(painter, option, widget);
}

QNEConnection* QNEConnection::GetConnection(QGraphicsScene* scene, void* fromUserData, void* toUserData)
{
    auto items = scene->items();
    for (auto item : items)
    {
        if (item && item->type() == QNEConnection::Type)
        {
            QNEConnection* conn = (QNEConnection*)item;
            if (conn->port1()->ptr() == fromUserData && conn->port2()->ptr() == toUserData)
                return conn;
            if (conn->port1()->ptr() == toUserData && conn->port2()->ptr() == fromUserData)
                return conn;
        }
    }
    return 0x0;
}
