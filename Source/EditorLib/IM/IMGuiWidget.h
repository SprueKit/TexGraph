#pragma once

#include <EditorLib/editorlib_global.h>
#include <EditorLib/IM/IMGuiBuilder.h>

#include <QWidget>

/// A QWidget with immediate mode style behaviour.
class EDITORLIB_EXPORT IMGuiWidget : public QWidget, public IMGuiBuilder
{
    Q_OBJECT;
public:
    IMGuiWidget(QWidget* parent);
    virtual ~IMGuiWidget();

private:
};