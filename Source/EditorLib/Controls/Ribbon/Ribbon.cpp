#include "Ribbon.h"

#include "RibbonPage.h"

#include <EditorLib/Dialogs/ConfigDlg.h>
#include <EditorLib/Localization/Localizer.h>
#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsValue.h>
#include <EditorLib/ApplicationCore.h>
#include <EditorLib/Controls/Document/OpenDocumentsButton.h>

#include <QPushButton>
#include <QTabBar>
#include <QToolButton>

Ribbon::Ribbon(QWidget* parent) :
    QTabWidget(parent),
    oldHeight_(150)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setMovable(false);
    setTabsClosable(false);
    setContentsMargins(0, 0, 0, 0);
    setDocumentMode(false);
    connect(this, &QTabWidget::tabBarClicked, this, &Ribbon::TabClicked);

    // Top Right section for toggle/settings
    {
        QToolButton* settingsBtn = new QToolButton();
        settingsBtn->setIcon(QIcon(":/Images/godot/icon_tools.png"));
        settingsBtn->setToolTip(Localizer::Translate("Settings"));
        connect(settingsBtn, &QToolButton::clicked, [](bool) {
            ConfigDlg::Show(Settings::GetInstance());
        });
        settingsBtn->setIconSize(QSize(24, 24));

        //toggleRibbon_ = new QToolButton();
        //toggleRibbon_->setIcon(QIcon(":/qss_icons/rc/up_arrow.png"));
        //toggleRibbon_->setToolTip(Localizer::Translate("Toggle ribbon expansion"));
        //toggleRibbon_->setCheckable(true);
        //toggleRibbon_->setChecked(true);

        QWidget* widget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(settingsBtn);
        //layout->addWidget(toggleRibbon_);

        setCornerWidget(widget);

        //connect(toggleRibbon_, &QToolButton::toggled, this, &Ribbon::SetRibbonDisplayed);
        //connect(toggleRibbon_, &QToolButton::toggled, [=](bool state) {
        //    if (state)
        //        toggleRibbon_->setIcon(QIcon(":/qss_icons/rc/up_arrow.png"));
        //    else
        //        toggleRibbon_->setIcon(QIcon(":/qss_icons/rc/down_arrow.png"));
        //});
    }

    // Create the application button
    {
        QWidget* appButton = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(appButton);
        layout->setContentsMargins(3, 0, 7, 0);

        applicationButton_ = new QPushButton("  File  ");
        applicationButton_->setObjectName("application_button");
        layout->addWidget(applicationButton_);

        documentButton_ = new OpenDocumentsButton();
        documentButton_->setObjectName("document_button");
        layout->addWidget(documentButton_);

        setCornerWidget(appButton, Qt::Corner::TopLeftCorner);

        //connect(actualButton, &QPushButton::clicked, this, &Ribbon::HandleApplicationButton);
    }
}

Ribbon::~Ribbon()
{

}

void Ribbon::AddPage(RibbonPage* page)
{
    if (page->IsActive())
        addTab(page, page->GetPageTitle());
    connect(page, &RibbonPage::ActiveChanged, this, &Ribbon::PageActivityChanged);
    allpages_.push_back(page);
    emit PageAdded(page);
}

RibbonPage* Ribbon::GetPage(unsigned index)
{
    return (RibbonPage*)widget(index);
}

void Ribbon::RemovePage(RibbonPage* page)
{
    int index = indexOf(page);
    if (index != -1)
        removeTab(index);
    disconnect(page, &RibbonPage::ActiveChanged, this, &Ribbon::PageActivityChanged);
    auto found = std::find(allpages_.begin(), allpages_.end(), page);
    if (found != allpages_.end())
        allpages_.erase(found);
    emit PageRemoved(page);
}

void Ribbon::SetAutoHide(bool state)
{
    autoHide_ = state;
    SetRibbonDisplayed(state);
    Settings::GetInstance()->GetValue("Secret GUI State/Ribbon Collapsed")->value_ = autoHide_;
}

void Ribbon::PageActivityChanged(RibbonPage* page)
{
    // Set the enabled/disabled state for the tab bar
    if (page->IsActive() && indexOf(page) == -1)
    {
        int lastActivePage = 0;
        const int numPages = count();
        for (int i = 0; i < allpages_.size(); ++i)
        {
            if (allpages_[i]->IsActive() && allpages_[i] != page)
                ++lastActivePage;
            else if (allpages_[i] == page)
            {
                insertTab(lastActivePage, page, page->GetPageTitle());
                break;
            }
        }
    }
    else if (!page->IsActive())
        removeTab(indexOf(page));
}

void Ribbon::TabClicked(int index)
{
    if (autoHide_)
    {
        if (QWidget* showTab = widget(index))
        {
            if (((RibbonPage*)showTab)->IsActive())
            {
                if (showTab->isVisible())
                {
                    showTab->setVisible(false);
                    oldHeight_ = height();
                    setFixedHeight(tabBar()->height());
                    updateGeometry();
                }
                else
                {
                    showTab->setVisible(true);
                    setFixedHeight(shownHeight_);// oldHeight_);
                    updateGeometry();
                }
            }
        }
    }
}

void Ribbon::SetRibbonDisplayed(bool showRibbon)
{
    autoHide_ = !showRibbon;
    if (!showRibbon)
    {
        for (int i = 0; i < count(); ++i)
            widget(i)->setVisible(false);
        oldHeight_ = height();
        setFixedHeight(tabBar()->height());
        updateGeometry();
    }
    else
    {
        for (int i = 0; i < count(); ++i)
            widget(i)->setVisible(GetPage(i)->IsActive() && currentIndex() == i);
        setFixedHeight(shownHeight_);//setFixedHeight(oldHeight_);
        updateGeometry();
    }
}

void Ribbon::HandleApplicationButton(bool)
{
    emit ApplicationButtonClicked();
}