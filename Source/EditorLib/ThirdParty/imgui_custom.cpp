#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui_custom.h"
#include "imgui_internal.h"

#include <string>

namespace ImGui
{

    QColor ToQTColor(ImU32 rawCol)
    {
        unsigned char r = ((unsigned char*)&rawCol)[0];
        unsigned char g = ((unsigned char*)&rawCol)[1];
        unsigned char b = ((unsigned char*)&rawCol)[2];
        unsigned char a = ((unsigned char*)&rawCol)[3];
        return QColor(r, g, b, a);
    }

    ImU32 FromQtColor(const QColor& col)
    {
        ImU32 ret = 0;
        ((unsigned char*)&ret)[0] = (unsigned char)col.red();
        ((unsigned char*)&ret)[1] = (unsigned char)col.green();
        ((unsigned char*)&ret)[2] = (unsigned char)col.blue();
        ((unsigned char*)&ret)[3] = (unsigned char)col.alpha();
        return ret;
    }

    bool Bitfield(const char* label, unsigned* v, unsigned* hoverIndex)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        unsigned oldFlags = window->Flags;
        //window->Flags |= ImGuiWindowFlags_ShowBorders;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImVec2 smallLabelSize = label_size * 0.5f;

        bool anyPressed = false;
        ImVec2 currentPos = window->DC.CursorPos;
        for (unsigned i = 0; i < 32; ++i)
        {
            const void* lbl = (void*)(label + i);
            const ImGuiID localID = window->GetID(lbl);
            if (i == 16)
            {
                currentPos.x = window->DC.CursorPos.x;
                currentPos.y += smallLabelSize.y + style.FramePadding.y * 2 + 2 /*little bit of space*/;
            }
            if (i == 8 || i == 24)
                currentPos.x += smallLabelSize.y;

            const ImRect check_bb(currentPos, currentPos + ImVec2(smallLabelSize.y + style.FramePadding.y * 2, smallLabelSize.y + style.FramePadding.y * 2));

            bool hovered, held;
            bool pressed = ButtonBehavior(check_bb, localID, &hovered, &held);
            if (pressed)
                *v ^= (1 << i);

            if (hovered && hoverIndex)
                *hoverIndex = i;

            RenderFrame(check_bb.Min, check_bb.Max, GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, 0 /*we don't want to round the button matrix*/);// style.FrameRounding);
            if (*v & (1 << i))
            {
                const float check_sz = ImMin(check_bb.GetWidth(), check_bb.GetHeight());
                const float pad = ImMax(1.0f, (float)(int)(check_sz / 6.0f));
                window->DrawList->AddRectFilled(check_bb.Min + ImVec2(pad, pad), check_bb.Max - ImVec2(pad, pad), GetColorU32(ImGuiCol_CheckMark), 0 /*we don't want to round it here either*/);// style.FrameRounding);
            }

            anyPressed |= pressed;
            currentPos.x = check_bb.Max.x + 2; //2 pt spacing
        }

        const ImRect matrix_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(
                (smallLabelSize.y + style.FramePadding.y * 2) * 16 /*# of checks in a row*/ + smallLabelSize.y /*space between sets of 8*/ + 15 * 2 /*spacing between each check*/, 
                (smallLabelSize.y + style.FramePadding.y * 2) * 2 /*# of rows*/ + 2 /*spacing between rows*/));
        ItemSize(matrix_bb, style.FramePadding.y);

        ImRect total_bb = matrix_bb;
        if (label_size.x > 0)
            SameLine(0, style.ItemInnerSpacing.x);
        
        const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);
        if (label_size.x > 0)
        {
            ItemSize(ImVec2(text_bb.GetWidth(), matrix_bb.GetHeight()), style.FramePadding.y);
            total_bb = ImRect(ImMin(matrix_bb.Min, text_bb.Min), ImMax(matrix_bb.Max, text_bb.Max));
        }

        if (!ItemAdd(total_bb, &id))
            return false;

        if (label_size.x > 0.0f)
            RenderText(text_bb.GetTL(), label);


        window->Flags = oldFlags;
        return anyPressed;
    }

    static int Gradient_TargetMark = -1;
    static bool Gradient_MarkDragging = false;
    static float Gradient_DraggedTime = 0.0f;
    static bool Gradient_MarkDoubleClick = false;
    static bool Gradient_MarkDeleted = false;

    bool Gradient_WasDragged(int* handle, float* newTime) { if (Gradient_MarkDragging) { *handle = Gradient_TargetMark; *newTime = Gradient_DraggedTime;  return true; } return false; }
    bool Gradient_WasDoubleClicked(int* handle) { if (Gradient_MarkDoubleClick) { *handle = Gradient_TargetMark; return true; } return false; }
    bool Gradient_WasDeleted(int* handle) { if (Gradient_MarkDeleted) { *handle = Gradient_TargetMark; return true; } return false; }

    float ToNormalized(float min, float max, float val)
    {
        return ImClamp((val - min) / (max - min), 0.0f, 1.0f);
    }

// Gradient
    static bool DrawGradientMarks(ImU32* colors, float* times, int tickCt,
        struct ImVec2 const & bar_pos,
        float maxWidth,
        float height)
    {
        ImU32 colorA = 0;
        ImU32 colorB = 0;
        float barBottom = bar_pos.y + height;
        
        int draggingMark = ImGui::GetStateStorage()->GetInt(ImGui::GetID("DRAG_MARK"), -1);
        ImGui::GetStateStorage()->SetInt(ImGui::GetID("DRAG_MARK"), -1);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        bool anyAction = false;
        for (int i = 0; i < tickCt; ++i)
        {
            float markPosition = times[i];

            float to = bar_pos.x + markPosition * maxWidth;

            colorA = colors[i];

            //draw_list->AddTriangleFilled(ImVec2(to, bar_pos.y + (height - 24)),
            //    ImVec2(to - 6, barBottom - 16),
            //    ImVec2(to + 6, barBottom - 16), IM_COL32(100, 100, 100, 255));

            const ImRect boxRect(ImVec2(to - 6, barBottom - 16), ImVec2(to + 6, bar_pos.y + (height + 12)));

            draw_list->AddRectFilled(ImVec2(to - 6, barBottom - 16),
                ImVec2(to + 6, bar_pos.y + (height + 12)),
                IM_COL32(100, 100, 100, 255), 1.0f, 1.0f);

            draw_list->AddRectFilled(ImVec2(to - 5, bar_pos.y + (height + 1) - 16),
                ImVec2(to + 5, bar_pos.y + (height + 11)),
                IM_COL32(0, 0, 0, 255), 1.0f, 1.0f);

            draw_list->AddRectFilledMultiColor(ImVec2(to - 3, bar_pos.y + (height + 3) - 16),
                ImVec2(to + 3, bar_pos.y + (height + 9)),
                colorA, colorA, colorA, colorA);

            ImGui::SetCursorScreenPos(ImVec2(to - 8, barBottom - 16));
            ImGui::InvisibleButton("mark", ImVec2(16, 24));

            if (!anyAction)
            {
                if (ImGui::IsMouseDown(0) && draggingMark != -1)
                {
                    Gradient_MarkDragging = true;
                    anyAction = true;
                    Gradient_DraggedTime = ToNormalized(bar_pos.x, bar_pos.x + maxWidth, ImGui::GetMousePos().x);
                    ImGui::GetStateStorage()->SetInt(ImGui::GetID("DRAG_MARK"), draggingMark);
                }
                else if (ImGui::IsItemHovered())
                {
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        Gradient_MarkDoubleClick = true;
                        Gradient_TargetMark = i;
                        anyAction = true;
                        ImGui::GetStateStorage()->SetInt(ImGui::GetID("DRAG_MARK"), -1);
                    }
                    else if (ImGui::IsMouseDown(0))
                    {
                        if (ImGui::GetIO().KeyCtrl)
                        {
                            Gradient_MarkDeleted = true;
                            Gradient_TargetMark = i;
                            anyAction = true;
                            ImGui::GetStateStorage()->SetInt(ImGui::GetID("DRAG_MARK"), -1);
                        }
                        else
                        {
                            Gradient_MarkDragging = false;
                            Gradient_TargetMark = i;
                            anyAction = true;
                            ImGui::GetStateStorage()->SetInt(ImGui::GetID("DRAG_MARK"), Gradient_TargetMark);
                        }
                    }
                    else
                        ImGui::GetStateStorage()->SetInt(ImGui::GetID("DRAG_MARK"), -1);
                }
                else
                    ImGui::GetStateStorage()->SetInt(ImGui::GetID("DRAG_MARK"), -1);
            }
        }

        ImGui::SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 20.0f));
        return anyAction;
    }

    bool Gradient(const char* label, ImU32* colors, float* times, int colorCt, float* insertionTime)
    {
        if (insertionTime)
            *insertionTime = FLT_MIN;
        Gradient_MarkDoubleClick = false;
        Gradient_MarkDragging = false;
        Gradient_MarkDeleted = false;

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGui::PushID(label);

        unsigned oldFlags = window->Flags;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float w = CalcItemWidth();

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
        const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

        frame_bb.Min.x += 5;
        frame_bb.Max.x -= 10;

        // NB- we don't call ItemSize() yet because we may turn into a text edit box below
        if (!ItemAdd(total_bb, &id))
        {
            ItemSize(total_bb, style.FramePadding.y);
            return false;
        }

        const bool hovered = IsHovered(frame_bb, id);
        if (hovered)
            SetHoveredID(id);

        auto drawList = window->DrawList;
        int pos = frame_bb.Min.x;

        float startTime = times[0];
        float endTime = times[colorCt - 1];

        for (int i = 0; i < colorCt; ++i)
        {
            const int prevIndex = std::max(i - 1, 0);
            const int curIndex = std::min(i, colorCt - 1);
            ImU32 leftColor = colors[prevIndex];
            ImU32 rightColor = colors[curIndex];

            int fractWidth = frame_bb.GetWidth() * times[i];
            drawList->AddRectFilledMultiColor(
                ImVec2(pos, (int)frame_bb.Min.y),
                ImVec2(frame_bb.Min.x + fractWidth, (int)frame_bb.Max.y),
                (ImU32)leftColor, 
                (ImU32)rightColor, 
                (ImU32)rightColor, 
                (ImU32)leftColor);
            pos = frame_bb.Min.x + fractWidth;
        }
        //if (pos < frame_bb.Max.x)
        {
            drawList->AddRectFilledMultiColor(
                ImVec2(frame_bb.Min.x + frame_bb.GetWidth() * times[colorCt-1], (int)frame_bb.Min.y),
                frame_bb.Max,
                (ImU32)colors[colorCt-1],
                (ImU32)colors[colorCt-1],
                (ImU32)colors[colorCt-1],
                (ImU32)colors[colorCt-1]);
        }

    // Draw indicator ticks
        bool anyChanges = DrawGradientMarks(colors, times, colorCt, frame_bb.Min, frame_bb.GetWidth(), frame_bb.GetHeight());

        if (!anyChanges && frame_bb.Contains(ImGui::GetMousePos()) && ImGui::IsMouseDoubleClicked(0))
        {
            anyChanges = true;
            if (insertionTime)
                *insertionTime = ToNormalized(frame_bb.Min.x, frame_bb.Max.x, ImGui::GetMousePos().x);
        }

        window->Flags = oldFlags;

        ImGui::PopID();

        return anyChanges;
    }
}