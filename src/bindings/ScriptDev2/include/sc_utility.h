/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_UTILITY_H
#define SC_UTILITY_H

// class for handling timed events
class MANGOS_DLL_DECL EventManager
{
protected:
    struct Event
    {
        uint32 id;          // event identifier
        uint32 period_min;  // repeating period (0 = doesn't repeat)
        uint32 period_max;  // random value in interval 'period_min' to 'period_max'
        uint32 phase_mask;  // phases where this event can happen (bit mask)
        uint32 cooldown;    // cooldown triggered after event executes
        uint32 category;    // cooldown category (will trigger cd only for this category)

        Event(uint32 _id, uint32 _pmin, uint32 _pmax, uint32 _phase, uint32 _cd, uint32 _cat):
            id(_id), period_min(_pmin), period_max(_pmax), phase_mask(_phase), cooldown(_cd), category(_cat)
        {
        }
    };

    struct Cooldown
    {
        uint32 duration;    // total length of the cooldown
        uint32 remaining;   // remaining time

        explicit Cooldown(uint32 _val = 0):
            duration(_val), remaining(_val)
        {
        }

        void Update(uint32 time_diff);
    };

    typedef std::multimap<uint32 /*time*/, Event> EventContainer;
    typedef std::map<uint32 /*category*/, Cooldown> CooldownMap;

    uint32 m_time;              // current elapsed time (in milliseconds)
    uint32 m_phase;             // current phase (value, not mask)
    EventContainer m_events;    // event queue
    CooldownMap m_cooldowns;    // cooldowns by category

    void InsertEvent(uint32 time_diff, Event const& ev);

public:
    enum { ALL_PHASES = 0xFFFFFFFF };   // default value for phase mask

    EventManager();

    uint32 GetTimer() const { return m_time; }
    uint32 GetPhase() const { return m_phase; }
    uint32 GetPhaseMask() const { return 1 << m_phase; }
    uint32 GetCooldownDuration(uint32 categ) const;
    uint32 GetCooldownTime(uint32 categ) const;

    void Reset();
    void ResetCooldowns();

    void Update(uint32 time_diff);
    void SetPhase(uint32 phase);
    void SetCooldown(uint32 time, uint32 categ = 0);

    // inserts an event in the queue that will execute in the next 'time' milliseconds and
    // will repeat every 'period' milliseconds (if not 0). Each triggering of this event
    // will cause a cooldown of 'cd' milliseconds on the 'cd_cat' category
    void ScheduleEvent(uint32 event_id, uint32 time, uint32 period = 0, uint32 cd = 0, uint32 cd_cat = 0, uint32 phase_mask = ALL_PHASES);
    // cancels pending events with the specified id and inserts new one into the queue
    void RescheduleEvent(uint32 event_id, uint32 time, uint32 period = 0, uint32 cd = 0, uint32 cd_cat = 0, uint32 phase_mask = ALL_PHASES);

    // schedule event with random time interval
    // if the _max value is 0, then its assumed no range (value equal to _min)
    void ScheduleEventInRange(uint32 event_id, uint32 time_min, uint32 time_max = 0, uint32 period_min = 0, uint32 period_max = 0, uint32 cd = 0, uint32 cd_cat = 0, uint32 phase_mask = ALL_PHASES);
    void RescheduleEventInRange(uint32 event_id, uint32 time_min, uint32 time_max = 0, uint32 period_min = 0, uint32 period_max = 0, uint32 cd = 0, uint32 cd_cat = 0, uint32 phase_mask = ALL_PHASES);

    bool HasEvent(uint32 time = 0) const;   // check if there is a pending event in the next 'time' milliseconds
    uint32 GetNextEvent() const;            // returns next queued event

    void CancelAllEvents();                                             // cancels all events
    void CancelEvent(uint32 event_id, uint32 phase_mask = ALL_PHASES);  // ^ with specified id
    void CancelNextEvent();                                             // cancels next queued event
    void CancelNextEventWithId(uint32 event_id, uint32 phase_mask = ALL_PHASES); // ^ with specified id
    void CancelEventsWithCategory(uint32 categ, uint32 phase_mask = ALL_PHASES); // cancels all events with specified cd category

    void DelayEvents(uint32 time, uint32 phase_mask = ALL_PHASES);      // delays all events by the specified amount
    void DelayEventsWithId(uint32 event_id, uint32 time, uint32 phase_mask = ALL_PHASES); // ^ with specified id

    uint32 ExecuteEvent();                  // process and return next pending event

private:
    EventManager(EventManager const&);
    EventManager& operator= (EventManager const&);
};

// helper for calculating bit masks (compile time constant that can be used in enums)
// usage: bit_mask<bit_num, ...>::value
template <int N1, int N2 = -1, int N3 = -1, int N4 = -1, int N5 = -1, int N6 = -1>
struct bit_mask { enum { value = 1UL << N1 | bit_mask<N2, N3, N4, N5, N6, -1>::value }; };

template <>
struct bit_mask<-1, -1, -1, -1, -1, -1> { enum { value = 0 }; };

#endif // SC_UTILITY_H
