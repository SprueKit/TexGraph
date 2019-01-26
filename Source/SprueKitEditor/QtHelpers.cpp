#include "QtHelpers.h"

#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QLayoutItem>
#include <QMimeData>
#include <QPainter>
#include <QStandardPaths>
#include <QWidget>
#include <QTextBrowser>
#include <QDir>
#include <QFile>

#include "SprueKitEditor.h"

#include <EditorLib/ThirdParty/QSexyToolTip.h>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/FString.h>
#include <SprueEngine/IEditable.h>
#include <SprueEngine/VectorBuffer.h>

using namespace SprueEngine;

namespace SprueEditor
{
namespace QtHelpers
{

    void junk()
    {
        std::vector< std::vector< std::vector<int> > > stuff(32, std::vector<std::vector<int>>(32));
    }

void ClearLayout(QLayout* layout)
{
    QLayoutItem* item;
    while ((item = layout->layout()->takeAt(0)) != NULL)
    {
        if (item->widget())
            item->widget()->deleteLater();
        if (item->layout())
            ClearLayout(item->layout());
        delete item;
    }
}

void HideLayout(QLayout* layout)
{
    for (int idx = 0; idx < layout->layout()->count(); ++idx)
    {
        auto item = layout->layout()->itemAt(idx);
        if (item->widget())
            item->widget()->hide();
        if (item->layout())
            HideLayout(item->layout());
    }
}

void SetEnabledOnAll(QLayout* layout, bool state)
{
    for (int idx = 0; idx < layout->layout()->count(); ++idx)
    {
        auto item = layout->layout()->itemAt(idx);
        if (item->widget())
            item->widget()->setEnabled(state);
        if (item->layout())
            SetEnabledOnAll(item->layout(), state);
    }
}

void ShowLayout(QLayout* layout)
{
    for (int idx = 0; idx < layout->layout()->count(); ++idx)
    {
        auto item = layout->layout()->itemAt(idx);
        if (item->widget())
            item->widget()->show();
        if (item->layout())
            ShowLayout(item->layout());
    }
}

void UpdateMenus(QMenuBar* bar)
{
    for (auto item : bar->children())
    {
        if (QMenu* menu = dynamic_cast<QMenu*>(item))
        {
            menu->updateGeometry();
            UpdateMenus(menu);
        }
        else if (QWidget* widget = dynamic_cast<QWidget*>(item))
        {
            widget->updateGeometry();
        }
    }
}

void UpdateMenus(QMenu* bar)
{
    for (auto item : bar->children())
    {
        if (QMenu* menu = dynamic_cast<QMenu*>(item))
        {
            menu->setMinimumWidth(400);
            menu->updateGeometry();
            UpdateMenus(menu);
        }
        else if (QWidget* widget = dynamic_cast<QWidget*>(item))
        {
            widget->setMinimumWidth(400);
            widget->updateGeometry();
        }
    }
}

std::string GetAppDataPath(const std::string& file)
{
    std::string basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
    return FString("%1/%2", basePath, file).str();
}

std::string GetDocumentsPath(const std::string& file)
{
    std::string docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdString();
    return FString("%1/%2", docPath, file).str();
}

QSize MeasureTextForceWrap(const QString& text, const QFontMetrics& metrics, int width, std::vector<std::pair<int,int> >* indices)
{
    if (text.isEmpty())
        return QSize();

    QString activeText = text;
    int height = 0;
    int startSub = 0;
    int charCount = 1;
    int lines = 0;
    int maxWidth = 0;
    for (;;)
    {
        int wordWidth = metrics.width(activeText, charCount);
        if (wordWidth > width)
        {
            maxWidth = qMax(wordWidth, maxWidth);
            activeText = text.mid(startSub, charCount);
            if (indices)
                indices->push_back(std::make_pair(startSub, charCount));

            startSub += charCount;
            charCount = 1;
        }
        else
            ++charCount;

        // reached the end
        if (startSub >= text.size() || charCount == text.size())
            break;
    }
    return QSize(maxWidth, lines * (metrics.height() + metrics.descent()));
}

void PaintTextForceWrap(QPainter* painter, const QPoint& position, const QString& text, QFontMetrics& metrics, int width)
{
    std::vector<std::pair<int, int> > indices;
    MeasureTextForceWrap(text, metrics, width, &indices);

    QPoint pos = position;
    for (auto line : indices)
    {
        painter->drawText(pos, text.mid(line.first, line.second));
        pos.setY(pos.y() + metrics.height() + metrics.descent());
    }
}

void AttachHelpFlyout(QAbstractButton* button, const QString& name)
{
    QSexyToolTip* helpWindow_ = new QSexyToolTip(SprueKitEditor::GetInstance());
    QVBoxLayout* flyoutLayout = new QVBoxLayout(helpWindow_);
    QTextBrowser* browser = new QTextBrowser(helpWindow_);
    browser->setMinimumSize(400, 400);
    flyoutLayout->addWidget(browser, 1);

    helpWindow_->setContentWidget(browser);
    helpWindow_->attach(button);

    const QString filePath = QDir(QApplication::applicationDirPath()).filePath(QString("Help/Docks/%1.html").arg(name));
    QFile file(filePath);
    if (file.exists() && file.open(QIODevice::OpenModeFlag::ReadOnly | QIODevice::OpenModeFlag::Text))
    {
        QString text = file.readAll();
        browser->setHtml(text);
        browser->setOpenExternalLinks(true);
    }
    else
        browser->setHtml(QString("<h1>Dock help information requires completion! %1</h1>").arg(name)); // not an option to leave it out. Period.
}

}

}