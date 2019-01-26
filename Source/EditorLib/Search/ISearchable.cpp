#include "ISearchable.h"

std::vector<ISearchable*> ISearchable::searchables_ = std::vector<ISearchable*>();

ISearchable::ISearchable()
{
    searchables_.push_back(this);
}

ISearchable::~ISearchable()
{
    auto found = std::find(searchables_.begin(), searchables_.end(), this);
    if (found != searchables_.end())
        searchables_.erase(found);
}

void ISearchable::CollectSearchResults(const QString& searchText, SearchResultVector& results, bool caseSensitive, bool exactMatch) const
{
    QStringList terms;
    terms.push_back(searchText);
    CollectSearchResults(terms, results, caseSensitive, exactMatch);
}