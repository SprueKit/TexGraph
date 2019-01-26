#pragma once

#include <SprueEngine/IEditable.h>

#include <SprueEngine/Graph/GraphConstants.h>

namespace SprueEngine
{
    class Deserializer;
    class Graph;
    class GraphNode;
    struct GraphSocket;
    class Serializer;

    struct GraphNodeVisitor {
        virtual void DepthPush() { }
        virtual void DepthPop() { }
        virtual void Visit(const GraphNode* node) = 0;
    };

    /// A node in an executable graph.
    class SPRUE GraphNode : public IEditable
    {
        BASECLASSDEF(GraphNode, IEditable);
        NOCOPYDEF(GraphNode);
    public:
        std::string name;
        unsigned id;
        std::vector<GraphSocket*> inputSockets;
        std::vector<GraphSocket*> outputSockets;
        GraphSocket* inputFlowSocket; // Can only have one of these, it is always a one to many
        std::vector<GraphSocket*> outputFlowSockets;
        Graph* graph;
        float XPos; // GUI recorded X pos
        float YPos; // GUI recorded Y pos

        GraphNode() : graph(0x0), selectedExit(0x0), XPos(0.0f), YPos(0.0f), inputFlowSocket(0x0) {}
        GraphNode(Graph* graph) : graph(graph), selectedExit(0x0), XPos(0.0f), YPos(0.0f), inputFlowSocket(0x0) {}
        virtual ~GraphNode();

        static void Register(Context*);

        /// Initialize a newly contstructed node (adding sockets)
        virtual void Construct() = 0;

        virtual StringHash GetTypeHash() const = 0;

        virtual bool CanPreview() const { return false; }

        virtual std::shared_ptr<FilterableBlockMap<RGBA> > GetPreview(unsigned width = 128, unsigned height = 128) { return std::shared_ptr<FilterableBlockMap<RGBA> >(); }

        /// Intercept and modify the parameter, example usage: transform nodes that manipulate the parameter
        virtual Variant FilterParameter(const Variant& param) const { return param; }

        /// Override if this node will manually force execute it's upstream nodes (prevents automatic up/down stream evaluation)
        virtual bool WillForceExecute() const { return false; }

        /// Optional OVERRIDE to perform prep work before the graph can be executed
        virtual void Prepare(const Variant& parameter) { }

        /// OVERRIDE Perform node execution, process inputs, write outputs, select flow control
        virtual int Execute(const Variant& parameter) = 0;

        /// ALWAYS CALL Base::Deserialize
        virtual bool Deserialize(Deserializer* src, const SerializationContext& context) override;

        /// ALWAYS CALL Base::Serialize
        virtual bool Serialize(Serializer* dest, const SerializationContext& context) const override;

    #ifndef SPRUE_NO_XML
        virtual bool Deserialize(tinyxml2::XMLElement* fromElement, const SerializationContext& context) override;
        virtual bool Serialize(tinyxml2::XMLElement* parentElement, const SerializationContext& context) const override;
    #endif

        virtual bool Compile(class VectorBuffer* buffer) const { return true; }

        // Methods for getting sockets by ID
        /// Returns a socket at the given "flat" index (all sockets counted)
        GraphSocket* GetSocketByFlatIndex(unsigned idx);
        /// Returns the index of a socket in "flat" order (all sockets counted)
        unsigned GetSocketFlatIndex(GraphSocket* socket);
        GraphSocket* GetSocketByID(unsigned id);
        GraphSocket* GetSocketByID(unsigned id, const std::vector<GraphSocket*>& sockets) const;
        GraphSocket* GetInputSocketByID(unsigned id) { return GetSocketByID(id, inputSockets); }
        GraphSocket* GetFlowOutSocketByID(unsigned id) { return GetSocketByID(id, outputFlowSockets); }
        GraphSocket* GetOutputSocketByID(unsigned id) { return GetSocketByID(id, outputSockets); }
        const GraphSocket* GetInputSocketByID(unsigned id) const { return GetSocketByID(id, inputSockets); }
        const GraphSocket* GetFlowOutSocketByID(unsigned id) const { return GetSocketByID(id, outputFlowSockets); }
        const GraphSocket* GetOuputSocketByID(unsigned id) const { return GetSocketByID(id, outputSockets); }

        // Methods for getting sockets by index
        GraphSocket* GetInputSocket(unsigned idx) { return inputSockets[idx]; }
        GraphSocket* GetOutputSocket(unsigned idx) { return outputSockets[idx]; }
        GraphSocket* GetFlowOutSocket(unsigned idx) { return outputFlowSockets[idx]; }
        GraphSocket* GetInputFlowSocket() { return inputFlowSocket; }

        // Socket addition methods.
        GraphSocket* AddInput(const std::string& name, unsigned type);
        GraphSocket* InsertInput(unsigned index, const std::string& name, unsigned type);
        GraphSocket* AddOutput(const std::string& name, unsigned type);
        GraphSocket* InsertOutput(unsigned index, const std::string& name, unsigned type);
        GraphSocket* AddInputFlow(const std::string& name, unsigned type);
        GraphSocket* AddOutputFlow(const std::string& name, unsigned type);
        GraphSocket* InsertOutputFlow(unsigned index, const std::string& name, unsigned type);

        void NotifySocketsChange() const;

        /// Migrate Outputs into downstream inputs
        void PropogateValues(bool down);

        /// Execute in upstream (right->left) based evaluation (like a Shader or texture graph), requires a master node
        void ExecuteUpstream(unsigned& executionContext, const Variant& parameter, unsigned ignoringNode = -1);

        void VisitUpstream(GraphNodeVisitor* visitor);

        void ForceExecuteUpstreamOnly(const Variant& parameter, unsigned ignoringNode = -1);

        /// Execute in downstream (left->right), requires a master node
        void ExecuteDownstream(unsigned& executionContext, const Variant& parameter, unsigned ignoringNode = -1);

        /// Execute in a hybrid of the above two, flow control goes downstream (left->right), variables are fetched upstream (right->left), execution should begin at an "Entry Node"
        void ExecuteHybrid(unsigned& executionContext, const Variant& parameter, unsigned ignoringNode = -1);

        std::vector< std::pair<GraphSocket*, GraphSocket*> > GetConnections() const;
        void RestoreConnections(const std::vector< std::pair<GraphSocket*, GraphSocket*> >& connections);

    protected:
        GraphSocket* selectedExit = 0x0;

    private:
        unsigned lastExecutionContext;

        int Execute(unsigned& executionContext, const Variant& paramter)
        {
            // Don't execute multiple times per graph walk
            if (executionContext != -1 && executionContext == lastExecutionContext)
                return GRAPH_EXECUTE_COMPLETE;
            lastExecutionContext = executionContext;

            // If we're looping then increment the execution context
            int retVal = Execute(paramter);
            if (retVal == GRAPH_EXECUTE_LOOP)
                ++executionContext;
            return retVal;
        }
    };

}