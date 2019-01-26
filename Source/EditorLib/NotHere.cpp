#include "NotHere.h"

#include <QMessageBox>

void NotHere()
{
    QMessageBox::information(0x0, "Not Implemented", "This feature has not been implemented yet.");
}

void NotHere(const QString& featureName)
{
    QMessageBox::information(0x0, "Not Implemented", QString("%1 has not been implemented yet.").arg(featureName));
}