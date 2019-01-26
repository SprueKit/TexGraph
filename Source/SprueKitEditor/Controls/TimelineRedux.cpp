#include "TimelineRedux.h"

#include "../InternalNames.h"

#include <SprueEngine/FString.h>

#include <qapplication.h>
#include <qcursor.h>
#include <qevent.h>
#include <qpainter.h>

namespace SprueEditor
{;

class TestKey : public TimelineKeyModel
{
public:
    virtual float GetKeyTime() const { return time; }
    virtual void SetKeyTime(float time) { this->time = time; }
    float time;
};

class TestTrack : public TimelineTrackModel
{
public:
    virtual std::string GetName() { return TestName; }
    std::string TestName;
};

class TestModel : public  TimelineModelBase
{
public:
    virtual unsigned TrackCount() const { return 5; }
    virtual unsigned KeyframeCount() const { return 20; }
    virtual float GetLength() const { return 15.0f; }
};

TimelineRedux::TimelineRedux(QScrollArea* linked) :
    draggingSplitter_(false),
    splitPosition_(90),
    linkedScrollArea_(linked),
    mouseButtons_(0)
{
    setObjectName(TIMELINE_CONTROL);
    SetModel(std::shared_ptr<TimelineModelBase>(new TestModel()));
    setMouseTracking(true);
}

TimelineRedux::~TimelineRedux()
{

}

bool TimelineRedux::event(QEvent* event)
{
    return QWidget::event(event);
}

void TimelineRedux::paintEvent(QPaintEvent* event)
{
    setMinimumSize(Measure());

    QPainter painter;
    painter.begin(this);

    int currentY = 0;

    //paint background
    painter.setPen(Qt::NoPen);
    painter.setBrush(TIMELINE_COLOR_BACKGROUND);
    painter.drawRect(QRect(0, 0, width(), height()));

    //paint scrubber
    DrawUpperRow(&painter);
    painter.setPen(Qt::NoPen);

    //paint track rows
    painter.setClipping(true);
    painter.setClipRect(splitPosition_, TIMELINE_ROW_HEIGHT * 2 + linkedScrollArea_->verticalScrollBar()->value(), width(), height());
    currentY = TIMELINE_ROW_HEIGHT * 2;
    for (int i = 0; i < model_->TrackCount(); ++i)
        DrawTrack(&painter, i, QPoint(splitPosition_, currentY + (i * TIMELINE_ROW_HEIGHT)), false);
    painter.setClipping(false);

    DrawScrubber(&painter);

    // Track Titles
    {
        // Fill out track header area so timeline scrolls under it
        painter.setPen(Qt::NoPen);
        QBrush bgBrush(TIMELINE_COLOR_TRACK_HEADER);
        painter.setBrush(bgBrush);
        painter.drawRect(0, 0, splitPosition_ + linkedScrollArea_->horizontalScrollBar()->value(), height());

        painter.setClipping(true);
        painter.setClipRect(linkedScrollArea_->horizontalScrollBar()->value(), TIMELINE_ROW_HEIGHT * 2 + linkedScrollArea_->verticalScrollBar()->value(), splitPosition_, height());

        // Paint track headers
        for (int i = 0; i < model_->TrackCount(); ++i)
            DrawTrackHeader(&painter, i, currentY + (i * TIMELINE_ROW_HEIGHT), "Test", 0, false, false);

        // Horizontal lines
        painter.setPen(Qt::NoPen);
        QBrush row(QColor(0, 0, 0));
        painter.setBrush(row);
        for (int i = 0; i < model_->TrackCount(); ++i)
            painter.drawRect(linkedScrollArea_->horizontalScrollBar()->value(), currentY + (i * TIMELINE_ROW_HEIGHT) + TIMELINE_ROW_HEIGHT - 1, splitPosition_, 1);

        painter.setClipping(false);

        // Draw overtop of any tracks in the top area
        //painter.setPen(Qt::NoPen);
        //painter.setBrush(bgBrush);
        //painter.drawRect(0, linkedScrollArea_->verticalScrollBar()->value(), splitPosition_ + linkedScrollArea_->horizontalScrollBar()->value(), TIMELINE_ROW_HEIGHT * 2);
    }

    // Draw horizontal line under the scrubbing timeline
    QBrush row(QColor(0, 0, 0));
    painter.setBrush(row);
    painter.drawRect(0, linkedScrollArea_->verticalScrollBar()->value() + TIMELINE_ROW_HEIGHT * 2, width(), 1);

    painter.setPen(Qt::NoPen);
    //paint splitter between left and right
    DrawSplitter(&painter);

    painter.end();
}

void TimelineRedux::mousePressEvent(QMouseEvent* evt)
{
    mouseButtons_ |= evt->buttons();
    QPoint mousePos = evt->pos();
    int splitterPos = splitPosition_;
    int testPos = mousePos.x() - linkedScrollArea_->horizontalScrollBar()->value();
    if (testPos > splitterPos - 3 && testPos < splitterPos + 3 && evt->buttons() & Qt::MouseButton::LeftButton)
    {
        draggingSplitter_ = true;
        return;
    }
    else if (mouseButtons_ & Qt::MouseButton::RightButton)
    {
        int desiredPosition = evt->pos().x() - splitPosition_;
        int maxPosition = Measure().width() - splitPosition_;
        currentTime_ = std::max(0, std::min(desiredPosition, maxPosition));
        repaint();
        return;
    }
}

void TimelineRedux::mouseReleaseEvent(QMouseEvent* evt)
{
    int newButtons = evt->buttons();
    const bool wasLeftMouse = (mouseButtons_ & ~newButtons) & Qt::MouseButton::LeftButton;
    const bool wasRightMouse = (mouseButtons_ & ~newButtons) & Qt::MouseButton::RightButton;
    mouseButtons_ &= newButtons;
    if (!draggingSplitter_ && wasLeftMouse)
    {
        int yPos = evt->y();

        // Offset Y by scrubber height and then check to make sure it falls inside the visible track area
        yPos -= (TIMELINE_ROW_HEIGHT * 2);
        if (yPos > linkedScrollArea_->verticalScrollBar()->value())
        {
            int selectedRow = yPos /= TIMELINE_ROW_HEIGHT;
            auto found = selectedRows_.find(selectedRow);
            if (found != selectedRows_.end())
                selectedRows_.erase(found);
            else if (QApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ControlModifier)
                selectedRows_.insert(selectedRow);
            else
            {
                selectedRows_.clear();
                selectedRows_.insert(selectedRow);
            }
            repaint();
        }
    }
        
    draggingSplitter_ = false;
}

void TimelineRedux::mouseMoveEvent(QMouseEvent* evt)
{
    QPoint mousePos = evt->pos();
    if (draggingSplitter_)
    {
        splitPosition_ = std::max(mousePos.x() - linkedScrollArea_->horizontalScrollBar()->value(), 90);
        repaint();
        return;
    }
    else if (mouseButtons_ & Qt::MouseButton::RightButton)
    {
        int desiredPosition = evt->pos().x() - splitPosition_;
        int maxPosition = Measure().width() - splitPosition_;
        currentTime_ = std::max(0, std::min(desiredPosition, maxPosition));
        repaint();
        return;
    }

    int splitterPos = splitPosition_ + linkedScrollArea_->horizontalScrollBar()->value();;
    if (mousePos.x() > splitterPos - 3 && mousePos.x() < splitterPos + 3)
        setCursor(Qt::CursorShape::SizeHorCursor);
    else
        setCursor(Qt::CursorShape::ArrowCursor);
}

void TimelineRedux::DrawUpperRow(QPainter* painter) const
{
    //??? what was this for? Drawing content in the track header box where aligned with the scrubber line?
}

void TimelineRedux::DrawScrubber(QPainter* painter_) const
{
    int yOffset = linkedScrollArea_->verticalScrollBar()->value();

    // Draw the background area
    painter_->setPen(Qt::NoPen);
    painter_->setBrush(TIMELINE_COLOR_BACKGROUND);
    painter_->drawRect(QRect(0, yOffset, width(), TIMELINE_ROW_HEIGHT * 2));

    // Draw the red indicator of the current time
    DrawCurrentTimeIndicator(painter_, splitPosition_);

    QBrush line(QColor(0, 0, 0));
    QBrush background(QColor(128, 128, 128));
    painter_->setPen(QPen(background, 1));
    painter_->setBrush(background);

    // Draw the text indicators for every 5th frame
    painter_->setFont(QFont("Segoe UI", 10));
    
    for (unsigned i = 0; i < model_->KeyframeCount(); i += (i == 0 ? 4 : 5))
        painter_->drawText(QPoint(splitPosition_ + (i) * TIMELINE_FRAME_WIDTH, yOffset + TIMELINE_ROW_HEIGHT  - 5), FString("%1", i + 1).c_str());

    // Draw the ticks for each frame
    painter_->setBrush(line);
    for (unsigned i = 0; i < model_->KeyframeCount(); ++i)
        painter_->drawRect(splitPosition_ + (i)* TIMELINE_FRAME_WIDTH + TIMELINE_FRAME_WIDTH / 2, yOffset + TIMELINE_ROW_HEIGHT, 1, TIMELINE_ROW_HEIGHT);

    // Draw the lines indicating the start of every 5th frame
    painter_->setPen(Qt::NoPen);
    QBrush frameBrush(QColor(20, 255, 20).darker());
    painter_->setBrush(frameBrush);
    for (unsigned i = 0; i < model_->KeyframeCount(); ++i)
    {
        if (i > 0 && (i + 2) % 5 == 0)
            painter_->drawRect(QRect(splitPosition_ + (i + 1) * TIMELINE_FRAME_WIDTH, 0, 1, TIMELINE_ROW_HEIGHT * 2 + TIMELINE_ROW_HEIGHT * model_->TrackCount()));
    }
}

void TimelineRedux::DrawSplitter(QPainter* painter_) const
{
    QBrush split(QColor(0, 0, 0));
    painter_->setBrush(split);
    painter_->drawRect(splitPosition_ + linkedScrollArea_->horizontalScrollBar()->value(), 0, 1, height());
}

void TimelineRedux::DrawTrackHeader(QPainter* painter_, int row, int startY, const char* name, int depth, bool hasChildren, bool expanded) const
{
    QBrush bgBrush(TIMELINE_COLOR_TRACK_HEADER);

    // draw the row background
    painter_->setPen(Qt::NoPen);
    const bool isSelected = selectedRows_.find(row) != selectedRows_.end();
    painter_->setBrush(isSelected ? TIMELINE_COLOR_ROW_SELECTED.darker(150) : bgBrush);

    const int x = linkedScrollArea_->horizontalScrollBar()->value();

    painter_->drawRect(QRect(x, startY, splitPosition_, TIMELINE_ROW_HEIGHT));

    painter_->setPen(QPen(TIMELINE_COLOR_TEXT));
    painter_->drawText(x + 16, startY + TIMELINE_ROW_HEIGHT - 4, name);
}

void TimelineRedux::DrawTrack(QPainter* painter_, int row, QPoint start, bool selected) const
{
    QBrush bgBrush(QColor(32, 32, 32));

    // draw the row background
    const bool isSelected = selectedRows_.find(row) != selectedRows_.end();
    painter_->setBrush(isSelected ? TIMELINE_COLOR_ROW_SELECTED : TIMELINE_COLOR_BACKGROUND);
    painter_->drawRect(QRect(start.x(), start.y(), width(), TIMELINE_ROW_HEIGHT));

    // Draw the dividers on the timeline
    QBrush frameBrush(TIMELINE_COLOR_LINE);
    QBrush keyBackBrush(TIMELINE_COLOR_ROW);

    float rowWidth = model_->KeyframeCount() * TIMELINE_FRAME_WIDTH;

    QLinearGradient grad(0.0f, start.y() + 10.0f, 0.0f, start.y() + 32.0f);
    QGradientStops stops;
    grad.setColorAt(0, isSelected ? TIMELINE_COLOR_ROW_SELECTED : TIMELINE_COLOR_ROW);
    grad.setColorAt(1, isSelected ? TIMELINE_COLOR_ROW_SELECTED.darker(150) : TIMELINE_COLOR_LINE);

    painter_->setPen(Qt::NoPen);
    painter_->setBrush(grad);
    painter_->drawRect(QRect(start.x(), start.y(), rowWidth, TIMELINE_ROW_HEIGHT));
}

void TimelineRedux::DrawKeyframe(QPainter* painter_, QPoint start, bool selected) const
{
    QPoint leftCorner(start.x(), start.y() + (TIMELINE_ROW_HEIGHT / 2));
    QPoint topCorner(start.x() + TIMELINE_FRAME_WIDTH / 2, start.y());
    QPoint rightCorner(start.x() + TIMELINE_FRAME_WIDTH, start.y() + TIMELINE_ROW_HEIGHT / 2);
    QPoint bottomCorner(start.x() + TIMELINE_FRAME_WIDTH / 2, start.y() + TIMELINE_ROW_HEIGHT);

    QPolygon poly;
    poly.append(leftCorner);
    poly.append(topCorner);
    poly.append(rightCorner);
    poly.append(bottomCorner);
    poly.append(leftCorner);

    if (selected)
    {
        QLinearGradient grad(0.0f, start.y() + 2.0f, 0.0f, start.y() + 16);
        QGradientStops stops;
        grad.setColorAt(0, TIMELINE_COLOR_TICK_SELECTED);
        grad.setColorAt(1, TIMELINE_COLOR_TICK_SELECTED.lighter(120));
        painter_->setBrush(grad);
    }
    else
        painter_->setBrush(QBrush(TIMELINE_COLOR_TICK));

    painter_->drawPolygon(poly);

    painter_->setPen(QPen(TIMELINE_COLOR_LINE, 2));
    painter_->drawPolyline(poly);
    painter_->setPen(Qt::NoPen);
}

void TimelineRedux::DrawCurrentTimeIndicator(QPainter* painter_, int startX) const
{
    int xPos = splitPosition_ + currentTime_;// +linkedScrollBar_->value();
    int height = Measure().height();

    //QPolygon caretPoly;
    //caretPoly.push_back(QPoint(xPos - 8, TIMELINE_ROW_HEIGHT)); //top left of arrow
    //caretPoly.push_back(QPoint(xPos + 8, TIMELINE_ROW_HEIGHT)); //top right of arrow
    //caretPoly.push_back(QPoint(xPos + 1, TIMELINE_ROW_HEIGHT + 8)); // top bottom right of top arrow start of line
    //
    //caretPoly.push_back(QPoint(xPos + 1, height + TIMELINE_ROW_HEIGHT - 8)); // line at bottom
    //caretPoly.push_back(QPoint(xPos + 8, height + TIMELINE_ROW_HEIGHT));
    //
    //caretPoly.push_back(QPoint(xPos - 8, height + TIMELINE_ROW_HEIGHT));
    //caretPoly.push_back(QPoint(xPos - 1, height + TIMELINE_ROW_HEIGHT - 8));
    //caretPoly.push_back(QPoint(xPos - 1, TIMELINE_ROW_HEIGHT + 8));

    painter_->setPen(QPen(QColor(90, 0, 0), 1));
    painter_->setBrush(QColor(200, 0, 0));
    //painter_->drawPolygon(caretPoly);
    painter_->drawRect(xPos - 1, 0, 3, height);
    

    //QPolygon topOuterPoly;
    //topOuterPoly.push_back(QPoint(xPos - 10, TIMELINE_ROW_HEIGHT));
    //topOuterPoly.push_back(QPoint(xPos + 10, TIMELINE_ROW_HEIGHT));
    //topOuterPoly.push_back(QPoint(xPos, TIMELINE_ROW_HEIGHT + 10));
    //
    //QPolygon topInnerPoly;
    //topInnerPoly.push_back(QPoint(xPos - 8, TIMELINE_ROW_HEIGHT + 2));
    //topInnerPoly.push_back(QPoint(xPos + 8, TIMELINE_ROW_HEIGHT + 2));
    //topInnerPoly.push_back(QPoint(xPos, TIMELINE_ROW_HEIGHT + 8));
    //
    //QPolygon bottomOuterPoly;
    //bottomOuterPoly.push_back(QPoint(xPos - 10, height + TIMELINE_ROW_HEIGHT));
    //bottomOuterPoly.push_back(QPoint(xPos, height - 10 + TIMELINE_ROW_HEIGHT));
    //bottomOuterPoly.push_back(QPoint(xPos + 10, height + TIMELINE_ROW_HEIGHT));
    //
    //QPolygon bottomPoly;
    //bottomPoly.push_back(QPoint(xPos - 8, height + TIMELINE_ROW_HEIGHT - 2));
    //bottomPoly.push_back(QPoint(xPos, height - 12 + TIMELINE_ROW_HEIGHT));
    //bottomPoly.push_back(QPoint(xPos + 8, height + TIMELINE_ROW_HEIGHT - 2));
    //
    //painter_->setPen(Qt::NoPen);
    //painter_->setBrush(QColor(50, 0, 0));
    //painter_->drawPolygon(topOuterPoly);
    ////painter_->drawPolygon(bottomOuterPoly);
    //painter_->drawRect(QRect(xPos - 1, TIMELINE_ROW_HEIGHT, 3, height));
    //
    //painter_->setBrush(QColor(255,0,0));
    //painter_->drawRect(QRect(xPos, TIMELINE_ROW_HEIGHT + 2, 1, height - 4));
    //painter_->drawPolygon(topInnerPoly);
    ////painter_->drawPolygon(bottomPoly);
}

QSize TimelineRedux::Measure() const
{
    QSize size = QSize(splitPosition_ + TIMELINE_FRAME_WIDTH * model_->KeyframeCount(), TIMELINE_ROW_HEIGHT * (model_->TrackCount() + 2)); //TODO replace with frame ct and track ct
    // Adjust our min height so that we can always at least see the timeline itself
    linkedScrollArea_->setMinimumHeight(TIMELINE_ROW_HEIGHT * 2 + (linkedScrollArea_->horizontalScrollBar()->isVisible() ? linkedScrollArea_->horizontalScrollBar()->height() : 0));

    return size;
}

QSize TimelineRedux::GetDrawOffset() const
{
    return QSize(splitPosition_ + linkedScrollArea_->horizontalScrollBar()->value(), 0);
}

}