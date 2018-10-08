#ifndef RS232_H
#define RS232_H

#include "Device.h"
#include <windows.h>
#include <thread>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Mutex.hpp>

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
    sf::Mutex mutex;
    std::thread thr;
    HANDLE hCom;

    std::string temponary_data;
    std::map<int, int> parsed_data;
    std::string send_data;

    sf::Clock recive_clock;

    void main();

    uint8_t thread_work = 0;

    void haveData();

    bool write(std::string);
};

#endif // RS232_H
