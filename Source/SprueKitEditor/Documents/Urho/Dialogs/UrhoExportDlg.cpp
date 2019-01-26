#include "UrhoExportDlg.h"

#include <QGridLayout>

namespace UrhoEditor
{

    bool UrhoExportDlg::zIsUp_ = false;
    bool UrhoExportDlg::exportSelected_ = true;

    UrhoExportDlg::UrhoExportDlg() :
        QFileDialog(0x0, "Export to OBJ", "Wavefront Object (*.obj)")
    {
        setOption(QFileDialog::Option::DontUseNativeDialog, true);
        setAcceptMode(QFileDialog::AcceptSave);
        setModal(true);
        QGridLayout* gl = dynamic_cast<QGridLayout*>(layout());
        if (gl)
        {
            int r = gl->rowCount();

            QVBoxLayout* form = new QVBoxLayout();
            QCheckBox* zUp = new QCheckBox("Z is Up");
            QCheckBox* exportSel = new QCheckBox("Selected objects only");

            zUp->setChecked(zIsUp_);
            exportSel->setChecked(exportSelected_);

            form->addWidget(zUp);
            form->addWidget(exportSel);

            gl->addLayout(form, r, 0, 1, 1);// gl->columnCount());

            connect(zUp, &QCheckBox::stateChanged, [=](int) {
                zIsUp_ = zUp->isChecked();
            });

            connect(exportSel, &QCheckBox::stateChanged, [=](int) {
                exportSelected_ = exportSel->isChecked();
            });
        }
    }

    UrhoExportDlg::~UrhoExportDlg()
    {

    }

}