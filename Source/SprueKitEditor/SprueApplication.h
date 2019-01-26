#pragma once

#include <qapplication.h>

#include <EditorLib/LogFile.h>
#include <EditorLib/Settings/Settings.h>

#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Quaternion.h>

namespace SprueEditor
{

    class SprueApplication : public QApplication
    {
    public:
        SprueApplication(Settings*, int argc, char** argv);
        virtual ~SprueApplication();

    private:
        Settings* settings_ = 0x0;
        LogFile* logFile_ = 0x0;
    };

}

Q_DECLARE_METATYPE(Urho3D::IntVector2);
Q_DECLARE_METATYPE(Urho3D::Vector2);
Q_DECLARE_METATYPE(Urho3D::Vector3);
Q_DECLARE_METATYPE(Urho3D::Quaternion);