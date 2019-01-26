#pragma once

#include <QListWidget>
#include <QStackedLayout>
#include <QWidget>

/// Ribbon style application menu for display as a popup.
/// May contain either command buttons or split buttons.
class ApplicationMenu : public QWidget
{
public:
    ApplicationMenu();
    virtual ~ApplicationMenu();

private:
    /// List containing the commands/splits on the left. Styled to resemble buttons more than list items.
    QListWidget* listWidget_;
    /// Stack for containing the pages on the right for any expansion buttons.
    QStackedLayout* splitPanel_;
};