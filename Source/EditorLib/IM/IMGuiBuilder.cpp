#include "IMGuiBuilder.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>

void IMGuiBuilder::Push(QLayout* layout)
{
    if (!stack_.empty())
    {
        if (auto grid = dynamic_cast<QGridLayout*>(stack_.top().layout_))
            grid->addItem(layout, gridPos_.x, gridPos_.y, gridPos_.rspan, gridPos_.cspan);
        else
            stack_.top().layout_->addItem(layout);
    }
    gridPos_.Reset();
    stack_.push(StackRecord(layout));
}

void IMGuiBuilder::PushSilent(QLayout* layout)
{
    gridPos_.Reset();
    stack_.push(StackRecord(layout));
}

void IMGuiBuilder::Push(const char* name, QWidget* widget)
{
    if (!stack_.empty())
    {
        if (auto grid = dynamic_cast<QGridLayout*>(stack_.top().layout_))
            grid->addWidget(widget, gridPos_.x, gridPos_.y, gridPos_.rspan, gridPos_.cspan);
        else
            stack_.top().layout_->addWidget(widget);
    }
    gridPos_.Reset();
    widgets_[name] = widget;
    lastWidget_ = widget;
}

IMGuiBuilder* IMGuiBuilder::PushHorizontal()
{
    QHBoxLayout* layout = new QHBoxLayout(stack_.top().widget_);
    Push(layout);
    return this;
}

IMGuiBuilder* IMGuiBuilder::PushVertical()
{
    QVBoxLayout* layout = new QVBoxLayout(stack_.top().widget_);
    Push(layout);
    return this;
}

IMGuiBuilder* IMGuiBuilder::PushGrid()
{
    QGridLayout* grid = new QGridLayout();
    Push(grid);
    return this;
}

IMGuiBuilder* IMGuiBuilder::PushGroup() 
{
    QGroupBox* box = new QGroupBox("", stack_.top().widget_);
    QVBoxLayout* layout = new QVBoxLayout();
    box->setLayout(layout);
    lastWidget_ = box;
    stack_.top().layout_->addWidget(box);
    PushSilent(layout);
    return this;
}

IMGuiBuilder* IMGuiBuilder::PushScroll()
{
    QScrollArea* scroll = new QScrollArea();
    QVBoxLayout* layout = new QVBoxLayout();
    scroll->setLayout(layout);
    stack_.top().layout_->addWidget(scroll);
    PushSilent(layout);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Pop()
{
    stack_.pop();
    return this;
}

// Controls
IMGuiBuilder* IMGuiBuilder::TextEdit(const char* name, std::string* text)
{
    QLineEdit* edit = new QLineEdit();
    edit->setText((*text).c_str());
    edit->connect(edit, &QLineEdit::textChanged, [=](const QString& txt) {
        *text = txt.toStdString();
    });
    Push(name, edit);
    return this;
}

IMGuiBuilder* IMGuiBuilder::TextEdit(const char* name, QString* text)
{
    QLineEdit* edit = new QLineEdit();
    edit->setText(*text);
    edit->connect(edit, &QLineEdit::textChanged, [=](const QString& txt) {
        *text = txt;
    });
    Push(name, edit);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Button(const char* name, const QString& text, void(*FUNC)())
{
    QPushButton* edit = new QPushButton(text);
    // This is about as dumb as it gets
    edit->connect(edit, &QPushButton::clicked, FUNC);
    Push(name, edit);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Slider(const char* name, int* value, int min, int max, int step)
{
    QSlider* edit = new QSlider();
    edit->setValue(*value);
    edit->setRange(min, max);
    edit->setSingleStep(step);
    Push(name, edit);
    return this;
}

IMGuiBuilder* IMGuiBuilder::IntSpinner(const char* name, int* value, int min, int max, int step)
{
    QSpinBox* edit = new QSpinBox();
    edit->setValue(*value);
    edit->setRange(min, max);
    edit->setSingleStep(step);
    edit->connect(edit, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int val) {
        *value = val;
    });
    Push(name, edit);
    return this;
}

IMGuiBuilder* IMGuiBuilder::FloatSpinner(const char* name, float* value, float min, float max, float step)
{
    QDoubleSpinBox* edit = new QDoubleSpinBox();
    edit->setValue(*value);
    edit->setRange(min, max);
    edit->setSingleStep(step);
    edit->connect(edit, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double val) {
        *value = val;
    });
    Push(name, edit);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Label(const char* name, const QString& text)
{
    QLabel* edit = new QLabel(text);
    Push(name, edit);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Radio(const char* name, const QString& text, bool* value)
{
    QRadioButton* edit = new QRadioButton(text);
    widgets_[name] = edit;
    edit->setChecked(*value);
    edit->connect(edit, &QRadioButton::toggled, [=](bool state) {
        *value = state;
    });
    Push(name, edit);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Checkbox(const char* name, const QString& text, bool* value)
{
    QCheckBox* edit = new QCheckBox(text);
    edit->setChecked(*value);
    widgets_[name] = edit;
    edit->connect(edit, &QCheckBox::toggled, [=](bool state) { *value = state; });
    Push(name, edit);
    return this;
}

IMGuiBuilder* IMGuiBuilder::ComboBox(const char* name, const char** items, int* index)
{
    QComboBox* edit = new QComboBox();
    int i = 0;
    const char** item = items;
    while (item)
    {
        edit->addItem(*item);
        ++item;
    }
    edit->setCurrentIndex(*index);
    edit->connect(edit, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int idx) { *index = idx; });
    return this;
}

IMGuiBuilder* IMGuiBuilder::CustomWidget(const char* name, QWidget* widget)
{
    Push(name, widget);
    return this;
}

IMGuiBuilder* IMGuiBuilder::HSpacer(const char* name)
{
    stack_.top().layout_->addItem(new QSpacerItem(1, 0, QSizePolicy::Expanding));
    return this;
}

IMGuiBuilder* IMGuiBuilder::VSpacer(const char* name)
{
    stack_.top().layout_->addItem(new QSpacerItem(0, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    return this;
}

IMGuiBuilder* IMGuiBuilder::Expand()
{
    if (stack_.top().layout_)
    {
        int ct = stack_.top().layout_->count();
        if (auto box = dynamic_cast<QBoxLayout*>(stack_.top().layout_))
            box->setStretch(ct - 1, 1);
    }
    return this;
}

IMGuiBuilder* IMGuiBuilder::MinWidth(int sz)
{
    if (lastWidget_)
        lastWidget_->setMinimumWidth(sz);
    return this;
}

IMGuiBuilder* IMGuiBuilder::MinHeight(int sz)
{
    if (lastWidget_)
        lastWidget_->setMinimumHeight(sz);
    return this;
}

IMGuiBuilder* IMGuiBuilder::MinSize(int x, int y)
{
    if (lastWidget_)
        lastWidget_->setMinimumSize(x, y);
    return this;
}

IMGuiBuilder* IMGuiBuilder::MaxWidth(int sz)
{
    if (lastWidget_)
        lastWidget_->setMaximumWidth(sz);
    return this;
}
IMGuiBuilder* IMGuiBuilder::MaxHeight(int sz)
{
    if (lastWidget_)
        lastWidget_->setMaximumHeight(sz);
    return this;
}
IMGuiBuilder* IMGuiBuilder::MaxSize(int x, int y)
{
    if (lastWidget_)
        lastWidget_->setMaximumSize(x, y);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Width(int sz)
{        
    if (lastWidget_)
        lastWidget_->setFixedWidth(sz);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Height(int sz)
{
    if (lastWidget_)
        lastWidget_->setFixedHeight(sz);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Size(int x, int y)
{
    if (lastWidget_)
        lastWidget_->setFixedSize(x, y);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Spacing(int sp)
{
    stack_.top().layout_->setSpacing(sp);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Margin(int m)
{
    stack_.top().layout_->setMargin(m);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Margin(int l, int t, int r, int b)
{
    stack_.top().layout_->setContentsMargins(l, t, r, b);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Tip(const QString& tip)
{
// Layout an sections
    if (lastWidget_)
        lastWidget_->setToolTip(tip);
    return this;
}

IMGuiBuilder* IMGuiBuilder::Cell(int x, int y, int cspan, int rspan)
{
    gridPos_.x = x;
    gridPos_.y = y;
    gridPos_.cspan = cspan;
    gridPos_.rspan = rspan;
    return this;
}