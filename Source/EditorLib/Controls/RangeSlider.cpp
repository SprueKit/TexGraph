#include "RangeSlider.h"

#include <QGridLayout>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

RangeSlider::RangeSlider(QWidget *parent) :
    QWidget(parent),
    valueMin_(12.0),
    valueMax_(120.0),
    cursorSize_(4.0),
    paddingSize_(4.0),
    mouseX_(0.0),
    moved_(0),
    onMin_(false),
    onMax_(false)
{
    painter_ = new QPainter();
    setCurrentMin(16);
    setCurrentMax(25);
    setMouseTracking(true);
    setMinimumHeight(20);
}

RangeSlider::~RangeSlider()
{
    delete painter_;
}

QSize RangeSlider::sizeHint() const
{
    return QSize(480, 20);
}

double RangeSlider::currentMin()
{
    return currentMin_ + valueMin_;
}

double RangeSlider::currentMax()
{
    return currentMax_ + valueMin_;
}

double RangeSlider::valueMin()
{
    return valueMax_;
}

double RangeSlider::valueMax()
{
    return valueMax_;
}

void RangeSlider::setCurrentMin(double currentMin)
{
    double newCurrentMin = (double)currentMin - valueMin_;

    if (newCurrentMin >= 0)
        currentMin_ = newCurrentMin;

    if (newCurrentMin > currentMax_)
        currentMax_ = newCurrentMin;

    update();
}

void RangeSlider::setCurrentMax(double currentMax)
{
    double newCurrentMax = (double)currentMax - valueMin_;

    if (newCurrentMax <= valueMax_)
        currentMax_ = newCurrentMax;

    if (newCurrentMax  < currentMin_)
        currentMin_ = newCurrentMax;

    update();
}

void RangeSlider::setValueMin(double valueMin)
{
    if (valueMin < valueMax_)
        valueMin_ = valueMin;

    if (currentMin_ < valueMin_)
        currentMin_ = valueMin_;

    update();
}

void RangeSlider::setValueMax(double valueMax)
{
    if (valueMax > valueMin_)
        valueMax_ = valueMax;

    if (currentMax_ > valueMax_)
        currentMax_ = valueMax_;

    update();
}

void RangeSlider::paintEvent(QPaintEvent* event)
{
    paddingSize_ = height() / 10;
    cursorSize_ = height() / 2 - paddingSize_;

    painter_->begin(this);
    painter_->setRenderHint(QPainter::TextAntialiasing);
    painter_->setBrush(Qt::SolidPattern);
    painter_->setPen(Qt::SolidLine);

    QColor color = palette().color(QPalette::Window);
    QColor colorBG = color.dark(210);
    QColor colorFG = color.dark(95);

    QRectF rectangle;
    double minX = currentMin_ / (valueMax_ - valueMin_) * ((width() - 1) - 2 * cursorSize_) + cursorSize_;
    double maxX = currentMax_ / (valueMax_ - valueMin_) * ((width() - 1) - 2 * cursorSize_) + cursorSize_;

    painter_->setBrush(QBrush(colorBG));
    rectangle.setCoords(cursorSize_, 0, width() - 1 - cursorSize_, height() - 1);
    painter_->drawRect(rectangle);

    // Draw selection
    painter_->setBrush(QBrush(color.lighter()));
    rectangle.setCoords(minX, 0, maxX, height() - 1);
    painter_->drawRect(rectangle);

    // Draw cursors
    if (moved_ != 2)
    {
        drawCursor(maxX, 2);
        drawCursor(minX, 1);
    }
    else
    {
        drawCursor(minX, 1);
        drawCursor(maxX, 2);
    }

    painter_->end();
}

void RangeSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
        mouseX_ = (float)(event->x());
    moved_ = 0;
    update();
}

void RangeSlider::mouseReleaseEvent(QMouseEvent *event)
{
    moved_ = 0;
    mouseMoveEvent(event);
    update();
}

void RangeSlider::mouseMoveEvent(QMouseEvent *event)
{
    // Set mouse cursor and handle mouse movement for slide
    if (onMax_ || onMin_)
        setCursor(Qt::SizeHorCursor);
    else
        setCursor(Qt::ArrowCursor);

    float mouseX = (float)(event->x());
    float minX = currentMin_ / (valueMax_ - valueMin_) * ((width() - 1) - 2 * cursorSize_) + cursorSize_;
    float maxX = currentMax_ / (valueMax_ - valueMin_) * ((width() - 1) - 2 * cursorSize_) + cursorSize_;

    onMin_ = ((mouseX > (minX - cursorSize_ * 1.1)) && (mouseX < (minX + cursorSize_ * 1.1))) && !onMax_ || (moved_ == 1);
    onMax_ = ((mouseX >(maxX - cursorSize_ * 1.1)) && (mouseX < (maxX + cursorSize_ * 1.1))) && !onMin_ || (moved_ == 2);

    if (event->buttons() & Qt::LeftButton)
    {
        if ((moved_ != 2) && onMin_)
        {
            minX = mouseX * width() / ((width() - 1) - 2 * cursorSize_) - cursorSize_;
            if (minX < 0)
                minX = 0;
            if (minX > width())
                minX = width();
            currentMin_ = (minX * (valueMax_ - valueMin_) / (width() - 1));
            if (currentMax_ < currentMin_)
                currentMax_ = currentMin_;
            moved_ = 1;
            emit minChanged(currentMin());
        }
        else if ((moved_ != 1) && onMax_)
        {
            maxX = mouseX * width() /
                ((width() - 1) - 2 * cursorSize_) - cursorSize_;
            if (maxX < 0)	maxX = 0;
            if (maxX > width()) maxX = width();
            currentMax_ = (maxX * (valueMax_ - valueMin_) / (width() - 1));
            if (currentMin_ > currentMax_)
                currentMin_ = currentMax_;
            moved_ = 2;
            emit maxChanged(currentMax());
        }
        update();
    }
    else
        moved_ = 0;

    mouseX_ = mouseX;
}

void RangeSlider::drawCursor(double pos, int id)
{
    if ((id != 1) && (id != 2))
        return;

    QColor color = palette().color(QPalette::Window).dark(135);
    QColor colorLight = palette().color(QPalette::Window).lighter(200);

    if (id == 1)
    {
        color = QColor(200, 0, 0);
    }
    else
    {
        color = QColor(0, 200, 0);
    }

    QRectF rectangle;

    painter_->setBrush(Qt::SolidPattern);
    painter_->setPen(Qt::NoPen);

    QLinearGradient grad(0.0f, 0.4f, 0.0f, 16.0f);
    grad.setColorAt(0, colorLight);
    grad.setColorAt(1, color);
    painter_->setBrush(QBrush(grad));
    rectangle.setCoords(
        pos - cursorSize_ / 2, paddingSize_,
        pos + cursorSize_ / 2, height() - paddingSize_ - 1);
    painter_->drawRoundedRect(rectangle, 3, 3);

    painter_->setBrush(Qt::NoBrush);
    painter_->setPen(Qt::SolidLine);

    rectangle.setCoords(
        pos - cursorSize_ / 2, paddingSize_,
        pos + cursorSize_ / 2, height() - paddingSize_ - 1);
    painter_->drawRoundedRect(rectangle, 3, 3);
}

void RangeSlider::printVals(int /*val*/)
{
    qDebug() << "Slice [" << qRound(currentMin_ + valueMin_)
        << "-" << qRound(currentMax_ + valueMin_) << "] \t"
        << "Total [" << qRound(valueMin_)
        << "-" << qRound(valueMax_) << "]";
}