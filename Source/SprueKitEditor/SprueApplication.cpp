#include "SprueApplication.h"

#include <EditorLib/Settings/Settings.h>

#include <qstandardpaths.h>
#include <QFont>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/MessageLog.h>

namespace SprueEditor
{

    SprueApplication::SprueApplication(Settings* settings, int argc, char** argv) :
        QApplication(argc, argv),
        settings_(settings)
    {
#ifdef SPRUE_TEXGEN
        QCoreApplication::setApplicationName("TexGraph");
        QCoreApplication::setApplicationVersion("0.1");
#elif defined(SPRUE_SCULPT)
        QCoreApplication::setApplicationName("Sculpt");
        QCoreApplication::setApplicationVersion("0.1");
#elif defined(URHO_EDITOR)
        QCoreApplication::setApplicationName("Urho3D Editor");
        QCoreApplication::setApplicationVersion("0.1");
#else
        QCoreApplication::setApplicationName("SprueKit");
        QCoreApplication::setApplicationVersion("0.1");
#endif
        QCoreApplication::setOrganizationDomain("http://www.spruekit.com");
        

        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#ifndef SPRUE_HTML_LOG
        path += "/log.html";
#else
        path += "/log.txt";
#endif
        logFile_ = new LogFile(path, "SprueKit");

        QApplication::setFont(QFont("Segoe UI", 10));
        SprueEngine::Context::GetInstance()->GetLog()->SetLogCallback(LOGFILE_CALLBACK);
    }
    
    SprueApplication::~SprueApplication()
    {
        settings_->SaveSettings();
    }

}