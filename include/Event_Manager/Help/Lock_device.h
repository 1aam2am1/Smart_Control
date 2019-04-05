//
// Created by Michal_Marszalek on 01.04.2019.
//

#ifndef SMART_CONTROL_LOCK_DEVICE_H
#define SMART_CONTROL_LOCK_DEVICE_H

#include <cstdint>

namespace Event_Manager_Imp {

    class Lock_device {
    public:
        virtual ~Lock_device();

        virtual void disconnect(uint32_t id) = 0;
    };

}

#endif //SMART_CONTROL_LOCK_DEVICE_H
