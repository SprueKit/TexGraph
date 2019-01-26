#pragma once

#include <vector>

namespace SprueEngine
{
    class SceneObject;
    class Graph;
    class GraphNode;
    struct GraphSocket;

    /// Function signature for an object added (to another object) callback.
    typedef void (*OnSceneObjectAdded)(SceneObject*);

    /// Function signature for an object removed from an object callback.
    typedef void (*OnSceneObjectRemoved)(SceneObject*);

    /// Signature for addition of a graph node to a graph.
    typedef void (*OnGraphNodeAdded)(GraphNode*);
    /// Signature for removal of a graph node from a graph.
    typedef void (*OnGraphNodeRemoved)(GraphNode*);
    /// Signature for when the sockets of a graph node are changed.
    typedef void(*OnGraphNodeSocketsChanged)(GraphNode*);
    /// Signature for graph connection callback.
    typedef void (*OnGraphConnectionAdded)(GraphSocket* from, GraphSocket* to);
    /// Signature for graph disconnection callback.
    typedef void (*OnGraphConnectionRemoved)(GraphSocket* from, GraphSocket* to);

    /// Utility struct for holding callbacks and handling their invocation.
    struct CallBacks
    {
        std::vector<OnSceneObjectAdded> sceneObjectAdded_;
        std::vector<OnSceneObjectRemoved> sceneObjectRemoved_;

        std::vector<OnGraphNodeAdded> graphNodeAdded_;
        std::vector<OnGraphNodeRemoved> graphNodeRemoved_;
        std::vector<OnGraphNodeSocketsChanged> graphNodeSocketsChanged_;
        std::vector<OnGraphConnectionAdded> graphConnAdded_;
        std::vector<OnGraphConnectionRemoved> graphConnRemoved_;

#define FIRE_CALLBACK(LIST, OBJECT) for (auto call : LIST) call(OBJECT)
#define FIRE_CALLBACK2(LIST, OBJ1, OBJ2) for (auto call : LIST) call(OBJ1, OBJ2)

        void SceneObjectAdded(SceneObject* obj) const { FIRE_CALLBACK(sceneObjectAdded_, obj); }
        void SceneObjectRemoved(SceneObject* obj) const { FIRE_CALLBACK(sceneObjectRemoved_, obj); }
        
        void GraphNodeAdded(GraphNode* node) const { FIRE_CALLBACK(graphNodeAdded_, node); }
        void GraphNodeRemoved(GraphNode* node) { FIRE_CALLBACK(graphNodeRemoved_, node); }
        void GraphNodeSocketsChanged(GraphNode* node) { FIRE_CALLBACK(graphNodeSocketsChanged_, node); }
        void GraphConnectionAdded(GraphSocket* from, GraphSocket* to) const {   FIRE_CALLBACK2(graphConnAdded_, from, to); }
        void GraphConnectionRemoved(GraphSocket* from, GraphSocket* to) const { FIRE_CALLBACK2(graphConnRemoved_, from, to); }

#undef FIRE_CALLBACK
#undef FIRE_CALLBACK2

#define CALLBACK_FUNCTIONS(NAME, CALL, LIST) void Add ## NAME(CALL callback) { LIST.push_back(callback); } void Remove ## NAME(CALL callback) { auto found = std::find(LIST.begin(), LIST.end(), callback); if (found != LIST.end()) LIST.erase(found); }
        CALLBACK_FUNCTIONS(SceneObjectAdded, OnSceneObjectAdded, sceneObjectAdded_);
        CALLBACK_FUNCTIONS(SceneObjectRemoved, OnSceneObjectRemoved, sceneObjectRemoved_);
        CALLBACK_FUNCTIONS(GraphNodeAdded, OnGraphNodeAdded, graphNodeAdded_);
        CALLBACK_FUNCTIONS(GraphNodeRemoved, OnGraphNodeRemoved, graphNodeRemoved_);
        CALLBACK_FUNCTIONS(GraphNodeSocketsChanged, OnGraphNodeSocketsChanged, graphNodeSocketsChanged_);
        CALLBACK_FUNCTIONS(GraphConnnectionAdded, OnGraphConnectionAdded, graphConnAdded_);
        CALLBACK_FUNCTIONS(GraphConnectionRemoved, OnGraphConnectionRemoved, graphConnRemoved_);
#undef CALLBACK_FUNCTIONS
    };
}