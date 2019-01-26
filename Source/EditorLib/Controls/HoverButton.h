//?? consider adding a timed delay

#pragma once

#include <QPushButton>
#include <QTimer>

/// Specialization of QPushButton that emits a signal during mouse enter events.
/// Presently only used for Application and Document buttons in Ribbon tab headers.
class HoverButton : public QPushButton
{
    Q_OBJECT;
public:
    /// Construct.
    HoverButton(unsigned msDelay = 500, QWidget* owner = 0x0) :
        QPushButton(owner),
        timer_(this),
        delayMS_(msDelay)
    {
        timer_.setSingleShot(true);
        connect(&timer_, &QTimer::timeout, this, &HoverButton::onTimeout);
    }

signals:
    /// Fired during the enterEvent override.
    void mouseEntered();

    private slots:
    void onTimeout()
    {
        emit mouseEntered();
        setFocus();
    }

protected:
    /// Start counting down before we emit the event.
    virtual void enterEvent(QEvent* evt) Q_DECL_OVERRIDE
    {
        timer_.start(delayMS_);
        QPushButton::enterEvent(evt);
    }

    /// If we leave then stop the timer.
    virtual void leaveEvent(QEvent* evt) Q_DECL_OVERRIDE
    {
        timer_.stop();
        QPushButton::leaveEvent(evt);
    }

    QTimer timer_;
    unsigned delayMS_;
};