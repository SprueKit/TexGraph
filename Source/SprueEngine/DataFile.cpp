#include "DataFile.h"

#include <SprueEngine/Core/Context.h>
#include "StringConversion.h"

namespace SprueEngine
{

    DataFile::DataFile()
    {

    }

    DataFile::~DataFile()
    {
        for (auto editable : editables_)
            delete editable;
    }

    void DataFile::Register(Context* context)
    {
        context->RegisterFactory<DataFile>("DataFile", "Base container for all types of data files");
    }

    StringHash DataFile::GetTypeHash() const
    {
        return StringHash("DataFile");
    }
    
    const char* DataFile::GetTypeName() const
    {
        return "DataFile";
    }

    bool DataFile::Serialize(Serializer* dst, const SerializationContext& context) const
    {
        dst->WriteStringHash(GetTypeHash());
        dst->WriteUInt(GetClassVersion());
        if (customData_.GetSize())
        {
            dst->WriteBool(true);
            dst->WriteBuffer(customData_.GetBuffer());
        }
        else
            dst->WriteBool(false);

        if (guiData_.GetSize())
        {
            dst->WriteBool(true);
            dst->WriteBuffer(guiData_.GetBuffer());
        }
        else
            dst->WriteBool(false);

        dst->WriteUInt(editables_.size());
        for (auto editable : editables_)
            editable->Serialize(dst, context);

        return true;
    }
        
    bool DataFile::Deserialize(Deserializer* src, const SerializationContext& context)
    {
        unsigned version = src->ReadUInt();
        if (src->ReadBool())
            customData_ = src->ReadBuffer();
        if (src->ReadBool())
            guiData_ = src->ReadBuffer();
        unsigned ct = src->ReadUInt();
        for (unsigned i = 0; i < ct; ++i)
        {
            if (auto newObj = Context::GetInstance()->Deserialize<IEditable>(src, context))
                editables_.push_back(newObj);
        }
        return true;
    }

    bool DataFile::Deserialize(tinyxml2::XMLElement* element, const SerializationContext& context)
    {
        if (auto custElem = element->FirstChildElement("custom-data"))
            SprueEngine::FromString(custElem->GetText(), &customData_);
        if (auto guiElem = element->FirstChildElement("gui-data"))
            SprueEngine::FromString(guiElem->GetText(), &guiData_);

        auto firstContent = element->FirstChildElement("contents");
        while (firstContent)
        {
            if (auto newObj = Context::GetInstance()->Deserialize<IEditable>(firstContent, context))
                editables_.push_back(newObj);
            firstContent = firstContent->NextSiblingElement("contents");
        }

        return true;
    }

    bool DataFile::Serialize(tinyxml2::XMLElement* element, const SerializationContext& context) const
    {
        bool good = true;
        if (customData_.GetSize())
        {
            auto custElem = element->GetDocument()->NewElement("custom-data");
            custElem->SetText(SprueEngine::ToString(customData_).c_str());
            element->LinkEndChild(custElem);
        }

        if (guiData_.GetSize())
        {
            auto guiElem = element->GetDocument()->NewElement("gui-data");
            guiElem->SetText(SprueEngine::ToString(guiData_).c_str());
            element->LinkEndChild(guiElem);
        }

        auto dataElem = element->GetDocument()->NewElement("contents");
        element->LinkEndChild(dataElem);
        for (auto editable : editables_)
            good &= editable->Serialize(dataElem, context);

        return good;
    }
}