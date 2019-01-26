// This code is in the public domain -- castanyo@yahoo.es

#ifndef NV_MESH_SNAP_H
#define NV_MESH_SNAP_H

#include <stdint.h>
#include <float.h>

namespace nv
{
	class TriMesh;

    uint32_t SnapVertices(TriMesh * mesh, float posThreshold = FLT_EPSILON, float texThreshold = 1.0f / 1024, float norThreshold = FLT_EPSILON /* nv_normal_epsilon */);

} // nv namespace


#endif // NV_MESH_SNAP_H
