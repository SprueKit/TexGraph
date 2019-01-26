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

#ifndef QSEXYTOOLTIP_H
#define QSEXYTOOLTIP_H

#include <QWidget>

class QAbstractButton;

class QSexyToolTip : public QWidget
{
    Q_OBJECT
public:
    enum {
        PreferLeft,
        PreferRight,
        PreferTop,
        PreferBottom
    };

public:
    QSexyToolTip(QObject *parent = 0);

    void setCornerRounding(int);
    void setMarkerWidth(int);
    void setMarkerHeight(int);
    void setPreferredDirection(int);
    void setBgColor(const QColor&);
    void setFrameColor(const QColor&);
    void setBgPixmap(const QPixmap&);
    void setFrameSize(int);
    void attach(QAbstractButton *btn);
    void setCenterInWidget(QWidget*);
    void setMargin(int);
    void setAutoHide(bool state) { autoHide = state; }
    // JRS
    void setContentWidget(QWidget* widget);

public slots:
    void popup(const QPoint &pt);
    void popup();
    void setVisible(bool);
    void reposition();

signals:
    void preshow();
    void shown();
    void hidden();

private slots:
    void checkActiveWindow();

protected:
    virtual void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent*) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent*) Q_DECL_OVERRIDE;
    virtual void hideEvent(QHideEvent*) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent*);

private:
    QWidget* contentWidget_ = 0x0;
    QTimer *checkActiveWindowTimer;
    int cornerRounding;
    int markerWidth;
    int markerHeight;
    int preferredDirection;
    QRect clientArea;
    QPolygon marker;
    QColor bgColor;
    QColor frameColor;
    QPixmap bgPixmap;
    int frameSize;
    QWidget *centerInWidget;
    QPainterPath contour;
    QAbstractButton* attachedTo = 0x0;
    int margin;
    bool autoHide = true;
    bool visible = false;
    bool suppressNextShow = false;
};

#endif // QSEXYTOOLTIP_H
