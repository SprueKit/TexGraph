#pragma once

#include <EditorLIb/editorlib_global.h>

#include <QIcon>
#include <QString>
#include <QMimeData>

#include <functional>
#include <memory>
#include <vector>

/// Interface for an object that can be renammed.
class EDITORLIB_EXPORT IRenamable
{
public:
    /// Returns the display name of this object.
    virtual QString GetName() const = 0;
    /// Sets the name of this object.
    virtual void SetName(const QString& name) = 0;
};

/// Interface for a singular item display.
class EDITORLIB_EXPORT IDataItemView
{
public:
    struct ItemData {
        QIcon icon_;
        QString text_;
        QColor textColor_;
        QString toolTip_;
    };

    virtual ItemData GetDataItemView() const = 0;
};

/// Interface for a document or DataSource which can be used for display in the generic "Scene Tree"
/// Multiple "Scene Tree" views may exist, implementation should return the appropriate object for the given tag.
/// All usages assume that implementation also implements IRenamable and IDataItemView
class EDITORLIB_EXPORT IHierarchialView
{
public:
    /// Enumerates the children of this object.
    virtual void GetHierarchyChildren(std::vector< std::shared_ptr<IHierarchialView> >& children) = 0;

    /// Returns true if this object can be drag-and-dropped.
    virtual bool CanDrag() const = 0;
    /// Returns true if this object is deletable.
    virtual bool CanDelete() const = 0;
        
    /// Returns false if node being DROPPED ON will not accept the given child item.
    virtual bool AcceptAsChild(std::shared_ptr<IHierarchialView> item) = 0;
    /// Returns false if the node being DRAGGED will not accept the given drop target as a parent.
    virtual bool AcceptAsParent(std::shared_ptr<IHierarchialView> item) = 0;
    /// Add the given child to this object. Returns false if the attempt was denied.
    virtual bool AddChild(std::shared_ptr<IHierarchialView> child) = 0;
    /// Remove the given child node. Returns false if the attempt was denied.
    virtual bool RemoveChild(std::shared_ptr<IHierarchialView> child) = 0;
};

/// Interface for a document that can display contents into the palette view tree.
/// Palette items support text, icon, tooltips, mimedata (for drag-drop), and a filtering function (for selection context).
/// Limited to exceptionally simple mimedata (text keys)
class EDITORLIB_EXPORT IPaletteView
{
public:
    struct PaletteItem
    {
        QString text_;
        QString tooltip_;
        QIcon icon_;
        QString mimeTypeID_;
        QString simpleMimeData_;
    };
    struct PaletteItemGroup
    {
        QString name_;
        std::vector<PaletteItem> items_;
    };

    /// Returns the list of PalatteItems to display.
    virtual std::vector<PaletteItemGroup> GetPlatteItems() const = 0;
};

/// Interface for a document or object that can populate the timeline editor.
class EDITORLIB_EXPORT ITimelineView
{
public:
};

/// Objects may be decorated, in which case the implementation of this interface should return the decorator object for the request
/// decorator tag. Decoration allows for external code to expand on the capabilities of a datasource or document.
/// In application:
///     When IDecoratable is encountered, GetDecoratableID() will be called, this ID will be used to check the decoration registry,
///     for a valid Decorator instance that can work with the given DecoratableID for the requested purpose.
/// Example Decoratable IDs:
///     For intelligibility it is best to use the typename of the object to be decorated. ie. "IEditableDataSource"
/// Example purposes:
///     Should for intelligibility use the interface type name: ie. "IHierarchialView", "ITimelineView", etc.
class EDITORLIB_EXPORT IDecoratable
{
public:
    /// Returns the full ID path, basemost_class -> base_class -> leaf_class, decorator seek will be performed from lowest -> highest level.
    virtual QStringList GetDecoratableID() = 0;
};