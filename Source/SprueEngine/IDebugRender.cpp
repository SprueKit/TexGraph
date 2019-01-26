#include "IDebugRender.h"

namespace SprueEngine
{

    void IDebugRender::DrawSpherePatch(const Mat3x4& transform, const Vec3& center, const Vec3& up, const Vec3& axis, float radius, float minTh, float maxTh, float minPs, float maxPs, const RGBA& color)
    {
        float stepDegrees = 30.0f;
        Vec3 vA[74];
        Vec3 vB[74];
        Vec3 *pvA = vA, *pvB = vB, *pT;
        Vec3 npole = center + up * radius;
        Vec3 spole = center - up * radius;
        Vec3 arcStart;
        float step = stepDegrees * DEG_TO_RAD;
        const Vec3& kv = up;
        const Vec3& iv = axis;
        Vec3 jv = kv.Cross(iv);
        bool drawN = false;
        bool drawS = false;
        if (minTh <= -HALFPI)
        {
            minTh = -HALFPI + step;
            drawN = true;
        }
        if (maxTh >= HALFPI)
        {
            maxTh = HALFPI- step;
            drawS = true;
        }
        if (minTh > maxTh)
        {
            minTh = -HALFPI + step;
            maxTh =  HALFPI - step;
            drawN = drawS = true;
        }
        int n_hor = (int)((maxTh - minTh) / step) + 1;
        if (n_hor < 2) 
            n_hor = 2;
        float step_h = (maxTh - minTh) / float(n_hor - 1);
        bool isClosed = false;
        if (minPs > maxPs)
        {
            minPs = -PI + step;
            maxPs = PI;
            isClosed = true;
        }
        else if ((maxPs - minPs) >= PI * float(2.f))
        {
            isClosed = true;
        }
        else
        {
            isClosed = false;
        }
        int n_vert = (int)((maxPs - minPs) / step) + 1;
        if (n_vert < 2) n_vert = 2;
        float step_v = (maxPs - minPs) / float(n_vert - 1);
        for (int i = 0; i < n_hor; i++)
        {
            float th = minTh + float(i) * step_h;
            float sth = radius * sinf(th);
            float cth = radius * cosf(th);
            for (int j = 0; j < n_vert; j++)
            {
                float psi = minPs + float(j) * step_v;
                float sps = sinf(psi);
                float cps = cosf(psi);
                pvB[j] = center + cth * cps * iv + cth * sps * jv + sth * kv;

                if (i)
                    DrawLine(transform * pvA[j], transform * pvB[j], color);
                else if (drawS)
                    DrawLine(transform * spole, transform * pvB[j], color);

                if (j)
                    DrawLine(transform * pvB[j - 1], transform * pvB[j], color);
                else
                    arcStart = pvB[j];

                if ((i == (n_hor - 1)) && drawN)
                    DrawLine(transform * npole, transform * pvB[j], color);

                if (true)//drawCenter)
                {
                    if (isClosed)
                    {
                        if (j == (n_vert - 1))
                            DrawLine(transform * arcStart, transform * pvB[j], color);
                    }
                    else
                    {
                        if (((!i) || (i == (n_hor - 1))) && ((!j) || (j == (n_vert - 1))))
                            DrawLine(transform * center, transform * pvB[j], color);
                    }
                }
            }
            pT = pvA; pvA = pvB; pvB = pT;
        }
    }

}