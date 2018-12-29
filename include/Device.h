#ifndef DEVICE_H
#define DEVICE_H

#include "Event.h"
#include <SFML/System/Mutex.hpp>
#include <map>
#include <vector>
#include <queue>
#include <Action_data_struct.h>
#include <CAL_STATE.h>
#include <Modes_data_struct.h>
#include <Date_data_struct.h>


class Device {
public:
    Device();

    virtual ~Device();

    bool pollEvent(Event &);

    virtual bool connect(std::string) = 0;

    virtual void close() = 0;

    virtual std::map<int, int> getData() = 0;

    virtual void toSendData(const std::map<int, int> &) = 0;

    virtual CAL_STATE getCAL_STATE() = 0;

    virtual void getCalendarDataSignal() = 0;

    virtual uint8_t getCalendarActiveDays() = 0;

    virtual std::map<int, std::vector<Action_data_struct>> getCalendarData() = 0;

    virtual void sendCalendarData(const std::pair<uint8_t, std::map<int, std::vector<Action_data_struct>>> &) = 0;

    virtual Modes_data_struct getModesData() = 0;

    virtual void sendModesData(const Modes_data_struct &) = 0;

    virtual Date_data_struct getDateData() = 0;

    virtual void sendDateData(const Date_data_struct &) = 0;

protected:
    sf::Mutex mutex;
    std::queue<Event> event;
};

#endif // DEVICE_H
