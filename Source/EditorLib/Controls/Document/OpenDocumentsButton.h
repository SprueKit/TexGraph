#pragma once

#include <EditorLib/editorlib_global.h>

#include <QPushButton>

class QSexyToolTip;

class DocumentBase;

/// Button for use in ribbon controls that pops up a window showing the currently open documents.
/// Populated with DocumentFauxButton.
class EDITORLIB_EXPORT OpenDocumentsButton : public QPushButton
{
    Q_OBJECT
public:
    /// Construct.
    OpenDocumentsButton(QWidget* parent = 0x0);
    /// Destruct.
    virtual ~OpenDocumentsButton();

    private slots:
    /// When the popup is shown the buttons contained will be updated.
    void PopupShown();
    /// When the active document changes update the contained buttons.
    void ActiveDocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc);
    /// Update button texts when a document's path changes.
    void DocumentPathChanged(const QString newPath);

private:
    /// Popup window that is displayed when the button is clicked.
    QSexyToolTip* popupWindow_ = 0x0;
    /// Widget containing the document controls.
    QWidget* documentsWidget_ = 0x0;
};