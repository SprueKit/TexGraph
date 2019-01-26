#pragma once

#include <QBoxLayout>

/// Layout for ribbon sections, handles the reduction of buttons.
/// Compresses RibbonButtons based on their weights to cram them together.
/// Insertion order is assumed to be appearance priority.
/// Any widgets that will not fit in the layout are moved into the RibbonSection's expansion menu.
class RibbonLayout : public QBoxLayout
{
public:

    enum NaturalType
    {
        RNT_Large,
        RNT_Medium,
        RNT_Small,
    };

    virtual void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;

    NaturalType GetNaturalType() const { return naturalType_; }
    void SetNaturalType(NaturalType type) { naturalType_ = type; invalidate(); }

private:
    NaturalType naturalType_ = RNT_Large;
};