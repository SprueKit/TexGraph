#pragma once

#include <EditorLib/editorlib_global.h>

#include <QWidget>

#include <QToolButton>
#include <QAction>

/// Similar to a "Large" RibbonButton expect it includes a lower expansion button.
class EDITORLIB_EXPORT RibbonExpansionButton : public QToolButton
{
public:
    /// Construct.
    RibbonExpansionButton();
    /// Construct with an action.
    RibbonExpansionButton(QAction* mainAction);

    /// Returns the child expansion button. Most likely a QSexyTooltip will be attached.
    QToolButton* GetExpandButton() { return expandButton_; }

private:
    /// Performs the common initialization of setting the button dimensions and creating the child expansion button.
    void init();
    /// Stored in a layout attached to the button where it is aligned to the bottom.
    QToolButton* expandButton_;
};