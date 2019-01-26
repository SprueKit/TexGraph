#include "SaveImageDialog.h"

#include <QLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>

namespace SprueEditor
{

    int SaveImageDialog::ImageWidth = 128;
    int SaveImageDialog::ImageHeight = 128;

    SaveImageDialog::SaveImageDialog() :
        QFileDialog(0x0, "Save Image", "", "PNG Image (*.png);;TGA Image (*.tga);;HDR Image (*.hdr);;DDS Compressed (*.dds)")
    {
        setOption(QFileDialog::Option::DontUseNativeDialog, true);
        setAcceptMode(QFileDialog::AcceptSave);
        setModal(true);
        QGridLayout* gl = dynamic_cast<QGridLayout*>(layout());
        if (gl)
        {
            int r = gl->rowCount();
            QFormLayout* form = new QFormLayout();
            QSpinBox* widthSpin = new QSpinBox();
            QSpinBox* heightSpin = new QSpinBox();
            form->addRow("Width", widthSpin);
            form->addRow("Height", heightSpin);

            widthSpin->setMinimum(1);
            widthSpin->setMaximum(4096 * 4);
            heightSpin->setMinimum(1);
            heightSpin->setMaximum(4096 * 4);

            widthSpin->setValue(ImageWidth);
            heightSpin->setValue(ImageHeight);
            gl->addLayout(form, r, 0, 1, 1);// gl->columnCount());

            connect(widthSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value) {
                SaveImageDialog::ImageWidth = qMax(value, 1);
            });

            connect(widthSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value) {
                SaveImageDialog::ImageHeight = qMax(value, 1);
            });
        }
    }

}