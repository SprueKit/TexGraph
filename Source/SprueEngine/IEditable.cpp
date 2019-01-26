#include <SprueEngine/IEditable.h>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Deserializer.h>
#include <SprueEngine/FString.h>
#include <SprueEngine/IDService.h>
#include <SprueEngine/Logging.h>
#include <SprueEngine/MessageLog.h>
#include <SprueEngine/Serializer.h>
#include <SprueEngine/VectorBuffer.h>
#include <SprueEngine/Variant.h>
#include <SprueEngine/MathGeoLib/Algorithm/Random/LCG.h>

#include <iostream>
#include <functional>

namespace SprueEngine
{

IEditable::IEditable()
{
    sourceID_ = instanceID_ = Context::GetInstance()->GetService<IDService>()->GetID();
}

IEditable::~IEditable()
{
    Context::GetInstance()->GetService<IDService>()->ReleaseID(instanceID_);
}

void IEditable::SetSourceID(unsigned aValue)
{
    sourceID_ = aValue;
}

bool IEditable::HasProperty(const StringHash& aHash) const
{
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
        for (auto& var : found->second)
            if (var->GetHash() == aHash)
                return true;
    return false;
}

Variant IEditable::GetProperty(const std::string& aName) const
{
    return GetProperty(StringHash(aName));
}

Variant IEditable::GetProperty(const StringHash& aHash) const
{
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
    {
        for (auto& var : found->second)
        {
            if (var->GetHash() == aHash)
                return var->Get((void*)this);
        }
    }
    Variant v;
    return Variant();
}

VariantType IEditable::GetPropertyType(const StringHash& aHash) const
{
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
    {
        for (auto& var : found->second)
        {
            if (var->GetHash() == aHash)
                return var->GetDefault().getType();
        }
    }
    return VT_None;
}

VariantVector IEditable::GetProperties() const
{
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
    {
        VariantVector ret(found->second.size());
        for (auto& var : found->second)
            ret.push_back(var->Get((void*)this));
        return ret;
    }
    return VariantVector();
}

const std::vector<TypeProperty*>& IEditable::GetProperties(unsigned aFlags) const
{
    std::vector<TypeProperty*> ret;

    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
    {
        for (auto& var : found->second)
        {
            if (var->GetFlags() & aFlags)
                ret.push_back(var);
        }
        return ret;
    }
    return ret;
}

std::vector<std::string> IEditable::GetPropertyNames() const
{
    std::vector<std::string> ret;
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
    {
        for (auto& var : found->second)
            ret.push_back(var->GetName());
    }
    return ret;
}

void IEditable::SetProperty(const std::string& aName, const Variant& aVariant)
{
    SetProperty(StringHash(aName), aVariant);
}

void IEditable::SetProperty(const StringHash& aHash, const Variant& aVariant)
{
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
    {
        for (auto& var : found->second)
        {
            if (var->GetHash() == aHash)
            {
                var->Set((void*)this, aVariant);
                AttributeUpdated(var->GetHash());
                return;
            }
        }
    }
}

void IEditable::SetProperties(const VariantVector& aProperties)
{
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
    {
        for (size_t i = 0; i < aProperties.size() && i < found->second.size(); ++i)
            found->second[i]->Set((void*)this, aProperties[i]);
    }
    AttributeUpdated(StringHash());
}

void IEditable::ResetProperty(const std::string& aName)
{
    ResetProperty(aName);
}

void IEditable::ResetProperty(const StringHash& aHash)
{
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
    {
        for (auto& var : found->second)
        {
            if (var->GetHash() == aHash)
            {
                var->Reset(this);
                AttributeUpdated(var->GetHash());
                return;
            }
        }
    }
}

void IEditable::ResetProperties()
{
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
    {
        for (auto& var : found->second)
            var->Reset(this);
    }
    AttributeUpdated(StringHash());
}

TypeProperty* IEditable::FindProperty(const StringHash& aHash)
{
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
    {
        for (auto prop : found->second)
            if (prop->GetHash() == aHash)
                return prop;
    }
    return 0x0;
}

bool IEditable::Deserialize(Deserializer* aSrc, const SerializationContext& context)
{
    bool okay = true;
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    sourceID_ = aSrc->ReadUInt();
    if (found != table.end())
    {
        LOOP_START:
        while (aSrc->ReadBool())
        {
            StringHash propHash = aSrc->ReadStringHash();
            for (auto property : found->second)
            {
                if (property->GetHash() == propHash)
                {
                    property->Deserialize(this, aSrc, context);

                    goto LOOP_START;
                    break; //unreachable
                }
            }
            SPRUE_LOG_WARNING(FString("Property not found for IEditable deserialization: %1", propHash.value_).c_str());
        }

        int fieldPermCt = aSrc->ReadInt();
        for (int i = 0; i < fieldPermCt; ++i)
        {
            StringHash key = aSrc->ReadStringHash();
            int valCt = aSrc->ReadInt();
            Permutations perms;
            for (int ii = 0; ii < valCt; ++ii)
            {
                FieldPermutation perm;
                perm.permutationName_ = aSrc->ReadString();
                perm.permutationWeight_ = aSrc->ReadUInt();
                perm.permutationFlags_ = aSrc->ReadUInt();
                perm.value_ = aSrc->ReadVariant();
                perms.push_back(perm);
            }
            fieldPermutations_[key] = perms;
        }
    }
    else
    {
        // This isn't necessarilly an error, object may just have no properties
        SPRUE_ASSERT(found != table.end(), "Failed to find a registrary table for an object");
        return okay;
    }

    return okay;
}

bool IEditable::Serialize(Serializer* aDest, const SerializationContext& context) const
{
    bool okay = true;
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    aDest->WriteStringHash(GetTypeHash());
    IEditable* self = const_cast<IEditable*>(this);
    unsigned sourceID = GetSourceID();
    aDest->WriteUInt(sourceID);
    if (found != table.end())
    {
        for (auto prop : found->second)
        {
            if (prop->GetFlags() & PS_NoSerialize)
                continue;
            aDest->WriteBool(true);
            aDest->WriteStringHash(prop->GetHash());
            prop->Serialize(self, aDest, context);
        }
        aDest->WriteBool(false);

        if (fieldPermutations_.size())
        {
            aDest->WriteInt(fieldPermutations_.size());
            for (auto perm : fieldPermutations_)
            {
                aDest->WriteStringHash(perm.first);
                aDest->WriteInt(perm.second.size());
                for (auto fld : perm.second)
                {
                    aDest->WriteString(fld.permutationName_);
                    aDest->WriteUInt(fld.permutationWeight_);
                    aDest->WriteUInt(fld.permutationFlags_);
                    aDest->WriteVariant(fld.value_);
                }
            }
        }
        else
            aDest->WriteInt(0);
    }
    else
        return false;

    return okay;
}

#ifndef SPRUE_NO_XML
bool IEditable::Deserialize(tinyxml2::XMLElement* aNode, const SerializationContext& context)
{
    bool success = true;
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());
    if (found != table.end())
    {
        tinyxml2::XMLElement* child = aNode->FirstChildElement();
        while (child)
        {
            const char* fieldName = child->Name();
            for (auto property : found->second)
            {
                if (property->GetXMLSafeName().compare(fieldName) == 0)
                {
                    property->Deserialize(this, child, context);
                    break;
                }
            }
            child = child->NextSiblingElement();
        }

        child = aNode->FirstChildElement("Permutations");
        if (child)
        {
            auto permSetNode = child->FirstChildElement();
            while (permSetNode)
            {
                StringHash key(permSetNode->IntAttribute("hash"));
                VariantType type = (VariantType)permSetNode->IntAttribute("type");

                auto permFieldNode = permSetNode->FirstChildElement();
                Permutations perms;
                while (permFieldNode)
                {
                    FieldPermutation perm;
                    perm.permutationName_ = permFieldNode->Attribute("name");
                    perm.permutationWeight_ = permFieldNode->IntAttribute("weight");
                    perm.permutationFlags_ = permFieldNode->IntAttribute("flags");
                    perm.value_.FromString(type, permFieldNode->GetText());
                    perms.push_back(perm);

                    permFieldNode = permFieldNode->NextSiblingElement();
                }
                fieldPermutations_[key] = perms;

                permSetNode = permSetNode->NextSiblingElement();
            }
        }
    }
    return success;
}

bool IEditable::Serialize(tinyxml2::XMLElement* aParent, const SerializationContext& context) const
{
    tinyxml2::XMLElement* myElement = aParent->GetDocument()->NewElement(GetTypeName());
    myElement->SetAttribute("version", GetClassVersion());
    aParent->LinkEndChild(myElement);
    bool ret = SerializeProperties(myElement, context);
    return ret;
}

bool IEditable::SerializeProperties(tinyxml2::XMLElement* aIntoElement, const SerializationContext& context) const
{
    bool okay = true;
    auto table = Context::GetInstance()->GetPropertyTable();
    auto found = table.find(GetTypeHash());

    if (found != table.end())
    {
        IEditable* self = const_cast<IEditable*>(this);
        for (auto prop : found->second)
        {
            if (prop->GetFlags() & PS_NoSerialize)
                continue;
            prop->Serialize(self, aIntoElement, context);
        }

        if (fieldPermutations_.size())
        {
            auto permsElem = aIntoElement->GetDocument()->NewElement("Permutations");
            aIntoElement->LinkEndChild(permsElem);
            for (auto permSet : fieldPermutations_)
            {
                if (permSet.second.empty())
                    continue;

                auto thisSetElem = aIntoElement->GetDocument()->NewElement("PermutationSet");
                permsElem->LinkEndChild(thisSetElem);
                thisSetElem->SetAttribute("hash", permSet.first.value_);
                thisSetElem->SetAttribute("type", permSet.second[0].value_.type_);
                for (auto permFld : permSet.second)
                {
                    auto thisFldElem = aIntoElement->GetDocument()->NewElement("Permutation");
                    thisSetElem->LinkEndChild(thisFldElem);

                    thisFldElem->SetAttribute("name", permFld.permutationName_.c_str());
                    thisFldElem->SetAttribute("weight", permFld.permutationWeight_);
                    thisFldElem->SetAttribute("flags", permFld.permutationFlags_);
                    thisFldElem->SetText(permFld.value_.ConvertToString().c_str());
                }
            }
        }
    }

    return okay;
}
#endif

IEditable* IEditable::Clone() const
{
    VectorBuffer buffer;
    SerializationContext ctx;
    ctx.isClone_ = true;
    if (this->Serialize(&buffer, ctx))
    {
        buffer.Seek(0);
        if (IEditable* clone = Context::GetInstance()->Deserialize<IEditable>(&buffer, ctx))
        {
            clone->SetSourceID(GetSourceID());
            return clone;
        }
    }
    return 0x0;
}

IEditable* IEditable::Duplicate() const
{
    if (auto ret = Clone())
    {
        ret->MakeUnique();
        return ret;
    }
    return 0x0;
}

Permutations IEditable::GetFieldPermutations(const StringHash& field) const
{
    auto found = fieldPermutations_.find(field);
    if (found != fieldPermutations_.end())
        return found->second;
    return Permutations();
}

void IEditable::SetFieldPermutations(const StringHash& field, const Permutations& variants)
{
    fieldPermutations_[field] = variants;
}

void IEditable::RandomizePermutations(unsigned seed)
{
    LCG lcg(seed);
    for (auto perm : fieldPermutations_)
    {
        auto vec = perm.second;
        int idx = lcg.Int(0, vec.size() - 1);
        SetProperty(perm.first, vec[idx].value_);
    }
}

void IEditable::RandomizePermutation(const StringHash& field, unsigned seed)
{
    auto found = fieldPermutations_.find(field);
    if (found != fieldPermutations_.end())
    {
        if (!found->second.empty())
        {
            LCG lcg(seed);
            int idx = lcg.Int(0, found->second.size() - 1);
            SetProperty(field, found->second[idx].value_);
        }
    }
}

void IEditable::ApplyNamedPermutation(const std::string& name)
{
    for (auto perm : fieldPermutations_)
    {
        for (auto val : perm.second)
        {
            if (val.permutationName_.compare(name) == 0)
            {
                SetProperty(perm.first, val.value_);
                break;
            }
        }
    }
}

void IEditable::ApplyNamedPermutation(const StringHash& field, const std::string& name)
{
    auto found = fieldPermutations_.find(field);
    if (found != fieldPermutations_.end())
    {
        for (auto val : found->second)
        {
            if (val.permutationName_.compare(name) == 0)
            {
                SetProperty(field, val.value_);
                break;
            }
        }
    }
}

void IEditable::RandomizePermutationsWeighted(unsigned seed)
{
    LCG lcg(seed);
    for (auto field : fieldPermutations_)
    {
        if (field.second.empty())
            continue;

        int sum = 0;
        for (auto val : field.second)
            sum += val.permutationWeight_;
        int rnd = lcg.Int(0, sum - 1);
        for (auto val : field.second)
        {
            if (rnd <= val.permutationWeight_)
            {
                SetProperty(field.first, val.value_);
                break;
            }
            rnd -= val.permutationWeight_;
        }
    }
}

void IEditable::RandomizePermutationsWeighted(const StringHash& field, unsigned seed)
{
    LCG lcg(seed);
    auto found = fieldPermutations_.find(field);
    if (found != fieldPermutations_.end())
    {
        if (found->second.empty())
            return;

        int sum = 0;
        for (auto val : found->second)
            sum += val.permutationWeight_;
        int rnd = lcg.Int(0, sum - 1);
        for (auto val : found->second)
        {
            if (rnd <= val.permutationWeight_)
            {
                SetProperty(found->first, val.value_);
                break;
            }
            rnd -= val.permutationWeight_;
        }
    }
}

}