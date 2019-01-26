#include "RaySelectController.h"

#include "../../Data/SprueDataSources.h"

#include "../../Documents/Sprue/SprueDocument.h"

#include "../InputRecorder.h"
#include "../RenderWidget.h"
#include "../../GlobalAccess.h"
#include "../../UrhoHelpers.h"
#include "../ViewBase.h"
#include "../ViewManager.h"

#include <EditorLib/DocumentManager.h>

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>

#include <QApplication>

using namespace Urho3D;

namespace SprueEditor
{

typedef std::pair<SprueEngine::SceneObject*, SprueEngine::IntersectionInfo> SelectionRecord;
typedef std::vector< SelectionRecord > SelectionList;

bool RaySelectController::CheckActivation(const ViewController* currentlyActive)
{
    if (currentlyActive)
        return false;
    return input_->WasMousePressed(0);
}

bool RaySelectController::CheckTermination()
{
    // terminate immediately after one update
    return true;
}

void RaySelectController::Update()
{
    const bool isShiftDown = input_->IsControlDown();

    if (SprueDocument* doc = Global_DocumentManager()->GetActiveDoc<SprueDocument>())
    {
        IntVector2 vec = view_->GetContext()->GetSubsystem<Input>()->GetMousePosition();
        const float pickX = vec.x_;
        const float pickY = vec.y_;

        Urho3D::Camera* camera = view_->GetCamera()->GetComponent<Camera>();
        float px = pickX / (float)view_->GetContext()->GetSubsystem<Graphics>()->GetWidth();
        float py = pickY / (float)view_->GetContext()->GetSubsystem<Graphics>()->GetHeight();
        Urho3D::Ray ray = camera->GetScreenRay(px, py);
        SprueEngine::Ray sprueRay(ToSprueVec(ray.origin_).ToPos4(), ToSprueVec(ray.direction_).ToDir4());

        std::vector<SprueEngine::SceneObject*> objects = doc->GetModel()->GetFlatList();
        SelectionList selected;

        for (auto item : objects)
        {
            item->ComputeBounds();
            SprueEngine::IntersectionInfo intersect;
            if (item->TestRayAccurate(sprueRay, &intersect))
                selected.push_back(std::make_pair(item, intersect));
        }

        auto current = viewManager_->GetRenderWidget()->GetSelectron()->GetSelected<SceneObjectDataSource>();
        SelectionList::iterator newSelection = selected.end();
        
        if (isShiftDown && selected.size())
        {
            //TODO
            for (unsigned i = 0; i < selected.size(); ++i)
            {
                if (viewManager_->GetRenderWidget()->GetSelectron()->AddSelected(0x0, std::shared_ptr<DataSource>(new SceneObjectDataSource(selected[i].first))))
                    return;
            }
            return;
        }

        if (selected.size() > 1)
        {
            const SprueEngine::Vec3 camPos = ToSprueVec(view_->GetCamera()->GetWorldPosition());
            std::sort(selected.begin(), selected.end(), [camPos](const SelectionRecord& lhs, const SelectionRecord& rhs) {
                float lhsDist = (lhs.second.hit - camPos).LengthSq();
                float rhsDist = (rhs.second.hit - camPos).LengthSq();
                return lhsDist < rhsDist;
            });

            auto foundCurrent = std::find_if(selected.begin(), selected.end(), [=](const SelectionRecord& rec) { return current && rec.first == current->GetSceneObject(); });
            if (foundCurrent == selected.end() - 1)
                newSelection = selected.begin();
            else if (foundCurrent != selected.end())
            {
                newSelection = foundCurrent + 1;
            }
            else
                newSelection = selected.begin();
        }
        else if (selected.size() == 1) // Special case for a single item list
        {
            if (!current || current->GetSceneObject() != selected[0].first)
                newSelection = selected.begin();
        }

        // did we manage to hit someone?
        if (newSelection != selected.end())
        {
            viewManager_->GetRenderWidget()->GetSelectron()->SetSelected(0x0, std::shared_ptr<DataSource>(new SceneObjectDataSource(newSelection->first)));
        }
        else
        {
            viewManager_->GetRenderWidget()->GetSelectron()->SetSelected(0x0, 0x0);
        }
    }
}

SprueEngine::IEditable* RaySelectController::GetRaycast() const
{
    if (SprueDocument* doc = Global_DocumentManager()->GetActiveDoc<SprueDocument>())
    {
        IntVector2 vec = view_->GetContext()->GetSubsystem<Input>()->GetMousePosition();
        const float pickX = vec.x_;
        const float pickY = vec.y_;

        Urho3D::Camera* camera = view_->GetCamera()->GetComponent<Camera>();
        float px = pickX / (float)view_->GetContext()->GetSubsystem<Graphics>()->GetWidth();
        float py = pickY / (float)view_->GetContext()->GetSubsystem<Graphics>()->GetHeight();
        Urho3D::Ray ray = camera->GetScreenRay(px, py);
        SprueEngine::Ray sprueRay(ToSprueVec(ray.origin_).ToPos4(), ToSprueVec(ray.direction_).ToDir4());

        std::vector<SprueEngine::SceneObject*> objects = doc->GetModel()->GetFlatList();
        SelectionList selected;

        for (auto item : objects)
        {
            item->ComputeBounds();
            SprueEngine::IntersectionInfo intersect;
            if (item->TestRayAccurate(sprueRay, &intersect))
                selected.push_back(std::make_pair(item, intersect));
        }

        if (selected.size() > 0)
            return selected[0].first;
    }
    return 0x0;
}

std::pair<Urho3D::Vector3, Urho3D::Vector3> RaySelectController::GetPickRay() const
{
    std::pair<Urho3D::Vector3, Urho3D::Vector3> ret;

    if (SprueDocument* doc = Global_DocumentManager()->GetActiveDoc<SprueDocument>())
    {
        IntVector2 vec = view_->GetContext()->GetSubsystem<Input>()->GetMousePosition();
        const float pickX = vec.x_;
        const float pickY = vec.y_;

        Urho3D::Camera* camera = view_->GetCamera()->GetComponent<Camera>();
        float px = pickX / (float)view_->GetContext()->GetSubsystem<Graphics>()->GetWidth();
        float py = pickY / (float)view_->GetContext()->GetSubsystem<Graphics>()->GetHeight();
        Urho3D::Ray ray = camera->GetScreenRay(px, py);
        ret.first = ray.origin_;
        ret.second = ray.direction_;
    }

    return ret;
}

}