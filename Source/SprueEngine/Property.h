#pragma once

#include <SprueEngine/GeneralUtility.h>
#include <SprueEngine/Resource.h>
#include <SprueEngine/ResourceLoader.h>
#include <SprueEngine/ResourceStore.h>
#include <SprueEngine/SerializationContext.h>
#include <SprueEngine/StringHash.h>
#include <SprueEngine/Variant.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <SprueEngine/Serializer.h>
#include <SprueEngine/Deserializer.h>

#ifndef SPRUE_NO_XML
    #include <SprueEngine/Libs/tinyxml2.h>
#endif

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace SprueEngine
{
    

    /// Flags denoting the control parameters of a property
    enum PropertySettings
    {
        PS_Default = 0,                 // Default no control parameters
        PS_Translation = 1,             // Vec3 is a translation (gizmo selection hint)
        PS_Scale = 1 << 1,              // Vec3 is a scale (gizmo selection hint)
        PS_Rotation = 1 << 2,           // Quat is a rotation (gizmo selection hint)
        PS_ObjectID = 1 << 3,           // Integer is an object identifier
        PS_Required = 1 << 4,           // Field is mandatory (strings cannot be empty)
        PS_Secret = 1 << 5,             // Field is secret and not shown in the UI, internal values that shouldn't be seen by the user
        PS_NoSerialize = 1 << 6,        // Field is not serialized
        PS_ReadOnly = 1 << 7,           // Field cannot be edited
        PS_NoDefault = 1 << 9,          // Field has no default reset capability (shouldn't be common?)
        PS_Gizmo = 1 << 10,             // Field can be gizmo manipulated (Matrix, Quat, or Vec3)
        PS_NoAlpha = 1 << 11,           // RGBA field ignores Alpha, allows only setting RGB
        PS_RaySelectable = 1 << 12,     // Field is a shape of some kind that should be ray checked for selection events
        PS_IsVisibility = 1 << 13,      // Field is a boolean control for "visibile" status (UI hint)
        PS_IsLock = 1 << 14,            // Field is a boolean control for "locked" status (UI hint)
        PS_Decompose = 1 << 15,         // Field is a Mat3x4 that should be decomposed into position, rotation, and scale
        PS_LocalMat = 1 << 16,          // Field is a local Mat3x4 that should be multiplied with the parent
        PS_Flags = 1 << 17,             // Field is a series of bit flags, enum names are instead used as bit names, must be 
        PS_UserFlags = 1 << 18,         // As above, but the user defines the flags - names must come from UI somewhere ALWAYS UINT
        PS_NormalRange = 1 << 19,       // Is limited to a range of 0.0 ... 1.0 (applies to floating point members as well ie. Vec3, RGBA)
        PS_VisualConsequence = 1 << 20, // Changes to this affect a visual display consequence (not typically considered in regards to transforms)
        PS_NoFileSerialize = 1 << 21,   // Is serialized when doing a mem read/write, but not when doing a file read/write (ie. IEditable IDs, where the mem read/write is used for copy)
        PS_IEditableObject = 1 << 22,   // Field is a Variant containing an IEditable object, property is expected to be an IEditableProperty
        PS_IEditableList = 1 << 23,     // Field is a VariantVector containing pointers to IEditable objects, property is expected to be an IEditableListProperty as well
        PS_SmallIncrement = 1 << 24,    // Field should increment in a small fashion, 0.1 per step
        PS_TinyIncrement = 1 << 25,     // Field should increment in a tiny fashion, 0.01 per step
        PS_Permutable = 1 << 26,        // Field supports permutations
        PS_MeshingConsequence = 1 << 27,// Changes to the field require updating geometry/UV-maps/bone-weights
        PS_TextureConsequence = 1 << 28,// Changes to the field require updating texture maps
    };

    /// Strongly inspired by Urho3D's "Attribute" registration system
    class TypeProperty
    {
    public:
        TypeProperty(const std::string& name, const std::string& desc, unsigned flags, const char** enumNames) 
        { 
            propertyName_ = name; 
            description_ = desc; 
            flags_ = flags;
            enumNames_ = enumNames;
            hash_ = name;
#ifndef SPRUE_NO_XML
            xmlSafeName_ = ToXmlSafe(propertyName_);
#endif
        }

        const std::string& GetName() const { return propertyName_; }
        const std::string& GetDescription() const { return description_; }
        bool IsReadonly() const { return (flags_ & (unsigned)PS_ReadOnly) > 0; }
        unsigned GetFlags() const { return flags_; }
        /// It is possible that a derived type may wish to change properties (such as PS_Secret).
        void SetFlags(unsigned aFlags) { flags_ = aFlags; }
        virtual bool CanReset() const { return true; }

        virtual Variant Get(void* obj) = 0;
        virtual void Set(void* obj, const Variant& variant) = 0;
        virtual void Reset(void* obj) = 0;
        virtual Variant GetDefault() = 0;

        /// Override as necessary to deal with more complicated items (most shouldn't need to)
        virtual void Serialize(void* obj, Serializer* dest, const SerializationContext& context) { dest->WriteVariant(Get(obj)); }

        /// Override as necessary to deal with more complicated items (most shouldn't need to)
        virtual void Deserialize(void* obj, Deserializer* src, const SerializationContext& context) { Set(obj, src->ReadVariant()); }

#ifndef SPRUE_NO_XML
        virtual void Serialize(void* obj, tinyxml2::XMLElement* element, const SerializationContext& context) {
            Variant value = Get(obj);
            tinyxml2::XMLElement* myElement = element->GetDocument()->NewElement(xmlSafeName_.c_str());
            element->LinkEndChild(myElement);
            myElement->SetAttribute("type", Variant::VariantTypeToString(value.getType()));
            myElement->SetText(value.ConvertToString().c_str());
        }
        virtual void Deserialize(void* obj, tinyxml2::XMLElement* element, const SerializationContext& context)
        {
            VariantType type = Variant::VariantTypeFromString(element->Attribute("type"));
            if (type != VT_None)
            {
                Variant var;
                var.FromString(type, element->GetText());
                Set(obj, var);
            }
        }

        const std::string& GetXMLSafeName() const { return xmlSafeName_; }
#endif

        const char** GetEnumNames() const { return enumNames_; }

        const StringHash& GetHash() const { return hash_; }

    private:
        StringHash hash_;
        std::string propertyName_;
#ifndef SPRUE_NO_XML
        std::string xmlSafeName_;
#endif
        std::string description_;
        const char** enumNames_;
        unsigned flags_;
    };

    template<typename CLASSTYPE, typename PROPTYPE, typename SECRETTYPE>
    class TypePropertyImpl : public TypeProperty
    {
    public:

        typedef void (CLASSTYPE::*SET_METHOD)(PROPTYPE type);
        typedef PROPTYPE (CLASSTYPE::*GET_METHOD)() const;

        TypePropertyImpl(GET_METHOD getter, SET_METHOD setter, PROPTYPE defaultValue, const std::string& name, const std::string& desc, unsigned flags) : TypeProperty(name, desc, flags, 0x0)
        {
            setter_ = setter;
            getter_ = getter;
            defaultValue_ = defaultValue;
        }

        TypePropertyImpl(GET_METHOD getter, SET_METHOD setter, PROPTYPE defaultValue, const std::string& name, const std::string& desc, unsigned flags, const char** enumNames) : TypeProperty(name, desc, flags, enumNames)
        {
            setter_ = setter;
            getter_ = getter;
            defaultValue_ = defaultValue;
        }

        PROPTYPE GetValue(void* obj) const { 
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            return (PROPTYPE)(src->*getter_)(); 
        }

        void SetValue(void* obj, const PROPTYPE& value) { 
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            (src->*setter_)((SECRETTYPE)value); 
        }

        virtual Variant Get(void* obj) { 
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            return (PROPTYPE)(src->*getter_)();
        }
        
        virtual void Set(void* obj, const Variant& variant) { 
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            (src->*setter_)((SECRETTYPE)variant.get<PROPTYPE>());
        }

        virtual void Reset(void* obj) {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            (src->*setter_)(defaultValue_);
        }

        virtual Variant GetDefault() {
            return defaultValue_;
        }

    private:
        GET_METHOD getter_;
        SET_METHOD setter_;
        PROPTYPE defaultValue_;
    };

    template<typename CLASSTYPE, typename PROPTYPE, typename SECRETTYPE>
    class TypePropertyImplConstSet : public TypeProperty
    {
    public:

        typedef void (CLASSTYPE::*SET_METHOD)(const PROPTYPE & type);
        typedef PROPTYPE(CLASSTYPE::*GET_METHOD)() const;

        TypePropertyImplConstSet(GET_METHOD getter, SET_METHOD setter, PROPTYPE defaultValue, const std::string& name, const std::string& desc, unsigned flags) : TypeProperty(name, desc, flags, 0x0)
        {
            setter_ = setter;
            getter_ = getter;
            defaultValue_ = defaultValue;
        }

        TypePropertyImplConstSet(GET_METHOD getter, SET_METHOD setter, PROPTYPE defaultValue, const std::string& name, const std::string& desc, unsigned flags , const char** enumNames) : TypeProperty(name, desc, flags, enumNames)
        {
            setter_ = setter;
            getter_ = getter;
            defaultValue_ = defaultValue;
        }

        PROPTYPE GetValue(void* obj) const {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            return (PROPTYPE)(src->*getter_)();
        }

        void SetValue(void* obj, const PROPTYPE& value) {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            (src->*setter_)((SECRETTYPE)value);
        }

        virtual Variant Get(void* obj) {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            return (PROPTYPE)(src->*getter_)();
        }

        virtual void Set(void* obj, const Variant& variant) {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            (src->*setter_)((SECRETTYPE)variant.get<PROPTYPE>());
        }

        virtual void Reset(void* obj) {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            (src->*setter_)(defaultValue_);
        }
        virtual Variant GetDefault() {
            return defaultValue_;
        }

    private:
        GET_METHOD getter_;
        SET_METHOD setter_;
        PROPTYPE defaultValue_;
    };

    template<typename CLASSTYPE, typename PROPTYPE, typename SECRETTYPE>
    class MemoryTypePropertyImpl : public TypeProperty
    {
    public:

        MemoryTypePropertyImpl(size_t offset, PROPTYPE defaultValue, const std::string& name, const std::string& desc, unsigned flags) : TypeProperty(name, desc, flags, 0x0)
        {
            offset_ = offset;
            defaultValue_ = defaultValue;
        }

        MemoryTypePropertyImpl(size_t offset, PROPTYPE defaultValue, const std::string& name, const std::string& desc, unsigned flags, const char** enumNames) : TypeProperty(name, desc, flags, enumNames)
        {
            offset_ = offset;
            defaultValue_ = defaultValue;
        }

        PROPTYPE GetValue(void* obj) const {
            unsigned char* src = reinterpret_cast<unsigned char*>(obj);
            return (PROPTYPE)*((SECRETTYPE*)(src + offset_));
        }

        void SetValue(void* obj, const PROPTYPE& value) {
            unsigned char* src = reinterpret_cast<unsigned char*>(obj);
            *((SECRETTYPE*)(src + offset_)) = ((SECRETTYPE)value);
        }

        virtual Variant Get(void* obj) {
            unsigned char* src = reinterpret_cast<unsigned char*>(obj);
            return (PROPTYPE)*((SECRETTYPE*)(src + offset_));
        }

        virtual void Set(void* obj, const Variant& variant) {
            unsigned char* src = reinterpret_cast<unsigned char*>(obj);
            *((SECRETTYPE*)(src + offset_)) = ((SECRETTYPE)variant.get<PROPTYPE>());
        }

        virtual void Reset(void* obj)
        {
            unsigned char* src = reinterpret_cast<unsigned char*>(obj);
            *((SECRETTYPE*)(src + offset_)) = ((SECRETTYPE)defaultValue_);
        }

        virtual Variant GetDefault() { return defaultValue_; }

    private:
        size_t offset_;
        PROPTYPE defaultValue_;
    };

    template<typename CLASSTYPE, typename PROPTYPE, typename SECRETTYPE>
    class EnumPropertyImpl : public TypeProperty
    {
        BASECLASSDEF(EnumPropertyImpl, TypeProperty);
    public:
        typedef void (CLASSTYPE::*SET_METHOD)(PROPTYPE type);
        typedef PROPTYPE(CLASSTYPE::*GET_METHOD)() const;

        EnumPropertyImpl(GET_METHOD getter, SET_METHOD setter, PROPTYPE defaultValue, const std::string& name, const std::string& desc, unsigned flags, const char** enumNames) : 
            base(name, desc, flags, enumNames)
        {
            setter_ = setter;
            getter_ = getter;
            defaultValue_ = defaultValue;
        }

        PROPTYPE GetValue(void* obj) const {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            return (PROPTYPE)(src->*getter_)();
        }

        void SetValue(void* obj, const PROPTYPE& value) {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            (src->*setter_)((SECRETTYPE)value);
        }

        virtual Variant Get(void* obj) {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            return (PROPTYPE)(src->*getter_)();
        }

        virtual void Set(void* obj, const Variant& variant) {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            (src->*setter_)((SECRETTYPE)variant.getInt());
        }

        virtual void Reset(void* obj) {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            (src->*setter_)(defaultValue_);
        }

        virtual Variant GetDefault() { return defaultValue_; }

    private:
        GET_METHOD getter_;
        SET_METHOD setter_;
        PROPTYPE defaultValue_;
    };

    template<typename CLASSTYPE, typename RESOURCETYPE>
    class ResourcePropertyImpl : public TypeProperty {
    public:
        typedef ResourceHandle (CLASSTYPE::*HANDLE_GETTER)() const;
        typedef void (CLASSTYPE::*HANDLE_SETTER)(const ResourceHandle&);
        typedef std::shared_ptr<RESOURCETYPE> (CLASSTYPE::*RESOURCE_GETTER)() const;
        typedef void (CLASSTYPE::*RESOURCE_SETTER)(const std::shared_ptr<RESOURCETYPE>&);

        ResourcePropertyImpl(HANDLE_GETTER hget, HANDLE_SETTER hset, RESOURCE_GETTER resourceGetter, RESOURCE_SETTER resourceSetter, ResourceHandle defaultVal, const char* propertyName, const char* propertyDesc, unsigned flags) :
            TypeProperty(propertyName, propertyDesc, flags, 0x0),
            handleGetter_(hget),
            handleSetter_(hset),
            resourceGetter_(resourceGetter),
            resourceSetter_(resourceSetter),
            defaultVal_(defaultVal)
        {

        }

        std::shared_ptr<RESOURCETYPE> GetResource(void* object) const
        {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            return (src->resourceGetter_)();
        }

        virtual Variant Get(void* obj) {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            return (src->*handleGetter_)();
        }

        virtual void Set(void* obj, const Variant& variant) {
            CLASSTYPE* src = static_cast<CLASSTYPE*>(obj);
            ResourceHandle handle = variant.getResourceHandle();
            (src->*handleSetter_)(handle);

            std::shared_ptr<RESOURCETYPE> resource = Context::GetInstance()->GetService<ResourceStore>()->GetOrLoadResource<RESOURCETYPE>(handle.Name.c_str(), handle.Type);
            (src->*resourceSetter_)(resource);
        }

        virtual void Reset(void* obj) { Set(obj, defaultVal_); }
        
        virtual Variant GetDefault() override { return defaultVal_; }

        /// Override as necessary to deal with more complicated items (most shouldn't need to)
        virtual void Serialize(void* obj, Serializer* dest, const SerializationContext& context) override
        { 
            ResourceHandle var = Get(obj).getResourceHandle();
            var.UnmapPath(context.relativePath_);
            dest->WriteVariant(var); 
        }

        /// Override as necessary to deal with more complicated items (most shouldn't need to)
        virtual void Deserialize(void* obj, Deserializer* src, const SerializationContext& context) override
        { 
            ResourceHandle read = src->ReadVariant().getResourceHandle();
            read.RemapPath(context.relativePath_);

            // Verify we can reach the file, if not then push a path error
            if (!FileAccessible(read.Name))
                context.pathErrors_.push_back(SerializationContext::PathError{ (IEditable*)obj, this, read.Name });

            Set(obj, read); 
        }

#ifndef SPRUE_NO_XML
        virtual void Serialize(void* obj, tinyxml2::XMLElement* element, const SerializationContext& context) override {
            ResourceHandle value = Get(obj).getResourceHandle();
            value.UnmapPath(context.relativePath_);
            Variant var = value;
            tinyxml2::XMLElement* myElement = element->GetDocument()->NewElement(ToXmlSafe(GetName()).c_str());
            element->LinkEndChild(myElement);
            myElement->SetAttribute("type", Variant::VariantTypeToString(var.getType()));
            myElement->SetText(var.ConvertToString().c_str());
        }
        
        virtual void Deserialize(void* obj, tinyxml2::XMLElement* element, const SerializationContext& context) override
        {
            VariantType type = Variant::VariantTypeFromString(element->Attribute("type"));
            if (type != VT_None)
            {
                Variant var;
                var.FromString(type, element->GetText());
                ResourceHandle handle = var.getResourceHandle();
                handle.RemapPath(context.relativePath_);

                // Verify we can reach the file, if not then push a path error
                if (!FileAccessible(handle.Name))
                    context.pathErrors_.push_back(SerializationContext::PathError{ (IEditable*)obj, this, handle.Name });

                Set(obj, handle);
            }
        }
#endif

        HANDLE_GETTER handleGetter_;
        HANDLE_SETTER handleSetter_;
        RESOURCE_GETTER resourceGetter_;
        RESOURCE_SETTER resourceSetter_;
        ResourceHandle defaultVal_;
    };

    template<typename CLASSTYPE, typename PROPTYPE, typename SECRETTYPE>
    class PointerBasedMemoryTypePropertyImpl : public TypeProperty
    {
    public:

        PointerBasedMemoryTypePropertyImpl(size_t offset, PROPTYPE defaultValue, const std::string& name, const std::string& desc, unsigned flags) : TypeProperty(name, desc, flags, 0x0)
        {
            offset_ = offset;
            defaultValue_ = defaultValue;
        }

        PointerBasedMemoryTypePropertyImpl(size_t offset, PROPTYPE defaultValue, const std::string& name, const std::string& desc, unsigned flags, const char** enumNames) : TypeProperty(name, desc, flags, enumNames)
        {
            offset_ = offset;
            defaultValue_ = defaultValue;
        }

        PROPTYPE GetValue(void* obj) const {
            unsigned char* src = reinterpret_cast<unsigned char*>(obj);
            return (PROPTYPE)*((SECRETTYPE*)(src + offset_));
        }

        void SetValue(void* obj, const PROPTYPE& value) {
            unsigned char* src = reinterpret_cast<unsigned char*>(obj);
            *((SECRETTYPE*)(src + offset_)) = ((SECRETTYPE*)&value);
        }

        virtual Variant Get(void* obj) {
            unsigned char* src = reinterpret_cast<unsigned char*>(obj);
            return ((SECRETTYPE*)(src + offset_));
        }

        virtual void Set(void* obj, const Variant& variant) {
            unsigned char* src = reinterpret_cast<unsigned char*>(obj);
            *((SECRETTYPE*)(src + offset_)) = *((SECRETTYPE*)variant.get<PROPTYPE>());
        }

        virtual void Reset(void* obj)
        {
            unsigned char* src = reinterpret_cast<unsigned char*>(obj);
            *((SECRETTYPE*)(src + offset_)) = ((SECRETTYPE*)&defaultValue_);
        }

        virtual Variant GetDefault() { return defaultValue_; }

    private:
        size_t offset_;
        PROPTYPE defaultValue_;
    };

    typedef std::vector<TypeProperty*> PropertyList;

    /// Maps typename hashes to a list of properties
    typedef std::map<StringHash, PropertyList> PropertyTable;
}