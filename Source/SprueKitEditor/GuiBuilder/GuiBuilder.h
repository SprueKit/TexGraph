#pragma once

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>

namespace SprueEditor
{
    class SprueKitEditor;

    class GuiBuilder : public QObject
    {
        Q_OBJECT;
    public:
        virtual void Build(SprueKitEditor*) = 0;
    };

}
