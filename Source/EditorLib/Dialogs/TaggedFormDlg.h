#pragma once

#include <EditorLib/editorlib_global.h>
#include <EditorLib/Controls/TaggedForm.h>

#include <QDialog>

/// Utility dialog for displaying a TaggedForm in a dialog prompt.
/// All code should migrate to favoring and expanding on TaggedForm for throw-away tasks.
class EDITORLIB_EXPORT TaggedFormDlg : public QDialog
{
public:
    /// Construct.
    TaggedFormDlg(const QString& title, std::vector<TaggedField>& fields, bool cancelable = false, QWidget* parent = 0x0);
    /// Destruct.
    virtual ~TaggedFormDlg();

    /// Returns the contained fields.
    std::vector<TaggedField>& GetFields() { return form_->GetFields(); }
    /// Returns the contained fields.
    const std::vector<TaggedField>& GetFields() const { return form_->GetFields(); }

    /// Shows a constructed modal dialog.
    static void ShowModal(const QString& title, std::vector<TaggedField>& fields, std::function<void()> accepted);
    static void ShowModal(const QString& title, std::vector<TaggedField>& fields, std::function<void()> accepted, const QSize& size);
    /// Shows a constructed modal dialog.
    static void ShowModal(const QString& title, std::vector<TaggedField>& fields, bool cancelable, std::function<void()> accepted);
    static void ShowModal(const QString& title, std::vector<TaggedField>& fields, bool cancelable, std::function<void()> accepted, const QSize& size);

private:
    /// Internal tagged form.
    TaggedForm* form_;
};