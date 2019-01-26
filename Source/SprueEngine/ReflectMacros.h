#pragma once

/// Mark a type as reflected, include additional info inside of the type info
#define REFLECTED(TYPE_INFO)

/// Mark a global variable to be exposed
/* example usage:
REFLECT_GLOBAL(Editor)
static MapData* currentMapData;
By exposing that to editor will be able to see all reflected material in MapData, including the std::vector<MapObject*> contents contained

Can also be placed on a method to expose the return value
*/
#define REFLECT_GLOBAL(GLOBAL_INFO)

/// Properties are exposed by default
/*
This can be used to define additional traits for the property:
name "Pretty Name To Print"
tip "Textual usage tip"
depend "FieldName"  (GUI hint: changing this field means having to refresh all fields with "depend")
Precise (GUI hint: should use 0.01 for steps, instead of 1.0 default)
Fine    (GUI hint: should use 0.1 for steps, instead of 1.0 default)
Q1      (GUI hint: fixed-pont using Q1.7 for 0.0 - 1.0 range, must be char/byte)
Q8      (GUI hint: fixed-point, using Q8.8, must be short)
Q16     (GUI hint: fixed-point, using Q16.16, must be int)
Q32     (GUI hint: fixed-point using Q32.32, must be int64)
get __GetterMethodName__ (BINDING: getter must be TYPE FUNCTION() const)
set __SetterMethodName__ (BINDING: setter must be void FUNCTION(const TYPE&) )
resource __ResourceMember__ (BINDING: named property is the holder for resource data that matches this resource handle object)
*/
#define PROPERTY(PROPERTY_INFO)
#define VIRTUAL_PROPERTY(PROPERTY_INFO)

/// Next property will not be reflected
#define NO_REFLECT

/// Specify an enumeration to use for a bitfiled
#define BITFIELD_FLAGS(ENUM_NAME)

/// Bind a method for GUI exposure
/*
name "Pretty Name To Print"
tip "Textual usage tip"
editor (command will be exposed in the editor GUI, otherwise it is assumed to be for scripting only)
*/
#define METHOD_COMMAND(METHOD_INFO)