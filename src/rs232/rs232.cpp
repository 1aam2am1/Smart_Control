#include "rs232/rs232.h"

#if defined(_WIN32)
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Lock.hpp>
#include <Funkcje_str.h>
#include <functional>
#include "crc.h"
#include "Console.h"

rs232::rs232()
        : hCom(INVALID_HANDLE_VALUE) {
    Event e;
    e.type = Event::Create;
    event.push(e);
}

rs232::~rs232() {
    close();
}

bool rs232::connect(std::string port) {
    sf::Lock lock(mutex);

    bool fSuccess_COM;
    DCB dcb;        //konfiguracja portu
    dcb.DCBlength = sizeof(DCB);

    if (this->hCom != INVALID_HANDLE_VALUE) { CloseHandle(this->hCom); }

    hCom = CreateFile((R"(\\.\)" + port).c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL, nullptr); //otwieranie portu

    if (hCom == INVALID_HANDLE_VALUE) {
        Console::printf(Console::ERROR_MESSAGE, "CreateFile failed with error %lu.\n", GetLastError());
        return false;
    }

    //pobranie aktualnych ustawien portu
    fSuccess_COM = GetCommState(hCom, &dcb);
    if (!fSuccess_COM) {
        Console::printf(Console::ERROR_MESSAGE, "GetCommState failed with error %lu.\n", GetLastError());
        return false;
    }

    //ustawienie naszej konfiguracji
    dcb.BaudRate = CBR_115200;     // predkosc transmisji
    dcb.ByteSize = 8;             // ilosc bitow danych
    dcb.Parity = NOPARITY;        // brak bitu parzystosci
    dcb.StopBits = ONESTOPBIT;    // jeden bity stopu
    dcb.fDtrControl = DTR_CONTROL_DISABLE; //Kontrola linii DTR: DTR_CONTROL_DISABLE (nieaktywny), DTR_CONTROL_ENABLE (aktywny)
    dcb.fRtsControl = RTS_CONTROL_DISABLE; //Kontrola linii RTS: RTS_CONTROL_DISABLE (nieaktywny), RTS_CONTROL_ENABLE (aktywny)


    fSuccess_COM = SetCommState(hCom, &dcb);
    if (!fSuccess_COM) {
        Console::printf(Console::ERROR_MESSAGE, "SetCommState failed with error %lu.\n", GetLastError());
        return false;
    }

    if (thread_work == 1) {
        thread_work = 0;

        PurgeComm(this->hCom, PURGE_RXABORT);
        for (int32_t i = 0; i < 10 && thread_work != 3; ++i) ///koniec watku
        {
            sf::sleep(sf::milliseconds(20));
        }
        thread_work = 0;
    }
    if (thr.joinable()) { thr.detach(); }

    thread_work = 1;
    thr = std::thread(std::bind(&rs232::main, this));

    Console::printf(Console::MESSAGE, "Polaczono z: %s\n", port.c_str());

    Event e;
    e.type = Event::Open;
    event.push(e);

    return true; //koniec ustawienia portu
}

void rs232::close() /// =>w watku jest to samo :)
{
    mutex.lock();

    if (thread_work == 1) {
        thread_work = 0;

        PurgeComm(this->hCom, PURGE_RXABORT);
        for (int32_t i = 0; i < 10 && thread_work != 3; ++i) ///koniec watku
        {
            mutex.unlock();

            sf::sleep(sf::milliseconds(20));

            mutex.lock();
        }
        thread_work = 0;
    }

    if (thr.joinable()) { thr.detach(); }
    thr = std::thread();

    CloseHandle(this->hCom);
    this->hCom = INVALID_HANDLE_VALUE;

    Event e;
    e.type = Event::Close;
    event.push(e);

    mutex.unlock();

    Console::printf(Console::MESSAGE, "Port zostal rozlaczony\n");
}

std::map<int, int> rs232::getData() {
    sf::Lock lock(mutex);

    auto result = std::move(parsed_data);
    parsed_data.clear();
    return result;
}

void rs232::toSendData(const std::map<int, int> &dane) {
    std::map<int, int> dane2 = dane;
    dane2.erase(46);
    if (static_cast<uint32_t>((dane2.rbegin()->first - dane2.begin()->first) + 1) != dane2.size()) {
        Console::printf(Console::ERROR_MESSAGE, "Brak wszystkich danych z paskow ERROR\n");

        return;
    }

    std::string result;
    for (auto i : dane2) {
        result += ",";
        result += Game_api::convertInt(i.second);
    }
    result.erase(0, 1); ///pierwszy przecinek

    sf::Lock lock(mutex);

    write(result);
}

void rs232::main() {
    write("");
    receive_clock.restart();
    while (thread_work) {
        {
            sf::Lock lock(mutex);

            if (receive_clock.getElapsedTime() > sf::seconds(5)) {
                write("");
                receive_clock.restart();
            }

            haveData();
        }

        if (!thread_work) { break; }
        sf::sleep(sf::milliseconds(20));
    }

    thread_work = 3; ///zaznaczanie konca watku
}

void rs232::haveData() {
    sf::Lock lock(mutex);

    if (this->hCom == INVALID_HANDLE_VALUE) { return; }

    COMSTAT statsread;
    DWORD RS_read;

    bool d = ClearCommError(hCom, nullptr, &statsread); //sprawdza status

    if (statsread.cbInQue == 0) {
        return;
    }

    if (GetLastError() != ERROR_SUCCESS || !d) //sprawdza errory
    {
        this->close();
        return;
    }

    std::string str;
    if (statsread.cbInQue != 0) {
        char *wsk = (char *) malloc(sizeof(char) * (statsread.cbInQue + 1));

        ReadFile(hCom, wsk, statsread.cbInQue, &RS_read, nullptr);
        str.assign(wsk, RS_read);

        if (!str.empty()) { Console::printf(Console::LOG, "Read: %s\n", str.c_str()); }

        free(wsk);

        temporary_data += str;
        receive_clock.restart();
    }

    while (true) {
        str = "";

        std::size_t found = temporary_data.find('#'); //pierwszy znak liczac od 0
        if (found == std::string::npos) { return; }

        temporary_data.erase(0, found); ///usuwamy wczesniejsze

        std::size_t found2 = temporary_data.find(';'); //do ostatniego
        if (found2 == std::string::npos) { return; }

        found = temporary_data.rfind('#', found2); ///znajdujemy od konca

        str.insert(0, temporary_data, found + 1, found2 - found - 1); ///pobieramy dane do str bez ;

        temporary_data.erase(0, found2 + 1); ///usuwamy wszystko do znaku ; wlacznie

        ///parsowanie
        std::string temp;
        std::string value;

        Console::printf(Console::LOG, "Parsowanie: %s\n", str.c_str());

        while (!str.empty()) {
            temp = Game_api::wypisz(str, '='); //flaga
            value = Game_api::wypisz(str, ','); //value
            parsed_data[Game_api::convertString(temp)] = Game_api::convertString(value);
        }

        Event e;
        e.type = Event::Data;
        event.push(e);
    }

}

bool rs232::write(std::string informacja) {
    sf::Lock lock(mutex);

    if (!informacja.empty()) {
        informacja += ",";
        informacja += Game_api::convertInt(
                crc8(reinterpret_cast<const uint8_t *>(informacja.c_str()), informacja.size())); ///suma crc
    }

    std::string informacja2 = "#" + informacja + ";\r\n";
    if (this->hCom != INVALID_HANDLE_VALUE) {
        COMSTAT statsread; //status
        DWORD RS_send;   //ilosc bitow odczytanych

        bool d = ClearCommError(hCom, nullptr, &statsread); //sprawdza status

        if (GetLastError() != ERROR_SUCCESS || !d) //sprawdza errory
        {
            this->close();
            return false;
        }

        if (!WriteFile(hCom, informacja2.c_str(), informacja2.size(), &RS_send, nullptr)) {
            this->close();
            return false;
        }

        FlushFileBuffers(hCom);

        Console::printf(Console::LOG, "Data write: %.*s\n", (int) RS_send, informacja2.c_str());

        return true;
    } else {
        return false;
    }
}

#else
rs232::rs232() = default;
rs232::~rs232() = default;
bool rs232::connect(std::string){}
void rs232::close(){}
std::map<int, int> rs232::getData(){}
void rs232::toSendData(const std::map<int, int> &dane){}
#endif
