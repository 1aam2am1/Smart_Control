//
// Created by Michal_Marszalek on 31.12.2018.
//

#ifndef SMART_CONTROL_P_SIMPLE_DEVICE_H
#define SMART_CONTROL_P_SIMPLE_DEVICE_H
#include "P_SIMPLE/Win32/P_COMImpl.h"
#include "DeviceEvent.h"
#include <SFML/System/Mutex.hpp>
#include <thread>
#include <queue>

class P_SIMPLE_DEVICE {
public:
    P_SIMPLE_DEVICE();

    virtual ~P_SIMPLE_DEVICE();

    /// Connect to the given port
    /// \param port
    /// \return true if connected
    bool connect(std::string port);

    /// Close port
    void close();

    /// Run the normal functioning of the device implementation Request=>answer=>responses
    void start_normal_operation();

    /// Stop the normal operation mode the answers and request only available by events
    void stop_normal_operation();

    /// Get event
    /// \param event Return event
    /// \return true if event
    bool pollEvent(DeviceEvent & event);

    /// Write data to the port (data is formatted beforehand spaces removed /r added)
    /// \param str Message to send
    /// \return Number of letters sent
    int writeCom(const std::string & str);



private:
    sf::Mutex mutex;
    std::queue<DeviceEvent> event;

    std::thread wsk;
    P_COMImpl serial_port;
};


#endif //SMART_CONTROL_P_SIMPLE_DEVICE_H
