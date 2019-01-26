#include "DocumentManager.h"

#include <EditorLib/ApplicationCore.h>
#include <EditorLib/DocumentBase.h>
#include <EditorLib/LogFile.h>
#include <EditorLib/Controls/Ribbon/Ribbon.h>
#include <EditorLib/Selectron.h>
#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QTimer>

DocumentManager::DocumentManager() :
    activeDocument_(0x0),
    selectron_(new Selectron())
{
    autoSaveTimer_ = new QTimer(this);
    autoSaveTimer_->setSingleShot(false);

    connect(autoSaveTimer_, &QTimer::timeout, this, &DocumentManager::OnAutoSaveTimer);

// Setup our settings callbacks
    auto backupsOn = Settings::GetInstance()->GetValue("Auto Backup/Enable automatic backup");
    auto backupTime = Settings::GetInstance()->GetValue("Auto Backup/Backup every N minutes");
    auto settingCallback = [=](const QVariant& value) {
        this->SetupTimer();
    };
    connect(backupsOn, &SettingValue::Changed, settingCallback);
    connect(backupTime, &SettingValue::Changed, settingCallback);

    SetupTimer();
}

DocumentManager::~DocumentManager()
{
    for (auto doc : documents_)
        delete doc;
}

bool DocumentManager::SaveDocument(DocumentBase* doc)
{
    if (doc)
    {
        bool ret = false;
        emit DocumentSaving(doc);
        if (doc->GetFilePath().isEmpty())
            ret = SaveActiveDocumentAs();
        else
            ret = doc->Save();
        if (ret)
            emit DocumentSaved(doc);
        return ret;
    }
    return false;
}

bool DocumentManager::SaveActiveDocument()
{
    if (activeDocument_)
        return SaveDocument(activeDocument_);
    return false;
}

bool DocumentManager::SaveActiveDocumentAs()
{
    if (activeDocument_)
    {
        QString fileName = QFileDialog::getSaveFileName(0x0, QString("Save %1 As...").arg(activeDocument_->GetHandler()->DocumentTypeName()), "", activeDocument_->GetHandler()->GetSaveFileFilter());
        if (!fileName.isEmpty())
        {
            activeDocument_->SetFilePath(fileName);
            emit DocumentSaving(activeDocument_);
            if (activeDocument_->Save())
            {
                emit DocumentSaved(activeDocument_);
                return true;
            }
        }
    }
    return false;
}

void DocumentManager::SetActiveDocument(DocumentBase* newDocument)
{
    DocumentBase* current = activeDocument_;
    activeDocument_ = newDocument;
    if (current)
    {
        current->PreDeactivate();
        current->SetRibbonTabIndex(ApplicationCore::GetRibbon()->currentIndex());
    }

    if (activeDocument_)
        activeDocument_->PreActivate();

    if (newDocument)
        selectron_->SetDelegate(newDocument->GetSelectron());
    else
        selectron_->SetDelegate(0x0);

    emit ActiveDocumentChanged(newDocument, current);

    if (current)
        current->PostDeactivate();
    if (activeDocument_)
    {
        activeDocument_->PostActivate();
        ApplicationCore::GetRibbon()->setCurrentIndex(activeDocument_->GetRibbonTabIndex());
    }
}

bool DocumentManager::CheckCloseProgram()
{
    int ctDirty = 0;
    for (auto doc : documents_)
    {
        if (doc->IsDirty())
        {
            QMessageBox::StandardButton result = QMessageBox::warning(0x0, "Save Document?", QString("Document %1 has unsaved changes. Changes will be lost if it is not saved.\n\nSave file?").arg(doc->GetFileName()), QMessageBox::Save | QMessageBox::Ignore | QMessageBox::Cancel);
            if (result == QMessageBox::Save)
                doc->Save();
            else if (result == QMessageBox::Cancel)
                return false;
        }
    }
    return true;
}

void DocumentManager::CreateNewDocument(DocumentHandler* handler)
{
    if (DocumentBase* newDoc = handler->CreateNewDocument())
    {
        documents_.push_back(newDoc);
        emit DocumentOpened(newDoc);
        SetActiveDocument(newDoc);
        connect(newDoc, &DocumentBase::ViewChanged, this, &DocumentManager::OnDocumentViewChange);
        connect(newDoc, &DocumentBase::DirtyChanged, [=](bool state) {
            emit DocumentDirtyChanged(newDoc);
        });
    }
}

void DocumentManager::OpenDocument(DocumentHandler* handler)
{
    QString openFileName = QFileDialog::getOpenFileName(0x0, QString("Open %1").arg(handler->DocumentTypeName()), QString(), handler->GetOpenFileFilter());
    if (!openFileName.isEmpty())
    {
        if (DocumentBase* opened = handler->OpenDocument(openFileName))
        {
            documents_.push_back(opened);
            emit DocumentOpened(opened);
            SetActiveDocument(opened);
            connect(opened, &DocumentBase::ViewChanged, this, &DocumentManager::OnDocumentViewChange);
        }
    }
}

bool DocumentManager::CloseDocument(DocumentBase* doc, bool prompt)
{
    if (doc == 0x0)
        return false;

    const bool isActive = doc == activeDocument_;
    if (!prompt || doc->IsDirty())
    {
        QMessageBox::StandardButton result = QMessageBox::warning(0x0, "Save Document?", "Document has unsaved changes. Changes will be lost if it is not saved.\n\nSave File?", QMessageBox::Save | QMessageBox::Ignore | QMessageBox::Cancel);
        if (result == QMessageBox::Save)
            doc->Save();
        else if (result == QMessageBox::Cancel) // Cancel and don't delete
            return false;
        else
        {
            // Carry on and delete without save
        }
    }

    if (isActive)
        activeDocument_ = 0x0;
    auto found = std::find(documents_.begin(), documents_.end(), doc);
    if (found != documents_.end())
        documents_.erase(found);

    emit DocumentClosed(doc);

    if (isActive)
    {
        if (documents_.size() > 0)
            SetActiveDocument(documents_[0]);
        else
            SetActiveDocument(0x0);
    }

    // deleting the doc is the last thing, someone may have connections to it they need to clean up
    delete doc;
    return true;
}

void DocumentManager::OnDocumentViewChange(DocumentBase* doc)
{
    emit DocumentViewChanged(doc);
}

void DocumentManager::OnAutoSaveTimer()
{
    autoSaveTimer_->stop();
    QString backupDir = QDir::cleanPath(QApplication::applicationDirPath() + QDir::separator() + "Backups");
    QDir test(backupDir);
    if (!test.exists())
        test.mkdir(backupDir);
    if (auto backupTime = Settings::GetInstance()->GetValue("Auto Backup/Write backups into"))
    {
        QString path = backupTime->value_.toString();
        if (!path.isEmpty())
            backupDir = path;
    }
    if (!documents_.empty())
        ApplicationCore::SetStatusBarMessage("Performing automatic backup...");
    for (size_t i = 0; i < documents_.size(); ++i)
    {
        if (documents_[i]->IsDirty())
        {
            if (documents_[i]->DoBackup(backupDir))
                LOGINFO(QString("Wrote backup of %1 to %2").arg(documents_[i]->GetFileName(), backupDir));
            else
                LOGERROR(QString("Failed to write backup of file %1 to %2").arg(documents_[i]->GetFileName(), backupDir));
        }
    }
    autoSaveTimer_->start();
}

//TODOvoid DocumentManager::OnSettingUpdated(const SprueEngine::StringHash& setting, SprueEditor::Settings* settings)
//TODO{
//TODO    SetupTimer();
//TODO}

void DocumentManager::SetupTimer()
{
    if (auto backupTime = Settings::GetInstance()->GetValue("Auto Backup/Backup every N minutes"))
    {
        int value = backupTime->value_.toInt() * 1000;
        autoSaveTimer_->setInterval(std::max(value, 5000));
    }
    if (auto backupsOn = Settings::GetInstance()->GetValue("Auto Backup/Enable automatic backup"))
    {
        bool value = backupsOn->value_.toBool();
        if (value && !autoSaveTimer_->isActive())
        {
            autoSaveTimer_->setSingleShot(false);
            autoSaveTimer_->start();
        }
        else if (!value && autoSaveTimer_->isActive())
            autoSaveTimer_->stop();
    }
}