#pragma once

#include <SprueEngine/IEditable.h>
#include <SprueEngine/Graph/GraphConstants.h>
#include <SprueEngine/Graph/GraphNode.h>
#include <SprueEngine/StringHash.h>
#include <SprueEngine/Variant.h>

#include <vector>
#include <unordered_map>

namespace SprueEngine
{

class Deserializer;
class Graph;
class GraphNode;
struct GraphSocket;
class Serializer;

/// A graph that accepts an arbitrary number of vertex connections
class Graph : public IEditable
{
    friend struct GraphSocket;
    friend class GraphNode;
public:    
    Graph();
    virtual ~Graph();

    /// Implement IEditable
    SPRUE_EDITABLE(Graph);
    static void Register(Context*);

    void AddNode(GraphNode* node, bool entryPoint);

    void RemoveNode(GraphNode* node);

    void SetMasterNode(GraphNode* node)
    {
        masterNode_ = node;
    }

    /// Gets a graph node by name.
    GraphNode* GetNode(const std::string& name);
    /// Gets a graph node by name.
    const GraphNode* GetNode(const std::string& name) const;
    /// Gets a graph node by the IEditable::SourceID.
    GraphNode* GetNodeBySourceID(unsigned id);
    /// Gets a graph node by the IEditable::SourceID.
    const GraphNode* GetNodeBySourceID(unsigned id) const;
    /// Gets a graph node by the IEditable::InstanceID.
    GraphNode* GetNodeByInstanceID(unsigned id);
    /// Gets a graph node by the IEditable::InstanceID.
    const GraphNode* GetNodeByInstanceID(unsigned id) const;

    /// Left->Right and Right->Left graphs must contain a "master node"
    GraphNode* GetMasterNode() { return masterNode_; }
    const GraphNode* GetMasterNode() const { return masterNode_; }

    std::vector<GraphNode*>& GetNodes() { return nodes_; }
    const std::vector<GraphNode*>& GetNodes() const { return nodes_; }

    template<typename T>
    std::vector<T*> GetNodesByType() const {
        std::vector<T*> ret;
        for (auto node : nodes_)
            if (dynamic_cast<T*>(node))
                ret.push_back((T*)node);
        return ret;
    }

    std::vector<GraphNode*>& GetEntryNodes() { return entryNodes_; }
    const std::vector<GraphNode*>& GetEntryNodes() const { return entryNodes_; }

    /// Hybrid graphs may contain many different entry points (such as "events")
    void AddEntryNode(GraphNode* node) { entryNodes_.push_back(node); }
    GraphNode* GetEntryNode(unsigned id);
    GraphNode* GetEntryNode(const std::string& name);
    GraphNode* GetEntryNode(const StringHash& eventID);
    GraphNode* GetEntryNodeByType(const StringHash& typeID);
    bool IsEntryNode(const GraphNode* node) const;

    /// Socket connection management
    bool Connect(GraphSocket* from, GraphSocket* to);
    bool Disconnect(GraphSocket* from, GraphSocket* to);
    bool DisconnectAll(GraphSocket* socket);
    bool DisconnectAll(GraphNode* node);
    bool Connected(GraphSocket* from, GraphSocket* to);
    bool CanConnect(GraphSocket* from, GraphSocket* to);

    /// Read from a stream
    virtual bool Deserialize(Deserializer* src, const SerializationContext& context) override;
    /// Write to a stream
    virtual bool Serialize(Serializer* dest, const SerializationContext& context) const override;
#ifndef SPRUE_NO_XML
    virtual bool Deserialize(tinyxml2::XMLElement*, const SerializationContext& context) override;
    virtual bool Serialize(tinyxml2::XMLElement*, const SerializationContext& context) const override;
#endif

    /// Intended only for GUI operations such as cut/paste where local connectivity needs to be reconstructed.
    /// Local connectivity is defined as the connectivity between the nodes in the vector.
    void CollectLocalConnectivity(const std::vector<GraphNode*>& queryNodes, Serializer* dest) const;
    /// Inverse of the above, reconstructing (where possible) the connections between the nodes contained in the list.
    void ReconstructLocalConnectivity(const std::vector<GraphNode*>& queryNodes, Deserializer* src);
    
    /// Assigns automatic IDs to everything in the graph, names should generally be used
    /// ABSOLUTELY CRITICAL FOR GUI COPY & PASTE FUNCTIONALITY in order to enable reconstruction
    void AssignIDs() const;

    // Reset all of the socket stored values
    void ResetData();

    void PrepareGraph(const Variant& parameter);

    const std::unordered_multimap<GraphSocket*, GraphSocket*>& GetUpstreamEdges() const { return upstreamEdges_; }

    class SPRUE NodeVisitor
    {
    public:
        /// Return false if the visitor no longer needs to continue
        virtual bool Visit(GraphNode*) = 0;
    };

    void VisitDownStream(NodeVisitor* visitor, GraphNode* startingNode);
    void VisitUpStream(NodeVisitor* visitor, GraphNode* startingNode);

    void* GetUserData() const { return userData_; }
    void SetUserData(void* userData) { userData_ = userData; }

    unsigned GetConnectionCount() { return upstreamEdges_.size(); }

private:
    void WriteSocketConnectivity(GraphSocket* socket, const std::vector<GraphNode*>& withNodes, Serializer* dest) const;

    /// Internal get a node for use in deserialization
    GraphNode* GetNode(unsigned id);
    const GraphNode* GetNode(unsigned id) const;

    void* userData_ = 0x0;
    GraphNode* masterNode_;
    std::vector<GraphNode*> nodes_;      // List of all of the contained nodes
    std::vector<GraphNode*> entryNodes_; // Nodes that are known to be possible points of entry
    std::unordered_multimap<GraphSocket*, GraphSocket*> upstreamEdges_;   // link edges that go right->left, input -> output, we only ever allow a single upstream edge - except for with flowControl
    std::unordered_multimap<GraphSocket*, GraphSocket*> downstreamEdges_; // link edges that go left->right, output -> input, flow control sockets can only have 1 downstream edge
    unsigned currentExecutionContext_;
};

}