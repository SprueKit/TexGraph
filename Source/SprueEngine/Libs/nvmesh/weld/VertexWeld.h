// Copyright NVIDIA Corporation 2006 -- Ignacio Castano <icastano@nvidia.com>

#ifndef NV_MESH_VERTEXWELD_H
#define NV_MESH_VERTEXWELD_H

namespace nv
{
	class TriMesh;
	class QuadTriMesh;

	void WeldVertices(TriMesh * mesh);
	void WeldVertices(QuadTriMesh * mesh);

} // nv namespace


#endif // NV_MESH_VERTEXWELD_H
