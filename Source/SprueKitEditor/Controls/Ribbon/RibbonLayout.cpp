#include "RibbonLayout.h"

#include <EditorLib/Controls/Ribbon/RibbonButton.h>

void RibbonLayout::setGeometry(const QRect& rect)
{
    int width = rect.width();

    // First calculate are desired width, if greater than are available width then compress into small
    int totalWidth = 0;
    for (int i = 0; i < count(); ++i)
    {
        if (QLayoutItem* item = itemAt(i))
        {
            if (RibbonButton* btn = dynamic_cast<RibbonButton*>(item->widget()))
                totalWidth += btn->width();
        }
    }


    if (naturalType_ == RNT_Large)
    {
        if (totalWidth > width)
        {
            for (int i = 0; i < count(); ++i)
            {
                if (QLayoutItem* item = itemAt(i))
                {
                    if (RibbonButton* btn = dynamic_cast<RibbonButton*>(item->widget()))
                        btn->MakeSmall();
                }
            }
            setDirection(QBoxLayout::TopToBottom);
        }
        else
        {
            for (int i = 0; i < count(); ++i)
            {
                if (QLayoutItem* item = itemAt(i))
                {
                    if (RibbonButton* btn = dynamic_cast<RibbonButton*>(item->widget()))
                        btn->MakeLarge();
                }
            }
            setDirection(QBoxLayout::LeftToRight);
        }
    }
    else if (naturalType_ == RNT_Medium)
    {
        
    }
    else
        QBoxLayout::setGeometry(rect);
}