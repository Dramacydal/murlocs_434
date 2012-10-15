#include "precompiled.h"

//--- EventManager --------------------------------------------------

void EventManager::Cooldown::Update(uint32 time_diff)
{
    if (!remaining)
        return;
    if (remaining <= time_diff)
        duration = remaining = 0;
    else
        remaining -= time_diff;
}

EventManager::EventManager():
    m_time(0), m_phase(0)
{
}

void EventManager::InsertEvent(uint32 time_diff, Event const& ev)
{
    m_events.insert(EventContainer::value_type(m_time + time_diff, ev));
}

uint32 EventManager::GetCooldownDuration(uint32 categ) const
{
    CooldownMap::const_iterator i_cd = m_cooldowns.find(categ);
    if (i_cd == m_cooldowns.end())
        return 0;
    return i_cd->second.duration;
}

uint32 EventManager::GetCooldownTime(uint32 categ) const
{
    CooldownMap::const_iterator i_cd = m_cooldowns.find(categ);
    if (i_cd == m_cooldowns.end())
        return 0;
    return i_cd->second.remaining;
}

void EventManager::Reset()
{
    m_time = m_phase = 0;
    m_events.clear();
    m_cooldowns.clear();
}

void EventManager::ResetCooldowns()
{
    m_cooldowns.clear();
}

void EventManager::Update(uint32 time_diff)
{
    m_time += time_diff;
    for (CooldownMap::iterator i = m_cooldowns.begin(); i != m_cooldowns.end(); ++i)
        i->second.Update(time_diff);
}

void EventManager::SetPhase(uint32 phase)
{
    if (phase >= 32)
        throw std::domain_error("EventManager::SetPhase(): phase value must be < 32");
    m_phase = phase;
}

void EventManager::SetCooldown(uint32 time, uint32 categ)
{
    m_cooldowns[categ] = Cooldown(time);
}

void EventManager::ScheduleEvent(uint32 event_id, uint32 time, uint32 period, uint32 cd, uint32 cd_cat, uint32 phase_mask)
{
    if (!event_id)
        throw std::invalid_argument("EventManager::ScheduleEvent(): event_id must be != 0");
    if (!phase_mask)
        throw std::invalid_argument("EventManager::ScheduleEvent(): phase_mask must be != 0");
    InsertEvent(time, Event(event_id, period, period, phase_mask, cd, cd_cat));
}

void EventManager::RescheduleEvent(uint32 event_id, uint32 time, uint32 period, uint32 cd, uint32 cd_cat, uint32 phase_mask)
{
    CancelEvent(event_id);
    ScheduleEvent(event_id, time, period, cd, cd_cat, phase_mask);
}

void EventManager::ScheduleEventInRange(uint32 event_id, uint32 time_min, uint32 time_max, uint32 period_min, uint32 period_max, uint32 cd, uint32 cd_cat, uint32 phase_mask)
{
    if (!event_id)
        throw std::invalid_argument("EventManager::ScheduleEventInRange(): event_id must be != 0");
    if (!phase_mask)
        throw std::invalid_argument("EventManager::ScheduleEventInRange(): phase_mask must be != 0");
    if (!time_max)
        time_max = time_min;
    if (!period_max)
        period_max = period_min;
    if (time_max < time_min)
        throw std::invalid_argument("EventManager::ScheduleEventInRange(): time_max must be >= time_min");
    if (period_max < period_min)
        throw std::invalid_argument("EventManager::ScheduleEventInRange(): period_max must be >= period_min");
    if (!period_min && period_max)
        throw std::invalid_argument("EventManager::ScheduleEventInRange(): period range must not include 0");
    uint32 time = time_min != time_max ? urand(time_min, time_max) : time_min;
    InsertEvent(time, Event(event_id, period_min, period_max, phase_mask, cd, cd_cat));
}

void EventManager::RescheduleEventInRange(uint32 event_id, uint32 time_min, uint32 time_max, uint32 period_min, uint32 period_max, uint32 cd, uint32 cd_cat, uint32 phase_mask)
{
    CancelEvent(event_id);
    ScheduleEventInRange(event_id, time_min, time_max, period_min, period_max, cd, cd_cat, phase_mask);
}

bool EventManager::HasEvent(uint32 time) const
{
    EventContainer::const_iterator i_ev = m_events.begin();
    if (i_ev == m_events.end())
        return false;
    return i_ev->first <= m_time + time;
}

uint32 EventManager::GetNextEvent() const
{
    EventContainer::const_iterator i_ev = m_events.begin();
    if (i_ev == m_events.end())
        return 0;
    return i_ev->second.id;
}

void EventManager::CancelAllEvents()
{
    m_events.clear();
}

void EventManager::CancelEvent(uint32 event_id, uint32 phase_mask)
{
    for (EventContainer::iterator i = m_events.begin(); i != m_events.end(); )
        if (i->second.id == event_id && i->second.phase_mask & phase_mask)
        {
            m_events.erase(i);
            i = m_events.begin();
        }
        else
            ++i;
}

void EventManager::CancelNextEvent()
{
    EventContainer::iterator i_ev = m_events.begin();
    if (i_ev == m_events.end())
        return;
    m_events.erase(i_ev);
}

void EventManager::CancelNextEventWithId(uint32 event_id, uint32 phase_mask)
{
    for (EventContainer::iterator i = m_events.begin(); i != m_events.end(); ++i)
        if (i->second.id == event_id && i->second.phase_mask & phase_mask)
        {
            m_events.erase(i);
            break;
        }
}

void EventManager::CancelEventsWithCategory(uint32 categ, uint32 phase_mask)
{
    for (EventContainer::iterator i = m_events.begin(); i != m_events.end(); )
        if (i->second.category == categ && i->second.phase_mask & phase_mask)
        {
            m_events.erase(i);
            i = m_events.begin();
        }
        else
            ++i;
}

void EventManager::DelayEvents(uint32 time, uint32 phase_mask)
{
    EventContainer delayed;
    for (EventContainer::iterator i = m_events.begin(); i != m_events.end(); )
        if (i->second.phase_mask & phase_mask)
        {
            delayed.insert(std::make_pair(i->first + time, i->second));
            m_events.erase(i);
            i = m_events.begin();
        }
        else
            ++i;
    m_events.insert(delayed.begin(), delayed.end());
}

void EventManager::DelayEventsWithId(uint32 event_id, uint32 time, uint32 phase_mask)
{
    EventContainer delayed;
    for (EventContainer::iterator i = m_events.begin(); i != m_events.end(); )
        if (i->second.id == event_id && i->second.phase_mask & phase_mask)
        {
            delayed.insert(std::make_pair(i->first + time, i->second));
            m_events.erase(i);
            i = m_events.begin();
        }
        else
            ++i;
    m_events.insert(delayed.begin(), delayed.end());
}

uint32 EventManager::ExecuteEvent()
{
    EventContainer::iterator i_ev = m_events.begin();
    // container empty, no events to process
    if (i_ev == m_events.end())
        return 0;
    // we got a event to process, executing
    if (i_ev->first <= m_time)
    {
        uint32 delta = m_time - i_ev->first;    // latency time in executing this event
        Event ev = i_ev->second;
        m_events.erase(i_ev);
        // if event is periodic, calculate next execution time
        uint32 period = ev.period_min != ev.period_max ? urand(ev.period_min, ev.period_max) : ev.period_min;
        if (period)
        {
            delta %= period; // in case we took way too long, get difference counting lost ticks
            InsertEvent(period - delta, ev);
        }
        // check if can be executed in current phase
        if (!(ev.phase_mask & GetPhaseMask()))
            return 0;
        // if we're on cooldown then cannot execute event
        CooldownMap::const_iterator i_cd = m_cooldowns.find(ev.category);
        if (i_cd != m_cooldowns.end() && i_cd->second.remaining)
        {
            // try again after cooldown ends, but don't wait longer than remaining period time
            if (!period || (i_cd->second.duration < period && i_cd->second.remaining < period - delta))
            {
                ev.period_min = ev.period_max = 0;
                InsertEvent(i_cd->second.remaining, ev);
            }
            return 0;
        }
        // all ok, now trigger cooldown if any
        SetCooldown(ev.cooldown, ev.category);
        // return succeful event executed
        return ev.id;
    }
    // all events in future, nothing to do for now
    return 0;
}
