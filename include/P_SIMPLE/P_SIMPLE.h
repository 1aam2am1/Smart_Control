#ifndef P_SIMPLE_H
#define P_SIMPLE_H

#include "Device.h"
#include <thread>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Mutex.hpp>
#include "P_SIMPLE/P_SIMPLE_Message.h"

#if defined(_WIN32)

#include "P_SIMPLE/Win32/P_COMImpl.h"

#else
#include "P_SIMPLE/Unix/P_COMImpl.h"
#endif

using namespace P_SIMPLE_Imp;

class P_SIMPLE : public Device {
public:
    explicit P_SIMPLE(bool usb = false);

    ~P_SIMPLE() override;

    bool connect(std::string) override;

    void close() override;

    std::map<int, int> getData() override;

    void toSendData(const std::map<int, int> &) override;

    CAL_STATE getCAL_STATE() override;

    void getCalendarDataSignal() override;

    uint8_t getCalendarActiveDays() override;

    std::map<int, std::vector<Action_data_struct>> getCalendarData() override;

    void sendCalendarData(const std::pair<uint8_t, day_to_actions> &) override;

    Modes_data_struct getModesData() override;

    void sendModesData(const Modes_data_struct &) override;

    Date_data_struct getDateData() override;

    void sendDateData(const Date_data_struct &) override;

private:
    bool usb;                               ///< Mode of open port
    P_COMImpl serial_port;                  ///< Port implementation
    std::thread wsk;                        ///< Thread of port
    void main();                            ///< Function stated by thread
    enum class ThreadState {
        Rest = 0,                           ///< The thread is not created
        Work = 1,                           ///< The thread is working signal before destoying it
        StopedWorking = 3                   ///< Signal of the thread that it stoped working
    } thread_work;                          ///< Is thread working

    std::map<int, int> data_received;       ///< Data received changed in the device
    std::map<int, int> data_to_send;        ///< Data to send to the device
    std::map<int, int> data_in_transfer;    ///< Data in transfer process to the device
    std::map<int, int> act_data;            ///< Actual data in display
    CAL_STATE stateOfCalendar;              ///< Can the calendar be activated, what timer?
    std::pair<uint8_t, day_to_actions> calendar_data; ///< Calendar data ///en,
    Modes_data_struct modes_data;           ///< Actual modes in device
    Date_data_struct date_data;             ///< Actual date in device

    bool needRequestCalendarData;           ///< We need request calendar data => actions
    bool needSendCalendarActiveDays;        ///< We need send active days in calendar
    uint8_t needSendCalendarActions;        ///< What days do we need send (actions in that day) ///CAL_QUER
    bool needSendModesData;                 ///< We need send modes to the device
    bool needSendDateData;                  ///< We need send date
    bool needReceiveDateData;               ///< We need request date from device

    int sendMessage(const std::shared_ptr<P_SIMPLE_Message> &);

    int sendMessage(const std::string &);

    enum class Receive_result {
        Serial_port_Error = -2,
        CRC_error = -1,
        No_Message = 0,
        Message = 1
    };

    Receive_result
    receiveMessage(std::shared_ptr<P_SIMPLE_Message> &, sf::Time); ///< Alloc and return message in pointer

    std::string unusedReceivedData;         ///< Received data to form messages

    std::vector<char> lastSendMessage;      ///< Message that was last send

    std::deque<std::pair<P_SIMPLE_Message::Ptr, Receive_result>> messages; ///< Received messages in queue to parse

    class MESSAGEMANAGEMENT {
    public:
        P_SIMPLE_Message::Ptr &alloc();     ///< Alloc new message
    private:
        std::vector<P_SIMPLE_Message::Ptr> created_messages;
    } message;
};

#endif // P_SIMPLE_H
