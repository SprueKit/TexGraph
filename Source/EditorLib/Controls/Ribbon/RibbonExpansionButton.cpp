#include "RibbonExpansionButton.h"

#include <QBoxLayout>

RibbonExpansionButton::RibbonExpansionButton()
{
    init();
}

RibbonExpansionButton::RibbonExpansionButton(QAction* mainAction)
{
    init();
    setDefaultAction(mainAction);
}

void RibbonExpansionButton::init()
{
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    setObjectName("ribbon_button");
    setMinimumSize(96, 96);
    setMaximumSize(96, 96);
    setIconSize(QSize(64, 64));
    setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

    expandButton_ = new QToolButton();
    expandButton_->setObjectName("ribbon_expansion_button");
    expandButton_->setMinimumSize(90, 24);
    expandButton_->setMaximumSize(90, 24);
    expandButton_->setIcon(QIcon(":/qss_icons/rc/down_arrow.png"));
    layout->addWidget(expandButton_);
}