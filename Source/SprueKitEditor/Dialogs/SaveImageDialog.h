#pragma once

#include <qfiledialog.h>

namespace SprueEditor
{

    class SaveImageDialog : public QFileDialog
    {
    public:
        SaveImageDialog();

        static int ImageWidth;
        static int ImageHeight;
    };

}