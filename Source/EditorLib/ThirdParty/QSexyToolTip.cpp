/*

Copyright (c) 2012, STANISLAW ADASZEWSKI
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
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "qsexytooltip.h"
#include "../Styling.h"

#include <QMainWindow>
#include <QLayout>
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>
#include <QTimer>
#include <QAbstractButton>
#include <QGraphicsEffect>
#include <QPropertyAnimation>
#include <qevent.h>

#ifdef WIN32

#endif

QSexyToolTip::QSexyToolTip(QObject *parent)
{
    if (parent)
    {
        setParent((QWidget*)parent);
        connect(parent, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    }

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    checkActiveWindowTimer = new QTimer(this);
    connect(checkActiveWindowTimer, SIGNAL(timeout()), this, SLOT(checkActiveWindow()));

    cornerRounding = 10;
    markerWidth = 16;
    markerHeight = 16;
    preferredDirection = PreferBottom;
    bgColor = STYLE_WIDGET_BG;// Qt::white;
    frameColor = QColor("#76797C");// Qt::darkGray;
    frameSize = 1;
    centerInWidget = 0;
    margin = 6;
}

void QSexyToolTip::setCornerRounding(int r)
{
    cornerRounding = r;
}

void QSexyToolTip::setMarkerWidth(int w)
{
    markerWidth = w;
}

void QSexyToolTip::setMarkerHeight(int h)
{
    markerHeight = h;
}

void QSexyToolTip::setPreferredDirection(int d)
{
    preferredDirection = d;
}

void QSexyToolTip::setBgColor(const QColor &c)
{
    bgColor = c;
}

void QSexyToolTip::setFrameColor(const QColor &c)
{
    frameColor = c;
}

void QSexyToolTip::setBgPixmap(const QPixmap &pm)
{
    bgPixmap = pm;
}

void QSexyToolTip::setFrameSize(int s)
{
    frameSize = s;
}

void QSexyToolTip::attach(QAbstractButton *btn)
{
    centerInWidget = btn;
    attachedTo = btn;
    connect(btn, SIGNAL(clicked()), this, SLOT(popup()));
}

void QSexyToolTip::setCenterInWidget(QWidget *w)
{
    centerInWidget = w;
}

void QSexyToolTip::setMargin(int m)
{
    margin = m;
}

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

void QSexyToolTip::popup(const QPoint &pt)
{
    emit preshow();

    QDesktopWidget *d = QApplication::desktop();

    QRect r(d->availableGeometry(pt));

    QRect geom;

    int w = width();
    int h = height();

    int actualDirection = preferredDirection;
    bool retried = false;

    /* setGeometry(r);
    if (layout())
    layout()->update(); */

    //show();

retry:

    switch (actualDirection)
    {
    case PreferBottom:
        if (layout())
        {
            layout()->setContentsMargins(frameSize + margin, markerHeight + margin, frameSize + margin, frameSize + margin);
            layout()->update();
            w = layout()->minimumSize().width() + markerHeight + frameSize * 2 + margin * 2;
            h = layout()->minimumSize().height() + markerHeight + frameSize * 2 + margin * 2;
        }

        geom = QRect(pt.x() - w / 2, pt.y(), w, h);
        marker = QPolygon(QVector<QPoint>() << QPoint(0, 0) << QPoint(markerWidth, markerHeight) << QPoint(-markerWidth, markerHeight));

        if (geom.left() < r.left())
        {
            marker[0] += QPoint(MAX(-r.left() + geom.left(), -geom.width() / 2), 0);
            QPoint maxShift(MAX(-r.left() + geom.left(), -geom.width() / 2 + cornerRounding + markerWidth), 0);
            marker[1] += maxShift;
            marker[2] += maxShift;
            geom.translate(r.left() - geom.left(), 0);
        }
        else if (geom.right() >= r.right())
        {
            marker[0] += QPoint(MIN(-r.right() + geom.right(), geom.width() / 2), 0);
            QPoint maxShift(MIN(-r.right() + geom.right(), geom.width() / 2 - cornerRounding - markerWidth), 0);
            marker[1] += maxShift;
            marker[2] += maxShift;
            geom.translate(r.right() - geom.right(), 0);
        }
        if (geom.top() < r.top())
            geom.translate(0, r.top() - geom.top());
        if (geom.bottom() >= r.bottom() && !retried)
        {
            retried = true;
            actualDirection = PreferTop;
            goto retry;
        }

        marker.translate(geom.width() / 2, 1);
        clientArea = QRect(0, markerHeight - frameSize, geom.width(), geom.height() - markerHeight);
        break;
    case PreferTop:
        if (layout())
        {
            layout()->setContentsMargins(frameSize + margin, frameSize + margin, frameSize + margin, markerHeight + margin);
            layout()->update();
            w = layout()->minimumSize().width() + markerHeight + frameSize * 2 + margin * 2;
            h = layout()->minimumSize().height() + markerHeight + frameSize * 2 + margin * 2;
        }

        geom = QRect(pt.x() - w / 2, pt.y() - h, w, h);
        marker = QPolygon(QVector<QPoint>() << QPoint(0, 0) << QPoint(markerWidth, -markerHeight) << QPoint(-markerWidth, -markerHeight));

        if (geom.left() < r.left())
        {
            marker[0] += QPoint(MAX(-r.left() + geom.left(), -geom.width() / 2), 0);
            QPoint maxShift(MAX(-r.left() + geom.left(), -geom.width() / 2 + cornerRounding + markerWidth), 0);
            marker[1] += maxShift;
            marker[2] += maxShift;
            geom.translate(r.left() - geom.left(), 0);
        }
        else if (geom.right() >= r.right())
        {
            marker[0] += QPoint(MIN(-r.right() + geom.right(), geom.width() / 2), 0);
            QPoint maxShift(MIN(-r.right() + geom.right(), geom.width() / 2 - cornerRounding - markerWidth), 0);
            marker[1] += maxShift;
            marker[2] += maxShift;
            geom.translate(r.right() - geom.right(), 0);
        }
        if (geom.bottom() > r.bottom())
            geom.translate(0, r.bottom() - geom.bottom());
        if (geom.top() < r.top() && !retried)
        {
            retried = true;
            actualDirection = PreferBottom;
            goto retry;
        }

        marker.translate(geom.width() / 2, geom.height() - 1);
        clientArea = QRect(0, 0, geom.width(), geom.height() - markerHeight + frameSize);
        break;
    case PreferRight:
        if (layout())
        {
            layout()->setContentsMargins(markerHeight + margin, frameSize + margin, frameSize + margin, frameSize + margin);
            layout()->update();
            w = layout()->minimumSize().width() + markerHeight + frameSize * 2 + margin * 2;
            h = layout()->minimumSize().height() + markerHeight + frameSize * 2 + margin * 2;
        }

        geom = QRect(pt.x(), pt.y() - h / 2, w, h);
        marker = QPolygon(QVector<QPoint>() << QPoint(0, 0) << QPoint(markerHeight, markerWidth) << QPoint(markerHeight, -markerWidth));

        if (geom.top() < r.top())
        {
            marker[0] += QPoint(0, MAX(-r.top() + geom.top(), -geom.height() / 2));
            QPoint maxShift(0, MAX(-r.top() + geom.top(), -geom.height() / 2 + cornerRounding + markerWidth));
            marker[1] += maxShift;
            marker[2] += maxShift;
            geom.translate(0, r.top() - geom.top());
        }
        else if (geom.bottom() >= r.bottom())
        {
            marker[0] += QPoint(0, MIN(-r.bottom() + geom.bottom(), geom.height() / 2));
            QPoint maxShift(0, MIN(-r.bottom() + geom.bottom(), geom.height() / 2 - cornerRounding - markerWidth));
            marker[1] += maxShift;
            marker[2] += maxShift;
            geom.translate(0, r.bottom() - geom.bottom());
        }
        if (geom.left() < r.left())
            geom.translate(r.left() - geom.left(), 0);
        else if (geom.right() >= r.right() && !retried)
        {
            retried = true;
            actualDirection = PreferLeft;
            goto retry;
        }

        marker.translate(1, geom.height() / 2);
        clientArea = QRect(markerHeight - frameSize, 0, geom.width() - markerHeight, geom.height());
        break;
    case PreferLeft:
        if (layout())
        {
            layout()->setContentsMargins(frameSize + margin, frameSize + margin, markerHeight + margin, frameSize + margin);
            layout()->update();
            w = layout()->minimumSize().width() + markerHeight + frameSize * 2 + margin * 2;
            h = layout()->minimumSize().height() + markerHeight + frameSize * 2 + margin * 2;
            /* setGeometry(0, 0, w, h);
            layout()->update();
            w = layout()->minimumSize().width() + markerHeight + cornerRounding * 2 + frameSize * 2;
            h = layout()->minimumSize().height() + markerHeight + cornerRounding * 2 + frameSize * 2; */
        }

        geom = QRect(pt.x() - w, pt.y() - h / 2, w, h);
        // geom = QRect(pt.x(), pt.y() - h/2, w, h);
        marker = QPolygon(QVector<QPoint>() << QPoint(0, 0) << QPoint(-markerHeight, markerWidth) << QPoint(-markerHeight, -markerWidth));

        if (geom.top() < r.top())
        {
            marker[0] += QPoint(0, MAX(-r.top() + geom.top(), -geom.height() / 2));
            QPoint maxShift(0, MAX(-r.top() + geom.top(), -geom.height() / 2 + cornerRounding + markerWidth));
            marker[1] += maxShift;
            marker[2] += maxShift;
            geom.translate(0, r.top() - geom.top());
        }
        else if (geom.bottom() >= r.bottom())
        {
            marker[0] += QPoint(0, MIN(-r.bottom() + geom.bottom(), geom.height() / 2));
            QPoint maxShift(0, MIN(-r.bottom() + geom.bottom(), geom.height() / 2 - cornerRounding - markerWidth));
            marker[1] += maxShift;
            marker[2] += maxShift;
            geom.translate(0, r.bottom() - geom.bottom());
        }
        if (geom.right() > r.right())
            geom.translate(r.right() - geom.right(), 0);
        else if (geom.left() < r.left() && !retried)
        {
            retried = true;
            actualDirection = PreferRight;
            goto retry;
        }

        marker.translate(geom.width() - 1, geom.height() / 2);
        clientArea = QRect(0, 0, geom.width() - markerHeight + frameSize, geom.height());
        break;
    }


    setGeometry(geom);

    QPainterPath pp1;
    pp1.addRoundedRect(clientArea.adjusted(frameSize, frameSize, -frameSize, -frameSize), cornerRounding, cornerRounding);
    QPainterPath pp2;
    pp2.addPolygon(marker);
    contour = pp1.united(pp2);

#if defined(Q_WS_X11)
    if (!QX11Info::isCompositingManagerRunning())
    {
        QPolygon mask = contour.toFillPolygon().toPolygon();
        for (int i = 0; i < mask.size(); i++)
        {
            if (mask[i].x() > mask.boundingRect().center().x())
                mask[i].setX(mask[i].x() + 1);
            if (mask[i].y() > mask.boundingRect().center().y())
                mask[i].setY(mask[i].y() + 1);
            /* if (mask[i].x() < mask.boundingRect().center().x())
            mask[i].setX(mask[i].x() -  1); */
            /* if (mask[i].y() < mask.boundingRect().center().y())
            mask[i].setY(mask[i].y() - 1); */
        }
        setMask(mask);
    }
#endif

    /* QRegion c(0, 0, cornerRounding * 2, cornerRounding * 2, QRegion::Ellipse);
    QRegion tlc = QRegion(0, 0, cornerRounding, cornerRounding).subtracted(c).translated(clientArea.left() - 1, clientArea.top() - 1); // top left corner
    QRegion trc = QRegion(cornerRounding, 0, cornerRounding, cornerRounding).subtracted(c).translated(clientArea.right() - cornerRounding * 2 + 1, clientArea.top() - 1); // top right corner
    QRegion blc = QRegion(0, cornerRounding, cornerRounding, cornerRounding).subtracted(c).translated(clientArea.left() - 1, clientArea.bottom() - cornerRounding * 2 + 1); // bottom left corner
    QRegion brc = QRegion(cornerRounding, cornerRounding, cornerRounding, cornerRounding).subtracted(c).translated(clientArea.right() - cornerRounding * 2 + 1, clientArea.bottom() - cornerRounding * 2 + 1); // bottom right corner

    #if defined(Q_WS_X11)
    if (!QX11Info::isCompositingManagerRunning())
    setMask(QRegion(clientArea).subtracted(tlc+trc+blc+brc) + marker);
    #endif */

    if (layout())
    {
        // layout()->setContentsMargins(clientArea.left() + cornerRounding, clientArea.top() + cornerRounding, geom.width() - clientArea.right() + cornerRounding, geom.height() - clientArea.bottom() + cornerRounding);
        //layout()->update();
    }

    /* show();

    if (!retried)
    {
    retried = true;
    goto retry;
    } */

    show();
    activateWindow();

    checkActiveWindowTimer->start(100);

    emit shown();
}

void QSexyToolTip::checkActiveWindow() {
    // First two checks are the major checks
    // The centerInWidget makes sure the window behaves as expected when clicking the button to hide it
    //      otherwise mouse down on the button gets rid of the window, resulting in mouse up showing it again == flickering
    if (!isActiveWindow() && autoHide) {
        if (centerInWidget)
        {
            if (centerInWidget->hasFocus())
                return;
        }
        if (attachedTo && attachedTo->isDown())
            suppressNextShow = true;
        checkActiveWindowTimer->stop();
        hide();
        setGraphicsEffect(0x0);
        emit hidden();
    }
}

void QSexyToolTip::popup()
{
    if (suppressNextShow)
    {
        suppressNextShow = false;
        return;
    }
    if (!visible)
    {
        if (contentWidget_ && !contentWidget_->isVisible())
            contentWidget_->setVisible(true);
        if (centerInWidget)
        {
            popup(centerInWidget->parentWidget()->mapToGlobal(centerInWidget->geometry().center()));
            if (QAbstractButton* btn = dynamic_cast<QAbstractButton*>(centerInWidget))
                btn->setChecked(true);
        }
        else
            popup(QCursor::pos());
        visible = true;
    }
    else
    {
        hide();
        visible = false;

    }
}

void QSexyToolTip::reposition()
{
    if (visible)
    {
        if (centerInWidget)
            popup(centerInWidget->parentWidget()->mapToGlobal(centerInWidget->geometry().center()));
        else
            popup(QCursor::pos());
    }
}

void QSexyToolTip::setVisible(bool visible)
{
    this->visible = visible;
    QWidget::setVisible(visible);
    if (centerInWidget)
    {
        if (QAbstractButton* btn = dynamic_cast<QAbstractButton*>(centerInWidget))
            btn->setChecked(false);
    }
    checkActiveWindowTimer->stop();
    // emit hidden();
}

void QSexyToolTip::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    int fs = frameSize;

#if defined (Q_WS_X11)
    if (!QX11Info::isCompositingManagerRunning())
    {
        fs += 2;
        p.eraseRect(rect());
    }
    else
        p.setRenderHint(QPainter::Antialiasing, true);
#else
    p.setRenderHint(QPainter::Antialiasing, true);
#endif

    if (bgPixmap.isNull())
        p.setBrush(bgColor);
    else
    {
        p.setBrush(Qt::NoBrush);
        p.setClipPath(contour);
        p.drawPixmap(rect(), bgPixmap);
        p.setClipping(false);
    }

    p.setPen(QPen(frameColor, fs));
    // p.drawRoundedRect(clientArea, cornerRounding, cornerRounding);
    p.drawPath(contour);

    /* if (bgPixmap.isNull())
    {
    p.setPen(Qt::NoPen);
    p.drawPolygon(marker.translated(-1, 0));
    p.drawPolygon(marker.translated(1, 0));
    p.drawPolygon(marker.translated(0, -1));
    p.drawPolygon(marker.translated(0, 1));
    } else {
    p.setClipRegion(marker.boundingRect().adjusted(-1, -1, 1, 1));
    p.drawPixmap(rect(), bgPixmap);
    p.setClipping(false);
    }
    p.setPen(QPen(frameColor, frameSize + 1));
    p.drawLine(marker[0], marker[1]);
    p.drawLine(marker[0], marker[2]); */
}

void QSexyToolTip::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    reposition();
}

void QSexyToolTip::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        hide();
    QWidget::keyPressEvent(event);
}

void QSexyToolTip::showEvent(QShowEvent* event)
{
    if (contentWidget_)
        contentWidget_->setVisible(true);
    QWidget::showEvent(event);
}

void QSexyToolTip::hideEvent(QHideEvent* event)
{
    if (contentWidget_)
        contentWidget_->setVisible(false);
    QWidget::hideEvent(event);
}

void QSexyToolTip::setContentWidget(QWidget* widget) { 
    contentWidget_ = widget;
}