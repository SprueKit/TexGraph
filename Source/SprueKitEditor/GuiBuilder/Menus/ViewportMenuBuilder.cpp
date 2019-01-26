#include "ViewportMenuBuilder.h"

#include "../../SprueKitEditor.h"

#include "../../InternalNames.h"
#include "../../Views/RenderWidget.h"
#include "../../Views/ViewManager.h"
#include "../../Views/ViewBase.h"
#include "../../GlobalAccess.h"

#include <EditorLib/Commands/RegisteredAction.h>

#include <QAction>
#include <QIcon>
#include <qkeysequence.h>

namespace SprueEditor
{

    void ViewportMenuBuilder::Build(SprueKitEditor* editor)
    {
        RegisteredAction* topAction = new RegisteredAction("Viewport", "Top");
        RegisteredAction* frontAction = new RegisteredAction("Viewport", "Front");
        RegisteredAction* leftAction = new RegisteredAction("Viewport", "Left");
        RegisteredAction* rightAction = new RegisteredAction("Viewport", "Right");
        RegisteredAction* backAction = new RegisteredAction("Viewport", "Back");
        RegisteredAction* bottomAction = new RegisteredAction("Viewport", "Bottom");

        RegisteredAction* toggleOrthoAction = new RegisteredAction("Viewport", "Toggle Ortho");
        toggleOrthoAction->setCheckable(true);

        auto menu = editor->GetOrCreateMenu("VIEWPORT_MENU", "Viewport");
        menu->addAction(topAction);
        menu->addAction(frontAction);
        menu->addAction(backAction);
        menu->addAction(leftAction);
        menu->addAction(rightAction);
        menu->addAction(bottomAction);
        menu->addAction(toggleOrthoAction);

#define BIND_VIEW_ACTION(ACTION) connect(ACTION, &QAction::triggered, [](bool toggled) { \
            if (auto activeView = Global_RenderWidget()->GetViewManager()->GetActiveView()) { \
                if (auto scene = activeView->GetScene()) { \
                    if (auto view = activeView->GetViewport()) { \
                        if (auto camera = view->GetCamera()) {
                           
#define END_VIEW_ACTION() } \
                    } \
                } \
            } \
        });

        BIND_VIEW_ACTION(topAction)
            float dist = camera->GetNode()->GetWorldPosition().Length();
            camera->GetNode()->SetWorldPosition(Urho3D::Vector3(0, dist, 0));
            camera->GetNode()->SetWorldDirection(Urho3D::Vector3::DOWN);
        END_VIEW_ACTION();

        BIND_VIEW_ACTION(bottomAction)
            float dist = camera->GetNode()->GetWorldPosition().Length();
            camera->GetNode()->SetWorldPosition(Urho3D::Vector3(0, -dist, 0));
            camera->GetNode()->SetWorldDirection(Urho3D::Vector3::UP);
        END_VIEW_ACTION();

        BIND_VIEW_ACTION(leftAction)
            float dist = camera->GetNode()->GetWorldPosition().Length();
            camera->GetNode()->SetWorldPosition(Urho3D::Vector3(-dist, 0, 0));
            camera->GetNode()->SetWorldDirection(Urho3D::Vector3::RIGHT);
        END_VIEW_ACTION();

        BIND_VIEW_ACTION(rightAction)
            float dist = camera->GetNode()->GetWorldPosition().Length();
            camera->GetNode()->SetWorldPosition(Urho3D::Vector3(dist, 0, 0));
            camera->GetNode()->SetWorldDirection(Urho3D::Vector3::LEFT);
        END_VIEW_ACTION();

        BIND_VIEW_ACTION(frontAction)
            float dist = camera->GetNode()->GetWorldPosition().Length();
            camera->GetNode()->SetWorldPosition(Urho3D::Vector3(0, 0, dist));
            camera->GetNode()->SetWorldDirection(Urho3D::Vector3::BACK);
        END_VIEW_ACTION();

        BIND_VIEW_ACTION(backAction)
            float dist = camera->GetNode()->GetWorldPosition().Length();
            camera->GetNode()->SetWorldPosition(Urho3D::Vector3(0, 0, -dist));
            camera->GetNode()->SetWorldDirection(Urho3D::Vector3::FORWARD);
        END_VIEW_ACTION();

        BIND_VIEW_ACTION(toggleOrthoAction)
            camera->SetOrthographic(toggled);
        END_VIEW_ACTION();
    }

}