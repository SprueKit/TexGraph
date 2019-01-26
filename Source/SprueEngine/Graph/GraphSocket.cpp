#include <SprueEngine/Graph/GraphSocket.h>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Deserializer.h>
#include <SprueEngine/Graph/Graph.h>
#include <SprueEngine/Graph/GraphNode.h>
#include <SprueEngine/Serializer.h>


namespace SprueEngine
{

bool GraphSocket::AcceptEdge(const GraphSocket* otherSocket) const
{
    if (otherSocket->output && input &&         // Require that we're valid connectivity
        (typeID & otherSocket->typeID) &&       // Require that our typeIDs are acceptable
        (control == otherSocket->control) &&    // If we're control flow then we both must be control flow
        (variable == otherSocket->variable))    // If we're variables then we both must be variables
        return true;
    return false;
}

void GraphSocket::Deserialize(Deserializer* src, const SerializationContext& context)
{
    socketID = src->ReadUInt();
    name = src->ReadString();
    unsigned nodeID = src->ReadUInt(); //umm wtf?
    typeID = src->ReadUInt();
    input = src->ReadBool();
    output = src->ReadBool();
    control = src->ReadBool();
    variable = src->ReadBool();
    secret = src->ReadBool();
}

void GraphSocket::Serialize(Serializer* dest, const SerializationContext& context) const
{
    dest->WriteUInt(socketID);
    dest->WriteString(name);
    dest->WriteUInt(node->id);
    dest->WriteUInt(typeID);
    dest->WriteBool(input);
    dest->WriteBool(output);
    dest->WriteBool(control);
    dest->WriteBool(variable);
    dest->WriteBool(secret);
}

#ifndef SPRUE_NO_XML
void GraphSocket::Deserialize(tinyxml2::XMLElement* fromElement, const SerializationContext& context)
{
    socketID = fromElement->IntAttribute("id");
    name = fromElement->Attribute("name");
    typeID = fromElement->IntAttribute("typeid");
    input = fromElement->BoolAttribute("input");
    output = fromElement->BoolAttribute("output");
    control = fromElement->BoolAttribute("control");
    secret = fromElement->BoolAttribute("secret");
    variable = fromElement->BoolAttribute("variable");
}

void GraphSocket::Serialize(tinyxml2::XMLElement* socketElement, const SerializationContext& context)
{
    socketElement->SetAttribute("id", socketID);
    socketElement->SetAttribute("name", name.c_str());
    socketElement->SetAttribute("typeid", typeID);
    socketElement->SetAttribute("input", input ? true : false);
    socketElement->SetAttribute("output", output ? true : false);
    socketElement->SetAttribute("control", control ? true : false);
    socketElement->SetAttribute("secret", secret ? true : false);
    socketElement->SetAttribute("variable", variable ? true : false);
}
#endif

bool GraphSocket::HasConnections() const
{
    if (node && node->graph)
    {
        Graph* graph = node->graph;
        GraphSocket* self = const_cast<GraphSocket*>(this);
        return graph->upstreamEdges_.find(self) != graph->upstreamEdges_.end() || graph->downstreamEdges_.find(self) != graph->downstreamEdges_.end();
    }
    return false;
}

std::vector< std::pair<GraphSocket*, GraphSocket*> > GraphSocket::GetConnections() const
{
    std::vector< std::pair<GraphSocket*, GraphSocket*> > ret;
    if (node && node->graph)
    {
        GraphSocket* self = const_cast<GraphSocket*>(this);

        auto upstreamRange = node->graph->upstreamEdges_.equal_range(self);
        for (auto it = upstreamRange.first; it != upstreamRange.second; ++it)
            ret.push_back(std::make_pair(it->first, it->second));

        auto downStreamRange = node->graph->downstreamEdges_.equal_range(self);
        for (auto it = downStreamRange.first; it != downStreamRange.second; ++it)
            ret.push_back(std::make_pair(it->first, it->second));
    }
    return ret;
}

bool GraphSocket::ValidateConnections(bool disconnect)
{
    bool ret = true;
    std::vector< std::pair<GraphSocket*, GraphSocket*> > connections = GetConnections();
    for (auto connection : connections)
    {
        if (!AcceptEdge(connection.second))
        {
            if (disconnect)
                node->graph->Disconnect(this, connection.second);
            ret = false;
        }
    }
    return ret;
}

}