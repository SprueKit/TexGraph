#pragma once

#include <SprueEngine/IEditable.h>
#include <SprueEngine/VectorBuffer.h>

namespace SprueEngine
{

    /// Container for editable objects in a file.
    class SPRUE DataFile : public IEditable
    {
    public:
        /// Construct.
        DataFile();
        /// Destruct.
        virtual ~DataFile();

        /// Register object factory and properties.
        static void Register(Context* context);
        /// Return the type ID.
        virtual StringHash GetTypeHash() const;
        /// Return the readable type name.
        virtual const char* GetTypeName() const;

        /// Returns a buffer that can be used to write arbitrary data into.
        VectorBuffer& GetCustomData() { return customData_; }
        /// Returns a buffer that can be used to read arbitrary data from.
        const VectorBuffer& GetCustomData() const { return customData_; }

        /// Returns a buffer that can be used to write arbitrary GUI related data into.
        VectorBuffer& GetGuiData() { return guiData_; }
        /// Returns a buffer that can be used to read arbitrary GUI related data from.
        const VectorBuffer& GetGuiData() const { return guiData_; }

        /// Gets the first editable.
        template<typename T>
        T* GetEditable() { 
            std::static_assert(std::is_base_of<IEditable, T>::value);
            if (editables_.empty()) 
                return 0x0; 
            return dynamic_cast<T*>(editables_.front()); 
        }

        /// Gets the editable object at a specific index.
        template<typename T>
        T* GetEditable(unsigned idx) { 
            std::static_assert(std::is_base_of<IEditable, T>::value);
            if (editables_.empty() || idx >= editables_.size())
                return 0x0;
            return dynamic_cast<T*>(editables_[i]); 
        }
        
        /// Returns the number of objects in the file.
        unsigned GetEditableCount() const { return editables_.size(); }

        /// Adds an IEditable object to the file.
        void AddEditable(IEditable* editable) { editables_.push_back(editable); }
        /// Sets the IEditable object at a specific index.
        void SetEditable(unsigned idx,IEditable* editable) { editables_[idx] = editable; }

        /// Write into a Serializer.
        virtual bool Serialize(Serializer*, const SerializationContext& context) const override;
        /// Read from a serializer.
        virtual bool Deserialize(Deserializer*, const SerializationContext& context) override;
#ifndef SPRUE_NO_XML
        /// Read from an xml element.
        virtual bool Deserialize(tinyxml2::XMLElement*, const SerializationContext& context) override;
        /// Write into an xml element.
        virtual bool Serialize(tinyxml2::XMLElement*, const SerializationContext& context) const override;
#endif

    private:
        std::vector<IEditable*> editables_;
        VectorBuffer customData_;
        VectorBuffer guiData_;
    };

}