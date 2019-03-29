#include "P_SIMPLE/P_SIMPLE.h"
#include <functional>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Lock.hpp>
#include <Funkcje_str.h>
#include "crc.h"
#include <vector>
#include "Console.h"
#include <cstring>


P_SIMPLE::P_SIMPLE(bool u)
        : needRequestCalendarData(true),
          needSendCalendarActiveDays(false),
          needSendCalendarActions(0),
          needSendModesData(false),
          needSendDateData(false),
          needReceiveDateData(false),
          usb(u),
          thread_work(ThreadState::Rest) {

    event.push(Event::Create);
}

P_SIMPLE::~P_SIMPLE() {
    close();
}

bool P_SIMPLE::connect(std::string port) {
    sf::Lock lock(mutex);

    this->close();
    event.back().type = Event::DisConnected;

    data_received.clear();
    data_to_send.clear();
    data_in_transfer.clear();
    act_data.clear();

    if (!serial_port.connect(port, usb ? 115200 : 9600)) { return false; }

    thread_work = ThreadState::Work;
    wsk = std::thread(std::bind(&P_SIMPLE::main, this));

    Console::printf(Console::MESSAGE, "Polaczono z: %s\n", port.c_str());

    event.push(Event::Open);

    return true; //koniec ustawienia portu
}

void P_SIMPLE::close() /// =>w watku jest to samo :)
{
    mutex.lock();

    if (thread_work == ThreadState::Work) {
        thread_work = ThreadState::Rest;

        serial_port.close();

        for (int32_t i = 0; i < 10 && thread_work != ThreadState::StopedWorking; ++i) ///koniec watku
        {
            mutex.unlock();

            sf::sleep(sf::milliseconds(20));

            mutex.lock();
        }
        thread_work = ThreadState::Rest;
    }

    if (wsk.joinable()) { wsk.detach(); }
    wsk = std::thread();

    event.push(Event::Close);

    mutex.unlock();

    Console::printf(Console::MESSAGE, "Port zostal rozlaczony\n");
}

std::map<int, int> P_SIMPLE::getData() {
    sf::Lock lock(mutex);

    auto result = std::move(data_received);
    data_received.clear();
    return result;
}

void P_SIMPLE::toSendData(const std::map<int, int> &new_data) {
    sf::Lock lock(mutex);

    auto block = Console::Printf_block::beginWrite();
    block.printf(Console::FUNCTION_LOG, "P_SIMPLE::toSendData:\n");

    for (const auto &new_data_it : new_data) {
        auto it_act = act_data.find(new_data_it.first);
        if (it_act != act_data.end())///jezeli id do wyslania jest w aktualnych danych
        {
            if (it_act->second != new_data_it.second)///jezeli nowe sa rozne od aktualnych
            {
                block.printf(Console::DATA_FUNCTION_LOG, "%i -> %i %i\n", new_data_it.first, it_act->second,
                             new_data_it.second);
                data_to_send[new_data_it.first] = new_data_it.second;
            } else {
                auto it_send = data_to_send.find(new_data_it.first);
                if (it_send !=
                    data_to_send.end())///jezeli aktualne i nowe sa takie same sprawc czy do wyslania nie sa pomylone :)
                {
                    data_to_send[new_data_it.first] = new_data_it.second;
                }
            }
        } else {
            block.printf(Console::DATA_FUNCTION_LOG, "%i -> %i\n", new_data_it.first, new_data_it.second);
            data_to_send[new_data_it.first] = new_data_it.second;
        }
    }
    block.endWrite();
}

CAL_STATE P_SIMPLE::getCAL_STATE() {
    sf::Lock lock(this->mutex);
    return this->stateOfCalendar;
}

void P_SIMPLE::getCalendarDataSignal() {
    sf::Lock lock(this->mutex);
    this->needRequestCalendarData = true;
}

uint8_t P_SIMPLE::getCalendarActiveDays() {
    sf::Lock lock(mutex);
    return this->calendar_data.first;
}

std::map<int, std::vector<Action_data_struct>> P_SIMPLE::getCalendarData() {
    sf::Lock lock(mutex);
    return this->calendar_data.second;
}

void P_SIMPLE::sendCalendarData(const std::pair<uint8_t, day_to_actions> &data) {
    sf::Lock lock(mutex);
    if (this->calendar_data.first != data.first) ///active day
    {
        this->calendar_data.first = data.first;
        this->needSendCalendarActiveDays = true;
    }

    for (auto &&it : data.second) {
        if (it.second != this->calendar_data.second[it.first]) {
            this->calendar_data.second[it.first] = it.second;
            this->needSendCalendarActions |= 1 << (it.first + 1);
        }
    }

}

Modes_data_struct P_SIMPLE::getModesData() {
    sf::Lock lock(mutex);
    return this->modes_data;
}

void P_SIMPLE::sendModesData(const Modes_data_struct &d) {
    sf::Lock lock(mutex);
    this->modes_data = d;
    needSendModesData = true;
}

Date_data_struct P_SIMPLE::getDateData() {
    sf::Lock lock(mutex);
    return this->date_data;
}

void P_SIMPLE::sendDateData(const Date_data_struct &d) {
    sf::Lock lock(mutex);
    Console::printf(Console::FUNCTION_LOG, "P_SIMPLE::sendDateData:\n");
    this->date_data = d;
    needSendDateData = true;
}

int P_SIMPLE::sendMessage(const std::string &data) {
    lastSendMessage.resize(data.size());
    {
        uint32_t j = 0;
        for (char i : data) {
            if (i != ' ') {
                lastSendMessage[j] = i;
                ++j;
            }
        }
        lastSendMessage.resize(j);
    }
    if (lastSendMessage.empty() || lastSendMessage.back() != '\r') { lastSendMessage.push_back('\r'); }

    sf::Lock lock(mutex);
    return serial_port.writeCom(lastSendMessage);
}

int P_SIMPLE::sendMessage(const std::shared_ptr<P_SIMPLE_Message> &data) {
    data->parse_protocol_message(lastSendMessage);

    sf::Lock lock(mutex);
    return serial_port.writeCom(lastSendMessage);
}

void P_SIMPLE::main() {
    sf::Clock clock;
    sf::Clock second120clock;
    sf::Clock needReceiveDateDataClock;
    std::shared_ptr<P_SIMPLE_Message> result;

    bool boot = true;
    bool second120 = false;
    bool needRequestStateDeviceData = true;
    this->needRequestCalendarData = true;


    while (thread_work == ThreadState::Work) {
        auto re = receiveMessage(result, sf::milliseconds(20));
        auto messageBlock = Console::Printf_block::beginWrite();

        if (thread_work != ThreadState::Work) { break; }

        switch (re) {
            case Receive_result::Serial_port_Error: ///blad hCom
            { ///close :)
                messageBlock.printf(Console::ERROR_MESSAGE, "Blad portu\n");

                sf::Lock lock(mutex);

                thread_work = ThreadState::Rest;
                if (wsk.joinable()) { wsk.detach(); }
                wsk = std::thread();

                serial_port.close();

                event.push(Event::Close);
                return;
            }
            case Receive_result::CRC_error: ///blad crc
                messageBlock.printf(Console::ERROR_MESSAGE, "Blad crc\n");

                clock.restart();

                if (result->address == 31) ///pytanie do mnie?
                {
                    sendMessage(
                            {"66 00 03 10 01 56"});///66 rozpoczecie 00 adres 03 dlugosc 10 podstawowe potwierdzenie 01 ze blad 56 crc
                }
                break;
            case Receive_result::No_Message: ///brak zadnej wiadmosci
                if ((second120clock.getElapsedTime() > sf::seconds(130)) && !boot) {
                    clock.restart();
                    second120clock.restart();
                    needReceiveDateDataClock.restart();
                    boot = true;
                    second120 = false;
                    needRequestStateDeviceData = true;
                    this->needRequestCalendarData = true;

                    event.push(Event::DisConnected);
                }
                if (second120clock.getElapsedTime() < sf::seconds(120) && boot && second120) {
                    clock.restart();
                }
                if ((clock.getElapsedTime() > sf::milliseconds(120)) && boot) {
                    if (!usb) { sendMessage({"66 00 02 01 cf"}); }
                    second120 = true;
                    second120clock.restart();
                    //boot = false;
                }
                if (needReceiveDateDataClock.getElapsedTime() > sf::seconds(60)) {
                    needReceiveDateData = true;
                }
                break;
            case Receive_result::Message:///wiadmosc jest poprawna
                messageBlock.printf(Console::DATA_FUNCTION_LOG, "Przetwarzanie wiadmosci\n");

                clock.restart();

                messageBlock.printf(Console::DATA_FUNCTION_LOG, "Adres: %02x\n", result->address);

                if (result->address == 31 || result->address == 255) ///pytanie do mnie i adres rozgloszeniowy
                {
                    messageBlock.printf(Console::DATA_FUNCTION_LOG, "Wiadmosc do mnie\n");
                    sf::Lock lock(mutex);

                    if (result->address == 31) {
                        boot = false;
                        second120 = false;
                        second120clock.restart();

                        event.push(Event::Connected);
                    }

                    if (!data_in_transfer.empty() && (result->command != 0x10)) ///naprawiam dane nie wyslane
                    {
                        std::swap(data_to_send, data_in_transfer);
                        toSendData(data_in_transfer);
                        data_in_transfer.clear();
                    }

                    if (result->command == 0x00)///reset
                    {
                        messageBlock.printf(Console::DATA_FUNCTION_LOG, "Reset\n");

                        event.push(Event::Reset);
                    } else if (result->command == 0x01)///searching devices
                    {
                        messageBlock.printf(Console::DATA_FUNCTION_LOG, "Wyszukiwanie urzadzen\n");

                        second120clock.restart();
                    } else if (result->command == 0x10)///odpowiedz
                    {
                        if (result->address == 255) { break; }

                        if (result->data[0] == 0x01) ///blad komunikacji crc
                        {
                            messageBlock.printf(Console::DATA_FUNCTION_LOG, "Ponowne wysylanie wiadmosci\n");

                            serial_port.writeCom(lastSendMessage);
                        } else if (result->data[0] == 0x00) {
                            messageBlock.printf(Console::DATA_FUNCTION_LOG, "Potwierdzenie otrzymania danych\n");

                            if (!data_in_transfer.empty()) {
                                for (auto &it : data_in_transfer)///dodawanie danych
                                {
                                    act_data[it.first] = it.second;
                                }
                                data_in_transfer.clear();
                            }
                        }
                        ///printf("Komunikacja jest bledna\nOdpowiedz mimo braku zapytania\n");
                    } else if (result->command == 0x20)///podtrzymanie komunikacji
                    {
                        if ((result->data[0] & 7) ==
                            6) { needRequestStateDeviceData = true; }//stan sterownika zostal zmieniony
                        if ((result->data[0] & 7) == 7) {
                            needRequestCalendarData = true;
                            needRequestStateDeviceData = true;
                        }//v5 rzadanie danych gdyz sie nieaktualizuje
                        if (result->address == 255) { break; }//brodcast nie potrzebny dalej

                        bool calendar = (needRequestCalendarData | needSendCalendarActiveDays |
                                         needSendCalendarActions |
                                         needSendModesData | needSendDateData | needReceiveDateData);
                        messageBlock
                                .printf(Console::DATA_FUNCTION_LOG, "Rzadanie danych: %s\n",
                                        needRequestStateDeviceData ? "true" : "false")
                                .printf(Console::DATA_FUNCTION_LOG, "Rzadanie kalendarza: %s\n",
                                        calendar ? "true" : "false")
                                .printf(Console::DATA_FUNCTION_LOG, "needRequestCalendarData: %s\n",
                                        needRequestCalendarData ? "true" : "false")
                                .printf(Console::DATA_FUNCTION_LOG, "needSendCalendarActiveDays: %s\n",
                                        needSendCalendarActiveDays ? "true" : "false")
                                .printf(Console::DATA_FUNCTION_LOG, "needSendCalendarActions: %s\n",
                                        needSendCalendarActions ? "true" : "false")
                                .printf(Console::DATA_FUNCTION_LOG, "needSendModesData: %s\n",
                                        needSendModesData ? "true" : "false")
                                .printf(Console::DATA_FUNCTION_LOG, "needSendDateData: %s\n",
                                        needSendDateData ? "true" : "false")
                                .printf(Console::DATA_FUNCTION_LOG, "needReceiveDateData: %s\n",
                                        needReceiveDateData ? "true" : "false")
                                .printf(Console::DATA_FUNCTION_LOG, "needSendData: %s\n",
                                        !data_to_send.empty() ? "true" : "false");

                        auto info = message.alloc();
                        info->start_bit = 0x66;
                        info->address = 0x00;

                        info->ret_data[0] = static_cast<uint8_t>((calendar ? 0x20 : 0) +
                                                                 (needRequestStateDeviceData ? 0x10 : 0) + 0x01);

                        uint32_t j = 1;
                        if (!data_to_send.empty() && (usb | (!needRequestStateDeviceData)))///mam dane do wyslania
                        {
                            messageBlock.printf(Console::DATA_FUNCTION_LOG, "Mam dane do wyslania\n");

                            data_in_transfer = std::move(data_to_send);
                            data_to_send.clear();

                            for (auto &it : data_in_transfer)///dodawanie danych
                            {
                                info->ret_data[j++] = static_cast<uint8_t>(it.first);
                                info->ret_data[j++] = static_cast<uint8_t>(it.second);
                            }
                        }

                        info->ret_data[j++] = 0x00;///errors
                        info->length = static_cast<uint8_t>(j + 1);

                        sendMessage(info);
                    } else if (result->command == 0x30)///kalendarz
                    {
                        messageBlock.printf(Console::DATA_FUNCTION_LOG, "Kalendarz podstawowa odpowiedz\n");
/*
                    if(!(result[5] & 1)) ///timer can't be start
                    {
                        needRequestCalendarData = false;
                        needSendCalendarActions = false;
                        calendar_data.first = 0;
                        calendar_data.second.clear();

                        Event e;
                        e.type = Event::CalendarData;
                        event.push(e);
                    }*/
                        this->stateOfCalendar.data = result->data[1];

                        auto info = message.alloc();
                        info->start_bit = 0x66;
                        info->address = 0x00;

                        if (!needSendCalendarActiveDays) {
                            this->calendar_data.first = result->data[0];

                            event.push(Event::CalendarData);
                        }

                        info->ret_data[0] = this->calendar_data.first;///en

                        if (needRequestCalendarData) {
                            info->ret_data[1] = 0xFF; ///what days
                            needRequestCalendarData = false;
                        } else if (needSendCalendarActions) {
                            info->ret_data[1] = needSendCalendarActions; ///what days
                            needSendCalendarActions = 0;
                        } else {
                            info->ret_data[1] = 0x01; ///what days
                        }

                        if (!needSendModesData) {
                            memcpy(modes_data.data, &result->data[8], 5);

                            event.push(Event::ModesData);
                        }

                        {
                            memcpy(&info->ret_data[2], modes_data.data, 5);

                            needSendModesData = false;
                        }

                        if (needSendDateData) {
                            //memcpy(info.data(), calendar_data.second[result[4]].data(), calendar_data.second[result[4]].size()*6);

                            memcpy(&info->ret_data[7], date_data.data, 6);


                            needSendDateData = false;
                            needReceiveDateData = false;
                            needReceiveDateDataClock.restart();
                        } else {
                            memcpy(date_data.data, &result->data[2], 6);

                            event.push(Event::TimeData);

                            needReceiveDateData = false;
                            needReceiveDateDataClock.restart();
                        }

                        sendMessage(info);

                        needSendCalendarActiveDays = false;
                    } else if (result->command == 0x31)///save calendar data
                    {
                        messageBlock.printf(Console::DATA_FUNCTION_LOG, "Zapisz dni kalendarza\n");

                        auto how_much = static_cast<uint32_t>((result->length - 3) / 6);
                        if ((result->length - 3) % 6) {
                            messageBlock.printf(Console::ERROR_MESSAGE, "Error save calendar data\n");
                        }

                        calendar_data.second[result->data[0]].clear();
                        calendar_data.second[result->data[0]].resize(how_much >= 0 ? how_much : 0);
                        for (uint32_t i = 0; i < how_much; ++i) {
                            Action_data_struct data{};
                            memcpy(data.data, &result->data[1 + i * 6], 6);///kopiowanie danych do miejsca +1
                            calendar_data.second[result->data[0]][i] = data;
                        }
                        //calendar_data.second[result[4]].resize(how_much);

                        //memcpy(calendar_data.second[result[4]].data(), &result[5], 6*how_much);

                        event.push(Event::CalendarData);

                        sendMessage(
                                {"66 00 03 10 00 08\r"}); ///66 rozpoczecie 00 adres 03 dlugosc 10 funkcja 00 ans 0C crc
                    } else if (result->command == 0x32)///send calendar data
                    {
                        messageBlock.printf(Console::DATA_FUNCTION_LOG, "Wyslij dzen: %i\n", result->data[0]);

                        auto info = message.alloc();
                        info->start_bit = 0x66;
                        info->address = 0x00;

                        //memcpy(info.data(), calendar_data.second[result[4]].data(), calendar_data.second[result[4]].size()*6);

                        for (uint32_t i = 0; i < calendar_data.second[result->data[0]].size(); ++i) {
                            memcpy(&info->ret_data[i * 6], calendar_data.second[result->data[0]][i].data, 6);
                        }

                        info->length = static_cast<uint8_t>(calendar_data.second[result->data[0]].size() * 6 + 1);
                        sendMessage(info);
                    } else if (result->command == 0x40)///prosba o parametry (wszystkie)
                    {
                        messageBlock.printf(Console::DATA_FUNCTION_LOG, "Prosba o parametry\n");

                        if (result->address == 255) {
                            messageBlock.printf(Console::ERROR_MESSAGE, "Error brodcast prosi o parametry\n");
                            break;
                        }
                        auto info = message.alloc();
                        info->start_bit = 0x66;
                        info->address = 0x00;

                        ///processed_data = std::move(send_data);
                        ///send_data.clear();

                        for (auto &it : data_to_send) {
                            act_data[it.first] = it.second;
                        }

                        data_in_transfer = act_data;
                        data_to_send.clear();

                        {
                            uint32_t j = 0;
                            for (auto &it : data_in_transfer)///dodawanie danych
                            {
                                info->ret_data[j++] = static_cast<uint8_t>(it.first);
                                info->ret_data[j++] = static_cast<uint8_t>(it.second);
                            }
                            data_in_transfer.clear(); ///!!! ja nie pragne potwierdzenia
                        }

                        info->length = static_cast<uint8_t>(2 * data_in_transfer.size() + 1);
                        sendMessage(info);

                    } else if (result->command == 0x50)///zapisz dane
                    {
                        messageBlock.printf(Console::DATA_FUNCTION_LOG, "Zapisz dane\n");

                        needRequestStateDeviceData = false;

                        std::map<int, uint32_t> tym;
                        for (uint32_t i = 0; i < (result->length - 2); i += 2) {
                            uint8_t id = result->data[i];
                            uint8_t value = result->data[i + 1];
                            tym[id] = (tym[id] << 8) | value;
                        }

                        for (auto &it : tym) {
                            data_received[it.first] = it.second;///dane ktore odbiera gui
                            data_to_send.erase(it.first);///usuwam z danych do wyslania
                            act_data[it.first] = it.second;///dodaje do danych aktualnych
                            messageBlock.printf(Console::DATA_FUNCTION_LOG, "%i -> %i\n", it.first, it.second);
                        }

                        event.push(Event::Data);

                        sendMessage(
                                {"66 00 03 10 00 08\r"}); ///66 rozpoczecie 00 adres 03 dlugosc 10 funkcja 00 ans 0C crc
                    } else {
                        messageBlock.printf(Console::DATA_FUNCTION_LOG, "Funkcja %i brak implementacji\n",
                                            result->command);
                    }
                } else ///nie moj watek
                {
                    sf::Lock lock(mutex);
                    if (!data_in_transfer.empty())///naprawiam dane nie wyslane
                    {
                        std::swap(data_to_send, data_in_transfer);
                        toSendData(data_in_transfer);
                        data_in_transfer.clear();
                    }
                }
                break;
            default:
                messageBlock.printf(Console::ERROR_MESSAGE, "Cos poszlo nie tak %s %i\n", __FILE__, __LINE__);
        }

        messageBlock.endWrite();
    }

    thread_work = ThreadState::StopedWorking; ///zaznaczanie konca watku
}

P_SIMPLE::Receive_result P_SIMPLE::receiveMessage(std::shared_ptr<P_SIMPLE_Message> &result, sf::Time time) {
    static auto ret_func = [&]() -> Receive_result {
        if (messages.empty()) { return Receive_result::No_Message; }
        for (auto it = messages.begin(); it != messages.end(); ++it) {
            if (it->first->address == 0xff) {
                result = it->first;
                auto ret_val = it->second;
                messages.erase(messages.begin(), it);///delete all messages a0...ff
                return ret_val;
            }
            if (it->first->address == 0x1f && messages.back().first.get() == it->first.get()) {
                result = it->first;
                auto ret_val = it->second;
                messages.clear();///delete all messages a0...ff
                return ret_val;
            }
            if (messages.back().first.get() == it->first.get()) {
                result = it->first;
                auto ret_val = it->second;
                messages.clear();///delete all messages a0...ff
                return ret_val;
            }
        }

        ///??????????????????????///=>end of function clion error
        result = messages.back().first;
        auto ret_val = messages.back().second;
        messages.clear();///delete all messages a0...ff
        return ret_val;
    };

    {
        int x;
        if ((x = serial_port.receive(unusedReceivedData, time)) < 0) { return Receive_result::Serial_port_Error; }
        if (x == 0) { return ret_func(); }
    }

    Console::printf(Console::LOG, "PARSE()\n");
    while (true) {
        std::size_t found = unusedReceivedData.find('\r'); ///ostatni znak ramki
        //r_char = found + 1;
        if (found == std::string::npos) {
            break;
        }

        std::string str = unusedReceivedData.substr(0, found + 1); ///kopiowanie z \r
        unusedReceivedData.erase(0, found + 1);

        ///szukaj 55 od konca dopuki validate nie zwruci true lub po false found2 wynosi 0
        std::size_t found2 = 0;
        while (true) {
            {///pierwszy znak ramki
                std::size_t found55 = str.find("55", found2);
                std::size_t found66 = str.find("66", found2);
                //found2 = ( (found55 == std::string::npos) ? found66 : ((found55 > found66) ? found55 : found66) ); ///wylicz najdalszy poczatek ramki i nie npos jeżeli mozna
                if (found55 == std::string::npos) {
                    found2 = found66;
                } else if (found66 == std::string::npos) {
                    found2 = found55;
                } else {
                    found2 = ((found55 < found66) ? found55 : found66);
                }
            }

            if (found2 == std::string::npos) { break; }
            //if (found2 == 0) { break; }

            std::string match_str;
            match_str.insert(0, str, found2, std::string::npos);

            found2 += 2;
            Console::printf(Console::DATA_FUNCTION_LOG, "Parsowanie: %s\n", match_str.c_str());
            auto m = message.alloc();
            int x;
            if ((x = P_SIMPLE_Imp::P_SIMPLE_Message::create(match_str, m)) != 0) {
                messages.emplace_back(m, x == 1 ? Receive_result::Message : Receive_result::CRC_error);
            }

        }
    }

    return ret_func();
}

P_SIMPLE_Imp::P_SIMPLE_Message::Ptr &P_SIMPLE::MESSAGEMANAGEMENT::alloc() {
    for (auto &wsk : created_messages) {
        if (wsk.use_count() == 1) { return wsk; }
    }

    return *created_messages.emplace(created_messages.end(), std::make_shared<P_SIMPLE_Imp::P_SIMPLE_Message>());
}