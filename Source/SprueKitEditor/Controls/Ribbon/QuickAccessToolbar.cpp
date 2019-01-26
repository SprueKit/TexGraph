#include "QuickAccessToolbar.h"

#include <QAction>
#include <QBoxLayout>
#include <QToolButton>

QuickAccessToolbar::QuickAccessToolbar(const std::vector<QAction*>& actions)
{
    setLayout(new QHBoxLayout());

    for (auto action : actions)
    {
        QToolButton* button = new QToolButton(this);
        button->setDefaultAction(action);
        button->setMaximumSize(32, 32);
        layout()->addWidget(button);
    }

    setFixedWidth(32 * actions.size());
}

void QuickAccessToolbar::WindowResized(const QSize& newWindowSize)
{

}