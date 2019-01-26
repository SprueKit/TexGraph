#pragma once

#include <QWidget>
#include <QBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QStackedWidget>

#include <vector>

namespace SprueEditor
{
    class SprueKitEditor;

    /// Base-class for ribbon style application menu
    class BaseApplicationMenu : public QWidget
    {
        Q_OBJECT
    public:
        /// Construct.
        BaseApplicationMenu(SprueKitEditor* editor);

        /// Adds a new button to the menu.
        QPushButton* AddButton(QIcon icon, const QString& text);
        /// Adds a button with a page widget to the menu.
        QPushButton* AddExpansionButton(QIcon icon, const QString& text, QWidget* insertWidget);
        /// Adds a horizontal seperator to the buttons pane.
        void AddSeperator();

    private:
        /// Vertical layout the buttons go into.
        QVBoxLayout* buttonsLayout_;
        /// Contains the pages for the expansion buttons.
        QStackedWidget* expansionPages_;
        /// List of buttons that are standard buttons.
        std::vector<QPushButton*> buttons_;
        /// List of buttons that are expansion buttons.
        std::vector<QPushButton*> expansionButtons_;
    };

}