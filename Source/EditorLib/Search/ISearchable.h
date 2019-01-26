#pragma once

#include <EditorLib/editorlib_global.h>
#include <EditorLib/DataSource.h>

#include <QString>
#include <QStringList>

#include <memory>
#include <string>
#include <vector>

/// Abstract base for a search result
class EDITORLIB_EXPORT SearchResult
{
public:
    virtual void GoTo() = 0;
    /// Used for sorting by hits
    int hitCount_;
    /// Name for the "source" object containing the search result.
    QString source_;
    /// Used for display in the search UI.
    QString text_;
    /// Data-source for the search object.
    std::shared_ptr<DataSource> dataSource_;
};

typedef std::vector<std::shared_ptr<SearchResult> > SearchResultVector;

/// Interface for objects that support being searched through.
/// When searching the list of ISearchable objects will be enumerated and queried.
class EDITORLIB_EXPORT ISearchable
{
public:
    ISearchable();
    virtual ~ISearchable();

    virtual void CollectSearchResults(const QString& searchText, SearchResultVector& results, bool caseSensitive = false, bool exactMatch = false) const;
    virtual void CollectSearchResults(const QStringList& searchTerms, SearchResultVector& results, bool caseSensitive = false, bool exactMatch = false) const = 0;

    static std::vector<ISearchable*>& GetSearchables() { return searchables_; }

private:
    static std::vector<ISearchable*> searchables_;
};
