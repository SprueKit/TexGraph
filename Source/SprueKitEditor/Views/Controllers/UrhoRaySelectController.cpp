#include "UrhoRaySelectController.h"

#include "../../Data/UrhoDataSources.h"

#include "../InputRecorder.h"
#include "../RenderWidget.h"
#include "../../SprueKitEditor.h"
#include "../../UrhoHelpers.h"
#include "../ViewBase.h"
#include "../ViewManager.h"

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>

using namespace Urho3D;

namespace SprueEditor
{
    typedef std::vector< RayQueryResult > UrhoSelectionList;

    bool UrhoRaySelectController::CheckActivation(const ViewController* currentlyActive)
    {
        if (currentlyActive)
            return false;
        return input_->WasMousePressed(0);
    }

    bool UrhoRaySelectController::CheckTermination()
    {
        // terminate immediately after one update
        return true;
    }

    void UrhoRaySelectController::Update()
    {
        if (scene_)
        {
            IntVector2 vec = view_->GetContext()->GetSubsystem<Input>()->GetMousePosition();
            const float pickX = vec.x_;
            const float pickY = vec.y_;

            Urho3D::Camera* camera = view_->GetCamera()->GetComponent<Camera>();
            float px = pickX / (float)view_->GetContext()->GetSubsystem<Graphics>()->GetWidth();
            float py = pickY / (float)view_->GetContext()->GetSubsystem<Graphics>()->GetHeight();
            Urho3D::Ray ray = camera->GetScreenRay(px, py);

            PODVector<RayQueryResult> objects;
            RayOctreeQuery query(objects, ray);
            scene_->GetComponent<Octree>()->Raycast(query);

            UrhoSelectionList selected;
            for (auto item : objects)
                selected.push_back(item);
           
            auto current = viewManager_->GetRenderWidget()->GetSelectron()->GetSelected<UrhoEditor::SerializableDataSource>();
            UrhoSelectionList::iterator newSelection = selected.end();
            if (selected.size() > 1)
            {
                const Vector3 camPos = view_->GetCamera()->GetWorldPosition();
                std::sort(selected.begin(), selected.end(), [](const RayQueryResult& lhs, const RayQueryResult& rhs) {
                    return lhs.distance_ < rhs.distance_;
                });

                auto foundCurrent = std::find_if(selected.begin(), selected.end(), [=](const RayQueryResult& rec) { return current && rec.node_ == current->GetObject(); });
                if (foundCurrent == selected.end() - 1)
                    newSelection = selected.begin();
                else if (foundCurrent != selected.end())
                    newSelection = foundCurrent + 1;
                else
                    newSelection = selected.begin();
            }
            else if (selected.size() == 1) // Special case for a single item list
            {
                if (!current || current->GetObject() != selected[0].node_)
                    newSelection = selected.begin();
            }

            // did we manage to hit someone?
            if (newSelection != selected.end())
                viewManager_->GetRenderWidget()->GetSelectron()->SetSelected(0x0, std::shared_ptr<DataSource>(new UrhoEditor::NodeDataSource(newSelection->node_)));
            else
                viewManager_->GetRenderWidget()->GetSelectron()->SetSelected(0x0, 0x0);
        }
    }

    Urho3D::Node* UrhoRaySelectController::GetRaycast() const
    {
        if (scene_)
        {
            IntVector2 vec = view_->GetContext()->GetSubsystem<Input>()->GetMousePosition();
            const float pickX = vec.x_;
            const float pickY = vec.y_;

            Urho3D::Camera* camera = view_->GetCamera()->GetComponent<Camera>();
            float px = pickX / (float)view_->GetContext()->GetSubsystem<Graphics>()->GetWidth();
            float py = pickY / (float)view_->GetContext()->GetSubsystem<Graphics>()->GetHeight();
            Urho3D::Ray ray = camera->GetScreenRay(px, py);

            PODVector<RayQueryResult> objects;
            RayOctreeQuery query(objects, ray);
            scene_->GetComponent<Octree>()->RaycastSingle(query);

            if (!objects.Empty())
                return objects[0].node_;
        }
        return 0x0;
    }

}