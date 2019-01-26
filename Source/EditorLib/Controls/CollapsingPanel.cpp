#include "CollapsingPanel.h"

#include <EditorLib/QtHelpers.h>

#include <QStyle>

CollapsingPanel::CollapsingPanel(const QString& title) :
    titleText_(title)
{
    setTitle(title);
    this->setProperty("collapser", QVariant(true));
    style()->unpolish(this);
    style()->polish(const_cast<CollapsingPanel*>(this));

    connect(this, &QGroupBox::toggled, [=](bool state) {
        if (state)
            QtHelpers::ShowLayout(layout());
        else
            QtHelpers::HideLayout(layout());
    });
}

void CollapsingPanel::SetDirty(bool state)
{
    setProperty("dirty", QVariant(state));
    style()->unpolish(this);
    style()->polish(this);
}