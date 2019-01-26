#pragma once

#include <EditorLib/editorlib_global.h>

#include <QScrollArea>

class QHBoxLayout;
class QVBoxLayout;
class QWidget;

class EDITORLIB_EXPORT ScrollAreaWidget : public QScrollArea
{
    Q_OBJECT;
public:
    ScrollAreaWidget();
    virtual ~ScrollAreaWidget();

    QVBoxLayout* GetLayout() { return vBoxLayout_; }

    QWidget* GetHiddenWidget() { return hiddenWidget_; }

protected:
    QHBoxLayout* AddHBoxLayout();

    QVBoxLayout* vBoxLayout_;
    QWidget* hiddenWidget_;
};