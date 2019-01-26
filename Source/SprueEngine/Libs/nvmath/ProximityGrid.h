#pragma once
#ifndef NV_MATH_PROXIMITYGRID_H
#define NV_MATH_PROXIMITYGRID_H

//#include "ftoi.h"

#include <SprueEngine/Math/MathDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <stdint.h>
#include <vector>


// A simple, dynamic proximity grid based on Jon's code.
// Instead of storing pointers here I store indices.

namespace nv {

    class Box;

    struct Cell {
        std::vector<uint32_t> indexArray;
    };

    struct ProximityGrid {
        ProximityGrid();

        void reset();
        void init(const std::vector<SprueEngine::Vec3>& pointArray);
        void init(const SprueEngine::BoundingBox& box, uint32_t count);

        int index_x(float x) const;
        int index_y(float y) const;
        int index_z(float z) const;
        int index(int x, int y, int z) const;
        int index(const SprueEngine::Vec3 & pos) const;
        
        uint32_t mortonCount() const;
        int mortonIndex(uint32_t code) const;

        void add(const SprueEngine::Vec3 & pos, uint32_t key);
        bool remove(const SprueEngine::Vec3 & pos, uint32_t key);

        void gather(const SprueEngine::Vec3 & pos, float radius, std::vector<uint32_t> & indices);

        std::vector<Cell> cellArray;

        SprueEngine::Vec3 corner;
        SprueEngine::Vec3 invCellSize;
        int sx, sy, sz;
    };

    // For morton traversal, do:
    // for (int code = 0; code < mortonCount(); code++) {
    //   int idx = mortonIndex(code);
    //   if (idx < 0) continue;
    // }



    inline int ProximityGrid::index_x(float x) const {
        return CLAMP(std::floorf((x - corner.x) * invCellSize.x),  0, sx-1);
    }

    inline int ProximityGrid::index_y(float y) const {
        return CLAMP(std::floorf((y - corner.y) * invCellSize.y),  0, sy-1);
    }

    inline int ProximityGrid::index_z(float z) const {
        return CLAMP(std::floorf((z - corner.z) * invCellSize.z),  0, sz-1);
    }

    inline int ProximityGrid::index(int x, int y, int z) const {
        SPRUE_ASSERT(x >= 0 && x < sx, "Illegal parameter in ProximityGrid::index for X");
        SPRUE_ASSERT(y >= 0 && y < sy, "Illegal parameter in ProximityGrid::index for Y");
        SPRUE_ASSERT(z >= 0 && z < sz, "Illegal parameter in ProximityGrid::index for Z");
        int idx = (z * sy + y) * sx + x;
        SPRUE_ASSERT(idx >= 0 && idx < cellArray.size(), "ProximityGrid::index combined value invalid");
        return idx;
    }

    inline int ProximityGrid::index(const SprueEngine::Vec3 & pos) const {
        int x = index_x(pos.x);
        int y = index_y(pos.y);
        int z = index_z(pos.z);
        return index(x, y, z);
    }


    inline void ProximityGrid::add(const SprueEngine::Vec3 & pos, uint32_t key) {
        uint32_t idx = index(pos);
        cellArray[idx].indexArray.push_back(key);
    }

    inline bool ProximityGrid::remove(const SprueEngine::Vec3 & pos, uint32_t key) {
        uint32_t idx = index(pos);
        cellArray[idx].indexArray.push_back(key);
        return true;
    }

} // nv namespace

#endif // NV_MATH_PROXIMITYGRID_H
