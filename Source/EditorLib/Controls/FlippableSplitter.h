#pragma once

#include <EditorLib/Controls/IPersistedWidget.h>

#include <QSplitter>

/// QSplitter that whose orientation can be changed in a context menu
class FlippableSplitter : public QSplitter, public IPersistedWidget
{
public:
    FlippableSplitter() : QSplitter()
    {

    }

protected:
    virtual void contextMenuEvent(QContextMenuEvent*) Q_DECL_OVERRIDE;
};