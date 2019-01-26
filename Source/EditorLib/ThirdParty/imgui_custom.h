#pragma once

#include "imgui.h"

#include <QColor>

namespace ImGui
{

    QColor ToQTColor(ImU32);
    ImU32 FromQtColor(const QColor&);

    bool Bitfield(const char* name, unsigned* value, unsigned* overIndex);

    bool Gradient_WasDragged(int* handle, float* newTime);
    bool Gradient_WasDoubleClicked(int* handle);
    bool Gradient_WasDeleted(int* handle);
    bool Gradient(const char* name, ImU32* colors, float* times, int colorCt, float* insertionTime);
}