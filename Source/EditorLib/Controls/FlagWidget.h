#pragma once

#include <QWidget>

#include <string>
#include <vector>

/// 16x2 collection of faux-checkboxes for each bit of a UInt
class FlagWidget : public QWidget
{
    Q_OBJECT
public:
    FlagWidget();
    virtual ~FlagWidget();

    /// Sets the number of bits to actually use
    void SetBitCount(unsigned count);
    /// Sets the value of the bits
    void SetBitField(unsigned value);
    /// Gets the value of the bits
    unsigned GetBitField() const;
    /// Set the tooltip for a particular bit
    void SetToolTip(int bit, const std::string& text);
    void SetBit(int bit, bool state);
    bool IsBitSet(int bit) const { return value_ & (1 << bit); }

    virtual bool event(QEvent *evt) override;
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;

signals:
    void BitFieldChanged();

private:
    struct Flag {
        float x;
        float y;
    };

    QPoint clickPos_;
    std::vector<Flag> flags_;
    QIcon* checkedIcon_;
    unsigned value_;
    QBrush foreGroundBrush_;
    QBrush backGroundBrush_;
    std::vector<std::string> tooltips_;
};
