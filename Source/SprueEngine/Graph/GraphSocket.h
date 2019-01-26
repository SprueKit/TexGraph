#pragma once

#include <SprueEngine/IEditable.h>

namespace SprueEngine
{
    class GraphNode;
    class Graph;

    /// A socket which can be connected to other sockets to form the graph.
    struct GraphSocket
    {
        friend class GraphNode;
        friend class Graph;
    private:
        unsigned socketID;

        GraphSocket(GraphNode* node, unsigned typeID, unsigned socketID) :
            typeID(typeID), socketID(socketID), node(node),
            input(0), output(0), control(0), variable(0), secret(0)
        {
        }
    public:
        unsigned typeID;
        GraphNode* node;
        std::string name;
        unsigned flags = 0;     // Usage specific to graph purposes
        unsigned input : 1;     // Socket is an input
        unsigned output : 1;    // Socket is an output
        unsigned control : 1;   // Socket is a control socket
        unsigned variable : 1;  // Socket is a variable input (may overlap with input|output for hybrid tree evaluation)
        unsigned secret : 1;

        GraphSocket(GraphNode* node, const std::string& name, unsigned typeID) : node(node), name(name), typeID(typeID), input(0), output(0), control(0), variable(0), secret(0) {

        }

        virtual Variant GetValue() { return storedValue_.getType() != VT_None ? storedValue_ : defaultValue_; }

        void StoreValue(const Variant& value) { storedValue_ = value; }

        Variant GetDefaultValue() const { return defaultValue_; }
        void SetDefaultValue(const Variant& value) { defaultValue_ = value; }

        bool AcceptEdge(const GraphSocket* otherSocket) const;

        void Deserialize(Deserializer* src, const SerializationContext& context);

        void Serialize(Serializer* dest, const SerializationContext& context) const;

    #ifndef SPRUE_NO_XML
        void Deserialize(tinyxml2::XMLElement* fromElement, const SerializationContext& context);
        void Serialize(tinyxml2::XMLElement* intoElement, const SerializationContext& context);
    #endif

        bool HasConnections() const;
        std::vector< std::pair<GraphSocket*, GraphSocket*> > GetConnections() const;

        /// Returns true if connections are valid, if *disconnect* is set then invalid connections will be removed
        bool ValidateConnections(bool disconnect = false);

        void NotifyChange() const;

    private:
        Variant storedValue_;
        Variant defaultValue_;
    };

}