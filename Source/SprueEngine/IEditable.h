#pragma once

#include <SprueEngine/Variant.h>
#include <SprueEngine/SerializationContext.h>
#include <vector>

#ifndef SPRUE_NO_XML
    #include <SprueEngine/Libs/tinyxml2.h>
#endif

namespace SprueEngine
{

    class Deserializer;
    class Serialzier;
    class TypeProperty;

    struct SPRUE FieldPermutation
    {
        std::string permutationName_;
        unsigned permutationWeight_ = 1;
        unsigned permutationFlags_ = 0;
        Variant value_;

        FieldPermutation() { }
        FieldPermutation(const std::string& name, unsigned weight, unsigned flags, const Variant& value) :
            permutationName_(name),
            permutationWeight_(weight),
            permutationFlags_(flags),
            value_(value)
        {

        }
    };

    typedef std::vector<FieldPermutation> Permutations;
    typedef std::map<StringHash, Permutations > PermutationMap;

    class SPRUE IEditable
    {
        NOCOPYDEF(IEditable);
    public:
        IEditable();
        virtual ~IEditable();

    // Clone management:
        /// Gets the construction identifier for this object, it is always who it really is.
        unsigned GetInstanceID() const { return instanceID_; }
        /// Identifier for 'who' this is supposed to be (may not really be the case, allows unique identification in a clone scenario)
        unsigned GetSourceID() const { return sourceID_; }

        /// Sets the 'source ID' for this is 'him' but it's not really 'him'
        void SetSourceID(unsigned aValue);

        /// Whether this object is the original or a duplicate
        bool IsOriginal() const { return sourceID_ == instanceID_; }
        /// Returns true if this object is not the original source but a clone.
        bool IsClone() const { return sourceID_ != instanceID_; }
        /// Returns true if this object is a clone of the given object.
        bool IsCloneOf(IEditable* rhs) { return sourceID_ == rhs->instanceID_; }

        /// This function is dangerous and should only be used as part of serialization.
        void MakeUnique() { sourceID_ = instanceID_; }
        /// Changes the IDs such that this object is an "imitation" of the source it was cloned from.
        void ImitateSource() { instanceID_ = sourceID_; }

    public:
        /// Indicates the type of this object, used for serialization/deserialization with factories
        virtual StringHash GetTypeHash() const = 0;
        /// Similar to the above, but returns the actual type name, must be identical to the input string for the above hash for XML to work
        virtual const char* GetTypeName() const = 0;
    
    public:
        /// Returns true if this object has a property with the given hashed name.
        virtual bool HasProperty(const StringHash& aHash) const;
        /// Gets the value of a property
        virtual Variant GetProperty(const std::string& aName) const;
        /// Gets the value of a property
        virtual Variant GetProperty(const StringHash& aHash) const;
        /// Gets the type of a property.
        virtual VariantType GetPropertyType(const StringHash& aHash) const;
        /// Gets all property values as a flat list
        virtual VariantVector GetProperties() const;
        /// Gets properites whose flags match those given
        const std::vector<TypeProperty*>& GetProperties(unsigned aFlags) const;
        /// Returns a list of all property names.
        std::vector<std::string> GetPropertyNames() const;
        /// Sets the value of a property
        virtual void SetProperty(const std::string& aName, const Variant& aVariant);
        /// Sets the value of a property
        virtual void SetProperty(const StringHash& aHash, const Variant& aVariant);
        /// Sets the value of all properties
        virtual void SetProperties(const VariantVector& aProperties);
        /// Resets the named property to it's defaults
        virtual void ResetProperty(const std::string& aName);
        /// Resets the property to it's defaults
        virtual void ResetProperty(const StringHash& aHash);
        /// Resets all properties to their defaults
        virtual void ResetProperties();
        /// Retrieves a property.
        TypeProperty* FindProperty(const StringHash& aHash);

        /// Called whenever an attribute is updated, override to handle especially
        virtual void AttributeUpdated(const StringHash& aAttr) { }

        /// Deserialize from a binary buffer
        virtual bool Deserialize(Deserializer* aSrc, const SerializationContext& context);
        /// Serialize into a binary buffer
        virtual bool Serialize(Serializer* aDest, const SerializationContext& context) const;

        virtual IEditable* Clone() const;
        virtual IEditable* Duplicate() const;

        virtual void VersionUpdate(unsigned fromVersion) { }
        virtual unsigned GetClassVersion() const { return 1; }

#ifndef SPRUE_NO_XML
        /// Deserialize from an XML document
        virtual bool Deserialize(tinyxml2::XMLElement*, const SerializationContext& context);
        /// Serialize into an XML document
        virtual bool Serialize(tinyxml2::XMLElement*, const SerializationContext& context) const;
        virtual bool SerializeProperties(tinyxml2::XMLElement* aIntoElement, const SerializationContext& context) const;
#endif

        PermutationMap& GetFieldPermutationMap() { return fieldPermutations_; }
        const PermutationMap& GetFieldPermutationMap() const { return fieldPermutations_; }

        Permutations GetFieldPermutations(const StringHash& field) const;
        void SetFieldPermutations(const StringHash& field, const Permutations& variants);

        /// Randomizes (equally weighted) the permutations for all fields.
        virtual void RandomizePermutations(unsigned seed);
        /// Randomizes (equally weighted) the permutation for a given field.
        virtual void RandomizePermutation(const StringHash& field, unsigned seed);
        /// Sets all fields to use a named permutation (where available)
        virtual void ApplyNamedPermutation(const std::string& name);
        /// Sets the field to use a named permutation (if available)
        virtual void ApplyNamedPermutation(const StringHash& field, const std::string& name);
        /// Sets the permutations for all fields by random weighting.
        virtual void RandomizePermutationsWeighted(unsigned seed);
        /// Sets the permutation for the given field by random weighting.
        virtual void RandomizePermutationsWeighted(const StringHash& field, unsigned seed);

    protected:
        std::map<StringHash, std::vector<FieldPermutation> > fieldPermutations_;

    private:
        unsigned instanceID_;
        unsigned sourceID_;
    };

#define SPRUE_EDITABLE(TYPENAME) public: virtual StringHash GetTypeHash() const override { return StringHash( #TYPENAME ); } virtual const char* GetTypeName() const { return #TYPENAME; } static StringHash GetStaticTypeHash() { return StringHash(#TYPENAME); } static const char* GetStaticTypeName() { return #TYPENAME; }

}