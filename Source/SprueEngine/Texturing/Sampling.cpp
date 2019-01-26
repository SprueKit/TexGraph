#include "Sampling.h"

#include <SprueEngine/MathGeoLib/Algorithm/Random/LCG.h>

namespace SprueEngine
{
    static LCG raycastLCG;


    void buildFrame(const Vec3& normal, Vec3& v1, Vec3& v2)
    {
        v1 = normal.Cross(Vec3(1, 0, 0));
        if (v1.LengthSq() < 0.01)
            v1 = normal.Cross(Vec3(0, 1, 0));
        v2 = normal.Cross(v1);
    }

    void nRooks(int n, std::vector<float>& x1, std::vector<float>& x2)
    {
        for (int i = 0; i < n; i++)
        {
            x1.push_back((float(i)) / n);
            x2.push_back((float(i)) / n);
        }

        for (int i = 0; i < n; i++)
        {
            int j = int(raycastLCG.Float(0, n - 0.0001f));
            std::swap(x1[i], x1[j]);
            j = int(raycastLCG.Float(0, n - 0.0001f));
            std::swap(x2[i], x2[j]);
        }
    }


    Vec3 cosineMap(float x1, float x2, float c)
    {
        float cosTheta2 = c * c + x1 * (1 - c * c);
        float cosTheta = sqrt(cosTheta2);
        float sinTheta = sqrt(1.0 - cosTheta2);
        float angle = 2 * PI * x2;
        return Vec3(sinTheta * cos(angle), sinTheta * sin(angle), cosTheta);
    }

    void ComputeRaySamples(const Vec3& aNormal, float aAngle, int aSamples, std::vector<Vec3>& aSampleDirs)
    {
    //    aSampleDirs.reserve(aSamples);
    //    Vec3 v1;
    //    Vec3 v2;
    //
    //    float c = cosf(aAngle);
    //    buildFrame(aNormal, v1, v2);
    //
    //    std::vector<float> x1;
    //    std::vector<float> x2;
    //    nRooks(aSamples, x1, x2);
    //
    //    for (int s = 0; s < aSamples; s++)
    //    {
    //        Vec3 d = cosineMap(x1[s], x2[s], c);
    //        aSampleDirs.push_back(d.x * v1 + d.y * v2 + d.z * aNormal);
    //    }
    //

        
        Quat quat = Quat::RotateFromTo(Vec3(0.0f, 1.0f, 0.0f), aNormal);
        //Quat quat(normal, Vec3(0.0f, 1.0f, 0.0f));

        aSampleDirs.resize(aSamples);
        unsigned int nSquare = (unsigned int)std::sqrt((float)aSamples);
        unsigned int latitudes = nSquare;
        unsigned int longitudes = (unsigned int)(aSampleDirs.size() / nSquare);
        unsigned int remainingSamples = (unsigned int)(aSampleDirs.size() % nSquare);
        unsigned int i = 0, j = 0, k = 0;
        float alpha = 0, beta = (HALFPI / (float)latitudes) / 2.0f;
        Vec3 p;

        while (j < latitudes) {
            bool justOnce = false;
            if ((j % 3) == 0)
                alpha = 0;
            else if ((j % 3) == 1)
                alpha = (PI2 / (float)longitudes) / 3.0f;
            else
                alpha = 2.0f * (PI2 / (float)longitudes) / 3.0f;
            i = 0;

            while (i < longitudes)
            {
                p.x = cosf(alpha) * cosf(beta);
                p.z = sinf(alpha) * cosf(beta);
                p.y = sinf(beta);
                p.Normalize();
                aSampleDirs[k] = quat * p;

                if (j == 0)
                {
                    alpha += PI2 / (float)(longitudes + remainingSamples);
                    if (i == remainingSamples && !justOnce)
                    {
                        i = 0;
                        justOnce = true;
                    }
                }
                else
                {
                    alpha += PI2 / (float)longitudes;
                }

                i++; k++;
            }
            beta += HALFPI / (float)latitudes;
            j++;
        }
    }

}