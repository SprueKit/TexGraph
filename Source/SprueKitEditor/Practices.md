# SprueKit Programming Practices

If you are developing your own program based on the SprueKit source code you may disregard these practices as you deem fit. 
However, you may not be able to use some built-in funtionality easily (or at all) if you disregard these practices as certain assumptions are made in the source code.

## Deprecated Practices

- "Panels", the panels approach was misguided
    - Most panels have already been removed, the remainder will be moved as time permits

## General Practices

- QString is preferred over std::string. Use std::string when needed.
    - This was not always true, migrate std::string to QString as encountered and refactoring scale is reasonable
- STL containers are preferred over Qt containers
- STL smart pointer types are preferred over raw pointers
    - Exceptions for "perfect" RAII types

## Widget Practices

- Use "accessible name" (setAccessibleName) to specify a human friendly name for every important widget.
    - This value is used for fetching display names whenever necessary (such as the "Location" name for search results)
    - It is also used as a translation key in many widgets
- Similarly use "accessibility description" (setAccessibleDescription) for setting the help/hint text.
    - "What's this?" should be the exact same content.
    - Do not use "What's This?", it is half baked.
- Widget "object name"s (setObjectName) should be unique and present in the InternalNames.h header in the SprueKitEditor root folder.
- QDockWidgets should always be named as with the suffix "Panel" (ie. "Scene Tree Panel")

## Selected Object Management

SprueKit uses a class called the "Selectron" to manage active program selections.

The "Selectron" class manages the program's selections. It is "linked" against other controls that implement the "SelectronLinked" baseclass. SprueKit programs use a single global selectron, but it viable to have multiple selectrons (such as per document, or set of functionality).

The Selectron emits signals when the selection is changed, the selection's data is changed, or a selection is rejected.

**Selection Changes**

A call to Selectron::SetSelected() will set a single object to the new selection then emit the SelectionChanged signal.

A call to Selectron::AddSelected() will add a new item to the list of selected items then emit the SelectionChanged signal.

A call to Selectron::RemoveSelected() will remove the given item from the list of selected items (if found) and then emit the SelectionChanged signal.

**Selection Data Changes**

A call to Selectron::NotifyDataChanged() will emit the DataChanged signal. The "hash" parameter can be used for identifying the specific property that was changed.

A common usage of this is for propogating changes to an object's name throughout the UI and for updating transform properties as a gizmo is moved.

### Selection Rejection

Selection rejection is not used.

## Special Widgets (include baseclasses)

- BaseGraphControl (derived from QWidget)
    - baseclass to be used for creating Graph based controls that use the SprueKit graph library
- BasePropertyPage
    - baseclass to be used for implementing pages that are available in the master Property Browser control
    - A page exists for every editable object type
    - A page can be dynamic (controls must be rebuilt) or static (controls are only built once and reused)
- DragNotifiedTree (derived from QTreeWidget, implements ISearchable and ISignificantControl)
    - Specialization that uses additional virtual functions to control internal drag-drop behavior
- FlippableSplitter (derived from QSplitter)
    - A QSplitter that can have it's horizontal/vertical orientation changed through a context menu
    - Implements IPersistedWidget for saving state
- FlagWidget (derived from QWidget)
    - Maps an array of checkboxes (with optional tooltips per checkbox) to the bits of an unsigned integer
- MarkdownViewer (derived from QBrowser)
    - Specialization that can render Markdown formatted files/text
- QRangeSlider (derived from QWidget)
    - Double handled slider that allows specifying min-max values
- SingleKeySequenceEdit (derived from QKeySequenceEdit)
    - Specialization that only allows a single keyboard shortcut combination to be specified
- TimelineWidget
    - Widget that uses a generic model to display a timeline dopesheet


## Special Interfaces

- ISearchable
    - Implement for any persistent types (typically widgets) that should be searchable via the search bar in the main menu.
- ISignificantControl
    - Attach as a baseclass to any widget that needs to be easy to find from any arbitrary block of code
- IPersistedWidget
    - Attach as a baseclass to any control whose geometry/state needs to be saved as part of layout.

## Locations of Operating Specific code

- Everything in the "Platform" folder
    - VideoCard.h/.cpp, for detecting video card information
    - Thumbnails.h/.cpp, responsible for getting thumbnail previews from file paths

## Document / GuiBuilder

SprueKit's GUI code was written with the intent of keeping things easy to split off, however without so much engineering overhead as to be "click a button and fork" easy to split off. Some manual work is required in trimming code and the like, but the bulk of application specific code exists in the "GuiBuilder" folder and the specialized documents contained in "Documents" folder.

- Exceptions
    - Data folder contains datasources and other static data
    - Applications folder contains the initialization of different applications
    - Developer contains arbitrary "task" specific code
        - Assumable to be dev-time blocks of code (such as baking preview image icons, etc)


### Ribbon is First Class

The ribbon is a first class citizen. All commands must be accessible from both the Ribbon and the legacy menu/toolbar UI, no pull requests will ever be approved that are not.

## Rendering Viewports

- All views derive from Views/ViewBase
- All gizmos derive from Views/Gizmo

The actual view is owned by a "DocumentBase" instance, of which a Document may have multiple different views which may be switched between for different reasons (such as viewing UV maps instead of a model). The view is responsible for managing it's internal Urho3D scene for rendering and registering any required ViewControllers.

Viewport interaction is controlled through "ViewController" derived control classes that are given the opportunity to assert themselves for processing during View logic execution. Views/Controllers/FlyController implements a flying camera control (WSAD/QE, mouse look/orbit) and Views/Controllers/GizmoController implements the manipulation of selected gizmos. Object selections are also handled through controllers (RaySelect and UrhoRaySelect).