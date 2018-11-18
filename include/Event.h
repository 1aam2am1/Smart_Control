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
        DisConnected = 4,
        Data = 5,
        CalendarData = 6,
        ModesData = 7,
        TimeData = 8,
        Reset = 100
    };

    EventType type;
};

#endif // EVENT_H
