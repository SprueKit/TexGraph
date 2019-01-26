#pragma once

#include <SprueEngine/Property.h>
#include <SprueEngine/Core/Context.h>

namespace SprueEngine
{

class IEditable;

class IEditablePropertyBase : public TypeProperty
{
public:
    IEditablePropertyBase(const std::string& name, const std::string& desc, unsigned flags) :
        TypeProperty(name, desc, flags, 0x0)
    {

    }
    // Get the list of types we can use
    virtual const char** GetTypeNames() const = 0;
    // Get hashes for the factories
    virtual const StringHash* GetTypeHashes() const = 0;
};

template<class CLASSTYPE>
class IEditableProperty : public IEditablePropertyBase
{
public:
    typedef void (CLASSTYPE::*SET_METHOD)(Variant type);
    typedef Variant (CLASSTYPE::*GET_METHOD)() const;

    IEditableProperty(GET_METHOD getter, SET_METHOD setter, const std::string& name, const std::string& desc, unsigned flags, const char** typeNames, const StringHash* typeHashes) :
        IEditablePropertyBase(name, desc, flags),
        getter_(getter),
        setter_(setter),
        typeNames_(typeNames),
        typeHashes_(typeHashes)
    {
    }

    virtual bool CanReset() const override { return false; }
    virtual void Reset(void* obj) { /*do nothing*/ }
    virtual Variant GetDefault() { return Variant(); }

    virtual Variant Get(void* obj)
    {
        CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
        return (src->*getter_)();
    }

    virtual void Set(void* obj, const Variant& value)
    {
        CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
        (src->*setter_)(value);
    }

    virtual void Serialize(void* obj, Serializer* src, const SerializationContext& context) override
    {
        Variant got = Get(obj);
        if (got.getVoidPtr())
        {
            src->WriteBool(true);
            src->WriteBool(false);
            IEditable* ptr = (IEditable*)got.getVoidPtr();
            //src->WriteStringHash(ptr->GetTypeHash());
            ptr->Serialize(src, context);
        }
        else if (got.getType() == VT_VariantVector)
        {
            VariantVector vec = got.getVariantVector();
            src->WriteBool(true);
            src->WriteBool(true);
            src->WriteUInt(vec.size());
            for (unsigned i = 0; i < vec.size(); ++i)
            {
                if (IEditable* ptr = (IEditable*)vec[i].getVoidPtr())
                    ptr->Serialize(src, context);
            }
        }
    }

    virtual void Deserialize(void* obj, Deserializer* dest, const SerializationContext& context) override
    {
        if (dest->ReadBool())
        {
            if (!dest->ReadBool())
            {
                IEditable* deserialized = Context::GetInstance()->Deserialize<IEditable>(dest, context);
                Set(obj, Variant((void*)deserialized));
            }
            else
            {
                unsigned count = dest->ReadUInt();
                VariantVector list;
                for (unsigned i = 0; i < count; ++i)
                    list.push_back((void*)Context::GetInstance()->Deserialize<IEditable>(dest, context));
                Set(obj, list);
            }
        }
        else
        {
            Set(obj, Variant((void*)0x0));
        }
    }

#ifndef SPRUE_NO_XML
    virtual void Serialize(void* object, tinyxml2::XMLElement* parent, const SerializationContext& context) override
    {
        Variant got = Get(object);
        if (IEditable* obj = (IEditable*)got.getVoidPtr())
        {
            tinyxml2::XMLElement* myElement = parent->GetDocument()->NewElement(ToXmlSafe(GetName()).c_str());
            parent->LinkEndChild(myElement);

            myElement->SetAttribute("type", obj->GetTypeName());
            obj->Serialize(myElement, context);
        }
    }

    virtual void Deserialize(void* object, tinyxml2::XMLElement* element, const SerializationContext& context) override
    {
        if (IEditable* child = Context::GetInstance()->Create<IEditable>(StringHash(element->Attribute("type"))))
        {
            if (child->Deserialize(element, context))
                Set(object, (void*)child);
            else
                delete child;
        }
    }
#endif

    // Get the list of types we can use
    const char** GetTypeNames() const { return typeNames_; }
    // Get hashes for the factories
    const StringHash* GetTypeHashes() const { return typeHashes_; }

private:
    GET_METHOD getter_;
    SET_METHOD setter_;

protected:
    const char** typeNames_;
    const StringHash* typeHashes_;
};

}