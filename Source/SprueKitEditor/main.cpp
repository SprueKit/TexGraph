#include "SprueKitEditor.h"

#include "Dialogs/LaunchDialog.h"
#include "SprueApplication.h"

#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <SprueEngine/Core/SceneObject.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QGuiApplication>
#include <QLabel>
#include <QLibrary>
#include <QFile>
#include <QScreen>
#include <QStyle>

// Application Builders
#ifdef SPRUE_SCULPT
#elif defined(URHO_EDITOR)
#elif defined(SPRUE_TEXGEN)
    // Dock builders
    #include "GuiBuilder/Docks/Texture/TexGenDockBuilder.h"

    // Ribbon builders
    #include "GuiBuilder/Ribbons/Texture/TextureHomeRibbonBuilder.h"
    #include "GuiBuilder/Ribbons/Texture/TextureCreateRibbonBuilder.h"

    // Documents
    #include "GuiBuilder/Documents/Texture/TextureDocument.h"
#else // Main spruekit program
    // Menu builders
    #include "Documents/Sprue/Builders/DesignMenuBuilder.h"    
    #include "Documents/Sprue/Builders/MeshingMenuBuilder.h"

    // Dock builders
    #include "Documents/Sprue/Builders/SprueDockBuilder.h"
    #include "Documents/TexGen/TexGenDockBuilder.h"

    // Ribbon builders
    #include "Documents/Sprue/Builders/SprueHomeRibbonBuilder.h"
    #include "Documents/Sprue/Builders/SprueCreateRibbonBuilder.h"
    #include "Documents/TexGen/TextureHomeRibbonBuilder.h"
    #include "Documents/TexGen/TextureCreateRibbonBuilder.h"
    #include "Documents/Urho/UrhoHomeRibbonBuilder.h"

    // Sculpting
    #include "Documents/Sculpt/SculptGUIBuilder.h"

    // Documents
    #include "Documents/Sprue/SprueDocument.h"
    #include "Documents/Sprue/AnimationDocument.h"
    #include "Documents/TexGen/TextureDocument.h"
    #include "Documents/Sculpt/SculptDocument.h"
    #include "Documents/Urho/UrhoSceneDocument.h"
#endif
// Menu Builders
#include "GuiBuilder/Menus/CoreMenuBuilder.h"
#include "GuiBuilder/Menus/EditMenuBuilder.h"
#include "GuiBuilder/Menus/LayoutMenuBuilder.h"
#include "GuiBuilder/Menus/ViewportMenuBuilder.h"
#include "GuiBuilder/Menus/ViewMenuBuilder.h"

// Dock Builders
#include "GuiBuilder/Docks/SharedDockBuilder.h"
#include "Documents/Urho/UrhoDockBuilder.h"

// Ribbon Builders
#include "GuiBuilder/Ribbons/CoreGUIRibbonBuilder.h"

#include "ThirdParty/TrueFramelessWindow/QWinWidget.h"

#include "Developer\TexGen\TextureNodeIconBuilder.h"

// Application
#include "Applications/TextureGraphApplication.h"

using namespace SprueEditor;

QDataStream &operator<<(QDataStream &out, SprueEngine::SceneObject*& rhs) {
    out.writeRawData(reinterpret_cast<char*>(&rhs), sizeof(rhs));
    return out;
}

QDataStream & operator >> (QDataStream &in, SprueEngine::SceneObject*& rhs) {
    in.readRawData(reinterpret_cast<char*>(rhs), sizeof(rhs));
    return in;
}

QDataStream &operator<<(QDataStream &out, Urho3D::Serializable*& rhs) {
    out.writeRawData(reinterpret_cast<char*>(&rhs), sizeof(rhs));
    return out;
}

QDataStream & operator >> (QDataStream &in, Urho3D::Serializable*& rhs) {
    in.readRawData(reinterpret_cast<char*>(rhs), sizeof(rhs));
    return in;
}

#define DECLARE_URHO_TYPE(TYPENAME) QDataStream &operator<<(QDataStream& out, const Urho3D::TYPENAME& rhs) { out.writeRawData((char*)(&rhs), sizeof(rhs)); return out; } \
    QDataStream & operator >> (QDataStream &in, Urho3D::TYPENAME& rhs) { in.readRawData(reinterpret_cast<char*>(&rhs), sizeof(rhs)); return in; }

DECLARE_URHO_TYPE(IntVector2);
DECLARE_URHO_TYPE(Vector2);
DECLARE_URHO_TYPE(Vector3);
DECLARE_URHO_TYPE(Quaternion);
DECLARE_URHO_TYPE(Matrix3);
DECLARE_URHO_TYPE(Matrix3x4);
DECLARE_URHO_TYPE(Matrix4);

#ifdef WIN32
#include <Windows.h>
typedef BOOL (*SetProcessDpiAwarenessT)(int value);
#endif

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

#ifdef _WIN32
  {
      QLibrary user32("user32.dll", NULL);
      SetProcessDpiAwarenessT SetProcessDpiAwarenessD = (SetProcessDpiAwarenessT)user32.resolve("SetProcessDpiAwarenessInternal");
      if (SetProcessDpiAwarenessD)
          SetProcessDpiAwarenessD(1); //PROCESS_SYSTEM_DPI_AWARE
  }
#endif

    qRegisterMetaTypeStreamOperators<SprueEngine::SceneObject*>("SprueEngine::SceneObject*");
    qRegisterMetaTypeStreamOperators<Urho3D::Serializable*>("Urho3D::Serializable*");
    
#define DECLARE_URHO_STREAM(TYPENAME) qRegisterMetaTypeStreamOperators<Urho3D:: TYPENAME>("Urho3D::" #TYPENAME);
    
    DECLARE_URHO_STREAM(IntVector2);
    DECLARE_URHO_STREAM(Vector2);
    DECLARE_URHO_STREAM(Vector3);
    DECLARE_URHO_STREAM(Quaternion);
    //DECLARE_URHO_STREAM(Matrix3);
    //DECLARE_URHO_STREAM(Matrix3x4);
    //DECLARE_URHO_STREAM(Matrix4);

    Settings* settings = new Settings();

#ifdef SPRUE_TEXGEN
    QCoreApplication::setApplicationName("TexGraph");
#elif defined(SPRUE_SCULPT)
    QCoreApplication::setApplicationName("Sculpt");
#elif defined(URHO_EDITOR)
    QCoreApplication::setApplicationName("Urho3D Editor");
#else
    QCoreApplication::setApplicationName("SprueKit");
#endif

    QCoreApplication::setLibraryPaths(QStringList{ "plugins/platforms" });
    // Marker for finding application unique blocks of code
    //@ApplicationUnique
#ifdef SPRUE_SCULPT
    Sculpt_ConstructSettings(settings);
#elif defined(URHO_EDITOR)
    Urho_ConstructSettings(settings);
#elif defined(SPRUE_TEXGEN)
    TextureGraph_ConstructSettings(settings);
#elif defined(SPRUEKIT)
    Sprue_ConstructSettings(settings);
#else
    TextureGraph_ConstructSettings(settings);
#endif

    settings->RestoreSettings();

    //QApplication::setAttribute(Qt::AA_Use96Dpi, true);
    qputenv("QT_SCALE_FACTOR", QByteArray(QVariant::fromValue(1.0f).toString().toStdString().c_str()));
    qputenv("_q_scaleFactor", QByteArray(QVariant::fromValue(1.0f).toString().toStdString().c_str()));

    SprueApplication a(settings, argc, argv);


    //if (settings->GetPage("General")->Get("Enable High-DPI scaling")->value_.toBool())
    //    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    //else

    /// Developer processes
    //TextureNodeIconBuilder();

    //@ApplicationUnique
    std::vector<GuiBuilder*> menuBuilders = {
        new CoreMenuBuilder(),
        new EditMenuBuilder(),
        new DesignMenuBuilder(),
        new MeshingMenuBuilder(),
        new LayoutMenuBuilder(),
        new ViewportMenuBuilder(),
        new ViewMenuBuilder(),

        new SharedDockBuilder(),
        new SprueDockBuilder(),
        new UrhoDockBuilder(),
        new TexGenDockBuilder(),

    // Sculpting
        new SculptGuiBuilder(),

        new SprueHomeRibbonBuilder(),
        new TextureHomeRibbonBuilder(),
        new SprueCreateRibbonBuilder(),
        new TextureCreateRibbonBuilder(),
        new UrhoEditor::UrhoHomeRibbonBuilder(),
        new CoreGUIRibbonBuilder(),
    };

    std::vector<DocumentHandler*> documentHandlers = {
#ifdef SPRUE_TEXGEN
        new TextureDocumentHandler()
#elif defined(URHO_EDITOR)
        new UrhoDocumentHandler()
#elif defined(SPRUE_SCULT)
        new SculptDocumentHandler()
#else
        new SprueDocumentHandler(),
        new AnimationDocumentHandler(),
        new TextureDocumentHandler(),
        new SculptDocumentHandler(),
        new UrhoEditor::UrhoSceneDocumentHandler()
#endif
    };

    QFile file(":/qdarkstyle/style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text))
        a.setStyleSheet(QLatin1String(file.readAll()));    

    //QWinWidget* widget = new QWinWidget();
    //QWidget* wrapper = new QWidget();
    //QVBoxLayout* layout = new QVBoxLayout(wrapper);
    //layout->setMargin(5);
    //layout->addWidget(new QLabel("Test"));
    //layout->addWidget(new QLabel("Test"));
    //layout->addWidget(new QLabel("Test"));
    //layout->addWidget(new QLabel("Test"));
    //layout->addWidget(new QLabel("Test"));

    SprueKitEditor* w = new SprueKitEditor(settings, menuBuilders, documentHandlers);
    w->CreateWidgets();
    //layout->addWidget(w);

    //widget->p_Widget->setCentralWidget(wrapper);
    //QWidget* left = new QWidget();
    //QVBoxLayout* leftlayout = new QVBoxLayout(left);
    //leftlayout->addWidget(new QLabel("Test 2"));
    //leftlayout->addWidget(new QLabel("Test 2"));
    //leftlayout->addWidget(new QLabel("Test 2"));
    //leftlayout->addWidget(new QLabel("Test 2"));
    //leftlayout->addWidget(new QLabel("Test 2"));
    //QDockWidget* ww = new QDockWidget();
    //ww->setWidget(left);
    //widget->p_Widget->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, ww);
    ////w->CreateWidgets();
    //widget->setGeometry(0, 0, 1920, 1080);
    
    //widget->show();
    w->show();
    w->RestoreLayout("PREVIOUS_LAYOUT.layout");

    //w.show();


    //??if (Settings::GetInstance()->GetPage("General")->Get("Show Quick Start Window?")->value_.toBool())
    //??{
    //??    LaunchDialog* dlg = new LaunchDialog();
    //??    dlg->show();
    //??}

    //w.showMaximized();
    return a.exec();
}