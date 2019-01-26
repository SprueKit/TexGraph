#include "LocalizedWidgets.h"

#include "../SprueKitEditor.h"

#include <EditorLib/Localization/Localizer.h>
#include <EditorLib/ThirdParty/QSexyToolTip.h>

#include <QApplication>
#include <QDir>
#include <QBoxLayout>
#include <QFile>
#include <QTextBrowser>
#include <QToolButton>

namespace SprueEditor
{

    std::vector<LocalizedDock*> LocalizedDock::allDocks_;

    LocalizedLabel::LocalizedLabel(const QString& text, const QString& tip) :
        QLabel(),
        textKey_(text),
        tip_(tip)
    {
        setText(Localizer::Translate(text));
        setToolTip(Localizer::Translate(tip));
        connect(Localizer::GetInstance(), &Localizer::LanguageChanged, this, &LocalizedLabel::LanguageChanged);
    }

    void LocalizedLabel::LanguageChanged()
    {
        setText(Localizer::Translate(textKey_));
        setToolTip(Localizer::Translate(tip_));
    }

    LocalizedButton::LocalizedButton(const QString& text, const QString& tip) :
        QPushButton(text),
        textKey_(text),
        tooltipKey_(tip)
    {
        setText(Localizer::Translate(text));
        setToolTip(Localizer::Translate(tip));
        connect(Localizer::GetInstance(), &Localizer::LanguageChanged, this, &LocalizedButton::LanguageChanged);
    }

    void LocalizedButton::LanguageChanged()
    {
        setText(Localizer::Translate(textKey_));
        setToolTip(Localizer::Translate(tooltipKey_));
    }

    LocalizedCheckBox::LocalizedCheckBox(const QString& text, const QString& tip) :
        QCheckBox(),
        textKey_(text),
        tooltipKey_(tip)
    {
        setText(Localizer::Translate(textKey_));
        setToolTip(Localizer::Translate(tooltipKey_));
        connect(Localizer::GetInstance(), &Localizer::LanguageChanged, this, &LocalizedCheckBox::LanguageChanged);
    }

    void LocalizedCheckBox::LanguageChanged()
    {
        setText(Localizer::Translate(textKey_));
        setToolTip(Localizer::Translate(tooltipKey_));
    }

    LocalizedComboBox::LocalizedComboBox() :
        QComboBox()
    {
        connect(Localizer::GetInstance(), &Localizer::LanguageChanged, this, &LocalizedComboBox::LanguageChanged);
    }

    void LocalizedComboBox::AddItem(const QString &text, const QVariant &userData)
    {
        addItem(Localizer::Translate(text), userData);
        originalItemText_.push_back(text);
    }

    void LocalizedComboBox::AddItem(const QIcon &icon, const QString &text, const QVariant &userData)
    {
        addItem(icon, Localizer::Translate(text), userData);
        originalItemText_.push_back(text);
    }

    void LocalizedComboBox::SetToolTip(const QString& tip)
    {
        setToolTip(Localizer::Translate(tip));
        tip_ = tip;
    }

    void LocalizedComboBox::LanguageChanged()
    {
        setToolTip(Localizer::Translate(tip_));
        for (unsigned i = 0; i < originalItemText_.size(); ++i)
            setItemText(i, Localizer::Translate(originalItemText_[i]));
    }

    LocalizedDockHeader::LocalizedDockHeader(const QString& label, QDockWidget* parent) : 
        QWidget(parent),
        dock_(parent)
    {
        QHBoxLayout* layout = new QHBoxLayout(this);
        titleLabel_ = new QLabel(label);
            
        helpButton_ = new QToolButton();
        helpButton_->setIcon(QIcon(":/qss_icons/rc/help.png"));
        popOutButton_ = new QToolButton();
        popOutButton_->setIcon(QIcon(":/qss_icons/rc/undock.png"));
        closeButton_ = new QToolButton();
        closeButton_->setIcon(QIcon(":/qss_icons/rc/close.png"));

        layout->setContentsMargins(QMargins(6, 0, 0, 0));
        layout->setSpacing(0);

        leftSide_ = new QHBoxLayout();
        leftSide_->setMargin(0);
        leftSide_->setContentsMargins(0, 0, 0, 0);
        layout->addLayout(leftSide_);

        layout->addWidget(titleLabel_, 1, Qt::AlignLeft);
        layout->addWidget(helpButton_, 0, Qt::AlignRight);
        layout->addWidget(popOutButton_, 0, Qt::AlignRight);
        layout->addWidget(closeButton_, 0, Qt::AlignRight);

        connect(popOutButton_, &QToolButton::clicked, [=]() {
            dock_->setFloating(true);
        });

        connect(closeButton_, &QToolButton::clicked, [=]() {
            dock_->setVisible(false);
        });

        helpWindow_ = new QSexyToolTip(SprueKitEditor::GetInstance());
        QVBoxLayout* flyoutLayout = new QVBoxLayout(helpWindow_);
        QTextBrowser* browser = new QTextBrowser(helpWindow_);
        browser->setMinimumSize(400, 400);
        flyoutLayout->addWidget(browser, 1);

        helpWindow_->setContentWidget(browser);
        helpWindow_->attach(helpButton_);

        const QString filePath = QDir(QApplication::applicationDirPath()).filePath(QString("Help/Docks/%1.html").arg(parent->objectName()));
        QFile file(filePath);
        if (file.exists() && file.open(QIODevice::OpenModeFlag::ReadOnly | QIODevice::OpenModeFlag::Text))
        {
            QString text = file.readAll();
            browser->setHtml(text);
            browser->setOpenExternalLinks(true);
        }
        else
            browser->setHtml(QString("<h1>Dock help information requires completion! %1</h1>").arg(parent->objectName())); // not an option to leave it out. Period.
    }

    LocalizedDock::LocalizedDock(const QString& objectName, const QString& title, const QString& tip) :
        QDockWidget(title),
        originalTitle_(title),
        originalTip_(tip)
    {
        setToolTip(tip);
        setObjectName(objectName);

        connect(Localizer::GetInstance(), &Localizer::LanguageChanged, this, &LocalizedDock::LanguageChanged);
        LanguageChanged();
        allDocks_.push_back(this);

        setTitleBarWidget(localizedHeader_ = new LocalizedDockHeader(title, this));

        connect(this, &QDockWidget::topLevelChanged, this, &LocalizedDock::FloatingChanged, Qt::ConnectionType::QueuedConnection);
    }

    LocalizedDock::~LocalizedDock()
    {
        auto found = std::find(allDocks_.begin(), allDocks_.end(), this);
        if (found != allDocks_.end())
            allDocks_.erase(found);
    }

    void LocalizedDock::SetTitle(const QString& title)
    {
        originalTitle_ = title;
        LanguageChanged();
    }
    
    void LocalizedDock::SetToolTip(const QString& tip)
    {
        originalTip_ = tip;
        LanguageChanged();
    }

    void LocalizedDock::LanguageChanged()
    {
        setWindowTitle(Localizer::Translate(originalTitle_));
        setToolTip(Localizer::Translate(originalTip_));
    }

    void LocalizedDock::FloatingChanged(bool isFloating)
    {
        if (isFloating)
        {
            localizedHeader_->blockSignals(true);
            setTitleBarWidget(0x0);
            localizedHeader_->blockSignals(false);
        }
        else
        {
            setTitleBarWidget(localizedHeader_);
        }
    }

    std::vector<LocalizedDock*> LocalizedDock::GetInstances()
    {
        return allDocks_;
    }

    LocalizedTreeWidget::LocalizedTreeWidget()
    {

    }

    void LocalizedTreeWidget::setHeaderLabels(const QStringList& list)
    {
        originalHeaderLabels_ = list;
        QStringList translated;
        for (auto str : list)
            translated.push_back(Localizer::Translate(str));
        QTreeWidget::setHeaderLabels(translated);
    }

    void LocalizedTreeWidget::LanguageChanged()
    {
        QStringList translated;
        for (auto str : originalHeaderLabels_)
            translated.push_back(Localizer::Translate(str));
        QTreeWidget::setHeaderLabels(translated);
    }

    LocalizedTableWidget::LocalizedTableWidget()
    {

    }

    void LocalizedTableWidget::setHorizontalHeaderLabels(const QStringList& list)
    {
        originalHorizontalLabels_ = list;
        QStringList translated;
        for (auto str : list)
            translated.push_back(Localizer::Translate(str));
        QTableWidget::setHorizontalHeaderLabels(translated);
    }
    void LocalizedTableWidget::setVerticalHeaderLabels(const QStringList& list)
    {
        originalVerticalLabels_ = list;
        QStringList translated;
        for (auto str : list)
            translated.push_back(Localizer::Translate(str));
        QTableWidget::setVerticalHeaderLabels(translated);
    }

    void LocalizedTableWidget::LanguageChanged()
    {
        QStringList translated;
        for (auto str : originalHorizontalLabels_)
            translated.push_back(Localizer::Translate(str));
        QTableWidget::setHorizontalHeaderLabels(translated);

        translated.clear();
        for (auto str : originalVerticalLabels_)
            translated.push_back(Localizer::Translate(str));
        QTableWidget::setVerticalHeaderLabels(translated);
    }

    LocalizedListWidget::LocalizedListWidget() :
        QListWidget(0x0)
    {

    }

    void LocalizedListWidget::LanguageChanged()
    {

    }
}