#ifndef P_SIMPLE_H
#define P_SIMPLE_H

#include "Device.h"
#include <thread>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Mutex.hpp>

#if defined(_WIN32)

#include "P_SIMPLE/Win32/P_COMImpl.h"

#else
#include "P_SIMPLE/Unix/P_COMImpl.h"
#endif

class P_SIMPLE : public Device {
public:
    P_SIMPLE(bool usb = false);

    virtual ~P_SIMPLE();

    virtual bool connect(std::string) override;

    virtual void close() override;

    virtual std::map<int, int> getData() override;

    virtual void toSendData(const std::map<int, int> &) override;

    virtual CAL_STATE getCAL_STATE() override;

    virtual void getCalendarDataSignal() override;

    virtual uint8_t getCalendarActiveDays() override;

    virtual std::map<int, std::vector<Action_data_struct>> getCalendarData() override;

    virtual void sendCalendarData(const std::pair<uint8_t, std::map<int, std::vector<Action_data_struct>>> &) override;

    virtual Modes_data_struct getModesData() override;

    virtual void sendModesData(const Modes_data_struct &) override;

    virtual Date_data_struct getDateData() override;

    virtual void sendDateData(const Date_data_struct &) override;

private:
    std::thread wsk;
    P_COMImpl serial_port;

    std::map<int, int> parsed_data;         ///dane odebrane
    std::map<int, int> send_data;           ///dane do wyslania
    std::map<int, int> processed_data;      ///dane aktualnie w procesie wysylania
    std::map<int, int> act_data;            ///dane aktualnie na paskach

    CAL_STATE stateOfCalendar;              ///can the calendar be activated
    bool needCalendarData;                  ///do we need new calendar data
    bool needSendCalendarActive;            ///do we need send if the calendar is active
    uint8_t needSendCalendarDay;            ///what day do we need send ///CAL_QUER
    std::pair<uint8_t, std::map<int, std::vector<Action_data_struct>>> calendar_data; ///calendar data ///en, [0-pn]={action},{action},{action}...

    bool needSendModesData;
    bool needSendDateData;
    bool needReceiveDateData;
    Modes_data_struct modes_data;
    Date_data_struct date_data;

    void main();

    uint8_t thread_work = 0;

    int writeCom(std::vector<char>);

    int writeCom(const std::string &);

    int receive(std::string &, std::vector<uint8_t> &, sf::Time);

    std::vector<char> last_message;

    bool usb;
};

#endif // P_SIMPLE_H
