#include "BaseApplicationMenu.h"

#include "../SprueKitEditor.h"

#include <EditorLib/Controls/HoverButton.h>

#include <QBoxLayout>
#include <QFrame>

namespace SprueEditor
{
    BaseApplicationMenu::BaseApplicationMenu(SprueKitEditor* editor)
    {
        //setMinimumSize(350, 350);
        //setMaximumSize(350, 800);
        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setAlignment(Qt::AlignLeft);
        layout->setMargin(0);
        
        buttonsLayout_ = new QVBoxLayout();
        buttonsLayout_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        layout->addLayout(buttonsLayout_);

        expansionPages_ = new QStackedWidget();
        layout->addWidget(expansionPages_);

        /// Add an empty widget.
        expansionPages_->addWidget(new QWidget());
    }

    QPushButton* BaseApplicationMenu::AddButton(QIcon icon, const QString& text)
    {
        HoverButton* btn = new HoverButton();
        btn->setIcon(icon);
        btn->setText(text);
        btn->setMinimumWidth(128);
        buttons_.push_back(btn);
        buttonsLayout_->addWidget(btn);
        connect(btn, &QPushButton::clicked, [=](bool) {
            expansionPages_->setCurrentIndex(0);
        });
        connect(btn, &HoverButton::mouseEntered, [=]() {
            expansionPages_->setCurrentIndex(0);
        });

        return btn;
    }

    QPushButton* BaseApplicationMenu::AddExpansionButton(QIcon icon, const QString& text, QWidget* insertWidget)
    {
        HoverButton* btn = new HoverButton();
        btn->setIcon(icon);
        btn->setText(text);
        btn->setMinimumWidth(128);
        expansionButtons_.push_back(btn);
        expansionPages_->addWidget(insertWidget);

        QHBoxLayout* layout = new QHBoxLayout(btn);
        layout->setAlignment(Qt::AlignRight);
        layout->setMargin(0);
        buttonsLayout_->addWidget(btn);

        //NOTE! DELIBERATELY USING THE POST-ADD value, because it accounts for the empty page 0 in the pages widget
        const unsigned currentIndex = expansionButtons_.size();

        connect(btn, &QPushButton::clicked, [=](bool) {
            expansionPages_->setCurrentIndex(currentIndex);
        });
        connect(btn, &HoverButton::mouseEntered, [=]() {
            expansionPages_->setCurrentIndex(currentIndex);
        });

        return btn;
    }

    void BaseApplicationMenu::AddSeperator()
    {
        QFrame* line = new QFrame();
        line->setFixedHeight(1);
        line->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Plain);
        buttonsLayout_->addWidget(line);
    }
}