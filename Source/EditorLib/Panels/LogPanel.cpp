#include "LogPanel.h"

#include "../LogFile.h"
#include "../Search/SearchLineEdit.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QTextBrowser>

LogPanel::LogPanel()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->setMargin(0);

    searchEdit_ = new SearchLineEdit();
    searchLayout->addWidget(searchEdit_, 1);
    QPushButton* searchBtn = new QPushButton(QIcon(":/Images/godot/icon_zoom.png"), "");
    searchBtn->setMaximumHeight(32);
    searchLayout->addWidget(searchBtn);

    connect(searchBtn, &QPushButton::clicked, [=](bool) { OnSearch(); });

    layout->addLayout(searchLayout);

    browser_ = new QTextBrowser();
    layout->addWidget(browser_, 1);

    connect(searchEdit_, &SearchLineEdit::SearchPerformed, this, &LogPanel::OnSearch);
    connect(LogFile::GetInstance(), &LogFile::NewMessage, this, &LogPanel::MessageAdded, Qt::QueuedConnection);
}

LogPanel::~LogPanel()
{

}

void LogPanel::MessageAdded(const QString& src, const QString& msg, const QString& level)
{
    QColor color(255, 255, 255);
    if (level.compare("ERROR") == 0)
        color = QColor(200, 0, 0);
    else if (level.compare("WARNING") == 0)
        color = QColor(200, 200, 0);
    else if (level.compare("DEBUG") == 0)
        color = QColor(200, 0, 200);

    browser_->append(QString("<font color='%1'><b><u>%2</u></b>: %3</font>").arg(color.name(), src, msg));
}

void LogPanel::OnSearch()
{
    if (!browser_->find(searchEdit_->text()))
    {
        browser_->moveCursor(QTextCursor::Start);
        browser_->find(searchEdit_->text());
    }
}