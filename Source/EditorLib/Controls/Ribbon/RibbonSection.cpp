#include "RibbonSection.h"

RibbonSection::RibbonSection(const QString& title)
{
    setObjectName("RibbonSection");
    mainLayout_ = new QVBoxLayout();
    mainLayout_->setAlignment(Qt::AlignLeft);
    mainLayout_->setContentsMargins(3, 0, 3, 0);
    mainLayout_->setSpacing(0);

    setLayout(mainLayout_);
    contentLayout_ = new QHBoxLayout();
    contentLayout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    contentLayout_->setMargin(0);
    contentLayout_->setSpacing(0);
    mainLayout_->addLayout(contentLayout_);

    mainLayout_->addItem(new QSpacerItem(0, 1));

    titleLabel_ = new QLabel(title);
    titleLabel_->setObjectName("ribbon_label");
    titleLabel_->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    mainLayout_->addWidget(titleLabel_);
}

RibbonSection::~RibbonSection()
{
    delete mainLayout_;
}

void RibbonSection::AddWidgets(QWidget* a, QWidget* b, QWidget* c)
{
    if (b == 0x0 && c == 0x0)
    {
        GetContentLayout()->addWidget(a);
        return;
    }
    else
    {
        QVBoxLayout* layout = new QVBoxLayout();
        //layout->setMargin(0);
        //??layout->setAlignment(Qt::AlignTop);
        GetContentLayout()->addLayout(layout, 1);
        if (a)
        {
            layout->addWidget(a, 1);
            layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Expanding));
        }
        if (b)
        {
            layout->addWidget(b, 1);
            layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Expanding));
        }
        if (c)
            layout->addWidget(c, 1);
    }
}

void RibbonSection::AddQuad(QWidget* topLeft, QWidget* bottomLeft, QWidget* topRight, QWidget* bottomRight)
{
    QHBoxLayout* baseLayout = new QHBoxLayout();
    baseLayout->setMargin(0);

    QVBoxLayout* topLayout = new QVBoxLayout();
    topLayout->setMargin(0);
    topLayout->addWidget(topLeft);
    topLayout->addWidget(bottomLeft);

    QVBoxLayout* bottomLayout = new QVBoxLayout();
    bottomLayout->setMargin(0);
    bottomLayout->addWidget(topRight);
    bottomLayout->addWidget(bottomRight);

    baseLayout->addLayout(topLayout);
    baseLayout->addLayout(bottomLayout);
}