#pragma once

#include "../Data/TimelineModelBase.h"

#include <EditorLib/Controls/ISignificantControl.h>

#include <QWidget>
#include <QScrollBar>
#include <QScrollArea>

#include <set>
#include <memory>

namespace SprueEditor
{

#define TIMELINE_ROW_HEIGHT 28
#define TIMELINE_FRAME_WIDTH 16

#define TIMELINE_COLOR_TRACK_HEADER QColor(0x31, 0x36, 0x3b)
#define TIMELINE_COLOR_BACKGROUND QColor(0x23, 0x26, 0x29)
#define TIMELINE_COLOR_LINE QColor(0x76, 0x79, 0x7C)
#define TIMELINE_COLOR_ROW QColor(0x31, 0x36, 0x3b)
#define TIMELINE_COLOR_ROW_SELECTED QColor(0x3d, 0xae, 0xe9)
#define TIMELINE_COLOR_TICK QColor(235, 235, 235)
#define TIMELINE_COLOR_TICK_SELECTED QColor(255, 193, 57)
#define TIMELINE_COLOR_ENTRY QColor(190, 190, 0)
#define TIMELINE_COLOR_BORDER QColor(0x76, 0x79, 0x7C)
#define TIMELINE_COLOR_TEXT QColor(0xef, 0xf0, 0xf1)

/// Contains the tracks, keyframes, and scrubber for an animation timeline
/// Header content (current animation, play/stop/loop) controls belong elsewhere
class TimelineRedux : public QWidget, public ISignificantControl
{
    Q_OBJECT
public:
    TimelineRedux(QScrollArea* linked);
    virtual ~TimelineRedux();

    void SetModel(std::shared_ptr<TimelineModelBase> model) { model_ = model; update(); repaint(); }

signals:
    void keyframeSelected();
    void keyframeMoved();
    void keyframeInserted();
    void keyframeDeleted();
    void trackSelected();
    void trackMoved();
    void trackDeleted();

protected:
    bool event(QEvent* event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* evt) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* evt) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* evt) Q_DECL_OVERRIDE;

private:
/// Core shell
    /// Draw the upper row
    void DrawUpperRow(QPainter* painter) const;
    /// Draw the timeline scrubber
    void DrawScrubber(QPainter* painter) const;
    /// Draw the divider that seperates headers form the frame timeline
    void DrawSplitter(QPainter* painter) const;
/// Track view
    /// Draw the header for a timeline tracks (tracks may be hierarchical, depth indicates indent)
    void DrawTrackHeader(QPainter* painter, int row, int startY, const char* name, int depth, bool hasChildren, bool expanded) const;
/// Actual timeline frames
    /// Draw the keyframe track at the right side
    void DrawTrack(QPainter* painter, int row, QPoint start, bool selected) const;
    /// Draw an individual keyframe tick
    void DrawKeyframe(QPainter* painter, QPoint start, bool selected) const;
    /// Draw the indicator for the current time in the timeline
    void DrawCurrentTimeIndicator(QPainter* painter, int startX) const;
    /// Calculate how much space this control should use
    QSize Measure() const;
    QSize GetDrawOffset() const;

    /// Position of the splitter between the two sides
    int splitPosition_; 
    /// Current zoom level of the timeline (only adjusts horizontal scaling, like blender)
    float zoomFraction_;

    /// Should be a horizontal scrollbar
    QScrollArea* linkedScrollArea_;

    std::shared_ptr<TimelineModelBase> model_;

private: // mouse state
    bool draggingSplitter_;
    float currentTime_;
    std::set<unsigned> selectedRows_;
    int mouseButtons_;
};

}