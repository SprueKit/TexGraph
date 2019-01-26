#include <SprueEngine/Graph/Graph.h>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Deserializer.h>
#include <SprueEngine/Graph/GraphSocket.h>
#include <SprueEngine/Serializer.h>
#include <SprueEngine/Logging.h>

#define GRAPH_VERSION 1

namespace SprueEngine
{

// Helper macros for dealign with otherwise ridiculously repetitive code, but that isn't worthy of moving into functions

#define MULTIMAP_ERASE_KVP(KEY, VAL, CONT) { auto iter = CONT.begin(); while (iter != CONT.end()) if (iter -> first == KEY && iter->second == VAL) { auto first = iter->first; auto second = iter->second; iter = CONT.erase(iter); SprueEngine::Context::GetInstance()->GetCallbacks().GraphConnectionRemoved(first, second); } else ++iter; }

// Iterate through multimap erase everything with a key (first) or value (second)
#define MULTIMAP_ERASE(VAL, CONT, TERM) { auto iter = CONT.begin(); while (iter != CONT.end()) if (iter -> TERM == VAL) { auto first = iter->first; auto second = iter->second; iter = CONT.erase(iter); SprueEngine::Context::GetInstance()->GetCallbacks().GraphConnectionRemoved(first, second); } else ++iter; }

// Iterate through multimap and erase everything with a key or value == VAL
#define MULTIMAP_ERASE_EITHER(VAL, CONT) { auto iter = CONT.begin(); while (iter != CONT.end()) if (iter -> first == VAL || iter -> second == VAL) { auto first = iter->first; auto second = iter->second; iter = CONT.erase(iter); SprueEngine::Context::GetInstance()->GetCallbacks().GraphConnectionRemoved(first, second); } else ++iter; }

GraphSocket* FindSocket(const std::vector<GraphNode*>& nodes, unsigned nodeID, unsigned socketID);

Graph::Graph() :
    masterNode_(0x0),
    currentExecutionContext_(0)
{

}

Graph::~Graph()
{
    for (GraphNode* nd : nodes_)
        delete nd;
}

void Graph::Register(Context* context)
{
    context->RegisterFactory<Graph>("Graph", "An edge directed graph that may contain nodes of arbitrary function");
}

void Graph::AddNode(GraphNode* node, bool entryPoint)
{
    if (node->graph != 0x0)
        node->graph->RemoveNode(node);
    node->graph = this;
    nodes_.push_back(node);
    if (entryPoint)
        entryNodes_.push_back(node);
    SprueEngine::Context::GetInstance()->GetCallbacks().GraphNodeAdded(node);
}

void Graph::RemoveNode(GraphNode* node)
{
    DisconnectAll(node);
    auto found = std::find(nodes_.begin(), nodes_.end(), node);
    if (found != nodes_.end())
        nodes_.erase(found);
    found = std::find(entryNodes_.begin(), entryNodes_.end(), node);
    if (found != entryNodes_.end())
        entryNodes_.erase(found);
    SprueEngine::Context::GetInstance()->GetCallbacks().GraphNodeRemoved(node);
}

GraphNode* Graph::GetNode(unsigned id)
{
    for (GraphNode* node : nodes_)
        if (node->id == id)
            return node;
    return 0x0;
}

const GraphNode* Graph::GetNode(unsigned id) const
{
    for (GraphNode* node : nodes_)
        if (node->id == id)
            return node;
    return 0x0;
}

GraphNode* Graph::GetNodeBySourceID(unsigned id)
{
    for (GraphNode* node : nodes_)
        if (node->GetSourceID() == id)
            return node;
    return 0x0;
}

const GraphNode* Graph::GetNodeBySourceID(unsigned id) const
{
    for (GraphNode* node : nodes_)
        if (node->GetSourceID() == id)
            return node;
    return 0x0;
}

GraphNode* Graph::GetNodeByInstanceID(unsigned id)
{
    for (GraphNode* node : nodes_)
        if (node->GetInstanceID() == id)
            return node;
    return 0x0;
}

const GraphNode* Graph::GetNodeByInstanceID(unsigned id) const
{
    for (GraphNode* node : nodes_)
        if (node->GetInstanceID() == id)
            return node;
    return 0x0;
}

GraphNode* Graph::GetNode(const std::string& name)
{
    for (GraphNode* node : nodes_)
        if (node->name.compare(name) == 0)
            return node;
    return 0x0;
}

const GraphNode* Graph::GetNode(const std::string& name) const
{
    for (GraphNode* node : nodes_)
        if (node->name.compare(name) == 0)
            return node;
    return 0x0;
}

GraphNode* Graph::GetEntryNode(unsigned id)
{
    for (auto nd : entryNodes_)
        if (nd->id == id)
            return nd;
    return 0x0;
}

GraphNode* Graph::GetEntryNode(const std::string& name)
{
    for (auto nd : entryNodes_)
        if (nd->name.compare(name) == 0)
            return nd;
    return 0x0;
}

GraphNode* Graph::GetEntryNode(const StringHash& eventID)
{
    
    return 0x0;
}

GraphNode* Graph::GetEntryNodeByType(const StringHash& typeID)
{
    for (auto nd : entryNodes_)
        if (nd->GetTypeHash() == typeID)
            return nd;
    return 0x0;
}

bool Graph::IsEntryNode(const GraphNode* node) const
{
    if (std::find(entryNodes_.begin(), entryNodes_.end(), node) != entryNodes_.end())
        return true;
    return false;
}

bool Graph::Connect(GraphSocket* from, GraphSocket* to)
{
    if (!from || !to)
        return false;

    if (CanConnect(from, to))
    {
        if (to->input)
            std::swap(from, to);

        // Only accept a SINGLE Upstream edge
        if (!from->control)
        {
            MULTIMAP_ERASE(from, upstreamEdges_, first);    // break anyone we're already connected to
            MULTIMAP_ERASE(from, downstreamEdges_, second); // break anyone that is connecting to us
        }

        // If we're connecting to a control socket then we have to break all links
        if (from->control)
        {
            MULTIMAP_ERASE(to, downstreamEdges_, first);
        }

        upstreamEdges_.insert(std::make_pair(from, to));
        downstreamEdges_.insert(std::make_pair(to, from));

        SprueEngine::Context::GetInstance()->GetCallbacks().GraphConnectionAdded(from, to);

        return true;
    }
    return false;
}

bool Graph::Disconnect(GraphSocket* from, GraphSocket* to)
{
    if (!from || !to)
        return false;

    if (to->input)
        std::swap(from, to);

    // Erase any upstream edges start at us
    MULTIMAP_ERASE_KVP(from, to, upstreamEdges_);

    // Erase any downstream edges that start at us
    MULTIMAP_ERASE_KVP(to, from, downstreamEdges_);

    return true;
}

bool Graph::DisconnectAll(GraphSocket* socket)
{
    MULTIMAP_ERASE_EITHER(socket, upstreamEdges_);
    MULTIMAP_ERASE_EITHER(socket, downstreamEdges_);
    return true;
}

bool Graph::DisconnectAll(GraphNode* node)
{
    for (GraphSocket* socket : node->inputSockets)
        DisconnectAll(socket);
    for (GraphSocket* socket : node->outputSockets)
        DisconnectAll(socket);
    for (GraphSocket* socket : node->outputFlowSockets)
        DisconnectAll(socket);
    DisconnectAll(node->inputFlowSocket);
    return true;
}

bool Graph::Connected(GraphSocket* from, GraphSocket* to)
{
    if (!from || !to)
        return false;

    if (to->input)
        std::swap(from, to);

    auto iter = upstreamEdges_.find(from);
    for (int i = 0; i < upstreamEdges_.count(from); ++i, ++iter)
    {
        if (iter->second == to)
            return true;
    }
    return false;
}

bool Graph::CanConnect(GraphSocket* from, GraphSocket* to)
{
    if (!from || !to)
        return false;

    if (to->input)
        std::swap(from, to);

    return from->AcceptEdge(to);
}

bool Graph::Deserialize(Deserializer* src, const SerializationContext& context)
{
    bool success = true;
    //if (src->ReadFileID().compare("GRPH") != 0)
    //{
    //    SPRUE_LOG_ERROR("Failed to read texture graph data");
    //    return false;
    //}

    unsigned version = src->ReadUShort();
    //TODO: as new versions come along add the necessary handling to update old files

    // Nodes
    unsigned nodeCt = src->ReadUInt();
    while (nodeCt > 0)
    {
        GraphNode* newNode = Context::GetInstance()->Deserialize<GraphNode>(src, context);
        if (newNode)
        {
            nodes_.push_back(newNode);
            newNode->graph = this;
        }
        --nodeCt;
    }

    // Master node
    unsigned masterID = src->ReadUInt();
    if (masterID != -1)
        masterNode_ = GetNode(masterID);

    // Entry points
    unsigned entryCt = src->ReadUInt();
    while (entryCt > 0)
    {
        if (GraphNode* entryPoint = GetNode(src->ReadUInt()))
            entryNodes_.push_back(entryPoint);
        --entryCt;
    }

    // Edge links
    unsigned edgeCt = src->ReadUInt();
    while (edgeCt > 0)
    {
        const unsigned fromNodeID = src->ReadUInt();
        const unsigned fromSocketID = src->ReadUInt();
        const unsigned toNodeID = src->ReadUInt();
        const unsigned toSocketID = src->ReadUInt();

        GraphNode* fromNode = GetNode(fromNodeID);
        GraphNode* toNode = GetNode(toNodeID);
        if (fromNode && toNode)
        {
            GraphSocket* fromSocket = fromNode->GetSocketByID(fromSocketID);
            GraphSocket* toSocket = toNode->GetSocketByID(toSocketID);

            if (fromSocket && toSocket)
                upstreamEdges_.insert(std::make_pair(fromSocket, toSocket));
        }

        --edgeCt;
    }

    edgeCt = src->ReadUInt();
    while (edgeCt > 0)
    {
        const unsigned fromNodeID = src->ReadUInt();
        const unsigned fromSocketID = src->ReadUInt();
        const unsigned toNodeID = src->ReadUInt();
        const unsigned toSocketID = src->ReadUInt();

        GraphNode* fromNode = GetNode(fromNodeID);
        GraphNode* toNode = GetNode(toNodeID);
        if (fromNode && toNode)
        {
            GraphSocket* fromSocket = fromNode->GetSocketByID(fromSocketID);
            GraphSocket* toSocket = toNode->GetSocketByID(toSocketID);

            if (fromSocket && toSocket)
                downstreamEdges_.insert(std::make_pair(fromSocket, toSocket));
        }

        --edgeCt;
    }

    return success;
}

bool Graph::Serialize(Serializer* dest, const SerializationContext& context) const
{
    bool success = true;
    // Make sure everything has good ids
    AssignIDs();

    //success &= dest->WriteFileID("GRPH");
    success &= dest->WriteStringHash(GetTypeHash());
    success &= dest->WriteUShort(GRAPH_VERSION);

    // Write all the ndoes
    success &= dest->WriteUInt((unsigned)nodes_.size());
    for (GraphNode* node : nodes_)
        success &= node->Serialize(dest, context);

    // Write the master node
    if (masterNode_)
        success &= dest->WriteUInt(masterNode_->id);
    else
        success &= dest->WriteUInt(-1);

    // Write the entry points
    success &= dest->WriteUInt((unsigned)entryNodes_.size());
    for (GraphNode* node : entryNodes_)
        success &= dest->WriteUInt(node->id);

    // write the edge connections
    success &= dest->WriteUInt((unsigned)upstreamEdges_.size());
    for (auto edge = upstreamEdges_.begin(); edge != upstreamEdges_.end(); ++edge)
    {
        success &= dest->WriteUInt(edge->first->node->id);
        success &= dest->WriteUInt(edge->first->socketID);
        success &= dest->WriteUInt(edge->second->node->id);
        success &= dest->WriteUInt(edge->second->socketID);
    }

    success &= dest->WriteUInt((unsigned)downstreamEdges_.size());
    for (auto edge = downstreamEdges_.begin(); edge != downstreamEdges_.end(); ++edge)
    {
        success &= dest->WriteUInt(edge->first->node->id);
        success &= dest->WriteUInt(edge->first->socketID);
        success &= dest->WriteUInt(edge->second->node->id);
        success &= dest->WriteUInt(edge->second->socketID);
    }

    return success;
}

#ifndef SPRUE_NO_XML
bool Graph::Deserialize(tinyxml2::XMLElement* parentElement, const SerializationContext& context)
{
    if (tinyxml2::XMLElement* child = parentElement->FirstChildElement("nodes"))
    {
        tinyxml2::XMLElement* nodeElem = child->FirstChildElement();
        while (nodeElem)
        {
            if (GraphNode* node = Context::GetInstance()->Deserialize<GraphNode>(nodeElem, context))
            {
                nodes_.push_back(node);
                node->graph = this;
            }
            nodeElem = nodeElem->NextSiblingElement();
        }
    }

    if (tinyxml2::XMLElement* master = parentElement->FirstChildElement("master"))
    {
        int id = atoi(master->GetText());
        masterNode_ = GetNode(id);
    }

    if (tinyxml2::XMLElement* entryPoints = parentElement->FirstChildElement("entrypoints"))
    {
        tinyxml2::XMLElement* entryPoint = entryPoints->FirstChildElement("entry");
        while (entryPoint)
        {
            int id = atoi(entryPoint->GetText());
            if (GraphNode* node = GetNode(id))
                entryNodes_.push_back(node);
            entryPoint = entryPoint->NextSiblingElement("entry");
        }
    }

    if (tinyxml2::XMLElement* upstreamEdges = parentElement->FirstChildElement("upstream"))
    {
        tinyxml2::XMLElement* edge = upstreamEdges->FirstChildElement("edge");
        while (edge)
        {
            int fromNodeID = edge->IntAttribute("from-node");
            int toNodeID = edge->IntAttribute("to-node");
            int fromSocketID = edge->IntAttribute("from-socket");
            int toSocketID = edge->IntAttribute("to-socket");

            GraphNode* fromNode = GetNode(fromNodeID);
            GraphNode* toNode = GetNode(toNodeID);
            if (fromNode && toNode)
            {
                GraphSocket* fromSocket = fromNode->GetSocketByID(fromSocketID);
                GraphSocket* toSocket = toNode->GetSocketByID(toSocketID);

                if (fromSocket && toSocket)
                    upstreamEdges_.insert(std::make_pair(fromSocket, toSocket));
            }

            edge = edge->NextSiblingElement("edge");
        }
    }

    if (tinyxml2::XMLElement* downstreamEdges = parentElement->FirstChildElement("downstream"))
    {
        tinyxml2::XMLElement* edge = downstreamEdges->FirstChildElement("edge");
        while (edge)
        {
            int fromNodeID = edge->IntAttribute("from-node");
            int toNodeID = edge->IntAttribute("to-node");
            int fromSocketID = edge->IntAttribute("from-socket");
            int toSocketID = edge->IntAttribute("to-socket");

            GraphNode* fromNode = GetNode(fromNodeID);
            GraphNode* toNode = GetNode(toNodeID);
            if (fromNode && toNode)
            {
                GraphSocket* fromSocket = fromNode->GetSocketByID(fromSocketID);
                GraphSocket* toSocket = toNode->GetSocketByID(toSocketID);

                if (fromSocket && toSocket)
                    downstreamEdges_.insert(std::make_pair(fromSocket, toSocket));
            }

            edge = edge->NextSiblingElement("edge");
        }
    }

    return true;
}

bool Graph::Serialize(tinyxml2::XMLElement* myElement, const SerializationContext& context) const
{
    AssignIDs();
    tinyxml2::XMLElement* nodesList = myElement->GetDocument()->NewElement("nodes");
    myElement->LinkEndChild(nodesList);

    for (auto node : nodes_)
        node->Serialize(nodesList, context);
    
    if (masterNode_)
    {
        tinyxml2::XMLElement* masterNode = myElement->GetDocument()->NewElement("master");
        myElement->LinkEndChild(masterNode);
        masterNode->SetText(masterNode_->id);
    }

    tinyxml2::XMLElement* entryPointList = myElement->GetDocument()->NewElement("entrypoints");
    myElement->LinkEndChild(entryPointList);
    for (auto entryPoint : entryNodes_)
    {
        tinyxml2::XMLElement* ep = myElement->GetDocument()->NewElement("entry");
        ep->SetText(entryPoint->id);
        entryPointList->LinkEndChild(ep);
    }

    tinyxml2::XMLElement* downStreamEdgeList = myElement->GetDocument()->NewElement("downstream");
    myElement->LinkEndChild(downStreamEdgeList);
    for (auto edge : downstreamEdges_)
    {
        tinyxml2::XMLElement* ep = myElement->GetDocument()->NewElement("edge");
        downStreamEdgeList->LinkEndChild(ep);
        ep->SetAttribute("from-node", edge.first->node->id);
        ep->SetAttribute("from-socket", edge.first->socketID);
        ep->SetAttribute("to-node", edge.second->node->id);
        ep->SetAttribute("to-socket", edge.second->socketID);
    }

    tinyxml2::XMLElement* upStreamEdgeList = myElement->GetDocument()->NewElement("upstream");
    myElement->LinkEndChild(upStreamEdgeList);

    for (auto edge : upstreamEdges_)
    {
        tinyxml2::XMLElement* ep = myElement->GetDocument()->NewElement("edge");
        upStreamEdgeList->LinkEndChild(ep);
        ep->SetAttribute("from-node", edge.first->node->id);
        ep->SetAttribute("from-socket", edge.first->socketID);
        ep->SetAttribute("to-node", edge.second->node->id);
        ep->SetAttribute("to-socket", edge.second->socketID);
    }

    return true;
}
#endif

void Graph::CollectLocalConnectivity(const std::vector<GraphNode*>& queryNodes, Serializer* dest) const
{
    for (auto node : queryNodes)
    {
        // Don't need to write output sockets, if we can use even them then we also have the input sockets to reconstruct from
        std::vector<GraphSocket*> allSockets(node->inputSockets);
        allSockets.insert(allSockets.end(), node->outputFlowSockets.begin(), node->outputFlowSockets.end());
        if (node->inputFlowSocket)
            allSockets.push_back(node->inputFlowSocket);

        dest->WriteUInt(allSockets.size());
        for (auto socket : allSockets)
            WriteSocketConnectivity(socket, queryNodes, dest);
    }
}

void Graph::ReconstructLocalConnectivity(const std::vector<GraphNode*>& queryNodes, Deserializer* src)
{
    for (auto node : queryNodes)
    {
        // Upstream edges
        unsigned socketCt = src->ReadUInt();
        for (unsigned j = 0; j < socketCt; ++j)
        {
            unsigned edgeCt = src->ReadUInt();
            for (unsigned i = 0; i < edgeCt; ++i)
            {
                if (src->ReadBool())
                {
                    unsigned mySocketID = src->ReadUInt();
                    unsigned otherNodeID = src->ReadUInt();
                    unsigned otherSocketID = src->ReadUInt();
                    if (GraphSocket* mySocket = node->GetSocketByID(mySocketID))
                    {
                        if (GraphSocket* other = FindSocket(queryNodes, otherNodeID, otherSocketID))
                        {
                            Connect(mySocket, other);
                        }
                    }
                }
            }
        }
    }
}

void Graph::WriteSocketConnectivity(GraphSocket* socket, const std::vector<GraphNode*>& withNodes, Serializer* dest) const
{
    auto upstreamEdges = this->upstreamEdges_.equal_range(socket); //<-- const nonsense
    unsigned ct = upstreamEdges_.count(socket);
    dest->WriteUInt(ct);
    for (auto edge = upstreamEdges.first; edge != upstreamEdges.second; ++edge)
    {
        // Did we find this edge?
        if (std::find(withNodes.begin(), withNodes.end(), edge->second->node) != withNodes.end())
        {
            dest->WriteBool(true); //has edge
            dest->WriteUInt(socket->socketID);
            dest->WriteUInt(edge->second->node->id);
            dest->WriteUInt(edge->second->socketID);
        }
        else
            dest->WriteBool(false);
    }
}

void Graph::AssignIDs() const
{
    unsigned startID = 0;
    for (GraphNode* node : nodes_)
    {
        node->id = ++startID;
        for (GraphSocket* socket : node->inputSockets)
            socket->socketID = ++startID;
        for (GraphSocket* socket : node->outputSockets)
            socket->socketID = ++startID;
        for (GraphSocket* socket : node->outputFlowSockets)
            socket->socketID = ++startID;
        if (node->inputFlowSocket)
            node->inputFlowSocket->socketID = ++startID;
    }
}

void Graph::ResetData()
{
    for (GraphNode* node : nodes_)
    {
        for (GraphSocket* socket : node->inputSockets)
            socket->StoreValue(Variant());
        for (GraphSocket* socket : node->outputSockets)
            socket->StoreValue(Variant());
        for (GraphSocket* socket : node->outputFlowSockets)
            socket->StoreValue(Variant());
        if (node->inputFlowSocket)
            node->inputFlowSocket->StoreValue(Variant());
    }
}

void Graph::PrepareGraph(const Variant& parameter) 
{
    for (auto node : nodes_)
        node->Prepare(parameter);
}

GraphSocket* FindSocket(const std::vector<GraphNode*>& nodes, unsigned nodeID, unsigned socketID)
{
    for (auto node : nodes)
    {
        if (node->id == nodeID)
            return node->GetSocketByID(socketID);
    }
    return 0x0;
}

void Graph::VisitDownStream(NodeVisitor* visitor, GraphNode* startingNode)
{
    if (visitor->Visit(startingNode))
    {
        for (GraphSocket* socket : startingNode->outputSockets)
        {
            if (!(socket->typeID))
                continue;

            auto foundDownstream = downstreamEdges_.equal_range(socket);
            for (auto edge = foundDownstream.first; edge != foundDownstream.second; ++edge)
                VisitDownStream(visitor, edge->second->node);
        }
    }
}

void Graph::VisitUpStream(NodeVisitor* visitor, GraphNode* startingNode)
{
    if (visitor->Visit(startingNode))
    {
        for (GraphSocket* socket : startingNode->inputSockets)
        {
            if (!(socket->typeID))
                continue;

            auto upstreamEdges = upstreamEdges_.equal_range(socket);
            for (auto edge = upstreamEdges.first; edge != upstreamEdges.second; ++edge)
                VisitUpStream(visitor, edge->second->node);
        }
    }
}

}