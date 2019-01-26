#include "QImGui.h"

#include "QImGuiSlave.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCursor>
#include <QDesktopWidget>
#include <QPainter>
#include <QTooltip>

#include <QBoxLayout>
#include <qevent.h>

#include <EditorLib/ThirdParty/imgui_internal.h>
#include <EditorLib/ThirdParty/QSexyToolTip.h>

//#ifdef WIN_64
#include <Windows.h>
#undef CreateFont

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

const int IM_KEY_CODE_TAB = VK_TAB;
const int IM_KEY_CODE_DELETE = VK_DELETE;
const int IM_KEY_CODE_UP = VK_UP;
const int IM_KEY_CODE_DOWN = VK_DOWN;
const int IM_KEY_CODE_RIGHT = VK_RIGHT;
const int IM_KEY_CODE_LEFT = VK_LEFT;
const int IM_KEY_CODE_BACKSPACE = VK_BACK;
const int IM_KEY_CODE_RETURN = VK_RETURN;
const int IM_KEY_CODE_HOME = VK_HOME;
const int IM_KEY_CODE_END = VK_END;
const int IM_KEY_CODE_PAGEUP = VK_PRIOR;
const int IM_KEY_CODE_PAGEDOWN = VK_NEXT;
const int IM_KEY_CODE_ESCAPE = VK_ESCAPE;

const int IM_KEY_A = 0x41;
const int IM_KEY_C = 0x43;
const int IM_KEY_V = 0x56;
const int IM_KEY_X = 0x58;
const int IM_KEY_Y = 0x59;
const int IM_KEY_Z = 0x5A;

#define FONT_FACE "Segoe UI"
#define FONT_SIZE 10

//#endif

void ShowStyleEditor(ImGuiStyle* ref);

ImFont* QImGui::imFont_ = 0x0;

struct ImGuiFont : public ImFont
{
    ImGuiFont(const QFontMetrics& metrics)
    {
        for (int i = 0; i < 255; ++i)
        {
            Glyph g;
            g.Codepoint = i;
            g.XAdvance = metrics.width(i);
            if (i == '\t')
                g.XAdvance = metrics.width(' ') * 4;
            IndexXAdvance.push_back(g.XAdvance);
            Glyphs.push_back(g);
        }
        Scale = 1.0f;
        FontSize = metrics.height() + metrics.leading();
        Descent = metrics.descent();
        Ascent = metrics.ascent();
        FallbackXAdvance = 0.0f;
        ConfigData = 0x0;
    }

    virtual const Glyph* FindGlyph(ImWchar c) const override
    {
        if (c >= 0 && c <= 256)
            return &Glyphs[c];
        return FallbackGlyph;
    }
};

QImGui::QImGui(int updateFreq) :
    font_(QFont(FONT_FACE, FONT_SIZE)),
    metrics_(QFont(FONT_FACE, FONT_SIZE)),
    keysDown_(512, false),
    mouseButtons_{false,false,false}
{
    font_ = QFont(QFont(FONT_FACE), this);
    font_.setPixelSize(20);
    metrics_ = QFontMetrics(font_, this);
    Initialize();

    // prepare the auto-update timer
    connect(&repaintTimer_, &QTimer::timeout, [=]() { 
        //if (hasFocus() && repaintTimer_.interval() != this->assignedUpdateRate_)
        //    repaintTimer_.start(this->assignedUpdateRate_);
        //else if (!hasFocus() && repaintTimer_.interval() != this->assignedUpdateRate_ * 3)
        //    repaintTimer_.start(this->assignedUpdateRate_ * 3);
        //int inter = repaintTimer_.interval();
        update(); 
    });

    repaintTimer_.setSingleShot(false);
    SetAutoUpdate(updateFreq);
    setToolTipDuration(0);
}

QImGui::QImGui(GUI_CALLBACK callback, int autoUpdateFreq) :
    QImGui(autoUpdateFreq)
{
    guiCallback_ = callback;
}

QImGui::~QImGui()
{
    Destroy();
}

/// Set the automatic repaint frequency, use 0 to disable automatic repaint.
void QImGui::SetAutoUpdate(int ms)
{
    assignedUpdateRate_ = ms;
    if (ms == 0)
        repaintTimer_.stop();
    else
        repaintTimer_.start(ms);
}

void QImGui::cut()
{
    keysDown_[IM_KEY_X] = true;
    disposeKeycode_ = IM_KEY_X;
    forceCtrl_ = true;
    setFocus(Qt::FocusReason::ActiveWindowFocusReason);
    DoUpdate();
}

void QImGui::copy()
{
    keysDown_[IM_KEY_C] = true;
    disposeKeycode_ = IM_KEY_C;
    forceCtrl_ = true;
    setFocus(Qt::FocusReason::ActiveWindowFocusReason);
    DoUpdate();
}

void QImGui::paste()
{
    keysDown_[IM_KEY_V] = true;
    disposeKeycode_ = IM_KEY_V;
    forceCtrl_ = true;
    setFocus(Qt::FocusReason::ActiveWindowFocusReason);
    DoUpdate();
}

void QImGui::undo()
{
    keysDown_[IM_KEY_Z] = true;
    disposeKeycode_ = IM_KEY_Z;
    forceCtrl_ = true;
    setFocus(Qt::FocusReason::ActiveWindowFocusReason);
    DoUpdate();
}

void QImGui::redo()
{
    keysDown_[IM_KEY_Y] = true;
    disposeKeycode_ = IM_KEY_Y;
    forceCtrl_ = true;
    setFocus(Qt::FocusReason::ActiveWindowFocusReason);
    DoUpdate();
}

std::string storedString_, storedString2_;
std::string GetStoredStr() { return storedString_; }
std::string GetStoredStr2() { return storedString2_; }
void SetStoredStr(std::string val) { storedString_ = val; }
void SetStoredStr2(std::string val) { storedString2_ = val; }

void QImGui::GenerateUI(QPainter* painter, const QSize& size)
{
    if (guiCallback_)
    {
        guiCallback_(this, painter, size);
        return;
    }

    static std::string textData = "Testing some string data";
    static int testInt = 42;
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
    ImGui::SetNextWindowSize(ImVec2(size.width(), size.height()), ImGuiSetCond_Always);
    ImGui::Begin("Another Window", 0x0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
    ImGui::Text("Hello");
    ImGui::Text("Here's some more text");
    ImGui::Text("And there's even some extra long text too to include. Just to see what happens when the text gets too long");
    
    ImGui::AlignFirstTextHeightToWidgets();
    ImGui::Text("Int Editor"); ImGui::SameLine(); ImGui::InputInt("##Int Editor", &testInt, 1, 100, 0);
    ImGui::Button("Button Time");
    ImGui::Button("Double Button Time");

    std::string str1 = GetStoredStr();
    if (EditString("Str1", str1))
        SetStoredStr(str1);
    std::string str2 = GetStoredStr2();
    if (EditString("Str2", str2))
        SetStoredStr2(str2);

    if (ImGui::TreeNode("Collapsing Headers"))
    {
        static bool closable_group = true;
        if (ImGui::CollapsingHeader("Header"))
        {
            ImGui::Checkbox("Enable extra group", &closable_group);
            for (int i = 0; i < 5; i++)
                ImGui::Text("Some content %d", i);
        }
        if (ImGui::CollapsingHeader("Header with a close button", &closable_group))
        {
            for (int i = 0; i < 5; i++)
                ImGui::Text("More content %d", i);
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Colored Text"))
    {
        // Using shortcut. You can use PushStyleColor()/PopStyleColor() for more flexibility.
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Pink");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Yellow");
        ImGui::TextDisabled("Disabled");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Word Wrapping"))
    {
        // Using shortcut. You can use PushTextWrapPos()/PopTextWrapPos() for more flexibility.
        ImGui::TextWrapped("This text should automatically wrap on the edge of the window. The current implementation for text wrapping follows simple rules suitable for English and possibly other languages.");
        ImGui::Spacing();

        static float wrap_width = 200.0f;
        ImGui::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");

        ImGui::Text("Test paragraph 1:");
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrap_width, pos.y), ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight()), IM_COL32(255, 0, 255, 255));
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
        ImGui::Text("The lazy dog is a good dog. This paragraph is made to fit within %.0f pixels. Testing a 1 character word. The quick brown fox jumps over the lazy dog.", wrap_width);
        ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
        ImGui::PopTextWrapPos();

        ImGui::Text("Test paragraph 2:");
        pos = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrap_width, pos.y), ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight()), IM_COL32(255, 0, 255, 255));
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
        ImGui::Text("aaaaaaaa bbbbbbbb, c cccccccc,dddddddd. d eeeeeeee   ffffffff. gggggggg!hhhhhhhh");
        ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
        ImGui::PopTextWrapPos();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Bullets"))
    {
        ImGui::BulletText("Bullet point 1");
        ImGui::BulletText("Bullet point 2\nOn multiple lines");
        ImGui::Bullet(); ImGui::Text("Bullet point 3 (two calls)");
        ImGui::Bullet(); ImGui::SmallButton("Button");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Selectables"))
    {
        if (ImGui::TreeNode("Basic"))
        {
            static bool selected[4] = { false, true, false, false };
            ImGui::Selectable("1. I am selectable", &selected[0]);
            ImGui::Selectable("2. I am selectable", &selected[1]);
            ImGui::Text("3. I am not selectable");
            ImGui::Selectable("4. I am selectable", &selected[2]);
            if (ImGui::Selectable("5. I am double clickable", selected[3], ImGuiSelectableFlags_AllowDoubleClick))
                if (ImGui::IsMouseDoubleClicked(0))
                    selected[3] = !selected[3];
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Rendering more text into the same block"))
        {
            static bool selected[3] = { false, false, false };
            ImGui::Selectable("main.c", &selected[0]);    ImGui::SameLine(300); ImGui::Text(" 2,345 bytes");
            ImGui::Selectable("Hello.cpp", &selected[1]); ImGui::SameLine(300); ImGui::Text("12,345 bytes");
            ImGui::Selectable("Hello.h", &selected[2]);   ImGui::SameLine(300); ImGui::Text(" 2,345 bytes");
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("In columns"))
        {
            ImGui::Columns(3, NULL, false);
            static bool selected[16] = { 0 };
            for (int i = 0; i < 16; i++)
            {
                char label[32]; sprintf(label, "Item %d", i);
                if (ImGui::Selectable(label, &selected[i])) {}
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Grid"))
        {
            static bool selected[16] = { true, false, false, false, false, true, false, false, false, false, true, false, false, false, false, true };
            for (int i = 0; i < 16; i++)
            {
                ImGui::PushID(i);
                if (ImGui::Selectable("Sailor", &selected[i], 0, ImVec2(50, 50)))
                {
                    int x = i % 4, y = i / 4;
                    if (x > 0) selected[i - 1] ^= 1;
                    if (x < 3) selected[i + 1] ^= 1;
                    if (y > 0) selected[i - 4] ^= 1;
                    if (y < 3) selected[i + 4] ^= 1;
                }
                if ((i % 4) < 3) ImGui::SameLine();
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Filtered Text Input"))
    {
        ImGui::PushAllowKeyboardFocus(true);
        static char buf1[64] = ""; ImGui::InputText("default", buf1, 64);
        static char buf2[64] = ""; ImGui::InputText("decimal", buf2, 64, ImGuiInputTextFlags_CharsDecimal);
        static char buf3[64] = ""; ImGui::InputText("hexadecimal", buf3, 64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
        static char buf4[64] = ""; ImGui::InputText("uppercase", buf4, 64, ImGuiInputTextFlags_CharsUppercase);
        static char buf5[64] = ""; ImGui::InputText("no blank", buf5, 64, ImGuiInputTextFlags_CharsNoBlank);
        struct TextFilters { static int FilterImGuiLetters(ImGuiTextEditCallbackData* data) { if (data->EventChar < 256 && strchr("imgui", (char)data->EventChar)) return 0; return 1; } };
        static char buf6[64] = ""; ImGui::InputText("\"imgui\" letters", buf6, 64, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterImGuiLetters);

        ImGui::Text("Password input");
        static char bufpass[64] = "password123";
        ImGui::InputText("password", bufpass, 64, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);
        ImGui::InputText("password (clear)", bufpass, 64, ImGuiInputTextFlags_CharsNoBlank);
        ImGui::PopAllowKeyboardFocus();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Scrolling"))
    {
        ImGui::TextWrapped("(Use SetScrollHere() or SetScrollFromPosY() to scroll to a given position.)");
        static bool track = true;
        static int track_line = 50, scroll_to_px = 200;
        ImGui::Checkbox("Track", &track);
        ImGui::PushItemWidth(100);
        ImGui::SameLine(130); track |= ImGui::DragInt("##line", &track_line, 0.25f, 0, 99, "Line %.0f");
        bool scroll_to = ImGui::Button("Scroll To");
        ImGui::SameLine(130); scroll_to |= ImGui::DragInt("##pos_y", &scroll_to_px, 1.00f, 0, 9999, "y = %.0f px");
        ImGui::PopItemWidth();
        if (scroll_to) track = false;

        for (int i = 0; i < 5; i++)
        {
            if (i > 0) ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%s", i == 0 ? "Top" : i == 1 ? "25%" : i == 2 ? "Center" : i == 3 ? "75%" : "Bottom");
            ImGui::BeginChild(ImGui::GetID((void*)(intptr_t)i), ImVec2(ImGui::GetWindowWidth() * 0.17f, 200.0f), true);
            if (scroll_to)
                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + scroll_to_px, i * 0.25f);
            for (int line = 0; line < 100; line++)
            {
                if (track && line == track_line)
                {
                    ImGui::TextColored(ImColor(255, 255, 0), "Line %d", line);
                    ImGui::SetScrollHere(i * 0.25f); // 0.0f:top, 0.5f:center, 1.0f:bottom
                }
                else
                {
                    ImGui::Text("Line %d", line);
                }
            }
            ImGui::EndChild();
            ImGui::EndGroup();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Multi-line Text Input"))
    {
        static bool read_only = false;
        static char text[1024 * 16] =
            "/*\n"
            " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
            " the hexadecimal encoding of one offending instruction,\n"
            " more formally, the invalid operand with locked CMPXCHG8B\n"
            " instruction bug, is a design flaw in the majority of\n"
            " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
            " processors (all in the P5 microarchitecture).\n"
            "*/\n\n"
            "label:\n"
            "\tlock cmpxchg8b eax\n";

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox("Read-only", &read_only);
        ImGui::PopStyleVar();
        ImGui::InputTextMultiline("##source", text, 1024 * 16, ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput | (read_only ? ImGuiInputTextFlags_ReadOnly : 0));
        ImGui::TreePop();
    }

    static bool a = false;
    if (ImGui::Button("Button")) { printf("Clicked\n"); a ^= 1; }
    if (a)
    {
        ImGui::SameLine();
        ImGui::Text("Thanks for clicking me!");
    }

    static bool check = true;
    ImGui::Checkbox("checkbox", &check);

    static int e = 0;
    ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
    ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
    ImGui::RadioButton("radio c", &e, 2);

    // Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
    for (int i = 0; i < 7; i++)
    {
        if (i > 0) ImGui::SameLine();
        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
        ImGui::Button("Click");
        ImGui::PopStyleColor(3);
        ImGui::PopID();
    }

#if 1
    if (ImGui::CollapsingHeader("Common Widgets"))
    {
        ImGui::LabelText("label", "Value");

        ImGui::Separator();

        static int item = 1;
        ImGui::Combo("combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");   // Combo using values packed in a single constant string (for really quick combo)

        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK" };
        static int item2 = -1;
        ImGui::Combo("combo scroll", &item2, items, IM_ARRAYSIZE(items));   // Combo using proper array. You can also pass a callback to retrieve array value, no need to create/copy an array just for that.

        {
            static char str0[128] = "Hello, world!";
            static int i0 = 123;
            static float f0 = 0.001f;
            ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));

            ImGui::InputInt("input int", &i0);

            ImGui::InputFloat("input float", &f0, 0.01f, 1.0f);

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            ImGui::InputFloat3("input float3", vec4a);
        }

        {
            static int i1 = 50, i2 = 42;
            ImGui::DragInt("drag int", &i1, 1);

            ImGui::DragInt("drag int 0..100", &i2, 1, 0, 100, "%.0f%%");

            static float f1 = 1.00f, f2 = 0.0067f;
            ImGui::DragFloat("drag float", &f1, 0.005f);
            ImGui::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
        }

        {
            static int i1 = 0;
            ImGui::SliderInt("slider int", &i1, -1, 3);

            static float f1 = 0.123f, f2 = 0.0f;
            ImGui::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
            ImGui::SliderFloat("slider log float", &f2, -10.0f, 10.0f, "%.4f", 3.0f);
            static float angle = 0.0f;
            ImGui::SliderAngle("slider angle", &angle);
        }

        static float col1[3] = { 1.0f,0.0f,0.2f };
        static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
        ImGui::ColorEdit3("color 1", col1);

        ImGui::ColorEdit4("color 2", col2);

        const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
        static int listbox_item_current = 1;
        ImGui::ListBox("listbox\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);

        if (ImGui::TreeNode("Range Widgets"))
        {
            ImGui::Unindent();

            static float begin = 10, end = 90;
            static int begin_i = 100, end_i = 1000;
            ImGui::DragFloatRange2("range", &begin, &end, 0.25f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%");
            ImGui::DragIntRange2("range int (no bounds)", &begin_i, &end_i, 5, 0, 0, "Min: %.0f units", "Max: %.0f units");

            ImGui::Indent();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Multi-component Widgets"))
        {
            ImGui::Unindent();

            static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            static int vec4i[4] = { 1, 5, 100, 255 };

            ImGui::InputFloat2("input float2", vec4f);
            ImGui::DragFloat2("drag float2", vec4f, 0.01f, 0.0f, 1.0f);
            ImGui::SliderFloat2("slider float2", vec4f, 0.0f, 1.0f);
            ImGui::DragInt2("drag int2", vec4i, 1, 0, 255);
            ImGui::InputInt2("input int2", vec4i);
            ImGui::SliderInt2("slider int2", vec4i, 0, 255);
            ImGui::Spacing();

            ImGui::InputFloat3("input float3", vec4f);
            ImGui::DragFloat3("drag float3", vec4f, 0.01f, 0.0f, 1.0f);
            ImGui::SliderFloat3("slider float3", vec4f, 0.0f, 1.0f);
            ImGui::DragInt3("drag int3", vec4i, 1, 0, 255);
            ImGui::InputInt3("input int3", vec4i);
            ImGui::SliderInt3("slider int3", vec4i, 0, 255);
            ImGui::Spacing();

            ImGui::InputFloat4("input float4", vec4f);
            ImGui::DragFloat4("drag float4", vec4f, 0.01f, 0.0f, 1.0f);
            ImGui::SliderFloat4("slider float4", vec4f, 0.0f, 1.0f);
            ImGui::InputInt4("input int4", vec4i);
            ImGui::DragInt4("drag int4", vec4i, 1, 0, 255);
            ImGui::SliderInt4("slider int4", vec4i, 0, 255);

            ImGui::Indent();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Vertical Sliders"))
        {
            ImGui::Unindent();
            const float spacing = 4;
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));

            static int int_value = 0;
            ImGui::VSliderInt("##int", ImVec2(18, 160), &int_value, 0, 5);
            ImGui::SameLine();

            static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
            ImGui::PushID("set1");
            for (int i = 0; i < 7; i++)
            {
                if (i > 0) ImGui::SameLine();
                ImGui::PushID(i);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(i / 7.0f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor::HSV(i / 7.0f, 0.6f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor::HSV(i / 7.0f, 0.7f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor::HSV(i / 7.0f, 0.9f, 0.9f));
                ImGui::VSliderFloat("##v", ImVec2(18, 160), &values[i], 0.0f, 1.0f, "");
                if (ImGui::IsItemActive() || ImGui::IsItemHovered())
                    ImGui::SetTooltip("%.3f", values[i]);
                ImGui::PopStyleColor(4);
                ImGui::PopID();
            }
            ImGui::PopID();

            ImGui::SameLine();
            ImGui::PushID("set2");
            static float values2[4] = { 0.20f, 0.80f, 0.40f, 0.25f };
            const int rows = 3;
            const ImVec2 small_slider_size(18, (160.0f - (rows - 1)*spacing) / rows);
            for (int nx = 0; nx < 4; nx++)
            {
                if (nx > 0) ImGui::SameLine();
                ImGui::BeginGroup();
                for (int ny = 0; ny < rows; ny++)
                {
                    ImGui::PushID(nx*rows + ny);
                    ImGui::VSliderFloat("##v", small_slider_size, &values2[nx], 0.0f, 1.0f, "");
                    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
                        ImGui::SetTooltip("%.3f", values2[nx]);
                    ImGui::PopID();
                }
                ImGui::EndGroup();
            }
            ImGui::PopID();

            ImGui::SameLine();
            ImGui::PushID("set3");
            for (int i = 0; i < 4; i++)
            {
                if (i > 0) ImGui::SameLine();
                ImGui::PushID(i);
                ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 40);
                ImGui::VSliderFloat("##v", ImVec2(40, 160), &values[i], 0.0f, 1.0f, "%.2f\nsec");
                ImGui::PopStyleVar();
                ImGui::PopID();
            }
            ImGui::PopID();
            ImGui::PopStyleVar();

            ImGui::Indent();
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Graphs widgets"))
    {
        static bool animate = true;
        ImGui::Checkbox("Animate", &animate);

        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

        // Create a dummy array of contiguous float values to plot
        // Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float and the sizeof() of your structure in the Stride parameter.
        static float values[90] = { 0 };
        static int values_offset = 0;
        if (animate)
        {
            static float refresh_time = ImGui::GetTime(); // Create dummy data at fixed 60 hz rate for the demo
            for (; ImGui::GetTime() > refresh_time + 1.0f / 60.0f; refresh_time += 1.0f / 60.0f)
            {
                static float phase = 0.0f;
                values[values_offset] = cosf(phase);
                values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
                phase += 0.10f*values_offset;
            }
        }
        ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, "avg 0.0", -1.0f, 1.0f, ImVec2(0, 80));
        ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80));

        // Use functions to generate output
        // FIXME: This is rather awkward because current plot API only pass in indices. We probably want an API passing floats and user provide sample rate/count.
        struct Funcs
        {
            static float Sin(void*, int i) { return sinf(i * 0.1f); }
            static float Saw(void*, int i) { return (i & 1) ? 1.0f : 0.0f; }
        };
        static int func_type = 0, display_count = 70;
        ImGui::Separator();
        ImGui::PushItemWidth(100); ImGui::Combo("func", &func_type, "Sin\0Saw\0"); ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::SliderInt("Sample count", &display_count, 1, 400);
        float(*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
        ImGui::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        ImGui::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        ImGui::Separator();

        // Animate a simple progress bar
        static float progress = 0.0f, progress_dir = 1.0f;
        if (animate)
        {
            progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
            if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
            if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
        }

        // Typically we would use ImVec2(-1.0f,0.0f) to use all available width, or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Progress Bar");

        float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
        char buf[32];
        sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
        ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
    }

    if (ImGui::CollapsingHeader("Layout"))
    {
        if (ImGui::TreeNode("Child regions"))
        {
            ImGui::Text("Without border");
            static int line = 50;
            bool goto_line = ImGui::Button("Goto");
            ImGui::SameLine();
            ImGui::PushItemWidth(100);
            goto_line |= ImGui::InputInt("##Line", &line, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopItemWidth();
            ImGui::BeginChild("Sub1", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
            for (int i = 0; i < 100; i++)
            {
                ImGui::Text("%04d: scrollable region", i);
                if (goto_line && line == i)
                    ImGui::SetScrollHere();
            }
            if (goto_line && line >= 100)
                ImGui::SetScrollHere();
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
            ImGui::BeginChild("Sub2", ImVec2(0, 300), true);
            ImGui::Text("With border");
            ImGui::Columns(2);
            for (int i = 0; i < 100; i++)
            {
                if (i == 50)
                    ImGui::NextColumn();
                char buf[32];
                sprintf(buf, "%08x", i * 5731);
                ImGui::Button(buf, ImVec2(-1.0f, 0.0f));
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Widgets Width"))
        {
            static float f = 0.0f;
            ImGui::Text("PushItemWidth(100)");
            ImGui::PushItemWidth(100);
            ImGui::DragFloat("float##1", &f);
            ImGui::PopItemWidth();

            ImGui::Text("PushItemWidth(GetWindowWidth() * 0.5f)");
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::DragFloat("float##2", &f);
            ImGui::PopItemWidth();

            ImGui::Text("PushItemWidth(GetContentRegionAvailWidth() * 0.5f)");
            ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.5f);
            ImGui::DragFloat("float##3", &f);
            ImGui::PopItemWidth();

            ImGui::Text("PushItemWidth(-100)");
            ImGui::PushItemWidth(-100);
            ImGui::DragFloat("float##4", &f);
            ImGui::PopItemWidth();

            ImGui::Text("PushItemWidth(-1)");
            ImGui::PushItemWidth(-1);
            ImGui::DragFloat("float##5", &f);
            ImGui::PopItemWidth();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Basic Horizontal Layout"))
        {
            ImGui::TextWrapped("(Use ImGui::SameLine() to keep adding items to the right of the preceding item)");

            // Text
            ImGui::Text("Two items: Hello"); ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

            // Adjust spacing
            ImGui::Text("More spacing: Hello"); ImGui::SameLine(0, 20);
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

            // Button
            ImGui::AlignFirstTextHeightToWidgets();
            ImGui::Text("Normal buttons"); ImGui::SameLine();
            ImGui::Button("Banana"); ImGui::SameLine();
            ImGui::Button("Apple"); ImGui::SameLine();
            ImGui::Button("Corniflower");

            // Button
            ImGui::Text("Small buttons"); ImGui::SameLine();
            ImGui::SmallButton("Like this one"); ImGui::SameLine();
            ImGui::Text("can fit within a text block.");

            // Aligned to arbitrary position. Easy/cheap column.
            ImGui::Text("Aligned");
            ImGui::SameLine(150); ImGui::Text("x=150");
            ImGui::SameLine(300); ImGui::Text("x=300");
            ImGui::Text("Aligned");
            ImGui::SameLine(150); ImGui::SmallButton("x=150");
            ImGui::SameLine(300); ImGui::SmallButton("x=300");

            // Checkbox
            static bool c1 = false, c2 = false, c3 = false, c4 = false;
            ImGui::Checkbox("My", &c1); ImGui::SameLine();
            ImGui::Checkbox("Tailor", &c2); ImGui::SameLine();
            ImGui::Checkbox("Is", &c3); ImGui::SameLine();
            ImGui::Checkbox("Rich", &c4);

            // Various
            static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
            ImGui::PushItemWidth(80);
            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
            static int item = -1;
            ImGui::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); ImGui::SameLine();
            ImGui::SliderFloat("X", &f0, 0.0f, 5.0f); ImGui::SameLine();
            ImGui::SliderFloat("Y", &f1, 0.0f, 5.0f); ImGui::SameLine();
            ImGui::SliderFloat("Z", &f2, 0.0f, 5.0f);
            ImGui::PopItemWidth();

            ImGui::PushItemWidth(80);
            ImGui::Text("Lists:");
            static int selection[4] = { 0, 1, 2, 3 };
            for (int i = 0; i < 4; i++)
            {
                if (i > 0) ImGui::SameLine();
                ImGui::PushID(i);
                ImGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
                ImGui::PopID();
                //if (ImGui::IsItemHovered()) ImGui::SetTooltip("ListBox %d hovered", i);
            }
            ImGui::PopItemWidth();

            // Dummy
            ImVec2 sz(30, 30);
            ImGui::Button("A", sz); ImGui::SameLine();
            ImGui::Dummy(sz); ImGui::SameLine();
            ImGui::Button("B", sz);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Groups"))
        {
            ImGui::TextWrapped("(Using ImGui::BeginGroup()/EndGroup() to layout items. BeginGroup() basically locks the horizontal position. EndGroup() bundles the whole group so that you can use functions such as IsItemHovered() on it.)");
            ImGui::BeginGroup();
            {
                ImGui::BeginGroup();
                ImGui::Button("AAA");
                ImGui::SameLine();
                ImGui::Button("BBB");
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Button("CCC");
                ImGui::Button("DDD");
                ImGui::EndGroup();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Group hovered");
                ImGui::SameLine();
                ImGui::Button("EEE");
                ImGui::EndGroup();
            }
            // Capture the group size and create widgets using the same size
            ImVec2 size = ImGui::GetItemRectSize();
            const float values[5] = { 0.5f, 0.20f, 0.80f, 0.60f, 0.25f };
            ImGui::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, size);

            ImGui::Button("ACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x)*0.5f, size.y));
            ImGui::SameLine();
            ImGui::Button("REACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x)*0.5f, size.y));
            ImGui::EndGroup();
            ImGui::SameLine();

            ImGui::Button("LEVERAGE\nBUZZWORD", size);
            ImGui::SameLine();

            ImGui::ListBoxHeader("List", size);
            ImGui::Selectable("Selected", true);
            ImGui::Selectable("Not Selected", false);
            ImGui::ListBoxFooter();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Text Baseline Alignment"))
        {
            ImGui::TextWrapped("(This is testing the vertical alignment that occurs on text to keep it at the same baseline as widgets. Lines only composed of text or \"small\" widgets fit in less vertical spaces than lines with normal widgets)");

            ImGui::Text("One\nTwo\nThree"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");

            ImGui::Text("Banana"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("One\nTwo\nThree");

            ImGui::Button("HOP##1"); ImGui::SameLine();
            ImGui::Text("Banana"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");

            ImGui::Button("HOP##2"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");

            ImGui::Button("TEST##1"); ImGui::SameLine();
            ImGui::Text("TEST"); ImGui::SameLine();
            ImGui::SmallButton("TEST##2");

            ImGui::AlignFirstTextHeightToWidgets(); // If your line starts with text, call this to align it to upcoming widgets.
            ImGui::Text("Text aligned to Widget"); ImGui::SameLine();
            ImGui::Button("Widget##1"); ImGui::SameLine();
            ImGui::Text("Widget"); ImGui::SameLine();
            ImGui::SmallButton("Widget##2");

            // Tree
            const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
            ImGui::Button("Button##1");
            ImGui::SameLine(0.0f, spacing);
            if (ImGui::TreeNode("Node##1")) { for (int i = 0; i < 6; i++) ImGui::BulletText("Item %d..", i); ImGui::TreePop(); }    // Dummy tree data

            ImGui::AlignFirstTextHeightToWidgets();         // Vertically align text node a bit lower so it'll be vertically centered with upcoming widget. Otherwise you can use SmallButton (smaller fit).
            bool node_open = ImGui::TreeNode("Node##2");  // Common mistake to avoid: if we want to SameLine after TreeNode we need to do it before we add child content.
            ImGui::SameLine(0.0f, spacing); ImGui::Button("Button##2");
            if (node_open) { for (int i = 0; i < 6; i++) ImGui::BulletText("Item %d..", i); ImGui::TreePop(); }   // Dummy tree data

                                                                                                                  // Bullet
            ImGui::Button("Button##3");
            ImGui::SameLine(0.0f, spacing);
            ImGui::BulletText("Bullet text");

            ImGui::AlignFirstTextHeightToWidgets();
            ImGui::BulletText("Node");
            ImGui::SameLine(0.0f, spacing); ImGui::Button("Button##4");

            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Columns"))
    {
        // Basic columns
        if (ImGui::TreeNode("Basic"))
        {
            ImGui::Text("Without border:");
            ImGui::Columns(3, "mycolumns3", false);  // 3-ways, no border
            ImGui::Separator();
            for (int n = 0; n < 14; n++)
            {
                char label[32];
                sprintf(label, "Item %d", n);
                if (ImGui::Selectable(label)) {}
                //if (ImGui::Button(label, ImVec2(-1,0))) {}
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::Separator();

            ImGui::Text("With border:");
            ImGui::Columns(4, "mycolumns"); // 4-ways, with border
            ImGui::Separator();
            ImGui::Text("ID"); ImGui::NextColumn();
            ImGui::Text("Name"); ImGui::NextColumn();
            ImGui::Text("Path"); ImGui::NextColumn();
            ImGui::Text("Flags"); ImGui::NextColumn();
            ImGui::Separator();
            const char* names[3] = { "One", "Two", "Three" };
            const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
            static int selected = -1;
            for (int i = 0; i < 3; i++)
            {
                char label[32];
                sprintf(label, "%04d", i);
                if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
                    selected = i;
                ImGui::NextColumn();
                ImGui::Text(names[i]); ImGui::NextColumn();
                ImGui::Text(paths[i]); ImGui::NextColumn();
                ImGui::Text("...."); ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::TreePop();
        }
    }
    
    if (ImGui::CollapsingHeader("Keyboard, Mouse & Focus"))
    {
        if (ImGui::TreeNode("Tabbing"))
        {
            ImGui::Text("Use TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            static char buf[32] = "dummy";
            ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
            ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
            ImGui::InputText("3", buf, IM_ARRAYSIZE(buf));
            ImGui::PushAllowKeyboardFocus(false);
            ImGui::InputText("4 (tab skip)", buf, IM_ARRAYSIZE(buf));
            //ImGui::SameLine(); ShowHelperMarker("Use ImGui::PushAllowKeyboardFocus(bool)\nto disable tabbing through certain widgets.");
            ImGui::PopAllowKeyboardFocus();
            ImGui::InputText("5", buf, IM_ARRAYSIZE(buf));
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Focus from code"))
        {
            bool focus_1 = ImGui::Button("Focus on 1"); ImGui::SameLine();
            bool focus_2 = ImGui::Button("Focus on 2"); ImGui::SameLine();
            bool focus_3 = ImGui::Button("Focus on 3");
            int has_focus = 0;
            static char buf[128] = "click on a button to set focus";

            if (focus_1) ImGui::SetKeyboardFocusHere();
            ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive()) has_focus = 1;

            if (focus_2) ImGui::SetKeyboardFocusHere();
            ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive()) has_focus = 2;

            ImGui::PushAllowKeyboardFocus(false);
            if (focus_3) ImGui::SetKeyboardFocusHere();
            ImGui::InputText("3 (tab skip)", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive()) has_focus = 3;
            ImGui::PopAllowKeyboardFocus();
            if (has_focus)
                ImGui::Text("Item with focus: %d", has_focus);
            else
                ImGui::Text("Item with focus: <none>");
            ImGui::TextWrapped("Cursor & selection are preserved when refocusing last used item in code.");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Dragging"))
        {
            ImGui::TextWrapped("You can use ImGui::GetMouseDragDelta(0) to query for the dragged amount on any widget.");
            ImGui::Button("Drag Me");
            if (ImGui::IsItemActive())
            {
                // Draw a line between the button and the mouse cursor
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->PushClipRectFullScreen();
                draw_list->AddLine(ImGui::CalcItemRectClosestPoint(ImGui::GetIO().MousePos, true, -2.0f), ImGui::GetIO().MousePos, ImColor(ImGui::GetStyle().Colors[ImGuiCol_Button]), 4.0f);
                draw_list->PopClipRect();
                ImVec2 value_raw = ImGui::GetMouseDragDelta(0, 0.0f);
                ImVec2 value_with_lock_threshold = ImGui::GetMouseDragDelta(0);
                ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
                ImGui::SameLine(); ImGui::Text("Raw (%.1f, %.1f), WithLockThresold (%.1f, %.1f), MouseDelta (%.1f, %.1f)", value_raw.x, value_raw.y, value_with_lock_threshold.x, value_with_lock_threshold.y, mouse_delta.x, mouse_delta.y);
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Keyboard & Mouse State"))
        {
            ImGuiIO& io = ImGui::GetIO();

            ImGui::Text("MousePos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            ImGui::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (io.MouseDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            ImGui::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
            ImGui::Text("Mouse dbl-clicked:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDoubleClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
            ImGui::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseReleased(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
            ImGui::Text("MouseWheel: %.1f", io.MouseWheel);

            ImGui::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("%d (%.02f secs)", i, io.KeysDownDuration[i]); }
            ImGui::Text("Keys pressed:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyPressed(i)) { ImGui::SameLine(); ImGui::Text("%d", i); }
            ImGui::Text("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyReleased(i)) { ImGui::SameLine(); ImGui::Text("%d", i); }
            ImGui::Text("KeyMods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");

            ImGui::Text("WantCaptureMouse: %s", io.WantCaptureMouse ? "true" : "false");
            ImGui::Text("WantCaptureKeyboard: %s", io.WantCaptureKeyboard ? "true" : "false");
            ImGui::Text("WantTextInput: %s", io.WantTextInput ? "true" : "false");

            ImGui::Button("Hovering me sets the\nkeyboard capture flag");
            if (ImGui::IsItemHovered())
                ImGui::CaptureKeyboardFromApp(true);
            ImGui::SameLine();
            ImGui::Button("Holding me clears the\nthe keyboard capture flag");
            if (ImGui::IsItemActive())
                ImGui::CaptureKeyboardFromApp(false);

            ImGui::TreePop();
        }
    }

    auto& style = ImGui::GetStyle();
    ShowStyleEditor(&style);
#endif

    ImGui::End();
}

void QImGui::BuildStyles()
{
    auto& style = ImGui::GetStyle();

// Roundness
    style.WindowRounding = 0;
    style.GrabRounding = 2.0f;
    style.FrameRounding = 6.0f;
    style.ChildWindowRounding = 6.0f;
    
// Padding
    style.FramePadding = ImVec2(6, 5);
    
// Colors
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
}

void QImGui::DoUpdate()
{
    if (repaintTimer_.isActive())
        repaintTimer_.start(repaintTimer_.interval());
    update();
}

void QImGui::moveEvent(QMoveEvent*)
{
    for (auto slave : recentSlaves_)
        slave->UpdatePositioning();
}

void QImGui::focusOutEvent(QFocusEvent* event)
{
    // Did this through the routing of a slave? If so then ignore.
    //if (QWidget::focusWidget() == this)
    //    return;

    QWidget* focusing = QApplication::focusWidget();
    if (focusing == this || focusing == 0x0 && recentSlaves_.size())
        return;

    // If it's one of our slaves taking focus then we don't care.
    for (auto recentSlave : recentSlaves_)
    {
        if (recentSlave == focusing)
            return;
    }

    forceFakeMouse_ = true;
    DoUpdate();
}

bool QImGui::focusNextPrevChild(bool next)
{
    disposeKeycode_ = IM_KEY_CODE_TAB;
    keysDown_[IM_KEY_CODE_TAB] = true;
    DoUpdate();
    return true;
}

void QImGui::keyPressEvent(QKeyEvent* event)
{
    event->accept();
    keysDown_[event->nativeVirtualKey()] = true;
    textInput_ = event->text();
    DoUpdate();
}

void QImGui::keyReleaseEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
    event->accept();

    // QT is a jerk
    if (event->key() == Qt::Key::Key_C && (event->modifiers() & Qt::KeyboardModifier::ControlModifier)) { copy(); return; }
    else if (event->key() == Qt::Key::Key_V && (event->modifiers() & Qt::KeyboardModifier::ControlModifier)) { paste(); return; }
    else if (event->key() == Qt::Key::Key_X && (event->modifiers() & Qt::KeyboardModifier::ControlModifier)) { cut(); return; }
    else if (event->key() == Qt::Key::Key_Z && (event->modifiers() & Qt::KeyboardModifier::ControlModifier)) { undo(); return; }
    else if (event->key() == Qt::Key::Key_Y && (event->modifiers() & Qt::KeyboardModifier::ControlModifier)) { redo(); return; }

    keysDown_[event->nativeVirtualKey()] = false;
    DoUpdate();
}

void QImGui::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
    DoUpdate();
}

void QImGui::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    mouseButtons_[0] = event->buttons().testFlag(Qt::MouseButton::LeftButton);
    mouseButtons_[1] = event->buttons().testFlag(Qt::MouseButton::RightButton);
    mouseButtons_[2] = event->buttons().testFlag(Qt::MouseButton::MiddleButton);
    setFocus(Qt::FocusReason::ActiveWindowFocusReason);
    DoUpdate();
}

void QImGui::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    mouseButtons_[0] = event->buttons().testFlag(Qt::MouseButton::LeftButton);
    mouseButtons_[1] = event->buttons().testFlag(Qt::MouseButton::RightButton);
    mouseButtons_[2] = event->buttons().testFlag(Qt::MouseButton::MiddleButton);
    DoUpdate();
}

void QImGui::wheelEvent(QWheelEvent* event)
{
    QWidget::wheelEvent(event);
    lastWheelDelta_ = event->delta();
    if (lastWheelDelta_ < 0)
        lastWheelDelta_ = -1;
    else if (lastWheelDelta_ > 0)
        lastWheelDelta_ = 1;
    DoUpdate();
}

void QImGui::dragEnterEvent(QDragEnterEvent* event)
{
    DragDropTarget* tgt = 0x0;
    if (GetDropTarget(event->pos(), DDT_DropOn | DDT_DropBelow | DDT_DropAbove, tgt))
    {
        event->acceptProposedAction();
        return;
    }
    event->accept();
    event->setDropAction(Qt::DropAction::IgnoreAction);
    return;
}

void QImGui::dragLeaveEvent(QDragLeaveEvent* event)
{
    //??do we care
}

void QImGui::dragMoveEvent(QDragMoveEvent* event)
{
    DragDropTarget* tgt = 0x0;
    if (GetDropTarget(event->pos(), DDT_DropOn | DDT_DropBelow | DDT_DropAbove, tgt))
    {
        event->acceptProposedAction();
        return;
    }
    event->accept();
    event->setDropAction(Qt::DropAction::IgnoreAction);
    return;
}

void QImGui::dropEvent(QDropEvent* event)
{
    DragDropTarget* tgt = 0x0;
    if (GetDropTarget(event->pos(), DDT_DropOn | DDT_DropBelow | DDT_DropAbove, tgt))
    {
        event->acceptProposedAction();
    }
    return;
}

void QImGui::paintEvent(QPaintEvent*)
{
    if (paintDepth_)
        return;
    ++paintDepth_;

    int dpiX = logicalDpiX();
    int dpiY = logicalDpiY();
    int pDpiX = physicalDpiX();
    int pDpiY = physicalDpiY();

    float scaleFrac = devicePixelRatioF();
    float otherFrac = devicePixelRatioFScale();
    QPainter painter;
    painter.begin(this);
    QFont paintFont(font_, this);
    paintFont.setPixelSize(20);
    painter.setFont(font_);
    //painter.setFont(paintFont);//font_);
    //painter.setRenderHint(QPainter::Antialiasing, true);
    PrepareRender();
    Render(&painter);
    EndRender();
    painter.end();

    isFirstRun_ = false;

    if (deferredCall_)
    {
        if (deferredResetMouse_)
        {
            mouseButtons_[0] = false;
            mouseButtons_[1] = false;
            mouseButtons_[2] = false;
            deferredResetMouse_ = false;
        }

        auto copy = deferredCall_;
        deferredCall_ = nullptr;
        copy();
    }

    if (imguiContext_->Windows.size() && autoFit_)
    {
        ImVec2 contentSize = imguiContext_->Windows[1]->SizeContents;

        // width still has to be set ... IMGUI has no clue on that
        if (contentSize.y != height())
        {
            autoFitSize_ = QSize(0, contentSize.y);
            setFixedHeight(max(1, contentSize.y));
            emit SignalSize(QSize(width(), contentSize.y));
        }
    }
    --paintDepth_;
}

float QImGui::GetEstimatedDeltaTime()
{
    float lastTime = timer_.elapsed() / 1000.01f;
    timer_.restart();
    return lastTime >= 0.0f ? lastTime : 0.0f;
}

void QImGui::Initialize()
{
    if (imguiContext_ == 0x0)
        imguiContext_ = ImGui::CreateContext();

    ImGui::SetCurrentContext(imguiContext_);

    auto& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = IM_KEY_CODE_TAB;
    io.KeyMap[ImGuiKey_Delete] = IM_KEY_CODE_DELETE;
    io.KeyMap[ImGuiKey_UpArrow] = IM_KEY_CODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = IM_KEY_CODE_DOWN;
    io.KeyMap[ImGuiKey_RightArrow] = IM_KEY_CODE_RIGHT;
    io.KeyMap[ImGuiKey_LeftArrow] = IM_KEY_CODE_LEFT;
    io.KeyMap[ImGuiKey_Backspace] = IM_KEY_CODE_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = IM_KEY_CODE_RETURN;
    io.KeyMap[ImGuiKey_Home] = IM_KEY_CODE_HOME;
    io.KeyMap[ImGuiKey_End] = IM_KEY_CODE_END;
    io.KeyMap[ImGuiKey_PageUp] = IM_KEY_CODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = IM_KEY_CODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Escape] = IM_KEY_CODE_ESCAPE;
    io.KeyMap[ImGuiKey_A] = IM_KEY_A;
    io.KeyMap[ImGuiKey_C] = IM_KEY_C;
    io.KeyMap[ImGuiKey_V] = IM_KEY_V;
    io.KeyMap[ImGuiKey_X] = IM_KEY_X;
    io.KeyMap[ImGuiKey_Y] = IM_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = IM_KEY_Z;

    io.RenderDrawListsFn = &QImGui::RenderDrawData;
    io.ShowTooltipFn = &QImGui::ShowTooltip;
    io.ShowTooltipUserData = this;
    io.GetClipboardTextFn = &QImGui::ReadClipboard;
    io.SetClipboardTextFn = &QImGui::WriteClipboard;

    if (imFont_ == 0x0)
    {
        imFont_ = CreateFont();
        io.Fonts->AddExternalFont(imFont_);
    }
}

void QImGui::Render(QPainter* painter)
{
    ImGuiIO& io = ImGui::GetIO();
    QDesktopWidget desktop;
    QRect screen = desktop.availableGeometry(this);

// Basic preparations
    // Alternatively this widget's width()/height() could be used, using screen dims however allows for popups
    //      to go off widget down and to the right.
    // TODO: use paint-offsets and instead place the owned window at the widget's actual screen coordinates.
    // Investigate: problems with spanning multiple screens ... generally the land of screwed to begin with - might not be worth concern
    
    // Get our top-left and substract it from the display size so that popups don't misinterpret their available space.
    const QPoint rootPosition = mapToGlobal(QPoint(0,0));
    io.DisplaySize = ImVec2(screen.x() + screen.width() - rootPosition.x(), screen.y() + screen.height() - rootPosition.y());
    io.DisplayFramebufferScale = ImVec2(1,1);
    io.DeltaTime = GetEstimatedDeltaTime();

// Mouse state
    QPoint mousePos = mapFromGlobal(QCursor::pos());
    bool isUnderMouse = underMouse();
    for (auto activeSlave : recentSlaves_)
        isUnderMouse |= activeSlave->underMouse();

    if (isUnderMouse)
        io.MousePos = ImVec2(mousePos.x(), mousePos.y());
    else
        io.MousePos = ImVec2(-10000, -10000); // hack numbers, *should* always be outside of bounds

    io.MouseDown[0] = mouseButtons_[0] || forceFakeMouse_; // forcing a click should only be done to deal with forcing popups to go away and using the -10k,-10k bogus mouse position
    io.MouseDown[1] = mouseButtons_[1];
    io.MouseDown[2] = mouseButtons_[2];
    
// Keyboard modifiers
    io.KeyCtrl = QApplication::keyboardModifiers() & Qt::KeyboardModifier::ControlModifier || forceCtrl_;
    io.KeyAlt = QApplication::keyboardModifiers() & Qt::KeyboardModifier::AltModifier;
    io.KeyShift = QApplication::keyboardModifiers() & Qt::KeyboardModifier::ShiftModifier;

// Keyboard State
    for (int i = 0; i < keysDown_.size(); ++i)
        io.KeysDown[i] = keysDown_[i];
    if (!textInput_.isEmpty())
        io.AddInputCharactersUTF8(textInput_.toUtf8().constData());
    textInput_.clear();

    io.MouseWheel = lastWheelDelta_;

// Setup callbacks to the latest painter and ourself
    ourPaintCallbackData_.first = painter;
    ourPaintCallbackData_.second = this;
    io.UserData = this;

// Go
    if (io.DisplaySize.x <= 0 || io.DisplaySize.y <= 0)
        return;

    ImGui::NewFrame();
    GenerateUI(painter, QSize(width(), height()));
}

void QImGui::PrepareRender()
{
    if (!imguiContext_)
        Initialize();

    ImGui::SetCurrentContext(imguiContext_);

// First time we prepare
    if (IsFirstRun())
        BuildStyles();

// Clear drop targets
    dragDropTargets_.clear();

// Prep our slaves
    for (auto slave : recentSlaves_)
        slave->PrepareFrame();
}

void QImGui::EndRender()
{
    ImGui::Render();

    // Clean out forced keycodes, CTRL, and mouse wheel.
    auto& io = ImGui::GetIO();
    if (disposeKeycode_)
        keysDown_[disposeKeycode_] = false;
    for (int i = 0; i < keysDown_.size(); ++i)
        keysDown_[i] = false;
    if (forceCtrl_)
        forceCtrl_ = false;
    disposeKeycode_ = 0;
    lastWheelDelta_ = 0;
    forceFakeMouse_ = false;

    ourPaintCallbackData_.first = 0x0;

    switch (imguiContext_->MouseCursor)
    {
    case ImGuiMouseCursor_None:
    case ImGuiMouseCursor_Arrow:
        setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        break;
    case ImGuiMouseCursor_Move:
        setCursor(QCursor(Qt::CursorShape::SizeAllCursor));
        break;
    case ImGuiMouseCursor_ResizeEW:
        setCursor(QCursor(Qt::CursorShape::SizeHorCursor));
        break;
    case ImGuiMouseCursor_ResizeNS:
        setCursor(QCursor(Qt::CursorShape::SizeVerCursor));
        break;
    case ImGuiMouseCursor_ResizeNWSE:
        setCursor(QCursor(Qt::CursorShape::SizeFDiagCursor));
        break;
    case ImGuiMouseCursor_ResizeNESW:
        setCursor(QCursor(Qt::CursorShape::SizeBDiagCursor));
        break;
    case ImGuiMouseCursor_TextInput:
        setCursor(QCursor(Qt::CursorShape::IBeamCursor));
        break;
    }

// Clean up our slaves
    for (unsigned i = 0; i < recentSlaves_.size(); ++i)
    {
        auto slave = recentSlaves_[i];
        if (!slave->ReceivedDraws())
        {
            recentSlaves_.erase(recentSlaves_.begin());
            availableSlaves_.push_back(slave);
            slave->setVisible(false);
            slave->hide();
            i -= 1;
        }
        else
        {
            //slave->paintOffset_.setY(ImGui::GetScrollX());
        }
    }
}

void QImGui::Destroy()
{
    if (imguiContext_)
    {
        ImGui::DestroyContext(imguiContext_);
        // we don't shut down because that'll nuke fonts
        //ImGui::Shutdown();
        imguiContext_ = 0x0;
    }
}

ImFont* QImGui::CreateFont()
{
    QFont font(FONT_FACE);
    font.setPixelSize(20);
    return new ImGuiFont(QFontMetrics(font));
    //return new ImGuiFont(QFontMetrics(QFont(FONT_FACE, FONT_SIZE)));
}

void QImGui::RenderDrawData(ImDrawData* data)
{
    QImGui* self = (QImGui*)ImGui::GetIO().UserData;
    QPainter* painter = self->ourPaintCallbackData_.first;
    if (!data->Valid || !painter || !self || !painter->isActive())
        return;

    int cmdsProcessed = 0;
    int triangles = 0;
    for (unsigned listIdx = 0; listIdx < data->CmdListsCount; ++listIdx)
    {
        //?? Popup/slave
        if (listIdx > 0 && data->CmdLists[listIdx]->_OwnerName[0] == '#')
        {
            auto cmdList = data->CmdLists[listIdx];
            auto slave = self->GetOrCreateSlave(cmdList->_OwnerName);
            slave->SetDrawList(cmdList);
        }
        else
            RenderDrawList(self, painter, data->CmdLists[listIdx], QPoint(0,0));
    }
}

void QImGui::RenderDrawList(QImGui* self, QPainter* painter, ImDrawList* cmdList, const QPoint& paintOffset)
{
    unsigned idxStart = 0;
    if (!painter->isActive())
        return;

    for (unsigned cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.size(); ++cmdIdx)
    {
        ImDrawCmd& cmd = cmdList->CmdBuffer[cmdIdx];
        auto clipRect = cmd.ClipRect;
        QSize sz(clipRect.z - clipRect.x, clipRect.w - clipRect.y);
        QPoint topLeft(clipRect.x, clipRect.y);

        painter->setClipRect(QRect(topLeft - paintOffset, sz));

        if (cmd.CmdType == IMCMD_Buffer || cmd.ElemCount)
        {
            for (unsigned i = 0; i < cmd.ElemCount; i += 3)
            {
                auto vertA = cmdList->VtxBuffer[cmdList->IdxBuffer[idxStart + i]];
                auto vertB = cmdList->VtxBuffer[cmdList->IdxBuffer[idxStart + i + 1]];
                auto vertC = cmdList->VtxBuffer[cmdList->IdxBuffer[idxStart + i + 2]];

#define ROUND_F(VAL) VAL = floorf(VAL) + 0.5f
                ROUND_F(vertA.pos.x);
                ROUND_F(vertA.pos.y);
                ROUND_F(vertB.pos.x);
                ROUND_F(vertB.pos.y);
                ROUND_F(vertC.pos.x);
                ROUND_F(vertC.pos.y);

                unsigned char r = ((unsigned char*)&vertA.col)[0];
                unsigned char g = ((unsigned char*)&vertA.col)[1];
                unsigned char b = ((unsigned char*)&vertA.col)[2];
                unsigned char a = ((unsigned char*)&vertA.col)[3];

                QColor color = QColor(r, g, b, a);
                painter->setPen(Qt::NoPen);
                //painter->setPen(QPen(color, 0.1f));
                painter->setBrush(color);
                std::vector<QPointF> pts = { QPointF(vertA.pos.x, vertA.pos.y) - paintOffset, QPointF(vertB.pos.x, vertB.pos.y) - paintOffset, QPointF(vertC.pos.x, vertC.pos.y) - paintOffset };
                painter->drawPolygon(pts.data(), pts.size());// , Qt::WindingFill);
            }
            idxStart += cmd.ElemCount;
        }
            

        if (cmd.CmdType == IMCMD_Text)
        {
            auto rawCol = cmd.CmdData.TextData.Color;
            unsigned char r = ((unsigned char*)&rawCol)[0];
            unsigned char g = ((unsigned char*)&rawCol)[1];
            unsigned char b = ((unsigned char*)&rawCol)[2];
            unsigned char a = ((unsigned char*)&rawCol)[3];

            painter->setPen(QPen(QColor(r, g, b, a), 1));
            painter->setBrush(Qt::NoBrush);

            auto pt = cmd.CmdData.TextData.Position;
            auto str = cmd.CmdData.TextData.Text;
            auto len = cmd.CmdData.TextData.Length;
            QString txt = (const char*)str;
            txt.chop(txt.length() - len);
            txt = txt.replace("\t", "    ");
            painter->drawText(QPoint(pt.x - paintOffset.x(), pt.y + imFont_->Ascent - paintOffset.y()), txt);
        }
        else if (cmd.CmdType == IMCMD_Rect)
        {
            auto rawCol = cmd.CmdData.RectData.LeftColor;
            
            const bool isGradient = cmd.CmdData.RectData.LeftColor != cmd.CmdData.RectData.RightColor;

            unsigned char r = ((unsigned char*)&rawCol)[0];
            unsigned char g = ((unsigned char*)&rawCol)[1];
            unsigned char b = ((unsigned char*)&rawCol)[2];
            unsigned char a = ((unsigned char*)&rawCol)[3];
            auto color = QColor(r, g, b, a);

            auto rect = cmd.CmdData.RectData.Rectangle;
            if (cmd.CmdData.RectData.Stroke)
            {
                painter->setPen(QPen(color, cmd.CmdData.RectData.Stroke));
                painter->setBrush(Qt::NoBrush);
            }
            else
            {
                painter->setPen(Qt::NoPen);
                if (!isGradient)
                    painter->setBrush(color);
                else
                {
                    QLinearGradient gradient(0, 0, 1, 0);
                    gradient.setCoordinateMode(QGradient::CoordinateMode::ObjectBoundingMode);
                    QGradientStops stops;

                    auto rawRightCol = cmd.CmdData.RectData.RightColor;
                    unsigned char r = ((unsigned char*)&rawRightCol)[0];
                    unsigned char g = ((unsigned char*)&rawRightCol)[1];
                    unsigned char b = ((unsigned char*)&rawRightCol)[2];
                    unsigned char a = ((unsigned char*)&rawRightCol)[3];
                    auto rightCol = QColor(r, g, b, a);

                    gradient.setColorAt(0, color);
                    gradient.setColorAt(1, rightCol);
                    painter->setBrush(gradient);
                }
            }

            QRect drawRect(QPoint(rect.x, rect.y) - paintOffset, QSize(rect.z - rect.x, rect.w - rect.y));
            if (cmd.CmdData.RectData.Rounding)
                painter->drawRoundedRect(drawRect, cmd.CmdData.RectData.Rounding, cmd.CmdData.RectData.Rounding);
            else
                painter->drawRect(drawRect);
        }
        else if (cmd.CmdType == IMCMD_Circle)
        {
            auto rawCol = cmd.CmdData.CircleData.Color;
            unsigned char r = ((unsigned char*)&rawCol)[0];
            unsigned char g = ((unsigned char*)&rawCol)[1];
            unsigned char b = ((unsigned char*)&rawCol)[2];
            unsigned char a = ((unsigned char*)&rawCol)[3];
            auto color = QColor(r, g, b, a);

            if (cmd.CmdData.CircleData.Stroke)
            {
                painter->setPen(QPen(color, cmd.CmdData.CircleData.Stroke));
                painter->setBrush(Qt::NoBrush);
            }
            else
            {
                painter->setPen(Qt::NoPen);
                painter->setBrush(color);
            }

            auto center = cmd.CmdData.CircleData.Center;
            auto rad = cmd.CmdData.CircleData.Radius;
            painter->drawEllipse(QPointF(center.x, center.y) - paintOffset, rad, rad);
        }
        else if (cmd.CmdType == IMCMD_Line)
        {
            auto& cmdData = cmd.CmdData.LineData;
            unsigned char r = ((unsigned char*)&cmdData.Color)[0];
            unsigned char g = ((unsigned char*)&cmdData.Color)[1];
            unsigned char b = ((unsigned char*)&cmdData.Color)[2];
            unsigned char a = ((unsigned char*)&cmdData.Color)[3];
            painter->setPen(QPen(QColor(r, g, b, a), cmdData.Width));
            
            painter->drawLine(cmdData.A.x - paintOffset.x(), cmdData.A.y - paintOffset.y(), cmdData.B.x - paintOffset.x(), cmdData.B.y - paintOffset.y());
        }
        else if (cmd.CmdType == IMCMD_Image)
        {
            auto& cmdData = cmd.CmdData.ImageData;
            auto& imageData = self->imageInfos_[(int)(cmdData.textureID_)];

            QRect drawRect(QPoint(cmdData.rect_.x, cmdData.rect_.y) - paintOffset, QSize(cmdData.rect_.z - cmdData.rect_.x, cmdData.rect_.w - cmdData.rect_.y));

            if (imageData.image_)
                painter->drawImage(drawRect, *imageData.image_);
            else
                painter->drawPixmap(drawRect, *imageData.pixmap_);
        }
    }
}

const char* QImGui::ReadClipboard(void* user_data)
{
    static char buffer[1024];
    memset(buffer, 0, 1024);
    QByteArray text = QApplication::clipboard()->text().toUtf8();;
    memcpy(buffer, text.constData(), text.size());
    return buffer;
}

void QImGui::WriteClipboard(void* user_data, const char* text)
{
    QString txt(text);
    QApplication::clipboard()->setText(txt);
}

void QImGui::ShowTooltip(void* user_data, const char* text)
{
    // Flickering mess
    if (user_data)
        ((QWidget*)user_data)->setToolTip(text);
}

// Utility functions

void ShowStyleEditor(ImGuiStyle* ref)
{
    ImGuiStyle& style = ImGui::GetStyle();

    // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to (else it compares to the default style)
    const ImGuiStyle default_style; // Default style
    if (ImGui::Button("Revert Style"))
        style = ref ? *ref : default_style;

    if (ref)
    {
        ImGui::SameLine();
        if (ImGui::Button("Save Style"))
            *ref = style;
    }

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.55f);

    if (ImGui::TreeNode("Rendering"))
    {
        ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
        ImGui::Checkbox("Anti-aliased shapes", &style.AntiAliasedShapes);
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, NULL, 2.0f);
        if (style.CurveTessellationTol < 0.0f) style.CurveTessellationTol = 0.10f;
        ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
        ImGui::PopItemWidth();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Settings"))
    {
        ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat("ChildWindowRounding", &style.ChildWindowRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
        ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
        ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 16.0f, "%.0f");
        ImGui::Text("Alignment");
        ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Colors"))
    {
        static int output_dest = 0;
        static bool output_only_modified = false;
        if (ImGui::Button("Copy Colors"))
        {
            if (output_dest == 0)
                ImGui::LogToClipboard();
            else
                ImGui::LogToTTY();
            ImGui::LogText("ImGuiStyle& style = ImGui::GetStyle();\n");
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const ImVec4& col = style.Colors[i];
                const char* name = ImGui::GetStyleColName(i);
                if (!output_only_modified || memcmp(&col, (ref ? &ref->Colors[i] : &default_style.Colors[i]), sizeof(ImVec4)) != 0)
                    ImGui::LogText("style.Colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);\n", name, 22 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
            }
            ImGui::LogFinish();
        }
        ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0"); ImGui::PopItemWidth();
        ImGui::SameLine(); ImGui::Checkbox("Only Modified Fields", &output_only_modified);

        static ImGuiColorEditMode edit_mode = ImGuiColorEditMode_RGB;
        ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditMode_RGB);
        ImGui::SameLine();
        ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditMode_HSV);
        ImGui::SameLine();
        ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditMode_HEX);
        //ImGui::Text("Tip: Click on colored square to change edit mode.");

        static ImGuiTextFilter filter;
        filter.Draw("Filter colors", 200);

        ImGui::BeginChild("#colors", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::PushItemWidth(-160);
        ImGui::ColorEditMode(edit_mode);
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColName(i);
            if (!filter.PassFilter(name))
                continue;
            ImGui::PushID(i);
            ImGui::ColorEdit4(name, (float*)&style.Colors[i], true);
            if (memcmp(&style.Colors[i], (ref ? &ref->Colors[i] : &default_style.Colors[i]), sizeof(ImVec4)) != 0)
            {
                ImGui::SameLine(); if (ImGui::Button("Revert")) style.Colors[i] = ref ? ref->Colors[i] : default_style.Colors[i];
                if (ref) { ImGui::SameLine(); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i]; }
            }
            ImGui::PopID();
        }
        ImGui::PopItemWidth();
        ImGui::EndChild();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Fonts", "Fonts (%d)", ImGui::GetIO().Fonts->Fonts.Size))
    {
        ImFontAtlas* atlas = ImGui::GetIO().Fonts;
        if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
        {
            ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
            ImGui::TreePop();
        }
        ImGui::PushItemWidth(100);
        for (int i = 0; i < atlas->Fonts.Size; i++)
        {
            ImFont* font = atlas->Fonts[i];
            ImGui::BulletText("Font %d: \'%s\', %.2f px, %d glyphs", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size);
            ImGui::TreePush((void*)(intptr_t)i);
            ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) ImGui::GetIO().FontDefault = font;
            ImGui::PushFont(font);
            ImGui::Text("The quick brown fox jumps over the lazy dog");
            ImGui::PopFont();
            if (ImGui::TreeNode("Details"))
            {
                ImGui::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
                ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
                ImGui::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
                //ImGui::Text("Texture surface: %d pixels (approx) ~ %dx%d", font->MetricsTotalSurface, (int)sqrtf((int)font->MetricsTotalSurface), (int)sqrtf((int)font->MetricsTotalSurface));
                for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
                {
                    ImFontConfig* cfg = &font->ConfigData[config_i];
                    ImGui::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
                }
                if (ImGui::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
                {
                    // Display all glyphs of the fonts in separate pages of 256 characters
                    const ImFont::Glyph* glyph_fallback = font->FallbackGlyph; // Forcefully/dodgily make FindGlyph() return NULL on fallback, which isn't the default behavior.
                    font->FallbackGlyph = NULL;
                    for (int base = 0; base < 0x10000; base += 256)
                    {
                        int count = 0;
                        for (int n = 0; n < 256; n++)
                            count += font->FindGlyph((ImWchar)(base + n)) ? 1 : 0;
                        if (count > 0 && ImGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
                        {
                            float cell_spacing = style.ItemSpacing.y;
                            ImVec2 cell_size(font->FontSize * 1, font->FontSize * 1);
                            ImVec2 base_pos = ImGui::GetCursorScreenPos();
                            ImDrawList* draw_list = ImGui::GetWindowDrawList();
                            for (int n = 0; n < 256; n++)
                            {
                                ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size.x + cell_spacing), base_pos.y + (n / 16) * (cell_size.y + cell_spacing));
                                ImVec2 cell_p2(cell_p1.x + cell_size.x, cell_p1.y + cell_size.y);
                                const ImFont::Glyph* glyph = font->FindGlyph((ImWchar)(base + n));;
                                draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                                font->RenderChar(draw_list, cell_size.x, cell_p1, ImGui::GetColorU32(ImGuiCol_Text), (ImWchar)(base + n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
                                if (glyph && ImGui::IsMouseHoveringRect(cell_p1, cell_p2))
                                {
                                    ImGui::BeginTooltip();
                                    ImGui::Text("Codepoint: U+%04X", base + n);
                                    ImGui::Separator();
                                    ImGui::Text("XAdvance+1: %.1f", glyph->XAdvance);
                                    //ImGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
                                    //ImGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
                                    ImGui::EndTooltip();
                                }
                            }
                            ImGui::Dummy(ImVec2((cell_size.x + cell_spacing) * 16, (cell_size.y + cell_spacing) * 16));
                            ImGui::TreePop();
                        }
                    }
                    font->FallbackGlyph = glyph_fallback;
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        static float window_scale = 1.0f;
        ImGui::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.1f");              // scale only this window
        ImGui::DragFloat("global scale", &ImGui::GetIO().FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.1f"); // scale everything
        ImGui::PopItemWidth();
        ImGui::SetWindowFontScale(window_scale);
        ImGui::TreePop();
    }

    //if (ImGui::Button("Popup"))
    {
        if (ImGui::BeginMenu("Options"))
        {
            static bool enabled = true;
            ImGui::MenuItem("Enabled", "", &enabled);
            //ImGui::BeginChild("child", ImVec2(0, 60), true);
            //for (int i = 0; i < 10; i++)
            //    ImGui::Text("Scrolling Text %d", i);
            //ImGui::EndChild();
            static float f = 0.5f;
            static int n = 0;
            ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
            ImGui::InputFloat("Input", &f, 0.1f);
            ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
            ImGui::EndMenu();
        }
    }

    ImGui::PopItemWidth();
}

void QImGui::SetupPropertyStyleSpacing(int components, float w_full)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiStyle& style = GImGui->Style;
    if (w_full <= 0.0f)
        w_full = ImGui::CalcItemWidth();
    const float w_item_one = ImMax(1.0f, (float)(int)((w_full - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
    const float w_item_last = ImMax(1.0f, (float)(int)(w_full - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));
    window->DC.ItemWidthStack.push_back(w_item_last);
    for (int i = 0; i < components - 1; i++)
        window->DC.ItemWidthStack.push_back(w_item_one);
    window->DC.ItemWidth = window->DC.ItemWidthStack.back();
}

ImTextureID QImGui::RegisterTexture(QImage* image)
{
    imageInfos_.push_back(image);
    return (void*)(imageInfos_.size() - 1);
}

ImTextureID QImGui::RegisterTexture(QPixmap* image)
{
    imageInfos_.push_back(image);
    return (void*)(imageInfos_.size() - 1);
}

void QImGui::CreateDragDropTarget(int typeCode, void* data, int traits)
{
    DragDropTarget tgt;

    auto minPt = ImGui::GetItemRectMin();
    auto size = ImGui::GetItemRectSize();

    tgt.rect_ = QRect(minPt.x, minPt.y, size.x, size.y);
    tgt.typeCode_ = typeCode;
    tgt.data_ = data;
    tgt.traits_ = traits;

    dragDropTargets_.push_back(tgt);
}

void QImGui::CreateDragDropTarget(QRect rect, int typeCode, void* data, int traits)
{
    DragDropTarget tgt;
    tgt.rect_ = rect;
    tgt.typeCode_ = typeCode;
    tgt.data_ = data;
    tgt.traits_ = traits;
    dragDropTargets_.push_back(tgt);
}

bool QImGui::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        keyPressEvent((QKeyEvent*)event);
        return true;
    }
    else if (event->type() == QEvent::KeyRelease)
    {
        keyPressEvent((QKeyEvent*)event);
        return true;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        mouseMoveEvent((QMouseEvent*)event);
        return true;
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        mousePressEvent((QMouseEvent*)event);
        return true;
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        mouseReleaseEvent((QMouseEvent*)event);
        return true;
    }
    else if (event->type() == QEvent::Wheel)
    {
        wheelEvent((QWheelEvent*)event);
        return true;
    }
    return false;
}

QImGuiSlave* QImGui::GetOrCreateSlave(const char* name)
{
    for (auto slave : recentSlaves_)
    {
        if (slave->objectName().compare(name) == 0)
            return slave;
    }

    if (availableSlaves_.size())
    {
        auto slave = availableSlaves_.back();
        slave->setObjectName(name);
        availableSlaves_.pop_back();
        slave->Reset();

        // Must add slave first before activation so we can detect it during focus changes
        recentSlaves_.push_back(slave);
        slave->show();
        slave->setVisible(true);
        //slave->activateWindow();
        return slave;
    }

    QImGuiSlave* slave = new QImGuiSlave(this);

    // track it first, so focus event works
    recentSlaves_.push_back(slave);

    slave->installEventFilter(this);
    slave->setObjectName(name);
    slave->setGeometry(QRect(0, 0, 1920, 1080));
    slave->show();
    //slave->activateWindow();
    return slave;
}

bool QImGui::GetDropTarget(const QPoint& pos, int traits, DragDropTarget* out)
{
    for (int i = 0; i < dragDropTargets_.size(); ++i)
    {
        auto& tgt = dragDropTargets_[i];
        if (tgt.traits_ & traits)
        {
            if (tgt.rect_.contains(pos))
            {
                out = &tgt;
                return true;
            }
        }
    }
    return false;
}

QSexyToolTip* QImGui::WrapInPopup(QImGui* self, QWidget* owner)
{
    QSexyToolTip* ret = new QSexyToolTip(owner);

    self->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    ret->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    ret->connect(self, &QImGui::SignalSize, [=](QSize sz) {
        ret->reposition();
    });

    QVBoxLayout* flyoutLayout = new QVBoxLayout(ret);
    //// Dear IMGUI already has an internal margine
    //flyoutLayout->setAlignment(Qt::AlignTop);
    flyoutLayout->setContentsMargins(0, 0, 0, 0);
    flyoutLayout->setSpacing(0);
    flyoutLayout->addWidget(self);
    ret->setContentWidget(self);
    return ret;
}

QSize QImGui::minimumSizeHint() const
{
    if (autoFit_ && autoFitSize_.height() > 0)
        return QSize(width(), autoFitSize_.height());
    return QWidget::minimumSizeHint();
}

void QImGui::hideEvent(QHideEvent* evt)
{
    QWidget::hideEvent(evt);
    for (unsigned i = 0; i < recentSlaves_.size(); ++i)
    {
        auto slave = recentSlaves_[i];
        recentSlaves_.erase(recentSlaves_.begin());
        availableSlaves_.push_back(slave);
        slave->setVisible(false);
        slave->hide();
        i -= 1;
    }
}