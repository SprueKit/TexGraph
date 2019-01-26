#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <map>

namespace SprueEngine
{
    class SceneObject;
    class SprueModel;

    // Regions are used for selecting things in bounding box base
    // Divide a cube into 8 octants
    // Centers are a box-tube 3d cross
    /* ___________
      /|         /|
     /_|________/ |
    |  |        | |
    |  |________|_|
    | /         | /
    |/__________|/
    */
    enum RegionSelection
    {
        REGS_Any = 0,
        REGS_Front = 1,                 // Positive Z
        REGS_Back = 1 << 1,             // Negative Z
        REGS_Top = 1 << 2,              // Positive Y
        REGS_Bottom = 1 << 3,           // Negative Y
        REGS_Left = 1 << 4,             // Negative X
        REGS_Right = 1 << 5,            // Positive Z
        REGS_CenterX = 1 << 6,          // Box tube running through the X axis
        REGS_CenterY = 1 << 7,          // Box tube running through the Y axis
        REGS_CenterZ = 1 << 8,          // Box tube running through the Z axis
    };
    typedef unsigned short RegionSelector;

    /// Flip left right selectors (saggital plane)
    inline unsigned char MirrorRegionSelection(unsigned short regionSelection)
    {
        if (regionSelection & REGS_Left)
            return (regionSelection & ~REGS_Left) | REGS_Right;
        else if (regionSelection & REGS_Right)
            return (regionSelection & ~REGS_Right) | REGS_Left;
        return regionSelection;
    }

    enum MorphologySelection
    {
        // Placement selectors, only one of these may be used
        MORPH_Any = 0,
        MORPH_NearestFront = 1,             // Checked after all parts have been selected, use sort method below
        MORPH_FurthestBack = 1 << 1,        // Checked after all parts have been selected, use sort method below
        MORPH_RightMost = 1 << 2,           // Checked after all parts have been selected, use sort method below
        MORPH_LeftMost = 1 << 3,            // Checked after all parts have been selected, use sort method below
        MORPH_TopMost = 1 << 4,             // Checked after all parts have been selected, use sort method below
        MORPH_BottomMost = 1 << 5,          // Checked after all parts have been selected, use sort method below
        MORPH_LTRMiddleMost = 1 << 6,       // Use LeftMost sort and pick from middle
        MORPH_FTBMiddleMost = 1 << 7,       // Use NearestFront sort and pick from middle

        // Structure Selectors, perform more intensive analysis
        MORPH_MostInterior = 1 << 8,        // A limb whose end is closet to it's root
        MORPH_MostExterior = 1 << 9,        // A limb whose end is furtherest away
        MORPH_Reachable = 1 << 10,          // Must be possible to form a contact between this and parent selector

        // Special selectors
        MORPH_IsSymmetric = 1 << 11,        // Only considers objects that were generated with (or from) symmetry
        MORPH_AboveSpine = 1 << 12,         // Body must be located above it's nearest spine joint
        MORPH_BelowSpine = 1 << 13,         // Body must be located below it's nearest spine joint
        MORPH_CoronalToSpine = 1 << 14,     // Body must be roughly horizontally level with it's nearest spine joint
    };
    typedef unsigned short MorphSelector;

    enum PermutationSelection
    {
        // Permutation selectors resolve how to deal with cases where after MorphologySelection there are still many bodies that still fulfill a selector
        PERMH_None = 0,         
        PERMH_Random = 1,               // Selects one body from random
        PERMH_Even = 1 << 1,            // Selects every even body from the results of a query (value tested is vector::size + 1), produces multiple results
        PERMH_Odd = 1 << 2,             // Selects every odd body from the results of a query (value tested is vector::size + 1), produces multiple results
    };
    typedef unsigned char PermutationSelector;

    enum InheritanceSelection
    {
        INHERIT_None = 0,                   // No inheritance check
        INHERIT_ParentLocal = 1,            // Requires a parent selector (that is the center), bounds is refitted onto that parent
        INHERIT_ParentLocalSmall = 1 << 1,  // Like above, but uses half the usual bounding box size
        INHERIT_SelectToRoot = 1 << 2,      // Requires a parent selector (that is the end), very simple selector (visits to last joint in our 'immediate' part hierarchy)
        INHERIT_SelectToSpine = 1 << 3,     // Requires a parent selector (that is the end), very simple selector (visits to the first spinal joint)
        INHERIT_ExcludeExisting = 1 << 4,   // Child selectors will not ignore objects that have already been selected
        INHERIT_DelegateToSpine = 1 << 5,   // Instead of this body the connected spine body will be where the animation is delegated to
    };
    typedef unsigned char InheritanceSelector;

    struct SPRUE Selector
    {
        MorphSelector MorphologySelection; // What morphological traits should be used?
        unsigned PartFlags;      // What flags must this part have?
        unsigned PartCaps;       // What capabilities must this part have?

        RegionSelector RequireRegions;   // Regions that are mandatory, MUST be in ALL of them
        RegionSelector AllowedRegions;   // Regions that are acceptable, MUST be in at least ONE of them
        RegionSelector ExcludeRegions;   // Regions that must be excluded, MUST NOT be in ANY of them
        InheritanceSelector Inheritance; // Flags for inheriting selections

        bool IsSelectorValid(const SprueModel* model, const SceneObject* forObject, const Selector* parentSelector = 0x0) const;

        /// Returns the regional classes applicable to the given object for the bounds.
        static RegionSelector ClassifyRegion(const SceneObject* object, const BoundingBox& bounds);

        /// Sort the vector according to the selection mask's extremity desires (ordered as left -> right or front -> back)
        static void Sort(unsigned selectionMask, std::vector<SceneObject*>& objects);
    };
}