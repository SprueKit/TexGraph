#include "UrhoObjectSpawner.h"

#include <QBoxLayout>
#include <QLabel>

namespace UrhoEditor
{

#define SET_RANGE(SPINNER) SPINNER->setMinimum(INT_MIN); SPINNER->setMaximum(INT_MAX); 
    //SPINNER->setMaximumWidth(80);

#define XStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #DD0000; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
#define YStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #00AA00; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
#define ZStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #3F3FDD; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
#define AStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #AA007F; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"

#define CREATE_LBL(VARNAME, TEXT, STYLE, TIP) QLabel* VARNAME = new QLabel(" " TEXT); \
VARNAME->setMaximumWidth(18); \
VARNAME->setMinimumWidth(18); \
VARNAME->setMaximumHeight(18); \
VARNAME->setStyleSheet(STYLE); \
VARNAME->setAlignment(Qt::AlignCenter); \
VARNAME->setToolTip(TIP);

#define ADD_LABEL(TEXT) mainLayout->addWidget(new QLabel(tr(TEXT)))

    UrhoObjectSpawner::UrhoObjectSpawner()
    {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        {
            ADD_LABEL("Local Offset");
            QHBoxLayout* localOffsetLayout = new QHBoxLayout();
            CREATE_LBL(offsetXLbl, "X", XStyle, "");
            CREATE_LBL(offsetYLbl, "Y", YStyle, "");
            CREATE_LBL(offsetZLbl, "Z", ZStyle, "");
            localOffsetLayout->addWidget(offsetXLbl);
            localOffsetLayout->addWidget(offsetXSpinner_ = new QDoubleSpinBox());
            localOffsetLayout->addWidget(offsetYLbl);
            localOffsetLayout->addWidget(offsetYSpinner_ = new QDoubleSpinBox());
            localOffsetLayout->addWidget(offsetZLbl);
            localOffsetLayout->addWidget(offsetZSpinner_ = new QDoubleSpinBox());
            SET_RANGE(offsetXSpinner_);
            SET_RANGE(offsetYSpinner_);
            SET_RANGE(offsetZSpinner_);
            mainLayout->addLayout(localOffsetLayout);
        }
        {
            ADD_LABEL("Random Rotation");
            QHBoxLayout* randomRotationLayout = new QHBoxLayout();
            CREATE_LBL(XLbl, "X", XStyle, "");
            CREATE_LBL(YLbl, "Y", YStyle, "");
            CREATE_LBL(ZLbl, "Z", ZStyle, "");
            randomRotationLayout->addWidget(XLbl);
            randomRotationLayout->addWidget(rotationXSpinner_ = new QDoubleSpinBox());
            randomRotationLayout->addWidget(YLbl);
            randomRotationLayout->addWidget(rotationYSpinner_ = new QDoubleSpinBox());
            randomRotationLayout->addWidget(ZLbl);
            randomRotationLayout->addWidget(rotationZSpinner_ = new QDoubleSpinBox());
            SET_RANGE(rotationXSpinner_);
            SET_RANGE(rotationYSpinner_);
            SET_RANGE(rotationZSpinner_);
            mainLayout->addLayout(randomRotationLayout);
        }

        {
            ADD_LABEL("Random Scale");
            QHBoxLayout* randomScaleLayout = new QHBoxLayout();
            CREATE_LBL(MinLbl, "-", XStyle, "");
            CREATE_LBL(MaxLbl, "+", YStyle, "");
            randomScaleLayout->addWidget(MinLbl);
            randomScaleLayout->addWidget(minScaleSpinner_ = new QDoubleSpinBox());
            randomScaleLayout->addWidget(MaxLbl);
            randomScaleLayout->addWidget(maxScaleSpinner_ = new QDoubleSpinBox());
            minScaleSpinner_->setMinimum(0.01);
            maxScaleSpinner_->setMinimum(0.01);
            minScaleSpinner_->setMaximum(100.0);
            maxScaleSpinner_->setMaximum(100.0);
            mainLayout->addLayout(randomScaleLayout);

            minMaxScaleSlider_ = new RangeSlider();
            minMaxScaleSlider_->setValueMin(0.01);
            minMaxScaleSlider_->setValueMax(100.0);
            mainLayout->addWidget(minMaxScaleSlider_);
        }

        alignWithHitNormal_ = new QCheckBox(tr("Align with Hit normal"));
        alignToAABBBottom_  = new QCheckBox(tr("Align to AABB bottom"));
        parentToSelection_  = new QCheckBox(tr("Parent to selected object"));
        mainLayout->addWidget(alignWithHitNormal_);
        mainLayout->addWidget(alignToAABBBottom_);
        mainLayout->addWidget(parentToSelection_);

        ADD_LABEL("Objects");
        objectsTable_ = new QTableWidget();
        mainLayout->addWidget(objectsTable_);

        ADD_LABEL("Inject Variables");
        variableInjectionTable_ = new QTableWidget();
        mainLayout->addWidget(variableInjectionTable_);
    }

    UrhoObjectSpawner::~UrhoObjectSpawner()
    {

    }

}