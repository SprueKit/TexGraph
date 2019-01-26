#include <SprueEngine/Graph/GraphNode.h>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Graph/Graph.h>
#include <SprueEngine/Graph/GraphSocket.h>

namespace SprueEngine
{

GraphNode::~GraphNode()
{
    for (GraphSocket* socket : outputFlowSockets)
        delete socket;
    for (GraphSocket* socket : inputSockets)
        delete socket;
    for (GraphSocket* socket : outputFlowSockets)
        delete socket;
    if (inputFlowSocket)
        delete inputFlowSocket;
}

void GraphNode::Register(Context* context)
{
    REGISTER_PROPERTY_MEMORY(GraphNode, float, offsetof(GraphNode, XPos), 0.0f, "XPos", "", PS_Secret);
    REGISTER_PROPERTY_MEMORY(GraphNode, float, offsetof(GraphNode, YPos), 0.0f, "YPos", "", PS_Secret);
    REGISTER_PROPERTY_MEMORY(GraphNode, unsigned, offsetof(GraphNode, id), 0, "ID", "", PS_Secret);
    REGISTER_PROPERTY_MEMORY(GraphNode, std::string, offsetof(GraphNode, name), std::string(), "Name", "", PS_ReadOnly);
}

bool GraphNode::Deserialize(Deserializer* src, const SerializationContext& context)
{
    base::Deserialize(src, context);
    outputSockets.clear();
    outputFlowSockets.clear();

    unsigned inSocketCt = src->ReadUInt();
    for (unsigned i = 0; i < inSocketCt; ++i)
    {
        GraphSocket* socket = new GraphSocket(this, 0, 0);
        socket->Deserialize(src, context);
        inputSockets.push_back(socket);
    }

    inSocketCt = src->ReadUInt();
    for (unsigned i = 0; i < inSocketCt; ++i)
    {
        GraphSocket* socket = new GraphSocket(this, 0, 0);
        socket->Deserialize(src, context);
        outputSockets.push_back(socket);
    }

    inSocketCt = src->ReadUInt();
    for (unsigned i = 0; i < inSocketCt; ++i)
    {
        GraphSocket* socket = new GraphSocket(this, 0, 0);
        socket->Deserialize(src, context);
        outputFlowSockets.push_back(socket);
    }

    if (src->ReadBool())
    {
        GraphSocket* socket = new GraphSocket(this, 0, 0);
        socket->Deserialize(src, context);
        inputFlowSocket = socket;
    }

    return true;
}

bool GraphNode::Serialize(Serializer* dest, const SerializationContext& context) const
{
    base::Serialize(dest, context);

    dest->WriteUInt((unsigned)inputSockets.size());
    if (inputSockets.size() > 0)
        for (GraphSocket* socket : inputSockets)
            socket->Serialize(dest, context);

    dest->WriteUInt((unsigned)outputSockets.size());
    if (outputSockets.size() > 0)
        for (GraphSocket* socket : outputSockets)
            socket->Serialize(dest, context);

    dest->WriteUInt((unsigned)outputFlowSockets.size());
    if (outputFlowSockets.size() > 0)
        for (GraphSocket* socket : outputFlowSockets)
            socket->Serialize(dest, context);

    dest->WriteBool(inputFlowSocket != 0x0);
    if (inputFlowSocket != 0x0)
        inputFlowSocket->Serialize(dest, context);

    return true;
}

bool GraphNode::Deserialize(tinyxml2::XMLElement* fromElement, const SerializationContext& context)
{
    base::Deserialize(fromElement, context);

#define LOAD_XML_SOCKET_LIST(LISTTAG, LIST) if (tinyxml2::XMLElement* foundList = fromElement->FirstChildElement(LISTTAG)) { \
        tinyxml2::XMLElement* socketElem = foundList->FirstChildElement("socket"); \
                while (socketElem) { \
            GraphSocket* socket = new GraphSocket(this, 0, 0); \
            socket->Deserialize(socketElem, context); \
            LIST .push_back(socket); \
            socketElem = socketElem->NextSiblingElement("socket"); \
                        } \
        }

    LOAD_XML_SOCKET_LIST("inputs", inputSockets);
    LOAD_XML_SOCKET_LIST("outputs", outputSockets);
    LOAD_XML_SOCKET_LIST("output-flows", outputFlowSockets);

    if (tinyxml2::XMLElement* inputFlow = fromElement->FirstChildElement("input-flow"))
    {
        GraphSocket* socket = new GraphSocket(this, 0, 0);
        socket->Deserialize(inputFlow, context);
        inputFlowSocket = socket;
    }

    return true;
}

bool GraphNode::Serialize(tinyxml2::XMLElement* parentElement, const SerializationContext& context) const
{
    tinyxml2::XMLElement* myElement = parentElement->GetDocument()->NewElement(GetTypeName());
    parentElement->LinkEndChild(myElement);

    SerializeProperties(myElement, context);

    tinyxml2::XMLElement* inputSockets = parentElement->GetDocument()->NewElement("inputs");
    myElement->LinkEndChild(inputSockets);
    for (auto socket : this->inputSockets)
    {
        tinyxml2::XMLElement* socketElement = parentElement->GetDocument()->NewElement("socket");
        inputSockets->LinkEndChild(socketElement);
        socket->Serialize(socketElement, context);
    }

    tinyxml2::XMLElement* outputSockets = parentElement->GetDocument()->NewElement("outputs");
    myElement->LinkEndChild(outputSockets);
    for (auto socket : this->outputSockets)
    {
        tinyxml2::XMLElement* socketElement = parentElement->GetDocument()->NewElement("socket");
        outputSockets->LinkEndChild(socketElement);
        socket->Serialize(socketElement, context);
    }

    tinyxml2::XMLElement* outputFlowSockets = parentElement->GetDocument()->NewElement("output-flows");
    myElement->LinkEndChild(outputFlowSockets);
    for (auto socket : this->outputFlowSockets)
    {
        tinyxml2::XMLElement* socketElement = parentElement->GetDocument()->NewElement("socket");
        outputFlowSockets->LinkEndChild(socketElement);
        socket->Serialize(socketElement, context);
    }

    if (inputFlowSocket)
    {
        tinyxml2::XMLElement* inputFlowSocket = parentElement->GetDocument()->NewElement("input-flow");
        myElement->LinkEndChild(inputFlowSocket);
        this->inputFlowSocket->Serialize(inputFlowSocket, context);
    }

    return true;
}


GraphSocket* GraphNode::GetSocketByFlatIndex(unsigned idx)
{
    if (inputFlowSocket && idx == 0)
        return inputFlowSocket;
    else if (inputFlowSocket)
        --idx;
    
    if (idx < inputSockets.size())
        return inputSockets[idx];
    else
        idx -= inputSockets.size();

    if (idx < outputSockets.size())
        return outputSockets[idx];
    else
        idx -= outputSockets.size();

    if (idx < outputFlowSockets.size())
        return outputFlowSockets[idx];
    else
        idx -= outputSockets.size();

    return 0x0;
}

unsigned GraphNode::GetSocketFlatIndex(GraphSocket* socket)
{
    unsigned currentIndex = 0;
    if (inputFlowSocket && socket == inputFlowSocket)
        return currentIndex;
    else if (inputFlowSocket)
        ++currentIndex;

    for (auto is : inputSockets)
        if (is == socket)
            return currentIndex;
        else
            ++currentIndex;

    for (auto is : outputSockets)
        if (is == socket)
            return currentIndex;
        else
            ++currentIndex;

    for (auto is : outputFlowSockets)
        if (is == socket)
            return currentIndex;
        else
            ++currentIndex;

    return -1;
}

GraphSocket* GraphNode::GetSocketByID(unsigned id, const std::vector<GraphSocket*>& sockets) const
{
    for (GraphSocket* socket : sockets)
        if (socket->socketID == id)
            return socket;
    return 0x0;
}

GraphSocket* GraphNode::GetSocketByID(unsigned id)
{
    if (inputFlowSocket != 0x0 && id == inputFlowSocket->socketID)
        return inputFlowSocket;

    if (GraphSocket* ret = GetSocketByID(id, inputSockets))
        return ret;
    if (GraphSocket* ret = GetSocketByID(id, outputSockets))
        return ret;
    if (GraphSocket* ret = GetSocketByID(id, outputFlowSockets))
        return ret;

    return 0x0;
}

GraphSocket* GraphNode::AddInput(const std::string& name, unsigned type) {
    GraphSocket* socket = new GraphSocket(this, name, type);
    socket->input = 1;
    inputSockets.push_back(socket);

    // Only signal a socket change if we're in a graph, because that means it's an real node that is likely constructed
    if (graph)
        Context::GetInstance()->GetCallbacks().GraphNodeSocketsChanged(this);
    return socket;
}

GraphSocket* GraphNode::InsertInput(unsigned index, const std::string& name, unsigned type)
{
    GraphSocket* socket = new GraphSocket(this, name, type);
    socket->input = 1;
    inputSockets.insert(inputSockets.begin() + index, socket);
    if (graph)
        Context::GetInstance()->GetCallbacks().GraphNodeSocketsChanged(this);
    return socket;
}

GraphSocket* GraphNode::AddOutput(const std::string& name, unsigned type) {
    GraphSocket* socket = new GraphSocket(this, name, type);
    socket->output = 1;
    outputSockets.push_back(socket);

    // Only signal a socket change if we're in a graph, because that means it's an real node that is likely constructed
    if (graph)
        Context::GetInstance()->GetCallbacks().GraphNodeSocketsChanged(this);
    return socket;
}

GraphSocket* GraphNode::InsertOutput(unsigned index, const std::string& name, unsigned type)
{
    GraphSocket* socket = new GraphSocket(this, name, type);
    socket->output = 1;
    outputSockets.insert(outputSockets.begin() + index, socket);
    if (graph)
        Context::GetInstance()->GetCallbacks().GraphNodeSocketsChanged(this);
    return socket;
}

GraphSocket* GraphNode::AddInputFlow(const std::string& name, unsigned type)
{
    if (inputFlowSocket)
        return 0x0;
    GraphSocket* socket = new GraphSocket(this, name, type);
    socket->input = 1;
    socket->control = 1;
    inputFlowSocket = socket;

    // Only signal a socket change if we're in a graph, because that means it's an real node that is likely constructed
    if (graph)
        Context::GetInstance()->GetCallbacks().GraphNodeSocketsChanged(this);
    return socket;
}

GraphSocket* GraphNode::AddOutputFlow(const std::string& name, unsigned type)
{
    GraphSocket* socket = new GraphSocket(this, name, type);
    socket->output = 1;
    socket->control = 1;
    outputFlowSockets.push_back(socket);

    // Only signal a socket change if we're in a graph, because that means it's an real node that is likely constructed
    if (graph)
        Context::GetInstance()->GetCallbacks().GraphNodeSocketsChanged(this);
    return socket;
}

GraphSocket* GraphNode::InsertOutputFlow(unsigned index, const std::string& name, unsigned type)
{
    GraphSocket* socket = new GraphSocket(this, name, type);
    socket->output = 1;
    socket->control = 1;
    outputFlowSockets.insert(outputFlowSockets.begin() + index, socket);
    if (graph)
        Context::GetInstance()->GetCallbacks().GraphNodeSocketsChanged(this);
    return socket;
}

void GraphNode::PropogateValues(bool down)
{
    // Propogate socket values
    if (down)
    {
        for (GraphSocket* socket : outputSockets)
        {
            auto edges = graph->downstreamEdges_.equal_range(socket);
            for (auto edge = edges.first; edge != edges.second; ++edge)
                edge->first->StoreValue(edge->second->GetValue());
        }
    }
    else
    {
        for (GraphSocket* socket : inputSockets)
        {
            auto upstreamEdges = graph->upstreamEdges_.equal_range(socket);
            for (auto edge = upstreamEdges.first; edge != upstreamEdges.second; ++edge)
                edge->first->StoreValue(edge->second->GetValue());
        }
    }
}

void GraphNode::ExecuteUpstream(unsigned& executionContext, const Variant& parameter, unsigned ignoringNode)
{
    if (executionContext != -1 && lastExecutionContext == executionContext || id == ignoringNode)
        return;

    Variant param = FilterParameter(parameter);

    do {
        if (!WillForceExecute())
        {
            // Evaluate upstream nodes first
            for (GraphSocket* socket : inputSockets)
            {
                if (!(socket->typeID))
                    continue;

                auto upstreamEdges = graph->upstreamEdges_.equal_range(socket);
                for (auto edge = upstreamEdges.first; edge != upstreamEdges.second; ++edge)
                    edge->second->node->ExecuteUpstream(executionContext, param);
            }

            PropogateValues(false);
        }

    } while (Execute(executionContext, parameter) == GRAPH_EXECUTE_LOOP);
}

void GraphNode::VisitUpstream(GraphNodeVisitor* visitor)
{
    if (!visitor)
        return;

    visitor->DepthPush();
    for (GraphSocket* socket : inputSockets)
    {
        if (!(socket->typeID))
            continue;

        auto upstreamEdges = graph->upstreamEdges_.equal_range(socket);
        for (auto edge = upstreamEdges.first; edge != upstreamEdges.second; ++edge)
            edge->second->node->VisitUpstream(visitor);
    }
    visitor->DepthPop();

    visitor->Visit(this);
}

void GraphNode::ForceExecuteUpstreamOnly(const Variant& parameter, unsigned ignoringNode)
{
    // Evaluate upstream nodes first
    for (GraphSocket* socket : inputSockets)
    {
        if (!(socket->typeID))
            continue;

        auto upstreamEdges = graph->upstreamEdges_.equal_range(socket);
        unsigned junk = -1;
        for (auto edge = upstreamEdges.first; edge != upstreamEdges.second; ++edge)
            edge->second->node->ExecuteUpstream(junk, parameter);
    }

    PropogateValues(false);
}

void GraphNode::ExecuteDownstream(unsigned& executionContext, const Variant& parameter, unsigned ignoringNode)
{
    if (lastExecutionContext == executionContext || id == ignoringNode)
        return;

    Variant param = FilterParameter(parameter);

    int result = 0;
    do {
        result = Execute(parameter);
        if (result == GRAPH_EXECUTE_TERMINATE)
            return;

        PropogateValues(true);

        for (GraphSocket* socket : outputSockets)
        {
            if (!(socket->typeID))
                continue;

            auto downstreamEdges = graph->downstreamEdges_.equal_range(socket);
            for (auto edge = downstreamEdges.first; edge != downstreamEdges.second; ++edge)
                edge->second->node->ExecuteDownstream(executionContext, param);
        }
    } while (result == GRAPH_EXECUTE_LOOP);
}

void GraphNode::ExecuteHybrid(unsigned& executionContext, const Variant& parameter, unsigned ignoringNode)
{
    if (lastExecutionContext == executionContext || id == ignoringNode)
        return;

    // Reset our selected exit
    selectedExit = 0x0;

    // Iterate through input variable sockets to get values
    int result = 1;

    do {
        // Retrieve values from upstream sockets
        PropogateValues(false);

        result = Execute(parameter);

        if (result == GRAPH_EXECUTE_TERMINATE)
            return;

        // Propogate into our output sockets
        PropogateValues(true);

        // Execute may assign our selectedExit
        if (!selectedExit)
        {
            // Execute all output flow sockets
            for (GraphSocket* socket : outputFlowSockets)
            {
                //TODO: deal with branching/looping execution
                auto downstreamEdges = graph->downstreamEdges_.equal_range(socket);
                for (auto edge = downstreamEdges.first; edge != downstreamEdges.second; ++edge)
                    edge->second->node->ExecuteHybrid(executionContext, parameter);
            }
        }
        else
        {
            // Execute all connections downstream from the flow exit socket
            auto edge = graph->downstreamEdges_.find(selectedExit);
            if (edge != graph->downstreamEdges_.end())
                edge->second->node->ExecuteHybrid(executionContext, parameter);
        }
    } while (result == GRAPH_EXECUTE_LOOP);
}

std::vector< std::pair<GraphSocket*, GraphSocket*> > GraphNode::GetConnections() const
{
    std::vector< std::pair<GraphSocket*, GraphSocket*> > ret;

    if (inputFlowSocket)
    {
        auto inputCon = inputFlowSocket->GetConnections();
        ret.insert(ret.end(), inputCon.begin(), inputCon.end());
    }

    for (auto socket : outputSockets)
    {
        auto sockets = socket->GetConnections();
        ret.insert(ret.end(), sockets.begin(), sockets.end());
    }

    for (auto socket : inputSockets)
    {
        auto sockets = socket->GetConnections();
        ret.insert(ret.end(), sockets.begin(), sockets.end());
    }

    for (auto socket : outputFlowSockets)
    {
        auto sockets = socket->GetConnections();
        ret.insert(ret.end(), sockets.begin(), sockets.end());
    }

    return ret;
}

void GraphNode::RestoreConnections(const std::vector< std::pair<GraphSocket*, GraphSocket*> >& connections)
{
    for (auto conn : connections)
        graph->Connect(conn.first, conn.second);
}

void GraphNode::NotifySocketsChange() const
{
    SprueEngine::Context::GetInstance()->GetCallbacks().GraphNodeSocketsChanged(const_cast<GraphNode*>(this));
}

}