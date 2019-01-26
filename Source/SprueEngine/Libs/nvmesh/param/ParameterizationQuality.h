// Copyright NVIDIA Corporation 2008 -- Ignacio Castano <icastano@nvidia.com>

#pragma once
#ifndef NV_MESH_PARAMETERIZATIONQUALITY_H
#define NV_MESH_PARAMETERIZATIONQUALITY_H

#include <SprueEngine/MathGeoLib/AllMath.h>

namespace nv
{
    namespace HalfEdge { class Mesh; }

    // Estimate quality of existing parameterization.
    class ParameterizationQuality
    {
    public:
        ParameterizationQuality();
        ParameterizationQuality(const HalfEdge::Mesh * mesh);

        bool isValid() const;

        float rmsStretchMetric() const;
        float maxStretchMetric() const;

        float rmsConformalMetric() const;
        float maxAuthalicMetric() const;

        void operator += (const ParameterizationQuality & pq);

    private:

        void processTriangle(SprueEngine::Vec3 p[3], SprueEngine::Vec2 t[3]);

    private:

        uint32_t m_totalTriangleCount;
        uint32_t m_flippedTriangleCount;
        uint32_t m_zeroAreaTriangleCount;

        float m_parametricArea;
        float m_geometricArea;

        float m_stretchMetric;
        float m_maxStretchMetric;

        float m_conformalMetric;
        float m_authalicMetric;

    };

} // nv namespace

#endif // NV_MESH_PARAMETERIZATIONQUALITY_H
