#include "LogFile.h"

#include "Platform/VideoCard.h"

//#include <CL/cl.hpp>

#include <QDateTime>
#include <QSysInfo>

#include <vector>

#ifdef WIN32
#include <Windows.h>
#endif


#ifdef WIN32

static void GetCPUInfo(QString& brandName, int& coreCount, double& cpuSpeed)
{
    int CPUInfo[4] = { -1 };
    unsigned   nExIds, i = 0;
    char CPUBrandString[0x40];
    // Get the information associated with each extended ID.
    __cpuid(CPUInfo, 0x80000000);
    nExIds = CPUInfo[0];
    for (i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfo, i);
        // Interpret CPU brand string
        if (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }
    //string includes manufacturer, model and clockspeed
    brandName = CPUBrandString;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    coreCount = sysInfo.dwNumberOfProcessors;

    wchar_t Buffer[_MAX_PATH];
    DWORD BufSize = _MAX_PATH;
    DWORD dwMHz = _MAX_PATH;
    HKEY hKey;

    // open the key where the proc speed is hidden:
    long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
    if (lError != ERROR_SUCCESS)
    {
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lError, 0, Buffer, _MAX_PATH, 0);
        wprintf(Buffer);
        return;
    }

    // query the key:
    RegQueryValueEx(hKey, L"~MHz", NULL, NULL, (LPBYTE)&dwMHz, &BufSize);
    cpuSpeed = (double)dwMHz;
}
#endif

static QString size_human(unsigned size)
{
    float num = size;
    QStringList list;
    list /*<< "KB"*/ << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("kb");

    while (num >= 1024.0 && i.hasNext())
    {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num, 'f', 2) + " " + unit;
}


LogFile* LogFile::instance_ = 0x0;

LogFile::LogFile(const QString& path, const QString& logName, const QString& extraHeaderData) :
    QObject(),
    file_(path),
    logName_(logName)
{
    instance_ = this;

    file_.open(QIODevice::WriteOnly);
    stream_ = new QTextStream(&file_);

#ifndef SPRUE_HTML_LOG
    QTextStream& str = *stream_;

#ifdef WIN32
    MEMORYSTATUSEX data;
    GlobalMemoryStatusEx(&data);

    ULONGLONG memoryBytes;
    GetPhysicallyInstalledSystemMemory(&memoryBytes);
    unsigned totalRam = memoryBytes; // data.ullTotalPhys + data.ullAvailPhys;
#endif

    QString brand;
    int cores = 0;
    double mhz = 0.0;
    GetCPUInfo(brand, cores, mhz);

    VideoCard card;

    str << "<html>\r\n<head>\r\n<title>SprueKit Log</title>\r\n<style>table { border: dashed #777777; }  p { color: #CCCCCC; } h1 { color: #CCCCCC; } tr.INFO { color: #CCCCCC; } tr.WARNING { background: #CCCC00; } tr.ERROR { border: solid #FF0000; background: #AA0000; color: CCCCCC; } tr.DEBUG { background: #007700; color: #CCCCCC; }</style></head>\r\n<body bgcolor='333333'>\r\n<h1>SprueKit Log</h1>\r\n";
    str << "<p>Began: " << QDateTime::currentDateTime().toString() << "</p>\r\n";
    str << "<p><b>OS:</b> " << QSysInfo::prettyProductName();
    str << "<br/><b>Arch:</b> " << QSysInfo::buildCpuArchitecture();
    str << "<br/><b>CPU:</b> " << brand;
    str << "<br/><b>RAM:</b> " << size_human(totalRam);
    str << "<br/><b>GPU:</b> " << card.cardName << " v" << card.deviceVersion;
    str << "<br/><b>GPU Status:</b> " << card.errorStatus;
    str << "<br/><b>GPU Memory:</b> " << card.videoMemory;

    if (!extraHeaderData.isEmpty())
        str << extraHeaderData;

    //??std::vector<cl::Platform> platforms;
    //??cl::Platform::get(&platforms);
    //??for (auto &p : platforms) {
    //??    str << "<br/>Supports " << p.getInfo<CL_PLATFORM_VERSION>().c_str();
    //??}

    str << "</p>\r\n";
    str << "<div id='messages'>\r\n";
    str << "<p><input class=\"search\" placeholder=\"Search\" /></p>\r\n";
    str << "<table>\r\n<tbody class = 'list'>\r\n";
    str << "<script src=\"http://listjs.com/assets/javascripts/list.min.js\"></script>\r\n";
    str << "<script type='text/javascript'>\r\n";
    str << "document.addEventListener('DOMContentLoaded', function() {\r\n";
    str << "    var options = { valueNames: ['time', 'source', 'level', 'msg'] };\r\n";
    str << "    var userList = new List('messages', options);\r\n";
    str << "}, false);\r\n";
    str << "</script>\r\n";
    stream_->flush();
#endif
}

LogFile::~LogFile()
{
    *stream_ << "</tbody></table></body></html>";
    stream_->flush();
    file_.flush();
    file_.close();
    delete stream_;
}

LogFile* LogFile::GetInstance()
{
    return instance_;
}

void LogFile::Write(const QString& source, const QString& message, int level)
{
    QMutexLocker locker(&mutex_);

    // Prevent flooding the log with duplicate messages, assume that repeated failures are identifiable
    if (lastMessage_.compare(message) == 0)
        return;
    lastMessage_ = message;
    QString levelMsg;
    switch (level)
    {
    case 1:
        levelMsg = "ERROR";
        break;
    case 2:
        levelMsg = "WARNING";
        break;
    case 3:
        levelMsg = "INFO";
        break;
    case 4:
        levelMsg = "DEBUG";
        break;
    }

#ifndef SPRUE_HTML_LOG
    *stream_ << QString("<tr class='%2'><td class='time'>%1</td><td class='source'>%4</td><td class='level'>%2</td><td class='msg'>%3</td></tr>\r\n").arg(QDateTime::currentDateTime().toString(), levelMsg, message, source);
    stream_->flush();
#else
    *stream_ << QString("%1: %2: %3\r\n").arg(QDateTime::currentDateTime().toString(), levelMsg, message);
    stream_->flush();
#endif
    emit LogUpdated();
    emit NewMessage(source, message, levelMsg);
}

void LOGFILE_CALLBACK(const char* msg, int level)
{
    LogFile::GetInstance()->Write(LogFile::GetInstance()->GetName(), msg, level + 1);
}