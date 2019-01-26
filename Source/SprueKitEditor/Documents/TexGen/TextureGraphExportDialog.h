#pragma once

#include <QDialog>

namespace SprueEditor
{

    class TextureGraphExportDialog : public QDialog
    {
        Q_OBJECT;
    public:
        TextureGraphExportDialog(QWidget* owner = 0x0);
        virtual ~TextureGraphExportDialog();

    
    };

}