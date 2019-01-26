#pragma once

#include <EditorLib/editorlib_global.h>

#include <qobject>
#include <qfile>
#include <qtextstream>
#include <qmutex>
#include <QString>


/// Logfile object. Logs are written to AppData/{ApplicationName}/Log.html and use (via web URI) ListJS for searching the log.
/// ListJS load and initialization may take some time with larger logs.
class EDITORLIB_EXPORT LogFile : public QObject
{
    Q_OBJECT
public:
    /// Construct a log that writes to the given path. LogName specifies default source and extraHeaderData allows adding extra fields to the header.
    LogFile(const QString& path, const QString& logName, const QString& extraHeaderData = QString());
    virtual ~LogFile();

    static LogFile* GetInstance();

    void Write(const QString& source, const QString& message, int level);
    QString GetName() const { return logName_; }

signals:
    void LogUpdated();
    void NewMessage(const QString& source, const QString& msg, const QString& lvl);

private:
    static LogFile* instance_;
    QMutex mutex_;
    QFile file_;
    QString lastMessage_;
    QString logName_;
    QTextStream* stream_;
};

void LOGFILE_CALLBACK(const char*, int);

#define LOGERROR(MSG) LogFile::GetInstance()->Write("Editor", MSG, 1)
#define LOGWARNING(MSG) LogFile::GetInstance()->Write("Editor", MSG, 2)
#define LOGINFO(MSG) LogFile::GetInstance()->Write("Editor", MSG, 3)
#define LOGDEBUG(MSG) LogFile::GetInstance()->Write("Editor", MSG, 4)