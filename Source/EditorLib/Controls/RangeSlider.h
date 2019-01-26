#pragma once

#include <QWidget>

class QMouseEvent;
class QPainter;

class RangeSlider : public QWidget
{
    Q_OBJECT
public:
    RangeSlider(QWidget *parent = 0x0);
    virtual ~RangeSlider();

    QSize sizeHint() const;

    /// Active range
    double currentMin();
    double currentMax();
    /// Allowed range (NOT WHAT YOU THINK IT IS!)
    double valueMin();
    double valueMax();

    void setValueMin(double valueMin);
    void setValueMax(double valueMax);

    public slots:
    void setCurrentMin(double currentMin);
    void setCurrentMax(double currentMax);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void drawCursor(double pos, int id);

    protected slots:
    void printVals(int = 0);

signals:
    void minChanged(int);
    void maxChanged(int);

private:
    double		currentMin_;
    double		currentMax_;
    double		valueMin_;
    double		valueMax_;
    double		cursorSize_;
    double		paddingSize_;
    double 		mouseX_;
    int 		moved_;
    bool 		onMin_;
    bool 		onMax_;
    QPainter	*painter_;
};