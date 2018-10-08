#ifndef EVENT_H
#define EVENT_H

#include <string>

class Event {
public:


    enum EventType {
        Create = 0,
        Open = 1,
        Close = 2,
        Connected = 3,
        Data = 4,
        CalendarData = 5,
        ModesData = 6,
        TimeData = 7,
        Reset = 100
    };

    EventType type;
};

#endif // EVENT_H
