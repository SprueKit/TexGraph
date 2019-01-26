#pragma once

#include <EditorLib/editorlib_global.h>

#include <QAbstractButton>
#include <QBoxLayout>
#include <QLabel>
#include <QWidget>

/// A section in a MS style ribbon control. Handles the layout of its contents.
class EDITORLIB_EXPORT RibbonSection : public QWidget
{
public:
    /// Construct with the given title.
    RibbonSection(const QString& title);
    /// Destruct.
    virtual ~RibbonSection();

    /// Returns the content layout into which controls should be inserted into the ribbon.
    QHBoxLayout* GetContentLayout() { return contentLayout_; }
    /// Returns the main layout (contains the content layout, space, and title label).
    QVBoxLayout* GetMainLayout() { return mainLayout_; }

    /// Returns the QLabel for the lower aligned title.
    QLabel* GetTitleLabel() { return titleLabel_; }

    /// Helper function for adding widgets to the section, will automatically create the vertical layout if needed.
    void AddWidgets(QWidget* a, QWidget* b = 0x0, QWidget* c = 0x0);
    /// Helper function for adding 4 widgets in a quad layout. Assumes both columns are to be equal sized (two vertical layouts).
    void AddQuad(QWidget* topLeft, QWidget* bottomLeft, QWidget* topRight, QWidget* bottomRight);

protected:
    /// The popup widget to display when expanding hidden contents.
    QWidget* expansionWidget_;
    /// Button used to activate the expansion widget.
    QAbstractButton* expandButton_;
    /// The label of the title along the botom.
    QLabel* titleLabel_;
    /// The layout used for the actual contents of the widget.
    QHBoxLayout* contentLayout_;
    /// The layout that the content layout and title, label/expand box.
    QVBoxLayout* mainLayout_;
};