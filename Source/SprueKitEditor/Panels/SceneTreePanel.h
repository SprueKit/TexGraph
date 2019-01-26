#pragma once

#include <EditorLib/Controls/Document/DocumentViewStackedWidget.h>
#include <EditorLib/Controls/ISignificantControl.h>
#include <EditorLib/ScrollAreaWidget.h>
#include <EditorLib/Selectron.h>

#include <qabstractitemmodel.h>

namespace SprueEditor
{

class SceneTreePanel : public ScrollAreaWidget, public ISignificantControl
{
    Q_OBJECT
public:
    SceneTreePanel();
    virtual ~SceneTreePanel();

    DocumentViewStackedWidget* GetStack() { return stackWidget_; }

private:
    DocumentViewStackedWidget* stackWidget_;
};

}