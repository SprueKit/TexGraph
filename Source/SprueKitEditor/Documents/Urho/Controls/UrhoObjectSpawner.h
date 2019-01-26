#pragma once

#include <EditorLib/Controls/RangeSlider.h>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QWidget>

namespace UrhoEditor
{

    /// Common control object spawning settings. Used in dialogs/flyouts for configurating spawning.
    /// Properties are recorded into "Settings" (with the exception of the "object" list.
    ///     Constant Local Offset = XYZ random range of displacement
    ///     Random Rotation = XYZ euler range of rotation (30 = -30 to 30)
    ///     Random Scale Range = min and max range for scale
    ///     Radius = the radius from the hit point that will be considered for object placement
    ///     Count = how many objects to spawn per attempt
    ///     Align With Hit Normal = object rotation will point so Y axis aligns with the hit point normal
    ///     Align to AABB Bottom = instead of the local 0,0,0 the AABB bottom center will be used for surface alignment
    ///     Parent to Selection = spawned objects will be created as children of the currently selected item (first Urho3D object), otherwise they'll be parented to the scene
    ///     Allow Overlap = if true then the AABBs of spawned objects may not overlap
    ///     Object list = list of Urho3D scene/prefabs to spawn
    ///     Inject Variables = table of variables to inject into the prefabs, "SYSTEM_BRUSH_CLASS" is used for "Allow Overlap" detection
    class UrhoObjectSpawner : public QWidget
    {
    public:
        UrhoObjectSpawner();
        virtual ~UrhoObjectSpawner();

    private:
        QDoubleSpinBox* offsetXSpinner_;
        QDoubleSpinBox* offsetYSpinner_;
        QDoubleSpinBox* offsetZSpinner_;
        QDoubleSpinBox* rotationXSpinner_;
        QDoubleSpinBox* rotationYSpinner_;
        QDoubleSpinBox* rotationZSpinner_;
        QDoubleSpinBox* minScaleSpinner_;
        QDoubleSpinBox* maxScaleSpinner_;
        RangeSlider* minMaxScaleSlider_;
        QDoubleSpinBox* radiusSpinner_;
        QSpinBox* countSpinner_;
        QCheckBox* alignWithHitNormal_;
        QCheckBox* alignToAABBBottom_;
        QCheckBox* parentToSelection_;

        QTableWidget* objectsTable_;
        QTableWidget* variableInjectionTable_;
    };

}