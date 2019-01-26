#include "DataSource.h"

MultiDataSource::MultiDataSource(std::vector< std::shared_ptr<DataSource> > sources) :
    sources_(sources)
{

}

void MultiDataSource::Destroy()
{
    for (auto src : sources_)
    {
        if (src)
            src->Destroy();
    }
}

QString MultiDataSource::GetName() const
{
    QString ret;
    for (unsigned i = 0; i < sources_.size(); ++i)
    {
        if (!ret.isEmpty())
            ret = ret.append(", ");
        ret = ret.append(sources_[i]->GetName());
    }
    return ret;
}

QString MultiDataSource::GetTypeName() const
{
    QString ret;
    for (unsigned i = 0; i < sources_.size(); ++i)
    {
        if (!ret.isEmpty())
            ret = ret.append(", ");
        ret = ret.append(sources_[i]->GetTypeName());
    }
    return ret;
}

bool MultiDataSource::equals(const DataSource* rhs) const
{
    if (const auto other = dynamic_cast<const MultiDataSource*>(rhs))
    {
        if (other->sources_.size() != sources_.size())
            return false;
        for (unsigned i = 0; i < sources_.size(); ++i)
        {
            if (*sources_[i] != other->sources_[i].get())
                return false;
        }
        return true;
    }
    return false;
}