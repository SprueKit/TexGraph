#include "PathPickerWidget.h"

#include <qdir.h>
#include <qfile.h>
#include <qfiledialog.h>


PathPickerWidget::PathPickerWidget(bool folder, bool includeDelButton) :
    QWidget(),
    forFolder_(folder)
{
    layout_ = new QHBoxLayout(this);
    layout_->setSpacing(0);
    layout_->setMargin(0);
    pathText_ = new QLineEdit();
    layout_->addWidget(pathText_, 1);
    browseButton_ = new QPushButton("...");
    browseButton_->setMaximumHeight(26);
    layout_->addWidget(browseButton_);

    if (includeDelButton)
    {
        QPushButton* delButton = new QPushButton(QIcon(":/Images/godot/icon_remove.png"), "");
        delButton->setMaximumHeight(26);
        layout_->addWidget(delButton);
        connect(delButton, &QPushButton::clicked, [=](bool) { emit Deleted(this); });
    }

    connect(pathText_, &QLineEdit::textChanged, this, &PathPickerWidget::TextChanged);

    connect(browseButton_, &QPushButton::clicked, this, &PathPickerWidget::BrowseButtonClick);
}

PathPickerWidget::~PathPickerWidget()
{
    delete pathText_;
    delete browseButton_;
    delete layout_;
}

void PathPickerWidget::SetPath(const std::string& text)
{
    pathText_->blockSignals(true);
    pathText_->setText(text.c_str());
    lastValidPath_ = text;
    pathText_->blockSignals(false);
}

std::string PathPickerWidget::GetPath() const
{
    return lastValidPath_;
}

void PathPickerWidget::TextChanged(const QString& newText)
{
    if (newText != lastValidPath_.c_str())
    {
        if ((forFolder_ && QDir(newText).exists()) || (!forFolder_ && QFile::exists(newText)))
        {
            lastValidPath_ = newText.toUtf8().constData();
            emit PathChanged(this, lastValidPath_.c_str());
        }
    }
}

void PathPickerWidget::BrowseButtonClick(bool)
{
    if (forFolder_)
    {
        QString folderPath = QFileDialog::getExistingDirectory(this, "Select folder", lastValidPath_.c_str());
        if (!folderPath.isEmpty())
        {
            lastValidPath_ = folderPath.toUtf8().constData();
            pathText_->setText(folderPath);
            emit PathChanged(this, lastValidPath_.c_str());
        }
    }
    else
    {
        QString filePath = QFileDialog::getOpenFileName(this, "Select file", lastValidPath_.c_str(), filter_.c_str());
        if (!filePath.isEmpty())
        {
            lastValidPath_ = filePath.toUtf8().constData();
            pathText_->setText(filePath);
            emit PathChanged(this, lastValidPath_.c_str());
        }
    }
}
