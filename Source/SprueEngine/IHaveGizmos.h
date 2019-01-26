#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>
#include <SprueEngine/Variant.h>

#include <memory>
#include <vector>

namespace SprueEngine
{

/// Insert a control point after the current gizmo
#define SPRUE_GIZ_CMD_INSERT_AFTER 1
/// Insert a control point before the current gizmo
#define SPRUE_GIZ_CMD_INSERT_BEFORE 2
/// Remove the current control point
#define SPRUE_GIZ_CMD_REMOVE 3
/// Insert a control point at the closest valid point to a mouse click (thresholds to be determined by command handler)
#define SPRUE_GIZ_CMD_INSERT_AT_POINT 4

enum GizmoMode
{    
    GIZ_None = 0,
    GIZ_Translate = 1,  // ALL gizmos are assumed to be translatable
    GIZ_Rotate = (1 << 1),
    GIZ_Scale = (1 << 2),
    GIZ_InitialControlPoint = (1 << 3),
    GIZ_DeformedControlPoint = (1 << 4),
    GIZ_Snapper = (1 << 5),     // Gizmo is designed to snap back to the old position if not acccepted
    GIZ_DisableX = (1 << 6),    // X axis is disabled
    GIZ_DisableY = (1 << 7),    // Y axis is disabled
    GIZ_DisableZ = (1 << 8),    // Z axis is disabled
};

class IDebugRender;
class IEditable;

struct SPRUE Gizmo
{
    Mat3x4 transform_;
    IEditable* source_;
    unsigned short flags_;
    unsigned gizmoId_ = 0; 

    Gizmo(IEditable* src, unsigned short gizmoFlags) :
        source_(src),
        flags_(gizmoFlags)
    {
    }

    bool CanTranslate() const { return true; }
    bool CanRotate() const { return flags_ & GIZ_Rotate; }
    bool CanScale() const { return flags_ & GIZ_Scale; }
    bool IsControlPoint() const { return flags_ & (GIZ_InitialControlPoint | GIZ_DeformedControlPoint); }
    bool IsInitialControlPoint() const { return flags_ & GIZ_InitialControlPoint; }
    bool IsDeformedControlPoint() const { return flags_ & GIZ_DeformedControlPoint; }
    bool IsSnapping() const { return flags_ & GIZ_Snapper; }

    /// Most gizmos don't have commands, gizmos involved in strips/meshes will
    virtual void GetCommands(std::vector<unsigned>& commands) { }
    /// Execute a command ID, gizmo commands should always be simple and assume that manual edits will follow. Return true if successful, false if not.
    virtual bool ExecuteCommand(unsigned commandID, const Variant& commandParam) { return false; }

    /// Perform a transform update and notify if it was approved or not.
    virtual bool UpdateValue() = 0;
    /// Will be called on ALL available gizmos whenever a gizmo is has it's value updated.
    virtual void RefreshValue() = 0;
    /// Some gizmos may want to draw additional debug drawing functionality beyond the usual.
    virtual void DrawDebug(IDebugRender* renderer) { }

    std::vector< std::shared_ptr<Gizmo> > dependentGizmos_;

    virtual bool Equals(Gizmo* rhs) const = 0;
};

/// When transforming something we ask it for all of it's supported gizmos, this will include the main transform gizmo
/// as well as any others for control points
class SPRUE IHaveGizmos
{
public:
    virtual std::vector<std::shared_ptr<Gizmo> > GetGizmos() = 0;
    /// Sets the transform for a gizmo with the given ID. Used for control points in undo/redo systems.
    virtual void SetGizmo(unsigned gizmoID, const SprueEngine::Mat3x4& transform) { }
};

}