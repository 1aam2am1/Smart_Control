//
// Created by Michal_Marszalek on 01.04.2019.
//

#ifndef SMART_CONTROL_LOCK_H
#define SMART_CONTROL_LOCK_H

#include "Event_Manager/Help/Lock_device.h"
#include <cstdint>

namespace Event_Manager_Imp {

    class Lock {
    public:
        Lock(Lock_device *lockDevice, uint32_t id, bool connected);

        virtual ~Lock();

        bool connected();

    private:
        Lock_device *lockDevice;
        uint32_t ID;
        bool result;
    };

}

#endif //SMART_CONTROL_LOCK_H
