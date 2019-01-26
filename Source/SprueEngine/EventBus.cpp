#include <SprueEngine/EventBus.h>

namespace SprueEngine
{

    EventReceiver::~EventReceiver()
    {
        ignoreRemoval_ = true;
        for (auto rec : listeningTo_)
            rec.first->Event_RemoveAll(this);
    }

    void EventReceiver::Event_AddBus(EventBus* bus)
    {
        for (unsigned i = 0; i < listeningTo_.size(); ++i)
        {
            if (listeningTo_[i].first == bus)
            {
                listeningTo_[i].second += 1;
                return;
            }
        }
        listeningTo_.push_back(std::make_pair(bus, 1));
    }

    void EventReceiver::Event_Remove(EventBus* listeningTo)
    {
        if (ignoreRemoval_)
            return;
        std::remove_if(listeningTo_.begin(), listeningTo_.end(), [](std::pair<EventBus*, unsigned>& rec) {
            return (rec.second -= 1) == 0;
        });
    }

    EventBus::EventRecord::~EventRecord() 
    { 
        if (object)
            object->Event_Remove(bus);
        if (dispatch) 
            delete dispatch; 
    }

    void EventBus::EventRecord::Dispatch(unsigned eventID, VariantMap& data)
    {
        if (object && dispatch)
            dispatch->Dispatch(object, eventID, data);
        else if (function)
            function(eventID, data);
    }

    EventBus::~EventBus()
    {
        eventRecords_.clear();
    }

    void EventBus::Event_Subscribe(EventReceiver* object, unsigned eventID, EventDispatch* dispatch)
    {
        object->Event_AddBus(this);
        auto found = eventRecords_.find(eventID);
        if (found != eventRecords_.end())
            found->second.push_back({ this, object, 0x0, dispatch });
        else
            eventRecords_[eventID].push_back({ this, object, 0x0, dispatch });
    }

    void EventBus::Event_Subscribe(unsigned eventID, SPRUE_EVENT_FUNCTION func)
    {
        auto found = eventRecords_.find(eventID);
        if (found != eventRecords_.end())
            found->second.push_back({ this, 0x0, func, 0x0 });
        else
            eventRecords_[eventID].push_back({ this, 0x0, func, 0x0 });
    }

    void EventBus::Event_Send(unsigned eventID)
    {
        auto found = eventRecords_.find(eventID);
        if (found != eventRecords_.end())
        {
            for (auto item : found->second)
                item.Dispatch(eventID, Event_GetEventDataMap());
        }
    }

    void EventBus::Event_Send(unsigned eventID, VariantMap& eventMap)
    {
        auto found = eventRecords_.find(eventID);
        if (found != eventRecords_.end())
        {
            for (auto item : found->second)
                item.Dispatch(eventID, eventMap);
        }
    }

    void EventBus::Event_RemoveAll(unsigned eventID)
    {
        auto found = eventRecords_.find(eventID);
        if (found != eventRecords_.end())
            found->second.clear();
    }

    void EventBus::Event_RemoveAll(EventReceiver* listener)
    {
        for (auto record : eventRecords_)
            std::remove_if(record.second.begin(), record.second.end(), [=](const EventRecord& item) { return item.object == listener; });
    }

    void EventBus::Event_Remove(EventReceiver* listener, unsigned eventID)
    {
        auto found = eventRecords_.find(eventID);
        if (found != eventRecords_.end())
            std::remove_if(found->second.begin(), found->second.end(), [=](const EventRecord& rec) { return rec.object == listener; });
    }
    
    void EventBus::Event_Remove(SPRUE_EVENT_FUNCTION func, unsigned eventID)
    {
        auto found = eventRecords_.find(eventID);
        if (found != eventRecords_.end())
            std::remove_if(found->second.begin(), found->second.end(), [=](const EventRecord& rec) { return rec.function == func; });
    }

    void EventBus::Event_Clear()
    {
        eventRecords_.clear();
    }
}