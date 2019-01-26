#pragma once

#include <QCheckBox>
#include <QFileDialog>

namespace UrhoEditor
{

    /// Dialog for handling exporting Urho3D scenes/selections to OBJ, basically a "Save As" dialog with a few extra options added
    class UrhoExportDlg : public QFileDialog
    {
    public:
        UrhoExportDlg();
        virtual ~UrhoExportDlg();

        static bool zIsUp_;
        static bool exportSelected_;
    };

}