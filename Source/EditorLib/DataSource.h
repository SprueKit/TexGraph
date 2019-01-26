#pragma once

#include <EditorLib/editorlib_global.h>

#include <QHash>
#include <QObject>
#include <QString>
#include <QVariant>

#include <memory>
#include <vector>

/// Describes a property in an object. DataSource implementations are expected to enumerate their available properties.
/// Properties should be remapped to QVariant capable types (QVector3D, QUrl, etc) and annotated with flags.
struct EDITORLIB_EXPORT PropertyInfo
{
    enum Flags {
        PI_BitField = 1,        // Integral field should be displayed as a set of boolean flags for each bit
        PI_ReadOnly = 1 << 1,   // Field cannot be edited, only viewed
        PI_Permutable = 1 << 2, // Field supports lists of alternative values
        PI_NoDataGrid = 1 << 3, // Field should not be displayed in datagrids
    };

    /// Will display as the field name.
    QString name_;
    /// Will display as a tooltip.
    QString tip_;
    /// Indicates the nature of the type, UInt, QVariantList, VoidStar are special (may be flags, or sub-objects, etc)
    /// Favor Q_DECLARE_METATYPE for simple struct types (ResponseCurve, etc)
    /// Use custom flags and factories for more complicated types (ie. virtual structs)
    QMetaType::Type type_;
    /// If valid then this property can be reset to this value.
    QVariant defaultValue_;
    /// Provides additional annotation
    unsigned flags_;
    /// Purpose of extra tag varies, for something indicated as BitField this would be the name-list of bits to lookup for tips
    QVariant extraTag_;
};

/// Base class for all sources of data. Datasources are used for controls and selections (in Selectron).
/// Derived implementations should be created, and continued inheritance is allowed and encouraged (DataSource -> IEditableDataSource -> SceneObjectDataSource).
/// DataSources are also used for storing objects in Undo/Redo commands, their typing is important for correct restoration of state.
class EDITORLIB_EXPORT DataSource : public QObject, public std::enable_shared_from_this<DataSource>
{
    Q_OBJECT;

    DataSource(const DataSource&) = delete;
public:
    /// Returns true if this datasource is the same as the other datasource.
    bool operator==(const DataSource* rhs) const { return equals(rhs); }
    /// Returns true if this datasource is different then the other datasource.
    bool operator!=(const DataSource* rhs) const { return !equals(rhs); }

    /// Returns a type identifier for this datasource.
    virtual int GetID() const = 0;
    /// Typically a data source doesn't own things, but when used in undo/redo it is responsible for any necessary destruction (or release of shared_ptr).
    virtual void Destroy() { }
    /// Returns the parent datasource, if any exists.
    std::shared_ptr<DataSource> GetParent() const { return parent_; }
    /// Returns a textual name for the object.
    virtual QString GetName() const = 0;
    /// Returns a textual name of the dataobject type for the object.
    virtual QString GetTypeName() const = 0;

    /// Dynamic data-sources require constant updating of their property page.
    virtual bool IsDynamic() const { return false; }

    bool Equals(const DataSource* rhs) const { return equals(rhs); }

    /// Return all columns.
    virtual std::vector<QString> EnumerateFields() = 0;
    /// Return false if not allowed to display the queried field (such as it's secret)
    virtual bool CanShow(const QString& column) = 0;
    /// Return true if the given columns sorts setting from text
    virtual bool CanSetText(const QString& column) = 0;
    /// Set the value based on a string.
    virtual void FromText(const QString& column, const QString& text) = 0;
    /// Return the value as a string (mostly likely want to stringify the results of ToVariant in implementation).
    /// It is the responsibility of the implementation to signal data change if occuring.
    virtual QString ToText(const QString& column) = 0;
    /// Get a QVariant type for the given object.
    virtual QVariant ToVariant(const QString& column) = 0;
    /// Set the value of a column with a QVariant.
    /// It is the responsibility of the implementation to signal data change if occuring.
    virtual void FromVariant(const QString& column, const QVariant& var) = 0;

protected:
    /// Construct.
    DataSource() { }
    /// Implementation is used by the comparison operators.
    virtual bool equals(const DataSource* rhs) const = 0;
    /// Possible for an entire hierarchy to be available thorugh the datasource.
    std::shared_ptr<DataSource> parent_;
};

/// Not intended for general purpose use. Intended only for usage in
/// ephemeral cases related to control (such as a gizmo interacting with multiple objects).
class EDITORLIB_EXPORT MultiDataSource : public DataSource
{
    MultiDataSource(const MultiDataSource&) = delete;
    MultiDataSource& operator=(const MultiDataSource&) = delete;
public:
    MultiDataSource(std::vector< std::shared_ptr<DataSource> > sources);

    virtual int GetID() const override { return 1001; }
    virtual void Destroy() override;
    virtual QString GetName() const override;
    virtual QString GetTypeName() const override;

    std::vector< std::shared_ptr<DataSource> >& GetDataSources() { return sources_; }
    const std::vector< std::shared_ptr<DataSource> >& GetDataSources() const { return sources_; }

    virtual std::vector<QString> EnumerateFields() override { return std::vector<QString>(); }
    virtual bool CanShow(const QString& column) override { return false; }
    virtual bool CanSetText(const QString& column) override { return false; }
    virtual void FromText(const QString& column, const QString& text) { }
    virtual QString ToText(const QString& column) override { return QString(); }
    virtual QVariant ToVariant(const QString& column) override { return QVariant(); }
    virtual void FromVariant(const QString& column, const QVariant& var) override { }

protected:
    virtual bool equals(const DataSource* rhs) const override;

    std::vector< std::shared_ptr<DataSource> > sources_;
};