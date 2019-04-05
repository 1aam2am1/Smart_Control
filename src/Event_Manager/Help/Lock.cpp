//
// Created by Michal_Marszalek on 01.04.2019.
//

#include "include/Event_Manager/Help/Lock.h"

Event_Manager_Imp::Lock::Lock(Event_Manager_Imp::Lock_device *lockDevice, uint32_t id, bool c) : lockDevice(lockDevice),
                                                                                                 ID(id), result(c) {}

Event_Manager_Imp::Lock::~Lock() {
    this->lockDevice->disconnect(ID);
}

bool Event_Manager_Imp::Lock::connected() {
    return this->result;
}

