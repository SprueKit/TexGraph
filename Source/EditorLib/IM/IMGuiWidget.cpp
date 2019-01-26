#include "IMGuiWidget.h"

#include <QBoxLayout>

IMGuiWidget::IMGuiWidget(QWidget* parent) :
    QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(parent);
    IMGuiBuilder::Start(layout);
}

IMGuiWidget::~IMGuiWidget()
{

}