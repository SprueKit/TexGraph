#include "RibbonButton.h"

#include <QMenu>

RibbonButton::RibbonButton()
{
    setFocusPolicy(Qt::FocusPolicy::NoFocus);
    setObjectName("ribbon_button");
    InitLarge();
    setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
}

RibbonButton::RibbonButton(QAction* action)
{
    setFocusPolicy(Qt::FocusPolicy::NoFocus);
    setObjectName("ribbon_button");
    InitLarge();
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
}

void RibbonButton::InitLarge()
{
    buttonType_ = 0;
    MakeLarge();
}

void RibbonButton::InitMedium()
{
    buttonType_ = 1;
    MakeMedium();
}

void RibbonButton::InitSmall(bool wide)
{
    buttonType_ = 2;
    isWide_ = wide;
    MakeSmall(wide);
}

void RibbonButton::MakeLarge()
{
    setMinimumSize(96, 96);
    setMaximumSize(96, 96);
    setIconSize(QSize(64, 64));
    setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
}

void RibbonButton::MakeMedium(bool wide)
{
    setMinimumSize(48, 48);
    if (wide)
        setMaximumSize(5000, 48);
    else
        setMaximumSize(48, 48);
    setIconSize(QSize(32, 32));
    isWide_ = wide;
    if (wide)
        setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
    else
        setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
}

void RibbonButton::MakeSmall(bool wide)
{
    setIconSize(QSize(16, 16));
    setMinimumSize(32, 32);
    setMaximumSize(5000, 32);
    isWide_ = wide;
    if (wide)
        setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
    else
        setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
}

void RibbonButton::MakeRegular()
{
    setIconSize(QSize(32, 32));
    setMinimumSize(48, 48);
    setMaximumSize(5000, 48);
    isWide_ = true;
    setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
}

QSize RibbonButton::GetSizeForType(int btnType) const
{
    switch (btnType)
    {
    case 0:
        return QSize(96, 96);
    case 1:
        return QSize(64, 64);
    case 2:
    {
        if (isWide_)
            return QSize(width(), 32);
        return QSize(32, 32);
    }
    }
    return QSize(64, 64);
}