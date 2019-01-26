#pragma once

#define PUSH_DOCUMENT_UNDO(OBJECT) Global_DocumentManager()->GetActiveDocument()->GetUndoStack()->push(OBJECT)

#define CMD_IEDITABLE_PROPERTY          1
#define CMD_SPRUE_SCENE_ADD_CHILD       2
#define CMD_SPRUE_SCENE_CHANGE_POSITION 3
#define CMD_SPRUE_SCENE_DELETE          4
#define CMD_IEDITABLE_CHILD_OBJECT      5
#define CMD_IEDITABLE_CHILD_LIST_ADD    6
#define CMD_IEDITABLE_CHILD_LIST_DELETE 7
#define CMD_IEDITABLE_CHILD_LIST_MOVE   8
#define CMD_SPRUE_GIZMO                 9
#define CMD_COMPOUND                    10


#define CMD_URHO_PROPERTY           100
#define CMD_URHO_ADD_COMP           101
#define CMD_URHO_ADD_NODE           102
#define CMD_URHO_DELETE_COMP        103
#define CMD_URHO_DELETE_NODE        104
#define CMD_URHO_MOVE_COMP          105
#define CMD_URHO_MOVE_NODE          106
#define CMD_URHO_RENDER_ZONE_CUBE   107
#define CMD_URHO_IMPORT_SCENE       108
#define CMD_URHO_GIZMO              109
#define CMD_URHO_ADD_UIELEMENT      110
#define CMD_URHO_DELETE_UIELEMENT   111
#define CMD_URHO_MOVE_UIELEMENT     112

#define CMD_GRAPH_CREATE        500
#define CMD_GRAPH_DELETE        501
#define CMD_GRAPH_CONNECT       502
#define CMD_GRAPH_DISCONNECT    503
#define CMD_GRAPH_PASTE         504