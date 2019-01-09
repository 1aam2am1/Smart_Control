//
// Created by Michal_Marszalek on 09.01.2019.
//

#ifndef SMART_CONTROL_DEVICEEVENT_H
#define SMART_CONTROL_DEVICEEVENT_H

#include <string>

class DeviceEvent {
public:

    enum EventType {
        Create = 0,
        Open = 1,
        Close = 2,
        Connected = 3,
        Data = 5,
    };

    EventType type;

    std::string data;
};


#endif //SMART_CONTROL_DEVICEEVENT_H
