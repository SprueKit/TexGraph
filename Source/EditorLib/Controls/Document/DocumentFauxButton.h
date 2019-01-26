#pragma once

#include <EditorLib/editorlib_global.h>

#include <QWidget>

class DocumentBase;

/// Emulates a button widget for displaying short information about a document and providing a quick means
/// of viewing dirty state and changing the current document.
/// TODO: add document file-type icon support.
class EDITORLIB_EXPORT DocumentFauxButton : public QWidget
{
    Q_OBJECT;
public:
    /// Construct.
    DocumentFauxButton(DocumentBase* document, QWidget* parent = 0x0);

    /// Sets the button to draw in a 'dirty' state.
    void SetDirty(bool state);
    /// Sets the button to draw in a 'my document is currently active' state.
    void SetCurrent(bool isCurrent);

    DocumentBase* GetDocument() const { return document_; }

signals:
    void Clicked();

protected:
    virtual void mousePressEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    virtual void enterEvent(QEvent* event) Q_DECL_OVERRIDE;

private:
    /// Document associated to this button.
    DocumentBase* document_ = 0x0;
    /// Local tracking for whether to draw as dirty or not.
    bool dirty_ = false;
    /// Local tracking for whether to draw as current or not.
    bool current_ = false;
    /// UI state tracking of mouse click.
    bool hasMouseDown_ = false;
};