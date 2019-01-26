#pragma once

#include <EditorLib/editorlib_global.h>

#include <QAction>
#include <QToolButton>

/// Button class for use in ribbons.
class EDITORLIB_EXPORT RibbonButton : public QToolButton
{
public:
    /// Construct default.
    RibbonButton();
    /// Construct for a QAction.
    RibbonButton(QAction* action);

    /// Initializes the button as large.
    void InitLarge();
    /// Initializes the button as medium.
    void InitMedium();
    /// Initializes the button as small.
    void InitSmall(bool wide = false);

    /// Configures this button as a large ribbon.
    void MakeLarge();
    /// Configures this button as a medium button (2 in a column).
    void MakeMedium(bool wide = false);
    /// Configures this button as a small button (3 in a column).
    void MakeSmall(bool wide = false);
    void MakeRegular();

    QSize GetSizeForType(int btnType) const;

    /// Sets the layout weight (priority of promoting or demoting this button).
    void SetWeight(float value) { weight_ = value; }
    /// Gets the layout weight used for demoting or promoting the button.
    float GetWeight() const { return weight_; }

    /// Returns the desired default mode for the button.
    int GetDesiredButtonType() const { return buttonType_; }
    /// Returns the wide stats of the button.
    bool IsWide() const { return isWide_; }

private:
    float weight_ = 1.0f;
    bool isWide_ = false;
    int buttonType_ = 0; // default as Large
};