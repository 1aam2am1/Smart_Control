//
// Created by Michal_Marszalek on 01.04.2019.
//

#ifndef SMART_CONTROL_EVENT_MANAGER_H
#define SMART_CONTROL_EVENT_MANAGER_H

#include "Event_Manager/Help/Lock.h"
#include "Event_Manager/Help/Lock_device.cpp"
#include "Event_Manager/Widgets/Widget_Switch.h"
#include <memory>

class Event_Manager : Event_Manager_Imp::Lock_device {
public:
    typedef std::shared_ptr<Event_Manager_Imp::Lock> Auto_disconnect;

    Event_Manager();

    Auto_disconnect connect(std::string ID, std::shared_ptr<Widget_Switch>);     ///< Connect widget

    Auto_disconnect register_device();      ///< Connect device

    void set_global_config();               ///< Set some global config: names, min, max, default ...

    Auto_disconnect set_local_config();


private:
    int32_t get_uniqueID();

    uint32_t max_id = 0;

    void disconnect(uint32_t id) override;  ///< Disconnect widget/device/config/?


};


#endif //SMART_CONTROL_EVENT_MANAGER_H
