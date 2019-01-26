#include "GroupNode.h"
#include "Graph.h"

#include <SprueEngine/Core/Context.h>

namespace SprueEngine
{

GroupNode::GroupNode()
{

}

/// Destruct.
GroupNode::~GroupNode()
{

}


static const char* GROUPNODE_GRAPH_NAMES[] = {
    "Graph",
    0x0,
};

static const StringHash GROUPNODE_GRAPH_TYPES[] = {
    StringHash("Graph")
};

/// Register factory and such
void GroupNode::Register(Context* context)
{
    context->RegisterFactory<GroupNode>("GroupNode", "A node that contains a nested graph internally");
    context->CopyBaseProperties("GraphNode", "GroupNode");
    REGISTER_EDITABLE(GroupNode, GetGraphProperty, SetGraphProperty, "Graph", "The nested graph contained in this group node", PS_Secret, GROUPNODE_GRAPH_NAMES, GROUPNODE_GRAPH_TYPES);
}

/// Add quick helpers for external input and output so the sub graph isn't entirely empty, also automatically connect them
void GroupNode::Construct()
{

}

int GroupNode::Execute(const Variant& param)
{
    return GRAPH_EXECUTE_COMPLETE;
}

}