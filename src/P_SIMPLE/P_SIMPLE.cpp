#include "P_SIMPLE/P_SIMPLE.h"
#include <functional>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Lock.hpp>
#include <Funkcje_str.h>
#include "crc.h"
#include <vector>
#include "Console.h"
#include "File_Communication_log.h"

static File_Communication_log communication_log;

P_SIMPLE::P_SIMPLE(bool usb)
        : hCom(INVALID_HANDLE_VALUE),
          needCalendarData(true),
          needSendCalendarActive(false),
          needSendCalendarDay(0),
          needSendModesData(false),
          needSendDateData(false),
          needReceiveDateData(false) {
    Event e;
    e.type = Event::Create;
    event.push(e);
}

P_SIMPLE::~P_SIMPLE() {
    close();
}

bool P_SIMPLE::connect(std::string port) {
    sf::Lock lock(mutex);

    parsed_data.clear();
    send_data.clear();
    processed_data.clear();
    act_data.clear();


    bool fSuccess_COM;
    DCB dcb;        //konfiguracja portu
    dcb.DCBlength = sizeof(DCB);

    if (this->hCom != INVALID_HANDLE_VALUE) { CloseHandle(this->hCom); }

    hCom = CreateFile(("\\\\.\\" + port).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL, NULL); //otwieranie portu

    if (hCom == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "CreateFile failed with error %lu.\n", GetLastError());
        return false;
    }

    //pobranie aktualnych ustawien portu
    fSuccess_COM = GetCommState(hCom, &dcb);
    if (!fSuccess_COM) {
        fprintf(stderr, "GetCommState failed with error %lu.\n", GetLastError());
        return false;
    }

    //ustawienie naszej konfiguracji
    if (!usb) {
        dcb.BaudRate = CBR_9600;     // predkosc transmisji
    } else {
        dcb.BaudRate = CBR_115200;     // predkosc transmisji
    }

    dcb.ByteSize = 8;             // ilosc bitow danych
    dcb.Parity = NOPARITY;        // brak bitu parzystosci
    dcb.StopBits = ONESTOPBIT;    // jeden bity stopu
    dcb.fDtrControl = DTR_CONTROL_DISABLE; //Kontrola linii DTR: DTR_CONTROL_DISABLE (nieaktywny), DTR_CONTROL_ENABLE (aktywny)
    dcb.fRtsControl = RTS_CONTROL_DISABLE; //Kontrola linii RTS: RTS_CONTROL_DISABLE (nieaktywny), RTS_CONTROL_ENABLE (aktywny)


    fSuccess_COM = SetCommState(hCom, &dcb);
    if (!fSuccess_COM) {
        fprintf(stderr, "SetCommState failed with error %lu.\n", GetLastError());
        return false;
    }

    if (thread_work == 1) {
        thread_work = 0;
        for (int32_t i = 0; i < 10 && thread_work != 3; ++i) ///koniec watku
        {
            mutex.unlock();

            sf::sleep(sf::milliseconds(20));

            mutex.lock();
        }
        thread_work = 0;
    }
    if (wsk.joinable()) { wsk.detach(); }

    thread_work = 1;
    wsk = std::thread(std::bind(&P_SIMPLE::main, this));

    Console::printf("Polaczono z: %s\n", port.c_str());

    Event e;
    e.type = Event::Open;
    event.push(e);

    return true; //koniec ustawienia portu
}

// TODO (Michał Marszałek#1#09/28/18): thread_work as shared atomic pointer. Pointer null=>thread.detach()=>fast close
void P_SIMPLE::close() /// =>w watku jest to samo :)
{
    mutex.lock();

    if (thread_work == 1) {
        thread_work = 0;
        for (int32_t i = 0; i < 10 && thread_work != 3; ++i) ///koniec watku
        {
            mutex.unlock();

            sf::sleep(sf::milliseconds(20));

            mutex.lock();
        }
        thread_work = 0;
    }

    if (wsk.joinable()) { wsk.detach(); }
    wsk = std::thread();


    CloseHandle(this->hCom);
    this->hCom = INVALID_HANDLE_VALUE;

    Event e;
    e.type = Event::Close;
    event.push(e);

    mutex.unlock();

    Console::printf("Port zostal rozlaczony\n");
}

std::map<int, int> P_SIMPLE::getData() {
    sf::Lock lock(mutex);

    auto result = std::move(parsed_data);
    parsed_data.clear();
    return result;
}

void P_SIMPLE::toSendData(const std::map<int, int> &dane) {
    sf::Lock lock(mutex);

    Console::printf("P_SIMPLE::toSendData:\n");

    for (auto it = dane.begin(); it != dane.end(); ++it) {
        auto it_act = act_data.find(it->first);
        if (it_act != act_data.end())///jezeli id do wyslania jest w aktualnych danych
        {
            if (it_act->second != static_cast<uint8_t>(it->second))///jezeli nowe sa rozne od aktualnych
            {
                Console::printf("%i -> %i %i\n", it->first, it_act->second, it->second);
                send_data[it->first] = it->second;
            } else {
                auto it_send = send_data.find(it->first);
                if (it_send !=
                    send_data.end())///jezeli aktualne i nowe sa takie same sprawc czy do wyslania nie sa pomylone :)
                {
                    send_data[it->first] = it->second;
                }
            }
        } else {
            Console::printf("%i -> %i\n", it->first, it->second);
            send_data[it->first] = it->second;
        }
    }
}

CAL_STATE P_SIMPLE::getCAL_STATE() {
    sf::Lock lock(this->mutex);
    return this->stateOfCalendar;
}

void P_SIMPLE::getCalendarDataSignal() {
    sf::Lock lock(this->mutex);
    this->needCalendarData = true;
}

uint8_t P_SIMPLE::getCalendarActiveDays() {
    sf::Lock lock(mutex);
    return this->calendar_data.first;
}

std::map<int, std::vector<Action_data_struct>> P_SIMPLE::getCalendarData() {
    sf::Lock lock(mutex);
    return this->calendar_data.second;
}

void P_SIMPLE::sendCalendarData(const std::pair<uint8_t, std::map<int, std::vector<Action_data_struct>>> &data) {
    sf::Lock lock(mutex);
    if (this->calendar_data.first != data.first) ///active day
    {
        this->calendar_data.first = data.first;
        this->needSendCalendarActive = true;
    }

    for (auto &&it : data.second) {
        if (it.second != this->calendar_data.second[it.first]) {
            this->calendar_data.second[it.first] = it.second;
            this->needSendCalendarDay |= 1 << (it.first + 1);
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
    Console::printf("P_SIMPLE::sendDateData:\n");
    this->date_data = d;
    needSendDateData = true;
}

int P_SIMPLE::writeCom(const std::string &data) {
    std::vector<char> v;
    v.insert(v.end(), data.begin(), data.end());
    return writeCom(v);
}

int P_SIMPLE::writeCom(const std::vector<char> &data) {
    std::vector<char> data2;
    data2.reserve(data.size());
// TODO (Michał Marszałek#1#09/22/17): Zamien na komiowanie dwu wskaznikowe i resize
    for (uint32_t i = 0; i < data.size(); ++i) {
        if (data[i] != ' ') { data2.push_back(data[i]); }
    }
    if (!data2.size() || data2.back() != '\r') { data2.push_back('\r'); }

    last_message = data2; ///kopiuj ostatnia wiadomosc

    sf::Lock lock(mutex);
    if (hCom != INVALID_HANDLE_VALUE) {
        COMSTAT statsread; //status
        DWORD RS_send;   //ilosc bitow odczytanych

        bool d = ClearCommError(hCom, nullptr, &statsread); //sprawdza status

        if (GetLastError() != ERROR_SUCCESS || !d) //sprawdza errory
        {
            return -2;
        }

        if (!WriteFile(hCom, data2.data(), data2.size(), &RS_send, nullptr)) {
            return -2;
        }

        FlushFileBuffers(hCom);

        communication_log.w_write(data2.data(), RS_send);
        Console::printf("Data write: %.*s\n", (int) RS_send, data2.data());

        return static_cast<int>(RS_send);
    }
    return 0;
}

int validate(const std::string &message, std::vector<uint8_t> &result);

void calculate(const std::string &message, std::vector<uint8_t> &result);

void create_message(uint8_t begining, uint8_t adres, const std::vector<uint8_t> &info, std::vector<char> &result);

int recive(HANDLE hCom, std::string &r_data, std::vector<uint8_t> &result, sf::Time time);

void P_SIMPLE::main() {
    sf::Clock clock;
    sf::Clock second120clock;
    sf::Clock needReceiveDateDataClock;
    std::string temponary_data;
    std::vector<uint8_t> result;

    bool boot = true;
    bool second120 = false;
    bool rzadanie_danych = true;
    this->needCalendarData = true;


    while (thread_work == 1) {
        int re = recive(hCom, temponary_data, result, sf::milliseconds(20));

        switch (re) {
            case -2: ///blad hCom
            { ///close :)
                Console::printf("Blad portu\n");

                mutex.lock(); ///1

                thread_work = 0;
                if (wsk.joinable()) { wsk.detach(); }
                wsk = std::thread();


                CloseHandle(this->hCom);
                this->hCom = INVALID_HANDLE_VALUE;

                Event e{Event::Close};
                event.push(e);

                mutex.unlock();///2
                return;
            }
                break;
            case -1: ///bload crc
                Console::printf("Blad crc\n");

                clock.restart();

                if (result[1] == 31) ///pytanie do mnie?
                {
                    writeCom(
                            {"66 00 03 10 01 56\r"});///66 rozpoczecie 00 adres 03 dlugosc 10 podstawowe potwierdzenie 01 ze blad 56 crc
                }
                break;
            case 0: ///brak zadnej wiadmosci
                if (second120clock.getElapsedTime() < sf::seconds(120) && boot && second120) {
                    clock.restart();
                }
                if ((clock.getElapsedTime() > sf::milliseconds(120)) && boot) {
                    writeCom({"66 00 02 01 cf\r"});
                    second120 = true;
                    second120clock.restart();
                    //boot = false;
                }
                if (needReceiveDateDataClock.getElapsedTime() > sf::seconds(60)) {
                    needReceiveDateData = true;
                }
                break;
            case 1:///wiadmosc jest poprawna
                Console::printf("Przetwarzanie wiadmosci\n");

                clock.restart();

                Console::printf("Adres: %02x\n", result[1]);

                if (result[1] == 31 || result[1] == 255) ///pytanie do mnie i adres rozgloszeniowy
                {
                    Console::printf("Wiadmosc do mnie\n");
                    sf::Lock lock(mutex);

                    if (result[1] == 31) {
                        boot = false;
                        second120 = false;
                        Event e{Event::Connected};
                        event.push(e);
                    }

                    if (!processed_data.empty() && (result[3] != 0x10)) ///naprawiam dane nie wyslane
                    {
                        std::swap(send_data, processed_data);
                        toSendData(processed_data);
                        processed_data.clear();
                    }

                    if (result[3] == 0x00)///reset
                    {
                        Console::printf("Reset\n");

                        Event e;
                        e.type = Event::Reset;
                        event.push(e);
                    } else if (result[3] == 0x01)///searching devices
                    {
                        Console::printf("Wyszukiwanie urzadzen\n");

                        second120clock.restart();
                    } else if (result[3] == 0x10)///odpowiedz
                    {
                        if (result[1] == 255) { break; }

                        if (result[4] == 0x01) ///blad komuniacji crc
                        {
                            Console::printf("Ponowne wysylanie wiadmosci\n");

                            writeCom(last_message);
                        } else if (result[4] == 0x00) {
                            Console::printf("Potwierdzenie otrzymania danych\n");

                            if (!processed_data.empty()) {
                                for (auto &it : processed_data)///dodawanie danych
                                {
                                    act_data[it.first] = it.second;
                                }
                                processed_data.clear();
                            }
                        }
                        ///printf("Komunikacja jest bledna\nOdpowiedz mimo braku zapytania\n");
                    } else if (result[3] == 0x20)///podtrzymanie komunikacji
                    {
                        if ((result[4] & 7) == 6) { rzadanie_danych = true; }//stan sterownika zostal zmieniony
                        if (result[1] == 255) { break; }//brodcast nie potrzebny dalej

                        Console::printf("Rzadanie danych: %s\n", rzadanie_danych ? "true" : "false");
                        bool calendar = (needCalendarData | needSendCalendarActive | needSendCalendarDay |
                                         needSendModesData | needSendDateData | needReceiveDateData);
                        Console::printf("Rzadanie kalendarza: %s\n", calendar ? "true" : "false");
                        Console::printf("needCalendarData: %s\n", needCalendarData ? "true" : "false");
                        Console::printf("needSendCalendarActive: %s\n", needSendCalendarActive ? "true" : "false");
                        Console::printf("needSendCalendarDay: %s\n", needSendCalendarDay ? "true" : "false");
                        Console::printf("needSendModesData: %s\n", needSendModesData ? "true" : "false");
                        Console::printf("needSendDateData: %s\n", needSendDateData ? "true" : "false");
                        Console::printf("needReceiveDateData: %s\n", needReceiveDateData ? "true" : "false");
// TODO (Michał Marszałek#1#09/16/18): Zwin ramke chce dane i che wyslac oraz kalendarz

                        if (rzadanie_danych && !calendar)///ja pragne dane
                        {
                            writeCom(
                                    {"66 00 03 10 00 08\r"}); ///66 rozpoczecie 00 adres 03 dlugosc 10 4bit chce dane 00 errors 0C crc
                        } else if (rzadanie_danych && calendar) {
                            // TODO (Michał Marszałek#1#09/14/18): Change to static meseage
                            std::vector<char> message;
                            std::vector<uint8_t> info;

                            info.push_back(0x30);///need calendar data
                            info.push_back(0x00);///errors

                            create_message(0x66, 0x00, info, message);
                            writeCom(message);
                        } else if (calendar | send_data.size()) {
                            std::vector<char> message;
                            std::vector<uint8_t> info;

                            info.push_back(0x20 + (send_data.size() ? 0x01
                                                                    : 0x00));///need calendar data | mam dane do wyslania

                            if (!send_data.empty())///mam dane do wyslania
                            {
                                Console::printf("Mam dane do wyslania\n");

                                processed_data = std::move(send_data);
                                send_data.clear();

                                {
                                    info.reserve(2 + 2 * processed_data.size());

                                    for (auto &it : processed_data)///dodawanie danych
                                    {
                                        info.push_back(it.first);
                                        info.push_back(it.second);
                                    }
                                }
                            }
                            info.push_back(0x00);///errors

                            create_message(0x66, 0x00, info, message);

                            writeCom(message);
                        } else///podtrzymanie komunikacji
                        {
                            Console::printf("Podtrzymanie komunikacji\n");

                            writeCom(
                                    {"66 00 03 01 00 20"});///66 rozpoczecie 00 adres 03 dlugosc 01 1bit gotowy 00 errors 20 crc
                        }
                    } else if (result[3] == 0x30)///kalendarz
                    {
                        Console::printf("Kalendarz podstawowa odpowiedz\n");
/*
                    if(!(result[5] & 1)) ///timer can't be start
                    {
                        needCalendarData = false;
                        needSendCalendarDay = false;
                        calendar_data.first = 0;
                        calendar_data.second.clear();

                        Event e;
                        e.type = Event::CalendarData;
                        event.push(e);
                    }*/
                        this->stateOfCalendar.data = result[5];

                        std::vector<char> message;
                        std::vector<uint8_t> info;
                        if (!needSendCalendarActive) {
                            this->calendar_data.first = result[4];

                            Event e;
                            e.type = Event::CalendarData;
                            event.push(e);
                        }

                        info.push_back(this->calendar_data.first);///en

                        if (needCalendarData) {
                            info.push_back(0xFF); ///what days
                            needCalendarData = false;
                        } else if (needSendCalendarDay) {
                            info.push_back(needSendCalendarDay); ///what days
                            needSendCalendarDay = 0;
                        } else {
                            info.push_back(0x01); ///what days
                        }

                        if (!needSendModesData) {
                            memcpy(modes_data.data, &result[12], 5);

                            Event e;
                            e.type = Event::ModesData;
                            event.push(e);
                        }

                        {
                            auto i = info.size();
                            info.resize(i + 5);

                            memcpy(&info[i], modes_data.data, 5);

                            needSendModesData = false;
                        }

                        if (needSendDateData) {
                            auto i = info.size();
                            info.resize(i + 6);

                            //memcpy(info.data(), calendar_data.second[result[4]].data(), calendar_data.second[result[4]].size()*6);

                            memcpy(&info[i], date_data.data, 6);


                            needSendDateData = false;
                            needReceiveDateData = false;
                            needReceiveDateDataClock.restart();
                        } else {
                            memcpy(date_data.data, &result[6], 6);

                            Event e;
                            e.type = Event::TimeData;
                            event.push(e);

                            needReceiveDateData = false;
                            needReceiveDateDataClock.restart();
                        }

                        create_message(0x66, 0x00, info, message);
                        writeCom(message);

                        needSendCalendarActive = false;
                    } else if (result[3] == 0x31)///save calendar data
                    {
                        Console::printf("Zapisz dni kalendarza\n");

                        uint32_t how_much = (result[2] - 3) / 6;
                        if ((result[2] - 3) % 6) { Console::printf("Error save calendar data\n"); }

                        calendar_data.second[result[4]].clear();
                        for (uint32_t i = 0; i < how_much; ++i) {
                            Action_data_struct data;
                            memcpy(data.data, &result[5 + i * 6], 6);///kopiowanie danych do miejsca +1
                            calendar_data.second[result[4]].push_back(data);
                        }
                        //calendar_data.second[result[4]].resize(how_much);

                        //memcpy(calendar_data.second[result[4]].data(), &result[5], 6*how_much);

                        Event e;
                        e.type = Event::CalendarData;
                        event.push(e);

                        writeCom(
                                {"66 00 03 10 00 08\r"}); ///66 rozpoczecie 00 adres 03 dlugosc 10 funckja 00 ans 0C crc
                    } else if (result[3] == 0x32)///send calendar data
                    {
                        Console::printf("Wyslij dzen: %i\n", result[4]);

                        std::vector<char> message;
                        std::vector<uint8_t> info;

                        info.resize(calendar_data.second[result[4]].size() * 6);

                        //memcpy(info.data(), calendar_data.second[result[4]].data(), calendar_data.second[result[4]].size()*6);

                        for (uint32_t i = 0; i < calendar_data.second[result[4]].size(); ++i) {
                            memcpy(&info[i * 6], calendar_data.second[result[4]][i].data, 6);
                        }

                        create_message(0x66, 0x00, info, message);
                        writeCom(message);
                    } else if (result[3] == 0x40)///prosba o parametry (wszystkie)
                    {
                        Console::printf("Prosba o parametry\n");

                        if (result[1] == 255) {
                            Console::printf("Error brodcast prosi o parametry\n");
                            break;
                        }
                        std::vector<char> message;
                        std::vector<uint8_t> info;

                        ///processed_data = std::move(send_data);
                        ///send_data.clear();

                        for (auto &it : send_data) {
                            act_data[it.first] = it.second;
                        }

                        processed_data = act_data;
                        send_data.clear();

                        {
                            info.reserve(2 * processed_data.size());

                            for (auto &it : processed_data)///dodawanie danych
                            {
                                info.push_back(it.first);
                                info.push_back(it.second);
                            }
                            processed_data.clear(); ///!!! ja nie pragne potwierdzenia
                        }

                        create_message(0x66, 0x00, info, message);

                        writeCom(message);

                    } else if (result[3] == 0x50)///zapisz dane
                    {
                        Console::printf("Zapisz dane\n");

                        rzadanie_danych = false;

                        std::map<int, uint32_t> tym;
                        for (uint32_t i = 4; i < (result.size() - 1); i += 2) {
                            uint8_t id = result[i];
                            uint8_t value = result[i + 1];
                            tym[id] = (tym[id] << 8) | value;
                        }

                        for (auto &it : tym) {
                            parsed_data[it.first] = it.second;///dane ktore odbiera gui
                            send_data.erase(it.first);///usuwam z danych do wyslania
                            act_data[it.first] = it.second;///dodaje do danych aktualnych
                            Console::printf("%i -> %i\n", it.first, it.second);
                        }

                        Event e;
                        e.type = Event::Data;
                        event.push(e);

                        writeCom(
                                {"66 00 03 10 00 08\r"}); ///66 rozpoczecie 00 adres 03 dlugosc 10 funckja 00 ans 0C crc
                    } else {
                        Console::printf("Funckja %i brak implementacji\n", result[3]);
                    }
                } else ///nie moj watek
                {
                    sf::Lock lock(mutex);
                    if (!processed_data.empty())///naprawiam dane nie wyslane
                    {
                        std::swap(send_data, processed_data);
                        toSendData(processed_data);
                        processed_data.clear();
                    }
                }
                break;
            default:
                Console::printf("Cos poszlo nie tak %s %i\n", __FILE__, __LINE__);
        }
    }

    thread_work = 3; ///zaznaczanie konca watku
}

int validate(const std::string &str, std::vector<uint8_t> &result) {
    if (str.size() < 9) { return 0; }///tekstowo 55 addres len fun crc \r
    for (uint32_t i = 0; i < (str.length() - 1); ++i) {
        if (!isxdigit(str[i])) { return 0; }///czy wszystkie znaki sa znakami hex? jezeli nie błąd transmisji
    }
    if ((str[0] != '5' || str[1] != '5') && (str[0] != '6' || str[1] != '6')) { return 0; }///poczatek komedy

    if (str[str.length() - 1] != '\r') { return 0; }///czy koniec zawiera \r

    /**Paczatek obrabiania danych**/

    calculate(str, result);
    if (3u + result[2] != ((str.size() - 1) / 2)) {
        result.clear();
        return 0;
    }///55 adres rozmiar + dane w tym crc

    if (crc8(result.data() + 2, result[2]) != result[result.size() - 1]) { return -1; }///suma crc

    return 1;
}

void calculate(const std::string &str, std::vector<uint8_t> &result) {
    result.clear();
    result.reserve((str.size() - 1) / 2);

    for (uint32_t i = 0; i < ((str.length() - 1) / 2); ++i) {
        uint16_t wartosc;///hhx => warning uint8_t :/
        sscanf(str.c_str() + 2 * i, "%2hx", &wartosc);///co dwa znaki

        result.push_back(static_cast<unsigned char &&>(wartosc));
    }
}

void create_message(uint8_t begining, uint8_t adres, const std::vector<uint8_t> &info, std::vector<char> &result) {
    std::vector<uint8_t> data2;
    data2.resize(info.size() + 1);
    uint32_t write = 0;
    {
        memcpy(&data2[1], info.data(), info.size());///kopiowanie danych do miejsca +1
        data2[0] = info.size() + 1; ///+1 za crc
    }

    result.clear();
    result.assign(4 + 2 * data2.size() + 3, 0);///66 00 dane  0C \r

    write += sprintf(result.data(), "%02hX%02hX", begining, adres);

    for (unsigned char i : data2) {
        write += sprintf(result.data() + write, "%02hX", i);
    }

    write += sprintf(result.data() + write, "%02hX", crc8(data2.data(), data2.size()));///crc
    sprintf(result.data() + write, "\r");

}

int recive(HANDLE hCom, std::string &r_data, std::vector<uint8_t> &result, sf::Time time) {
    COMSTAT statsread;
    COMMTIMEOUTS cto;
    DWORD RS_read;

    GetCommTimeouts(hCom, &cto);

    {
        ClearCommError(hCom, nullptr, &statsread); //sprawdza status
        if (statsread.cbInQue != 0)///czyta te w buforze
        {
            char *wsk = (char *) malloc(sizeof(char) * (statsread.cbInQue + 1));

            ReadFile(hCom, wsk, statsread.cbInQue, &RS_read, nullptr);

            if (RS_read != 0) {
                communication_log.r_write(wsk, RS_read);
                Console::printf("Read: %.*s\n", (int) RS_read, wsk);
            }

            r_data += std::string(wsk, RS_read);

            free(wsk);

            if (RS_read != 0) { goto parse; }
        }
    }

    {
        COMMTIMEOUTS newcom = cto;

        newcom.ReadTotalTimeoutConstant = time.asMilliseconds();
        if (newcom.ReadTotalTimeoutConstant == 0) { newcom.ReadTotalTimeoutConstant = 1; }
        newcom.ReadIntervalTimeout = 1;
        newcom.ReadTotalTimeoutMultiplier = 0;

        SetCommTimeouts(hCom, &newcom);
    }

    {
        std::array<char, 100> tab;

        ReadFile(hCom, tab.data(), 100, &RS_read, nullptr);

        if (RS_read != 0) {
            communication_log.r_write(tab.data(), RS_read);
            Console::printf("Read: %.*s\n", (int) RS_read, tab.data());
        }

        r_data.append(tab.data(), RS_read);
    }

    parse:

    std::size_t found = r_data.rfind('\r'); ///ostatni znak ramki
    if (found == std::string::npos) {
        SetCommTimeouts(hCom, &cto);
        return 0;
    }

    std::string str = r_data.substr(0, found + 1); ///kopiowanie z \r
    r_data.erase(0, found + 1);

    ///szukaj 55 od konca dopuki validate nie zwruci true lub po false found2 wynosi 0
    std::size_t found2 = found;
    while (true) {
        {///pierwszy znak ramki
            std::size_t found55 = str.rfind("55", found2 - 1);
            std::size_t found66 = str.rfind("66", found2 - 1);
            //found2 = ( (found55 == std::string::npos) ? found66 : ((found55 > found66) ? found55 : found66) ); ///wylicz najdalszy poczatek ramki i nie npos jeżeli mozna
            if (found55 == std::string::npos) {
                found2 = found66;
            } else if (found66 == std::string::npos) {
                found2 = found55;
            } else {
                found2 = ((found55 > found66) ? found55 : found66);
            }
        }

        if (found2 == std::string::npos) { break; }

        int va = validate(str.substr(found2, found - found2 + 1), result); ///od 55 do \r wlacznie
        if (va != 0) {
            Console::printf("Parsowanie: %s\n", str.substr(found2, found - found2 + 1).c_str());
            ///ramka prawidłowa lub z błędnym crc
            SetCommTimeouts(hCom, &cto);
            return va;
        } else {
            if (found2 == 0) { break; }
        }
    }


    SetCommTimeouts(hCom, &cto);
    result.clear();
    return 0;
}

