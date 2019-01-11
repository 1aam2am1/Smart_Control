#ifndef RS232_H
#define RS232_H

#include "Device.h"

#if defined(_WIN32)
#include <windows.h>
#include <thread>
#include <SFML/System/Clock.hpp>

#endif

class rs232 : public Device {
public:
    rs232();

    virtual ~rs232();

    virtual bool connect(std::string) override;

    virtual void close() override;

    virtual std::map<int, int> getData() override;

    virtual void toSendData(const std::map<int, int> &) override;

    virtual CAL_STATE getCAL_STATE() { return {}; };

    virtual void getCalendarDataSignal() {};

    virtual uint8_t getCalendarActiveDays() { return 0; };

    virtual std::map<int, std::vector<Action_data_struct>> getCalendarData() { return {}; };

    virtual void sendCalendarData(const std::pair<uint8_t, std::map<int, std::vector<Action_data_struct>>> &) {};

    virtual Modes_data_struct getModesData() { return {}; };

    virtual void sendModesData(const Modes_data_struct &) {};

    virtual Date_data_struct getDateData() { return {}; };

    virtual void sendDateData(const Date_data_struct &) {};

private:
#if defined(_WIN32)
    std::thread thr;
    HANDLE hCom;

    std::string temporary_data;
    std::map<int, int> parsed_data;

    sf::Clock receive_clock;

    void main();

    uint8_t thread_work = 0;

    void haveData();

    bool write(std::string);

#endif
};

#endif // RS232_H
