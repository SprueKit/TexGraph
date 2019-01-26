#pragma once

#include <EditorLib/editorlib_global.h>

#include <QStackedWidget>
#include <map>

class DocumentBase;
class Selectron;

/// QStackedWidget that uses a predicate object to select the appropriate widget to display for
/// the current document.
/// Purpose:
/// Use this widget to display the appropriate specific control where the same DockingPanel is
/// semantically appropriate (ie. "Scene Tree")
class EDITORLIB_EXPORT DocumentViewStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    /// Interface for a class used to confirm the validity of a widget for a given document.
    /// Results are prioritized by the returned specificity.
    struct IDocTypeChecker
    {
        virtual bool CheckDocument(DocumentBase* doc) = 0;
        virtual int Specificity() const { return 0; }
    };

    /// Simple DocTypeChecker that works solely against a specific type.
    /// Current view-index is not a concern
    template<class T>
    struct TypedDocTypeChecker : public IDocTypeChecker
    {
        virtual bool CheckDocument(DocumentBase* doc) { return dynamic_cast<T*>(doc) != 0x0; }
        virtual int Specificity() const override { return 1; }
    };

    /// Type checker that requires a specific type and current view-index.
    /// Has higher specificity than the type only checker.
    template<class T>
    struct TypedViewIndexDocTypeChecker : public IDocTypeChecker {
        TypedViewIndexDocTypeChecker(int idx) : viewIndex_(idx) {

        }

        virtual bool CheckDocument(DocumentBase* doc) { return dynamic_cast<T*>(doc) != 0x0 && doc->GetActiveViewIndex() == viewIndex_; }
        virtual int Specificity() const override { return 2; }

        int viewIndex_;
    };

    /// Simple DocTypeChecker that only cares about what the document has selected in its local Selectron.
    /// No concern for view-index
    template<typename T>
    struct DataSourceTypeChecker : public IDocTypeChecker {
        virtual bool CheckDocument(DocumentBase* doc) {
            static_assert(std::is_base_of<DataSource, T>::value, "T for DataSourceTypeChecker must be derived from DataSource");
            if (std::dynamic_pointer_cast<T>(doc->GetSelectron()->MostRecentSelected()))
                return true;
            return false;
        }
        // This is very specific
        virtual int Specificity() const override { return 3; }
    };

    /// IDocTypeChecker implementation that cares about the type of document and the active data-source in the document.
    template<typename DOC, typename DS>
    struct DocDataSourceTypeChecker : public DataSourceTypeChecker<DS>
    {
        static_assert(std::is_base_of<DocumentBase, DOC>::value, "DOC for DocDataSourceTypeChecker must be a DocumentBase derived type");
        static_assert(std::is_base_of<DocumentBase, DS>::value, "DS for DocDataSourceTypeChecker must be a DataSource derived type");

        virtual bool CheckDocument(DocumentBase* doc) {
            if (!dynamic_cast<DOC*>(doc))
                return false;
            return DataSourceTypeChecker::CheckDocument(doc);
        }
    };

    /// Construct.
    DocumentViewStackedWidget(QWidget* parent = 0x0);
    /// Destruct.
    virtual ~DocumentViewStackedWidget();

    /// Add a widget with a specific type checker.
    void AddWidget(IDocTypeChecker* checker, QWidget* widget);

    /// Helper function for adding a checked widget based solely on document type.
    template<typename T>
    void AddWidget(QWidget* widget) { AddWidget(new TypedDocTypeChecker<T>(), widget); }

    /// Helper function for adding a checked widget based on both document type and current view index.
    template<typename T, int IDX>
    void AddWidget(QWidget* widget) { AddWidget(new TypedViewIndexDocTypeChecker<T>(IDX), widget); }

private slots:
    /// Respond to view changes when a new document is focused.
    void OnDocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc);
    /// Respond to changes of the view in a document.
    void OnDocumentViewChanged(DocumentBase* doc);
    /// Respond to changes of the document's selectron.
    void OnSelectionChanged(void* src, Selectron* sel);

private:
    /// Maps the index of the active view to the index in the stacked widget.
    std::map<int, int> viewIndexToWidget_;
    /// Verifies if we actually care about a document.
    std::map<IDocTypeChecker*, int> checkerTable_;
};