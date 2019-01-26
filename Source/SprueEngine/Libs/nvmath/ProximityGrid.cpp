#include "ProximityGrid.h"

#include "../nvcore/Utils.h"
#include "Box.h"
#include "Box.inl"
#include "Morton.h"


using namespace nv;

ProximityGrid::ProximityGrid() {
}

void ProximityGrid::reset() {
    cellArray.clear();
}

void ProximityGrid::init(const std::vector<SprueEngine::Vec3> & pointArray) {

	// Compute bounding box.
    SprueEngine::BoundingBox box;
    box.minPoint = box.maxPoint = pointArray[0];

    const uint32_t count = pointArray.size();

    for (uint32_t i = 0; i < count; i++) {
		box.Enclose(pointArray[i].ToPos4());
	}

    init(box, count);

	// Insert all points.
	for (uint32_t i = 0; i < count; i++) {
        add(pointArray[i], i);
    }
}


void ProximityGrid::init(const SprueEngine::BoundingBox& box, uint32_t count) {
    reset();
 
    // Determine grid size.
    float cellWidth;

    SprueEngine::Vec3 diagonal = box.Diagonal();
    float volume = box.Volume();

    if (SprueEquals(volume, 0)) {
        // Degenerate box, treat like a quad.
        SprueEngine::Vec2 quad;
        if (diagonal.x < diagonal.y && diagonal.x < diagonal.z) {
            quad.x = diagonal.y;
            quad.y = diagonal.z;
        }
        else if (diagonal.y < diagonal.x && diagonal.y < diagonal.z) {
            quad.x = diagonal.x;
            quad.y = diagonal.z;
        }
        else {
            quad.x = diagonal.x;
            quad.y = diagonal.y;
        }

        float cellArea = quad.x * quad.y / count;
        cellWidth = sqrtf(cellArea); // pow(cellArea, 1.0f / 2.0f);
    }
    else {
        // Ideally we want one cell per point.
        float cellVolume = volume / count;
        cellWidth = pow(cellVolume, 1.0f / 3.0f);
    }

    //nvDebugCheck(cellWidth != 0);

    sx = std::max(1, (int)std::ceil(diagonal.x / cellWidth));
    sy = std::max(1, (int)std::ceil(diagonal.y / cellWidth));
    sz = std::max(1, (int)std::ceil(diagonal.z / cellWidth));

    invCellSize.x = float(sx) / diagonal.x;
    invCellSize.y = float(sy) / diagonal.y;
    invCellSize.z = float(sz) / diagonal.z;

	cellArray.resize(sx * sy * sz);

    corner = box.minPoint; // @@ Align grid better?
}

// Gather all points inside the given sphere.
// Radius is assumed to be small, so we don't bother culling the cells.
void ProximityGrid::gather(const SprueEngine::Vec3 & position, float radius, std::vector<uint32_t> & indexArray) {
    int x0 = index_x(position.x - radius);
    int x1 = index_x(position.x + radius);

    int y0 = index_y(position.y - radius);
    int y1 = index_y(position.y + radius);

    int z0 = index_z(position.z - radius);
    int z1 = index_z(position.z + radius);

    for (int z = z0; z <= z1; z++) {
        for (int y = y0; y <= y1; y++) {
            for (int x = x0; x <= x1; x++) {
                int idx = index(x, y, z);
                indexArray.insert(indexArray.end(), cellArray[idx].indexArray.begin(), cellArray[idx].indexArray.end());
            }
        }
    }
}


uint32_t ProximityGrid::mortonCount() const {
    uint64_t s = SprueMax(sx, SprueMax(sy, sz));
    s = nextPowerOfTwo(s);
    
    if (s > 1024) {
        return uint32_t(s * s * SprueMax(sx, SprueMax(sy, sz)));
    }

    return uint32_t(s * s * s);
}

int ProximityGrid::mortonIndex(uint32_t code) const {
    uint32_t x, y, z;

    uint32_t s = SprueMax(sx, SprueMax(sy, sz));
    if (s > 1024) {
        // Use layered two-dimensional morton order.
        s = nextPowerOfTwo(s);
        uint32_t layer = code / (s * s);
        code = code % (s * s);

        uint32_t layer_count = SprueMin(sx, SprueMin(sy, sz));
        if (sx == layer_count) {
            x = layer;
            y = decodeMorton2X(code);
            z = decodeMorton2Y(code);
        }
        else if (sy == layer_count) {
            x = decodeMorton2Y(code); 
            y = layer; 
            z = decodeMorton2X(code);
        }
        else /*if (sz == layer_count)*/ {
            x = decodeMorton2X(code);
            y = decodeMorton2Y(code);
            z = layer;
        }
    }
    else {
        x = decodeMorton3X(code);
        y = decodeMorton3Y(code);
        z = decodeMorton3Z(code);
    }

    if (x >= sx || y >= sy || z >= sz) {
        return -1;
    }

    return index(x, y, z);
}
