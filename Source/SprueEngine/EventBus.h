#pragma once

#include <SprueEngine/Variant.h>
#include <functional>
#include <unordered_map>

namespace SprueEngine
{

    typedef void (*SPRUE_EVENT_FUNCTION)(unsigned, VariantMap&);

    class EventBus;

    class SPRUE EventReceiver
    {
        friend class EventBus;
        bool ignoreRemoval_ = false;
        std::vector< std::pair<EventBus*, unsigned> > listeningTo_;

        void Event_AddBus(EventBus* bus);

    public:
        virtual ~EventReceiver();
        void Event_Remove(EventBus* listeningTo);
    };

    /// General purpose event dispatcher. May place on anything that needs to send events.
    /// Event_XXX function naming to minimize name collision possibilities.
    class SPRUE EventBus
    {
        struct EventDispatch {
            virtual void Dispatch(void* object, unsigned eventID, VariantMap& data) = 0;
        };
        
        struct EventRecord {
            EventBus* bus = 0x0;
            EventReceiver* object = 0x0;
            SPRUE_EVENT_FUNCTION function = 0x0;
            EventDispatch* dispatch = 0x0;

            EventRecord(EventBus* bus, EventReceiver* object, SPRUE_EVENT_FUNCTION func, EventDispatch* disp) : 
                bus(bus),
                object(object), 
                function(func), 
                dispatch(disp) 
            { }
            ~EventRecord();

            void Dispatch(unsigned eventID, VariantMap& data);
        };

        template<typename T>
        struct EventDispatchImpl {
            typedef void(T::*CALL_METHOD)(unsigned, VariantMap&);

            EventDispatchImpl(CALL_METHOD call) : call_(call) { }

            CALL_METHOD call_;

            virtual void Dispatch(void* object, unsigned eventID, VariantMap& data) override {
                (((T*)object)->*call_)(eventID, data);
            }
        };

        /// Handle subscription of arbitrary EventDispatchImpl
        void Event_Subscribe(EventReceiver* object, unsigned eventID, EventDispatch* dispatch);

    public:
        /// Clean up all events.
        virtual ~EventBus();

        /// Subscribe a free-function to an event.
        void Event_Subscribe(unsigned eventID, SPRUE_EVENT_FUNCTION func);
        
        /// Subscribe a class method and instance to an event.
        template<typename T>
        void Event_Subscribe(T* listener, unsigned eventID, int (T::*METHOD)(char, float))
        {
            Event_Subscribe(listener, eventID, new EventDispatchImpl<T>(METHOD));
        }
        
        /// Send an event that lacks data.
        void Event_Send(unsigned eventID);
        /// Send an event with the given data map.
        void Event_Send(unsigned eventID, VariantMap& eventMap);
        /// Remove any events from the given listener for an Event ID
        void Event_Remove(EventReceiver* listener, unsigned eventID);
        /// Remove any events from the given function for an Event ID
        void Event_Remove(SPRUE_EVENT_FUNCTION func, unsigned eventID);
        /// Remove all events for the given ID
        void Event_RemoveAll(unsigned eventID);
        /// Remove all events associated with the given listener
        void Event_RemoveAll(EventReceiver* listener);
        /// Remove ALL events. Period.
        void Event_Clear();

        /// Get a static shared eventdata map for loading.
        static VariantMap& Event_GetEventDataMap() { return eventDataMap_; }

    private:
        std::unordered_map<unsigned, std::vector<EventRecord> > eventRecords_;
        static VariantMap eventDataMap_;
    };

}