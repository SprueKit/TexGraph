#include "ScrollAreaWidget.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScroller>

ScrollAreaWidget::ScrollAreaWidget()
{
    setMinimumWidth(320);
    setWidgetResizable(true);

    QScroller::grabGesture(this, QScroller::TouchGesture);

    hiddenWidget_ = new QWidget();
    setWidget(hiddenWidget_);

    vBoxLayout_ = new QVBoxLayout();
    hiddenWidget_->setLayout(vBoxLayout_);
}

ScrollAreaWidget::~ScrollAreaWidget()
{
}

QHBoxLayout* ScrollAreaWidget::AddHBoxLayout()
{
    QHBoxLayout* hBoxLayout = new QHBoxLayout();
    vBoxLayout_->addLayout(hBoxLayout);

    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    return hBoxLayout;
}