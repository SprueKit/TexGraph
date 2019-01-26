#include "Localizer.h"

#include "LanguageData.h"

Localizer* Localizer::instance_ = 0x0;

/// Language recorder is used for recording all queries, it passes input as received and builds a translation table.
class LanguageRecorder : public LanguageData
{
public:
    virtual QString Localize(const QString& input) const override
    {
        translation_[input] = input;
        return input;
    }
};

Localizer::Localizer() :
    activeLanguageData_(0x0)
{
}

Localizer::~Localizer()
{
    if (activeLanguageData_)
        delete activeLanguageData_;
}

QString Localizer::Localize(const QString& input) const
{
    if (activeLanguageData_)
        return activeLanguageData_->Localize(input);
    return input;
}

void Localizer::LoadLanguage(const QString& file)
{
    //TODO: verify file exists
    if (activeLanguageData_)
        delete activeLanguageData_;
    activeLanguageData_ = new LanguageData(file);
    emit LanguageChanged();
}

void Localizer::StartRecording()
{
    if (activeLanguageData_)
        delete activeLanguageData_;
    activeLanguageData_ = new LanguageRecorder();
}

void Localizer::StopRecording(const QString& fileOutput) const
{
    if (activeLanguageData_)
        activeLanguageData_->Save(fileOutput);
}

Localizer* Localizer::GetInstance()
{
    if (instance_ == 0x0)
        instance_ = new Localizer();
    return instance_;
}

QString Localizer::Translate(const QString& input)
{
    return GetInstance()->Localize(input);
}