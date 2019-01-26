#include "SprueKitEditor.h"

#include "InternalNames.h"

#include "QtHelpers.h"

#include "Localization/LocalizedWidgets.h"

#include "GuiBuilder/GuiBuilder.h"
#include "GuiBuilder/BaseApplicationMenu.h"
#include "GuiBuilder/AppMenuPages/MainAppPages.h"

#include "Views/RenderWidget.h"
#include "Panels/ResourcePanel.h"
#include "Views/ViewManager.h"

#include <EditorLib/ApplicationCore.h>
#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Controls/Document/DocumentsTabBar.h>
#include <EditorLib/Search/ISearchable.h>
#include <EditorLib/LogFile.h>
#include <EditorLib/ThirdParty/QSexyToolTip.h>
#include <EditorLib/Commands/RegisteredAction.h>
#include <EditorLib/Controls/Ribbon/Ribbon.h>
#include <EditorLib/Controls/Ribbon/RibbonButton.h>
#include <EditorLib/Controls/Ribbon/RibbonGallery.h>
#include <EditorLib/Controls/Ribbon/RibbonPage.h>
#include <EditorLib/Controls/Ribbon/RibbonSection.h>
#include <EditorLib/Search/SearchLineEdit.h>
#include <EditorLib/Controls/SearchResultList.h>
#include <EditorLib/Settings/SettingsValue.h>
#include <EditorLib/TaskProcessor.h>

#include <QAction>
#include <QBoxLayout>
#include <QColorDialog>
#include <QDataStream>
#include <QDesktopWidget>
#include <QDir>
#include <QDockWidget>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <qstandardpaths.h>
#include <QToolbar>
#include <QToolButton>
#include <QTreeView>
#include <QTreeWidget>
#include <qevent.h>

#include <Urho3D/Core/Context.h>

#include <SprueEngine/Logging.h>
#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Property.h>
#include <SprueEngine/FString.h>
#include <SprueEngine/Core/SprueModel.h>
#include <SprueEngine/Core/SpruePieces.h>
#include <SprueEngine/Compute/GPGPU.h>

#include <sstream>
#include <memory>

using namespace SprueEngine;

namespace SprueEditor
{

SprueKitEditor* SprueKitEditor::instance_ = 0x0;

SprueKitEditor::SprueKitEditor(Settings* settings, const std::vector<GuiBuilder*>& guiBuilders, const std::vector<DocumentHandler*>& docHandlers, QWidget *parent) :
    QMainWindow(parent),
    settings_(settings),
    renderWidget_(0x0),
    taskProcessor_(new TaskProcessor()),
    secondaryTaskProcessor_(new TaskProcessor(true))
{
    new ApplicationCore(this);

    guiBuilders_ = guiBuilders;
    documentBuilders_  = docHandlers;
    
    connect(ApplicationCore::GetDocumentManager(), &DocumentManager::ActiveDocumentChanged, this, &SprueKitEditor::ActiveDocumentChanged);
    connect(ApplicationCore::GetDocumentManager(), &DocumentManager::DocumentClosed, this, &SprueKitEditor::DocumentClosed);

    setWindowTitle("SprueKit");
    instance_ = this;
    setDockNestingEnabled(false);

// Create search text
    QWidget* searchWidget = new QWidget();
    QHBoxLayout* searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setSpacing(0);
    searchLayout->setMargin(0);
    searchLayout->setAlignment(Qt::AlignRight);
    searchText_ = new SearchLineEdit();
    searchText_->setPlaceholderText("search...");
    QPushButton* searchBtn = new QPushButton(QIcon(":/Images/godot/icon_zoom.png"), "");
    searchBtn->setMaximumHeight(32);
    searchLayout->addWidget(searchText_);
    searchLayout->addWidget(searchBtn);
    searchText_->setMaximumWidth(220);
    menuBar()->setCornerWidget(searchWidget); // default is for top right corner

    connect((SearchLineEdit*)searchText_, &SearchLineEdit::SearchPerformed, this, &SprueKitEditor::SearchPerformed);
    connect(searchBtn, &QPushButton::clicked, this, &SprueKitEditor::SearchPerformed);

// Create the central window
    // Need to create an interior junk widget for the center space in order 
    QWidget* junk = new QWidget();
    junk->setObjectName("INTERNAL_HOLDER");
    QVBoxLayout* layout = new QVBoxLayout(junk);
    layout->setSpacing(0);
    layout->setMargin(0);
    setCentralWidget(junk);

    // Create the documents tab bar
    documentsTabBar_ = new DocumentsTabBar();
    documentsTabBar_->setObjectName("DOCUMENT_TABS");

    // Create the interior window
    internalWindow_ = new QMainWindow();
    internalWindow_->setObjectName("INTERNAL_MAIN");

    layout->addWidget(documentsTabBar_);
    ribbon_ = new Ribbon();
    ApplicationCore::SetRibbon(ribbon_);

    ribbon_->setObjectName("INTERNAL_RIBBON");

    ribbon_->setFixedHeight(ribbon_->tabBar()->height() + (96 + 24 + 24/*font adjustment*/));
    ribbon_->SetShownHeight(ribbon_->tabBar()->height() + (96 + 24 + 24/*font adjustment*/));
    layout->addWidget(ribbon_);
    layout->addWidget(internalWindow_);

    // App menu
    {
        appMenu_ = new QSexyToolTip(this);
        connect(SprueKitEditor::GetInstance(), &SprueKitEditor::WindowMoved, appMenu_, &QSexyToolTip::reposition);
        QVBoxLayout* layout = new QVBoxLayout(appMenu_);
        layout->setMargin(0);
        BaseApplicationMenu* menu = new BaseApplicationMenu(this);
        layout->addWidget(menu);

        appMenu_->attach(ribbon_->GetApplicationButton());

        menu->AddExpansionButton(QIcon(":/Images/godot/icon_new.png"), "New", new NewAppPage());
        menu->AddExpansionButton(QIcon(":/Images/godot/icon_open.png"), "Open", new OpenAppPage());
        menu->AddSeperator();
        connect(menu->AddButton(QIcon(":/Images/godot/icon_save.png"), "Save"), &QPushButton::clicked, [=](bool) {
            ApplicationCore::GetDocumentManager()->SaveActiveDocument();
        });
        connect(menu->AddButton(QIcon(), "Save As"), &QPushButton::clicked, [=](bool) {
            ApplicationCore::GetDocumentManager()->SaveActiveDocumentAs();
        });
        menu->AddSeperator();
        connect(menu->AddButton(QIcon(), "Export"), &QPushButton::clicked, [=](bool) { 
            if (auto doc = ApplicationCore::GetDocumentManager()->GetActiveDocument())
            {
                if (doc->HasExport())
                {
                    doc->BeginExport();
                    return;
                }
            }
            QMessageBox::information(this, "Not Supported", "This document does not have anything to export.");
        });
        connect(menu->AddButton(QIcon(), "Reports"), &QPushButton::clicked, [=](bool) { 
            if (auto doc = ApplicationCore::GetDocumentManager()->GetActiveDocument())
            {
                if (doc->HasReports())
                {
                    doc->BeginReport();
                    return;
                }
            }
            QMessageBox::information(this, "Not Supported", "This document does not have anything to report.");
        });
        menu->AddSeperator();
        auto exitButton = menu->AddButton(QIcon(), "Exit");
        connect(exitButton, &QPushButton::clicked, [=](bool) { QApplication::exit(); });
    }

    internalWindow_->setDockNestingEnabled(true);
    internalWindow_->setDockOptions(QMainWindow::DockOption::AllowNestedDocks | QMainWindow::DockOption::AllowTabbedDocks | QMainWindow::DockOption::AnimatedDocks | QMainWindow::DockOption::GroupedDragging);
    internalWindow_->setCorner(Qt::Corner::BottomLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
    internalWindow_->setCorner(Qt::Corner::BottomRightCorner, Qt::DockWidgetArea::BottomDockWidgetArea);
    internalWindow_->setCorner(Qt::Corner::TopLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
    internalWindow_->setCorner(Qt::Corner::TopRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);
    internalWindow_->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    taskProcessor_->start();
    secondaryTaskProcessor_->start();
    connect(taskProcessor_, &TaskProcessor::TaskChanged, this, &SprueKitEditor::SetTask, Qt::ConnectionType::QueuedConnection);
}

SprueKitEditor::~SprueKitEditor()
{
    RegisteredAction::Save();
}

SprueKitEditor* SprueKitEditor::GetInstance()
{
    return instance_;
}

Selectron* SprueKitEditor::GetObjectSelectron()
{
    return ApplicationCore::GetObjectSelectron();
}

ViewManager* SprueKitEditor::GetViewManager() const { return renderWidget_->GetViewManager(); }

void SprueKitEditor::CreateWidgets()
{
    windowMenu_ = new QMenu("Window");
    renderWidget_ = new RenderWidget();
    QWidget* centralHolder = new QWidget();
    QVBoxLayout* centralLayout = new QVBoxLayout(centralHolder);
    centralLayout->setMargin(0);
    centralLayout->setSpacing(0);

    internalWindow_->setCentralWidget(centralHolder);
    centralLayout->addWidget(viewBar_ = new QStackedWidget());
    centralLayout->addWidget(renderWidget_, 1);
    renderWidget_->SetSelectron(ApplicationCore::GetObjectSelectron());

    renderWidget_->setMinimumSize(0, 0);// 320, 240);
    renderWidget_->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
   
    for (auto documentHandler : documentBuilders_)
        documentHandler->OnProgramInitialized();

    for (auto guiBuilder : guiBuilders_)
        guiBuilder->Build(this);

    CreateMenuBar();

    menuBar()->addMenu(createPopupMenu());

    CreateStatusBar();

    //RegisteredAction::Restore();

    // NOTE! see duplicate code below
    if (settings_->GetValue("General/Use Ribbon UI")->value_.toBool())
    {
        menuBar()->setVisible(false);
        for (auto bar : toolbars_)
            bar.second->setVisible(false);
        documentsTabBar_->setVisible(false);
        ribbon_->setVisible(true);
    }
    else
    {
        ribbon_->setVisible(false);
        documentsTabBar_->setVisible(true);
    }

    if (auto setting = Settings::GetInstance()->GetValue("General/Use Ribbon UI"))
    {
        connect(setting, &SettingValue::Changed, [=](const QVariant& newValue) {
            if (newValue.toBool())
            {
                menuBar()->setVisible(false);
                for (auto bar : toolbars_)
                    bar.second->setVisible(false);
                ribbon_->setVisible(true);
                documentsTabBar_->setVisible(false);
            }
            else
            {
                ribbon_->setVisible(false);
                menuBar()->setVisible(true);
                for (auto bar : toolbars_)
                    bar.second->setVisible(true);
                documentsTabBar_->setVisible(true);
            }
        });
    }

    // This doesn't work like desired
    //if (SprueEditor::Settings::GetInstance()->GetValue("Secret GUI State/Ribbon Collapsed")->value_.toBool())
    //    ribbon_->SetAutoHide(true);
}

QMenu* SprueKitEditor::GetOrCreateMenu(const std::string& name, const std::string& title)
{
    auto found = menus_.find(name);
    if (found != menus_.end())
        return found->second;
    QMenu* menu = menuBar()->addMenu(title.c_str());
    menus_[name] = menu;
    return menu;
}

QToolBar* SprueKitEditor::GetOrCreateToolBar(const std::string& name, const std::string& title)
{
    auto found = toolbars_.find(name);
    if (found != toolbars_.end())
        return found->second;
    QToolBar* tb = addToolBar(title.c_str());
    tb->setObjectName(name.c_str());
    toolbars_[name] = tb;
    tb->setVisible(false);
    return tb;
}

LocalizedDock* SprueKitEditor::GetOrCreateDock(const std::string& name, const std::string& title, const std::string& defaultAccelerator)
{
    auto found = docks_.find(name);
    if (found == docks_.end())
    {
        LocalizedDock* newDock = new LocalizedDock(name.c_str(), tr(title.c_str()));
        newDock->setObjectName(name.c_str());
        docks_[name] = newDock;
        internalWindow_->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, newDock);
        connect(newDock, &QDockWidget::dockLocationChanged, this, &SprueKitEditor::WindowMoved);
        
        QAction* toggleAct = newDock->toggleViewAction();
        addAction(toggleAct);
        windowMenu_->addAction(toggleAct);

        RegisteredWrapperAction* toggleViewAction = new RegisteredWrapperAction("View", toggleAct);
        toggleViewAction->setShortcutContext(Qt::ApplicationShortcut);
        toggleViewAction->SetShortcut(QKeySequence(defaultAccelerator.c_str()));
        
        
        return newDock;
    }
    return found->second;
}

LocalizedDock* SprueKitEditor::GetDock(const std::string& name)
{
    auto found = docks_.find(name);
    if (found != docks_.end())
        return found->second;
    return 0x0;
}

void SprueKitEditor::AddDock(const std::string& name, LocalizedDock* dock)
{
    auto found = docks_.find(name);
    if (found != docks_.end())
        return;
    dock->setObjectName(name.c_str());
    connect(dock, &QDockWidget::dockLocationChanged, this, &SprueKitEditor::WindowMoved);
    docks_[name] = dock;
    internalWindow_->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dock);
    QAction* toggleViewAction = dock->toggleViewAction();
    windowMenu_->addAction(toggleViewAction);
}

void SprueKitEditor::closeEvent(QCloseEvent* event)
{
    //TODO check for any dirty documents
    SaveLayout("PREVIOUS_LAYOUT", "PREVIOUS_LAYOUT.layout");
    event->setAccepted(ApplicationCore::GetDocumentManager()->CheckCloseProgram());
}

void SprueKitEditor::moveEvent(QMoveEvent* event)
{
    int number = QApplication::desktop()->screenNumber(this);
    if (number != monitorIndex_)
    {
        monitorIndex_ = number;
        menuBar()->style()->unpolish(this);
        menuBar()->style()->polish(this);
        QtHelpers::UpdateMenus(menuBar());
    }
    QMainWindow::moveEvent(event);
    emit WindowMoved();
}

static QAction* CreateAction(QActionGroup* group, const QString& iconFileName, const QString& text, bool checked, const QString& shortcut = "")
{
    QAction* action = group->addAction(QIcon(iconFileName), text);
    action->setCheckable(true);
    action->setChecked(checked);
    if (!shortcut.isEmpty())
        action->setShortcut(QKeySequence::fromString(shortcut));
    return action;
}

void SprueKitEditor::CreateMenuBar()
{
    menuBar()->addMenu(windowMenu_);
}

void SprueKitEditor::CreateStatusBar()
{
    statusBar()->setVisible(true);
#ifdef WIN32
    statusBar()->setSizeGripEnabled(false);
#endif

    taskLbl_ = new QLabel("");
    statusBar()->addPermanentWidget(taskLbl_);
}

void SprueKitEditor::HandleBackgroundAction()
{
    // TODO 
    QColor qColor = QColor::fromRgbF(1.0f, 0.0f, 0.25f);
    QColor newQcolor = QColorDialog::getColor(qColor, this);
}

void SprueKitEditor::HandleTaskTimeout()
{
    taskProcessor_->Update();
}

void SprueKitEditor::ActiveDocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc)
{
    if (newDoc && newDoc->GetToolshelfWidget())
    {
        QWidget* shelf = newDoc->GetToolshelfWidget();
        if (viewBar_->indexOf(shelf) == -1)
            viewBar_->addWidget(shelf);
        viewBar_->setCurrentWidget(shelf);
        viewBar_->setMinimumHeight(shelf->minimumHeight());
        viewBar_->setMaximumHeight(shelf->maximumHeight());
    }
    else
        viewBar_->setFixedHeight(0);
}

void SprueKitEditor::DocumentClosed(DocumentBase* closingDoc)
{
    if (closingDoc && closingDoc->GetToolshelfWidget())
        viewBar_->removeWidget(closingDoc->GetToolshelfWidget());
}

void SprueKitEditor::SetTask(QString lbl)
{
    taskLbl_->setText(lbl);
}

void SprueKitEditor::SaveLayout(const std::string& name, const std::string& intoFile)
{
    std::string filePath;
    if (intoFile.empty())
    {
        filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
        filePath += "/DefaultLayout.layout";
    }
    else
    {
        filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
        filePath += FString("/%1", intoFile).str();
    }

    QFile file(filePath.c_str());
    if (!file.open(QIODevice::WriteOnly))
    {
        LOGERROR(FString("Unable to open layout file for write: %1", filePath).c_str());
        return;
    }

    QByteArray geoArray = internalWindow_->saveGeometry();
    QByteArray stateArray = internalWindow_->saveState();

    QByteArray myGeoArray = saveGeometry();
    QByteArray myStateArray = saveState();

    QDataStream stream(&file);

    QString nm(name.c_str());

    stream << nm;
    stream << geoArray;
    stream << stateArray;
    stream << myGeoArray;
    stream << myStateArray;

    stream << (uint64_t)docks_.size();
    for (auto d : docks_)
    {
        stream << QString(d.first.c_str());
        stream << d.second->width();
        stream << d.second->height();
    }

    file.flush();
    file.close();
    QResizeEvent* event = new QResizeEvent(this->size(), this->size());
    this->event(event);
    repaint();
}

void SprueKitEditor::RestoreLayout(const std::string& fromFile)
{
    LOGINFO(FString("Loading layout: %1", fromFile).c_str());
    std::string filePath;
    if (fromFile.empty())
    {
        filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
        filePath += "/DefaultLayout.layout";
    }
    else
    {
        if (fromFile.find('/') != std::string::npos || fromFile.find('\\') != std::string::npos && QFile::exists(fromFile.c_str()))
        {
            filePath = fromFile;
        }
        else
        {
            filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
            filePath += FString("/%1", fromFile).str();
        }
    }

    QFile file(filePath.c_str());
    LOGINFO(FString("Mapped layout to load as: %1", filePath).c_str());
    if (!file.open(QIODevice::ReadOnly))
    {
        LOGERROR(FString("Unable to open layout file for read: %1", filePath).c_str());
        return;
    }

    QDataStream stream(&file);

    QString name;
    stream >> name;

    QByteArray myGeoArray, myState, geoArray, stateArray;

    //stream >> myGeoArray;
    //stream >> myState;
    stream >> geoArray;
    stream >> stateArray;
    if (!stream.atEnd())
    {
        stream >> myGeoArray;
        stream >> myState;
    }

    if (!myGeoArray.isEmpty() && !myState.isEmpty())
    {
        restoreGeometry(myGeoArray);
        restoreState(myState);
    }

    internalWindow_->restoreGeometry(geoArray);
    internalWindow_->restoreState(stateArray);

    if (!stream.atEnd())
    {
        uint64_t ct = 0;
        stream >> ct;

        for (unsigned i = 0; i < ct; ++i)
        {
            QString name;
            stream >> name;
            int width = 0, height = 0;
            stream >> width;
            stream >> height;

            auto found = docks_.find(name.toStdString());
            if (found != docks_.end())
                found->second->resize(width, height);
        }
    }

    file.close();
}

std::vector< std::pair<std::string, std::string> > SprueKitEditor::EnumerateLayouts() const
{
    std::vector< std::pair<std::string, std::string> > ret;

    QString layoutFolderPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir layoutFolder(layoutFolderPath);
    if (!layoutFolder.exists())
    {
        LOGWARNING("Unable to find SprueKit local storage folder");
        return ret;
    }

    QFileInfoList fileList = layoutFolder.entryInfoList();
    for (auto item : fileList)
    {
        // Check for layotu files
        if (item.isFile() && item.completeSuffix().compare("layout", Qt::CaseInsensitive))
        {
            if (item.baseName().compare("PREVIOUS_LAYOUT", Qt::CaseSensitive))
                continue;

            QFile file(item.absoluteFilePath());
            if (!file.open(QIODevice::ReadOnly))
                continue;

            QDataStream stream(&file);
            QString layoutName;
            stream >> layoutName;
            
            ret.push_back(std::make_pair(layoutName.toStdString(), item.fileName().toStdString()));
            file.close();
        }
    }

    return ret;
}

void SprueKitEditor::SearchPerformed()
{
    QString txt = searchText_->text().trimmed();
    if (!txt.isEmpty())
    {
        QStringList terms = txt.split(' ', QString::SplitBehavior::SkipEmptyParts);
        SearchResultVector searchResults;

        auto searchables = ISearchable::GetSearchables();
        for (auto searchable : searchables)
            searchable->CollectSearchResults(terms, searchResults);
        
        /// If we have results than show the search panel if it is hidden
        if (!searchResults.empty())
        {
            if (QDockWidget* searchDock = GetDock(SEARCH_RESULTS_PANEL))
                searchDock->setVisible(true);
        }

        statusBar()->showMessage(FString("Found %1 search %?1=2?result:results;.", (unsigned)searchResults.size(), 1).c_str());

        if (SearchResultList* resultsList = ISignificantControl::GetByName<SearchResultList>(SEARCH_RESULTS_CONTROL))
            resultsList->SetResults(searchResults);
    }
    else
        searchText_->setText("");
}

}