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

#include "qneblock.h"

#include <QImage>
#include <QPen>
#include <QGraphicsScene>
#include <QFontMetrics>
#include <QPainter>
#include <qgraphicseffect.h>
#include <qgraphicsview.h>

#include "qneport.h"

QNEBlock::QNEBlock(BLOCK_MOVED_CALLBACK callback, QGraphicsItem *parent) : QGraphicsPathItem(parent),
    userData(0x0),
    moveCallback_(callback),
    previewProvider_(0x0)
{
    backgroundColor_ = QColor(100, 100, 100);
	QPainterPath p;
	p.addRoundedRect(-50, -15, 100, 30, 5, 5);
	setPath(p);
	setPen(QPen(Qt::darkGreen));

	setBrush(Qt::green);
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);
	horzMargin = 20;
	vertMargin = 5;
	width = horzMargin;
	height = vertMargin;

    //QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    //effect->setBlurRadius(5.0f);
    //effect->setColor(QColor(0, 0, 0, 128));
    //effect->setOffset(8.0f, 8.0f);
    //setGraphicsEffect(effect);
}

QNEBlock::~QNEBlock()
{

}

QNEPort* QNEBlock::addPort(const QString &name, bool isOutput, int flags, void* ptr, BRUSH_STRATEGY strat)
{
	QNEPort *port = new QNEPort(this);
	port->setName(name);
	port->setIsOutput(isOutput);
	port->setNEBlock(this);
	port->setPortFlags(flags);
    if (flags && QNEPort::NamePort)
        port->GetLabel()->setDefaultTextColor(Qt::white);
	port->setPtr(ptr);
    if (strat)
        strat(port);

    calculateLayout();

	return port;
}

void QNEBlock::addInputPort(const QString &name)
{
	addPort(name, false);
}

void QNEBlock::addOutputPort(const QString &name)
{
	addPort(name, true);
}

void QNEBlock::addInputPorts(const QStringList &names)
{
	foreach(QString n, names)
		addInputPort(n);
}

void QNEBlock::addOutputPorts(const QStringList &names)
{
	foreach(QString n, names)
		addOutputPort(n);
}

void QNEBlock::clearOutputPorts()
{
    auto allPorts = ports();
    for (auto port : allPorts)
    {
        if (port->isOutput())
        {
            port->scene()->removeItem(port);
            delete port;
        }
    }
}

#include <QStyleOptionGraphicsItem>

void QNEBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	if (isSelected()) {
        painter->setPen(QPen(Qt::green , 2, Qt::PenStyle::DotLine));
        QLinearGradient grad;
        grad.setCoordinateMode(QGradient::CoordinateMode::ObjectBoundingMode);
        grad.setStart(0.0f, 0.0f);
        grad.setFinalStop(0.0f, 1.0f);
        grad.setColorAt(0.0f, QColor(100, 100, 25));
        grad.setColorAt(0.6f, QColor(200, 200, 125));
        painter->setBrush(grad);
    
		//painter->setBrush(Qt::yellow);
	} else {
		painter->setPen(QPen(Qt::lightGray));
        QLinearGradient grad;
        grad.setCoordinateMode(QGradient::CoordinateMode::ObjectBoundingMode);
        grad.setStart(0.0f, 0.0f);
        grad.setFinalStop(0.0f, 1.0f);
        grad.setColorAt(0.0f, backgroundColor_);
        grad.setColorAt(0.6f, backgroundColor_.lighter(200));
        painter->setBrush(grad);
		//painter->setBrush(Qt::green);
	}

	painter->drawPath(path());
    QRectF pathRect = path().boundingRect();

    // Draw the title header
    QLinearGradient grad;
    QGradientStops stops;
    grad.setCoordinateMode(QGradient::CoordinateMode::ObjectBoundingMode);
    grad.setStart(0.25f, 0.0f); 
    grad.setFinalStop(1.25f, 0.0f);
    grad.setColorAt(0.0f, titleColor_);
    grad.setColorAt(1.0f, titleColor_.darker());
    painter->setBrush(grad);
    painter->setPen(Qt::NoPen);
    QRectF header = path().boundingRect();
    header.setLeft(header.left() + 2);
    header.setRight(header.right() - 2);
    header.setTop(header.top() + 2);
    header.setBottom(header.top() + 24);
    painter->drawRoundedRect(header, 3, 3);

    painter->drawPixmap(pathRect.left(), pathRect.top(), icon_.pixmap(32, 32));
    
    if (previewImage_)
    {
        QRectF rect = path().boundingRect();
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::black);

        const int desiredWidth = std::min(previewImage_->width(), 128);
        const int desiredHeight = std::min(previewImage_->height(), 128);
        painter->drawRect(rect.left() + 0 + (std::max((int)rect.width() - desiredWidth, 0))/2, rect.bottom() - std::min(previewImage_->height(), 128) - 3, 
            std::min(previewImage_->width(), 128) + 2, std::min(previewImage_->height(), 128) + 2);
        painter->drawImage(
            QRectF(
                rect.left() + 0 + (std::max((int)rect.width() - desiredWidth, 0)) / 2,
                rect.bottom() - std::min(previewImage_->height(), 128) - 2,
                desiredWidth,
                desiredHeight),
            *(previewImage_.get()));
    }
}

QNEBlock* QNEBlock::clone()
{
    QNEBlock *b = new QNEBlock(0);
    this->scene()->addItem(b);

	foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() == QNEPort::Type)
		{
			QNEPort *port = (QNEPort*) port_;
			b->addPort(port->portName(), port->isOutput(), port->portFlags(), port->ptr());
		}
	}

	return b;
}

QVector<QNEPort*> QNEBlock::ports()
{
	QVector<QNEPort*> res;
	foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() == QNEPort::Type)
			res.append((QNEPort*) port_);
	}
	return res;
}

QNEPort* QNEBlock::findPort(void* ptr)
{
    for (auto port : ports())
        if (port->ptr() == ptr)
            return port;
    return 0x0;
}

QVariant QNEBlock::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (moveCallback_)
        moveCallback_(this);
    Q_UNUSED(change);

	return value;
}

void QNEBlock::updatePreview()
{
    if (previewProvider_)
    {
        previewProvider_(this);
    }
    calculateLayout();
}

void QNEBlock::calculateLayout()
{
    QFontMetrics fm(scene()->font());
    int w = fm.width("      ");
    int h = fm.height();
    // port->setPos(0, height + h/2);
    if (w > width - horzMargin)
        width = w + horzMargin;
    height = 0;

    int leftWidth = 0;
    int rightWidth = 0;
    int titleWidth = 0;
    int ly = 0;
    int ry = 0;
    foreach(QGraphicsItem *port_, childItems()) {
        if (port_->type() != QNEPort::Type)
            continue;

        QNEPort *port = (QNEPort*)port_;

        if (port->portFlags() & QNEPort::NamePort)
        {
            ry += 4;
            ly += 4;
        }

        if (port->isOutput() && !port->isSpecialPort())
        {
            rightWidth = std::max(rightWidth, fm.width(port->portName()));
            ry += std::max(h, (int)port->boundingRect().height());
        }
        else if (!port->isSpecialPort())
        {
            leftWidth = std::max(leftWidth, fm.width(port->portName()));
            ly += std::max(h, (int)port->boundingRect().height());
        }
        else
        {
            titleWidth = std::max(titleWidth, fm.width(port->portName()));
            titleWidth += 32;
            ly += std::max(std::max(h, (int)port->boundingRect().height()), 30);
            ry += std::max(std::max(h, (int)port->boundingRect().height()), 30);
        }
    }

    height = std::max(ly, ry);
    width = std::max(rightWidth + leftWidth, titleWidth) + horzMargin;

    if (previewImage_)
    {
        width = std::max(width, std::min(previewImage_->width() + 6, 128+6));
        height += std::min(previewImage_->height() + 8, 128 + 8); /* 4 for margin */;
    }
    else
        height += 4;

    QPainterPath p;
    //p.addRect(-width / 2, -height / 2, width, height);
    p.addRoundedRect(-width / 2, -height / 2, width, height, 5, 5);
    setPath(p);


    ly = -height / 2 + vertMargin + 5;
    ry = -height / 2 + vertMargin + 5;
    foreach(QGraphicsItem *port_, childItems()) {
        if (port_->type() != QNEPort::Type)
            continue;

        QNEPort *port = (QNEPort*)port_;

        if (port->isOutput() && !port->isSpecialPort())
        {
            port->setPos(width / 2 + port->radius(), ry);
            ry += std::max(h, (int)port->boundingRect().height());
        }
        else if (!port->isSpecialPort())
        {
            port->setPos(-width / 2 - port->radius(), ly);
            ly += std::max(h, (int)port->boundingRect().height());
        }
        else
        {
            
            port->setPos(32 + -width / 2 - port->radius(), ly);
            
            //    port->setPos(-width / 2 - port->radius(), ly);
            ly += std::max(std::max(h, (int)port->boundingRect().height()), 30);
            ry += std::max(std::max(h, (int)port->boundingRect().height()), 30);
        }

        if (port->portFlags() & QNEPort::NamePort)
        {
            ly += 4;
            ry += 4;
        }
    }
}

QNEBlock* QNEBlock::FindBlock(QGraphicsScene* scene, void* userData)
{
    auto items = scene->items();
    for (auto item : items)
    {
        if (item && item->type() == QNEBlock::Type)
        {
            QNEBlock* block = (QNEBlock*)item;
            if (block->getUserData() == userData)
                return block;
        }
    }
    return 0x0;
}