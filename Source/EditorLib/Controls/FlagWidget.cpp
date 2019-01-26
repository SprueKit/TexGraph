#include "FlagWidget.h"

#include <qicon.h>
#include <qpainter.h>
#include <qevent.h>
#include <qtooltip.h>

#define FLAG_DIM 12

FlagWidget::FlagWidget() :
    QWidget()
{
    setToolTipDuration(1);
    setMinimumSize(FLAG_DIM * 16, FLAG_DIM * 2);
    checkedIcon_ = new QIcon(":/Images/checkbox.png");

    foreGroundBrush_ = QBrush("#b1b1b1");
    backGroundBrush_ = QBrush("#302F2F");
}

FlagWidget::~FlagWidget()
{
    delete checkedIcon_;
}

void FlagWidget::SetBitCount(unsigned count)
{
    flags_.resize(count);
    tooltips_.resize(count);
    repaint();
}

void FlagWidget::SetBitField(unsigned value)
{
    if (value_ != value)
    {
        value_ = value;
        emit BitFieldChanged();
    }
    else
        value_ = value;
    this->repaint();
}

void FlagWidget::SetBit(int bit, bool state)
{
    if (state)
        value_ = value_ | (1 << bit);
    else
        value_ = value_ & ~(1 << bit);
    emit BitFieldChanged();
    repaint();
}

unsigned FlagWidget::GetBitField() const
{
    return value_;
}

void FlagWidget::SetToolTip(int bit, const std::string& text)
{
    if (bit < tooltips_.size())
        tooltips_[bit] = text;
}

bool FlagWidget::event(QEvent* event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent* data = static_cast<QHelpEvent*>(event);
        QPoint newPos = data->pos();
        for (unsigned i = 0; i < flags_.size(); ++i)
        {
            Flag& flag = flags_[i];
            if (newPos.x() > flag.x && newPos.x() < flag.x + FLAG_DIM)
            {
                if (newPos.y() > flag.y && newPos.y() < flag.y + FLAG_DIM)
                {
                    if (tooltips_[i].empty())
                        break;
                    QToolTip::showText(data->globalPos(), tooltips_[i].c_str());
                    break;
                }
            }
        }
    }
    return QWidget::event(event);
}

void FlagWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() & Qt::LeftButton)
        clickPos_ = event->pos();
}

void FlagWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() & Qt::LeftButton)
    {
        QPoint newPos = event->pos();
        if ((newPos - clickPos_).manhattanLength() < 10)
        {
            for (unsigned i = 0; i < flags_.size(); ++i)
            {
                Flag& flag = flags_[i];
                if (newPos.x() > flag.x && newPos.x() < flag.x + FLAG_DIM)
                {
                    if (newPos.y() > flag.y && newPos.y() < flag.y + FLAG_DIM)
                    {
                        unsigned bit = 1 << i;
                        if (!(bit & value_))
                            value_ |= bit;
                        else
                            value_ &= ~bit;
                        emit BitFieldChanged();
                        repaint();
                        break;
                    }
                }
            }
        }
    }
}

void FlagWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter;
    painter.begin(this);

    int maxX = 0;
    int curX = 0;
    int y = 0;

    painter.setBrush(backGroundBrush_);
    painter.setPen(QPen(QColor("#b1b1b1"), 0.5f));

    for (int i = 0; i < 2; ++i)
    {
        curX = 0;
        for (int bit = 0; bit < 16; ++bit)
        {
            // Add some spacing to seperate the bits into 2 halves
            // Segmenting like this make it easier to visually remember bit layouts
            if (bit == 8)
                curX += FLAG_DIM;

            unsigned bitOffset = 16 * i + bit;
            if (bitOffset > flags_.size())
                break;

            painter.drawRect(curX, y, FLAG_DIM, FLAG_DIM);
            if ((1 << bitOffset) & value_)
                checkedIcon_->paint(&painter, curX, y, FLAG_DIM, FLAG_DIM);

            flags_[bitOffset].x = curX;
            flags_[bitOffset].y = y;

            curX += FLAG_DIM + 2;
        }
        maxX = fmaxf(maxX, curX);
        y += FLAG_DIM + 2;
    }

    setMinimumSize(maxX, y);
    painter.end();
}