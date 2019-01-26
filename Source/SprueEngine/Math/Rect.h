#pragma once

namespace SprueEngine
{

struct Rect
{
    float xMin, yMin;
    float xMax, yMax;

    Rect() { xMin = yMin = xMax = yMax = 0.0f; }
    Rect(float xMin, float yMin, float width, float height) : xMin(xMin), yMin(yMin), xMax(xMin + width), yMax(yMin + height) { }

    float Width() const { return xMax - xMin; }
    float Height() const { return yMax - yMin; }
};

}