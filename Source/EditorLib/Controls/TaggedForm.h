#pragma once

#include <QVariant>
#include <QWidget>

#include <algorithm>
#include <functional>
#include <string>

/// Enumeration of field types.
enum TaggedFieldType {
    TFT_Int,            // integer
    TFT_Float,          // double
    TFT_String,         // QString
    TFT_Path,           // Field is a file path (QString)
    TFT_PathList,       // List of file paths (QStringList)
    TFT_Folder,         // Path as a folder (QString)
    TFT_FolderList,     // StringList is a collection of folder paths (QStringList)
    TFT_SettingLinked,  // Responsibility will be deferred to a settings URI ("Graphics/Render Mode")
    TFT_InfoBoxTip,     // Field is not a widget, but an informational tip shown in a group-box
};

/// Field record for tagged display.
struct TaggedField {
    TaggedFieldType type_;
    std::string fieldName_;
    std::string fieldTip_;
    QVariant value_;
    QVariant default_;
    std::function<void(QVariant)> changed_;
};

/// Displays a collection of widgets based on a list definition of fields.
class TaggedForm : public QWidget
{
    Q_OBJECT;

public:
    /// Construct.
    TaggedForm(std::vector<TaggedField>& fields, QWidget* parent = 0x0);
    /// Destruct.
    virtual ~TaggedForm();

    /// Returns the contained fields.
    std::vector<TaggedField>& GetFields() { return fields_; }
    /// Returns the contained fields.
    const std::vector<TaggedField>& GetFields() const { return fields_; }

private:
    /// List of tagged fields for the form.
    std::vector<TaggedField>& fields_;
};