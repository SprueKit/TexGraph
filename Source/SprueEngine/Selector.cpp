#include "Selector.h"

#include <SprueEngine/Core/Bone.h>
#include <SprueEngine/Core/SprueModel.h>
#include <SprueEngine/Core/SceneObject.h>
#include <SprueEngine/Math/MathDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <algorithm>

namespace SprueEngine
{
    bool Selector::IsSelectorValid(const SprueModel* model, const SceneObject* forObject, const Selector* parentSelector) const
    {
        // Check part flags, do not do heavyweight bounds compoutations if we can fail

        // Does it meet the capabilities?
        if ((PartCaps & forObject->GetCapabilityBits()) == 0)
            return false;
        
        // Does it pass flags?
        if ((PartFlags & forObject->GetUserBits()) == 0)
            return false;
        
        // Now do our length bounds check
        //TODO: cache the bounding box for the model?
        //TODO: cache the bounds for region identification?
        BoundingBox bounds;
        if (Inheritance & (INHERIT_ParentLocal | INHERIT_ParentLocalSmall) && parentSelector)
        {
            //TODO get bounds
        }
        else
            bounds = model->GetWorldBounds().MinimalEnclosingAABB();


        // Perform regional checks
        const unsigned identifiedRegions = ClassifyRegion(forObject, bounds);

        // Do we meet ALL required regions
        //if (RequireRegions && (identifiedRegions ^ (~RequireRegions)) != RequireRegions) //XOR with inverse of Required, must equal required
        if (RequireRegions && (identifiedRegions & RequireRegions) != RequireRegions) // MUST CONTAIN ALL required regions
            return false;

        // Do we fail any exclusion rules?
        if (ExcludeRegions && (~ExcludeRegions) ^ identifiedRegions)
            return false;
        
        // Are we at least in an allowed in any of these regions? (any hit is good)
        if (AllowedRegions && (AllowedRegions & identifiedRegions) == 0)
            return false;
        
        return true;
    }

    RegionSelector Selector::ClassifyRegion(const SceneObject* object, const BoundingBox& bounds)
    {
        const Vec3 boundsExtent = bounds.Size();
        const BoundingBox objectBnds = object->GetWorldBounds().MinimalEnclosingAABB();

        BoundingBox leftRightHalves[2];
        bounds.Subdivide(leftRightHalves, 2, 1, 1);
        BoundingBox frontBackHalves[2];
        bounds.Subdivide(frontBackHalves, 1, 1, 2);
        BoundingBox topBottomHalves[2];
        bounds.Subdivide(topBottomHalves, 1, 2, 1);

        BoundingBox centerX = bounds.Shrink(Vec3(boundsExtent.x / 2, 0.0f, 0.0f));
        BoundingBox centerY = bounds.Shrink(Vec3(0.0f, boundsExtent.y / 2, 0.0f));
        BoundingBox centerZ = bounds.Shrink(Vec3(0.0f, 0.0f, boundsExtent.z / 2));

        RegionSelector metCriteria = 0;

        if (leftRightHalves[0].Intersects(objectBnds))
            metCriteria |= RegionSelection::REGS_Left;
        if (leftRightHalves[1].Intersects(objectBnds))
            metCriteria |= RegionSelection::REGS_Right;
        if (frontBackHalves[0].Intersects(objectBnds))
            metCriteria |= RegionSelection::REGS_Back;
        if (frontBackHalves[1].Intersects(objectBnds))
            metCriteria |= RegionSelection::REGS_Front;
        if (topBottomHalves[0].Intersects(objectBnds))
            metCriteria |= RegionSelection::REGS_Bottom;
        if (topBottomHalves[1].Intersects(objectBnds))
            metCriteria |= RegionSelection::REGS_Top;
        
        if (centerX.Intersects(objectBnds))
            metCriteria |= RegionSelection::REGS_CenterX;
        if (centerY.Intersects(objectBnds))
            metCriteria |= RegionSelection::REGS_CenterY;
        if (centerZ.Intersects(objectBnds))
            metCriteria |= RegionSelection::REGS_CenterZ;

        return metCriteria;
    }


    int CompareLeftToRight(SceneObject* lhs, SceneObject* rhs)
    {
        if (lhs->GetWorldPosition().x < rhs->GetWorldPosition().x)
            return -1;
        if (lhs->GetWorldPosition().x > rhs->GetWorldPosition().x)
            return 1;
        return 0;
    }

    int CompareFrontToBack(SceneObject* lhs, SceneObject* rhs)
    {
        if (lhs->GetWorldPosition().z > rhs->GetWorldPosition().z)
            return -1;
        if (lhs->GetWorldPosition().z < rhs->GetWorldPosition().z)
            return 1;
        return 0;
    }

    int CompareBottomToTop(SceneObject* lhs, SceneObject* rhs)
    {
        if (lhs->GetWorldPosition().y < rhs->GetWorldPosition().y)
            return -1;
        if (lhs->GetWorldPosition().y > rhs->GetWorldPosition().y)
            return 1;
        return 0;
    }

    void Selector::Sort(unsigned selectionMask, std::vector<SceneObject*>& objects)
    {
        if (selectionMask & (MORPH_LeftMost | MORPH_RightMost))
            std::sort(objects.begin(), objects.end(), CompareLeftToRight);
        else if (selectionMask & (MORPH_NearestFront | MORPH_FurthestBack))
            std::sort(objects.begin(), objects.end(), CompareFrontToBack);
        else if (selectionMask & (MORPH_BottomMost | MORPH_TopMost))
            std::sort(objects.begin(), objects.end(), CompareBottomToTop);
    }
}