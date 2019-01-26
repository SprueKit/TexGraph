#pragma once

#include <EditorLib/editorlib_global.h>

#include <stack>
#include <string>
#include <unordered_map>

class QLayout;
class QObject;
class QWidget;

/// Builder class uses an IM-style to populate a layout and associate editors with fields.
/// See IMGuiWidget which implements IMGuiBuilder to provide a quick IM style widget.
class EDITORLIB_EXPORT IMGuiBuilder
{
    IMGuiBuilder(const IMGuiBuilder&) = delete;
    void operator=(const IMGuiBuilder&) = delete;
public:
    IMGuiBuilder() { }
    IMGuiBuilder(QLayout* layout) { Start(layout); }
    virtual ~IMGuiBuilder() { }

    // Only named differently for clarity of intent
    inline void Start(QLayout* topMost) { Push(topMost); }

    // Layout an sections
    IMGuiBuilder* PushHorizontal();
    IMGuiBuilder* PushVertical();
    IMGuiBuilder* PushGrid();
    IMGuiBuilder* PushGroup();
    IMGuiBuilder* PushScroll();
    IMGuiBuilder* Pop();

    // Controls
    IMGuiBuilder* TextEdit(const char* name, std::string* text);
    IMGuiBuilder* TextEdit(const char* name, QString* text);
    IMGuiBuilder* Button(const char* name, const QString& text, void(*FUNC)());
    IMGuiBuilder* Slider(const char* name, int* value, int min = 0, int max = 100, int step = 1);
    IMGuiBuilder* IntSpinner(const char* name, int* value, int min, int max, int step = 1);
    IMGuiBuilder* FloatSpinner(const char* name, float* value, float min = 0.0f, float max = 1.0f, float step = 0.1f);
    IMGuiBuilder* Label(const char* name, const QString& text);
    IMGuiBuilder* Radio(const char* name, const QString& text, bool* value);
    IMGuiBuilder* Checkbox(const char* name, const QString& text, bool* value);
    IMGuiBuilder* ComboBox(const char* name, const char** items, int* index);
    IMGuiBuilder* CustomWidget(const char* name, QWidget* widget);
    IMGuiBuilder* HSpacer(const char* name);
    IMGuiBuilder* VSpacer(const char* name);
    
    // Behaviour
    IMGuiBuilder* Expand();
    IMGuiBuilder* MinWidth(int sz);
    IMGuiBuilder* MinHeight(int sz);
    IMGuiBuilder* MinSize(int x, int y);
    IMGuiBuilder* MaxWidth(int sz);
    IMGuiBuilder* MaxHeight(int sz);
    IMGuiBuilder* MaxSize(int x, int y);
    IMGuiBuilder* Width(int sz);
    IMGuiBuilder* Height(int sz);
    IMGuiBuilder* Size(int x, int y);
    IMGuiBuilder* Spacing(int sp);
    IMGuiBuilder* Margin(int m);
    IMGuiBuilder* Margin(int l, int t, int r, int b);
    IMGuiBuilder* Tip(const QString& tip);
    IMGuiBuilder* Cell(int x, int y, int cspan = 1, int rspan = 1);

protected:
    void Push(QLayout* layout);
    void PushSilent(QLayout* layout);
    void Push(const char* name, QWidget* widget);

    struct StackRecord {
        QLayout* layout_ = 0x0;
        QWidget* widget_ = 0x0;

        StackRecord(QLayout* obj)  :
            layout_(0x0)
        {
        }

        StackRecord(QWidget* obj) :
            widget_(0x0)
        {
        }
    };

    struct NextGridPos {
        int x = 0;
        int y = 0;
        int cspan = 1;
        int rspan = 1;

        void Reset() {
            x = y = 0;
            cspan = rspan = 1;
        }
    };

    NextGridPos gridPos_;
    QWidget* lastWidget_ = 0x0;
    std::stack<StackRecord> stack_;
    std::unordered_map<std::string, QWidget*> widgets_;
};