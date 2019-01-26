#include "SceneTreePanel.h"

#include "../Data/SprueDataSources.h"
#include "../GlobalAccess.h"
#include "../SprueKitEditor.h"

#include <QLabel>
#include <QTreeView>
#include <qtreewidget.h>
#include <QHBoxLayout>
#include <QVBoxLayout>

using namespace SprueEngine;

namespace SprueEditor
{

static const char* testStyle = "QLabel { background-color: #007070; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }";

SceneTreePanel::SceneTreePanel() : 
    ScrollAreaWidget()
{
    stackWidget_ = new DocumentViewStackedWidget();

    vBoxLayout_->setSpacing(0);
    vBoxLayout_->setMargin(0);
    vBoxLayout_->addWidget(stackWidget_);
}

SceneTreePanel::~SceneTreePanel()
{
    delete stackWidget_;
}

}