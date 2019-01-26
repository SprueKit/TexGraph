#include "Selectron.h"

void Selectron::SetDelegate(Selectron* delegateTarget)
{
    const bool delegateChanged = delegateTarget != delegate_;
    if (delegate_)
        disconnect(delegate_);

    // Set new, and if it's real attach
    if (delegate_ = delegateTarget)
    {
        // Connect for forwarding signals as ourself
        connect(delegate_, &Selectron::SelectionChanged, [=](void* who, Selectron* sel) {
            emit SelectionChanged(who, this);
        });

        connect(delegate_, &Selectron::DataChanged, [=](void* source, Selectron* selectron, unsigned propertyHash) {
            emit DataChanged(source, selectron, propertyHash);
        });
        
        connect(delegate_, &Selectron::SelectionRejected, [=](void* source) {
            emit SelectionRejected(source);
        });
    }

    // Delegate selectron has changed, now tell EVERYONE about it so they can respond
    if (delegateChanged)
        emit SelectionChanged(0x0, this);
}

std::shared_ptr<DataSource> Selectron::Selected(unsigned idx) const 
{
    if (delegate_)
        return delegate_->Selected(idx);

    return selected_.size() > idx ? selected_[idx] : 0x0; 
}

std::shared_ptr<DataSource> Selectron::MostRecentSelected() const 
{ 
    if (delegate_)
        return delegate_->MostRecentSelected();
    return selected_.size() > 0 ? selected_.back() : 0x0; 
}

void Selectron::SetSelected(void* who, std::shared_ptr<DataSource> sel)
{
    if (delegate_)
    {
        delegate_->SetSelected(who, sel);
        return;
    }

    if (!selected_.empty())
        selected_.clear();
    if (sel) // don't actually add a selection if we'd be adding 0x0/null
        AddSelected(who, sel);
    else
        emit SelectionChanged(who, this);
}

bool Selectron::AddSelected(void* who, std::shared_ptr<DataSource> sel)
{
    if (delegate_)
        return delegate_->AddSelected(who, sel);

    for (unsigned i = 0; i < selected_.size(); ++i)
    {
        if (selected_[i]->Equals(sel.get()))
            return false;
    }
    selected_.push_back(sel);
    emit SelectionChanged(who, this);
    return true;
}

void Selectron::RemoveSelected(void* who, std::shared_ptr<DataSource> sel)
{
    if (delegate_)
    {
        delegate_->RemoveSelected(who, sel);
        return;
    }

    auto foundData = std::find(selected_.begin(), selected_.end(), sel);
    if (foundData != selected_.end())
        selected_.erase(foundData);
    emit SelectionChanged(who, this);
}

void Selectron::ClearSelection(void* who)
{
    if (delegate_)
    {
        delegate_->ClearSelection(who);
        return;
    }

    selected_.clear();
    emit SelectionChanged(who, this);
}

void Selectron::NotifyDataChanged(void* who, std::shared_ptr<DataSource> sel, unsigned hash)
{
    // No delegation required
    if (std::find(selected_.begin(), selected_.end(), sel) == selected_.end())
        SetSelected(who, sel);
    emit DataChanged(who, this, hash);
}

void Selectron::NotifyDataChangedSoft(void* who, std::shared_ptr<DataSource> sel, unsigned hash)
{
    // No delegation required
    emit DataChanged(who, this, hash);
}

bool Selectron::AllSelectedSameType() const
{
    if (delegate_)
        return delegate_->AllSelectedSameType();

    if (selected_.size() > 1)
    {
        unsigned firstType = selected_.front()->GetID();
        for (auto it = selected_.begin() + 1; it != selected_.end(); ++it)
            if (firstType != it->get()->GetID())
                return false;
        return true;
    }
    else if (!selected_.empty())
        return true;
    return false;
}