#pragma once

#include <SprueEngine/Graph/Graph.h>

namespace SprueEngine
{

    /// Base class for a group node that contains a subgraph
    class GroupNode : public GraphNode
    {
        NOCOPYDEF(GroupNode);
        BASECLASSDEF(GroupNode, GraphNode);
    public:
        /// Construct.
        GroupNode();
        /// Destruct.
        virtual ~GroupNode();

        SPRUE_EDITABLE(GroupNode);

        /// Register factory and such
        static void Register(Context*);

        /// Add quick helpers for external input and output so the sub graph isn't entirely empty, also automatically connect them
        virtual void Construct() override;

        virtual int Execute(const Variant& param) override;

        Graph* GetGraph() const { return graph_; }
        void SetGraph(Graph* graph) { graph_ = graph; }
    
        /// For property registration. Gets the graph.
        Variant GetGraphProperty() const { return Variant((void*)graph_); }
        /// For property registration. Sets the density handler but DOES NOT delete any existing density handler.
        void SetGraphProperty(Variant var) { graph_ = (Graph*)var.getVoidPtr(); }

    private:
        Graph* graph_;
    };

}