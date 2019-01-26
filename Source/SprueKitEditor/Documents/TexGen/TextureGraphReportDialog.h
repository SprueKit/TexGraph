#pragma once

#include <QDialog>

namespace SprueEditor
{

    class TextureDocument;

    class TextureGraphReportDialog : public QDialog
    {
        Q_OBJECT;
    public:
        TextureGraphReportDialog();
        virtual ~TextureGraphReportDialog();
    };

}