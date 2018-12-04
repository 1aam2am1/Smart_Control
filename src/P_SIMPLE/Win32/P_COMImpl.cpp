//
// Created by Michal_Marszalek on 04.12.2018.
//

#include <include/Console.h>
#include "P_COMImpl.h"
#include "Console.h"
#include "File_Communication_log.h"

static File_Communication_log communication_log;


P_COMImpl::P_COMImpl()
        : hCom(INVALID_HANDLE_VALUE),
          FileEvent({0}) {}

P_COMImpl::~P_COMImpl() {
    this->close();
}

bool P_COMImpl::connect(const std::string &port, int BaudRate) {
    this->close();

    WINBOOL fSuccess_COM;
    DCB dcb;        //konfiguracja portu
    dcb.DCBlength = sizeof(DCB);

    hCom = CreateFile((R"(\\.\)" + port).c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
                      FILE_FLAG_OVERLAPPED, nullptr); //otwieranie portu

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
    dcb.BaudRate = static_cast<DWORD>(BaudRate);     // predkosc transmisji

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

    fSuccess_COM = SetCommMask(hCom, EV_RXCHAR | EV_RXFLAG);
    if (!fSuccess_COM) {
        Console::printf(Console::ERROR_MESSAGE, "SetCommMask failed with error %lu.\n", GetLastError());
        return false;
    }

    {
        COMMTIMEOUTS newcom = {0};

        newcom.ReadTotalTimeoutConstant = 0;
        newcom.ReadIntervalTimeout = 1;
        newcom.ReadTotalTimeoutMultiplier = 0;

        fSuccess_COM = SetCommTimeouts(hCom, &newcom);
        if (!fSuccess_COM) {
            Console::printf(Console::ERROR_MESSAGE, "SetCommTimeouts failed with error %lu.\n", GetLastError());
            return false;
        }
    }

    return true;
}

void P_COMImpl::close() {
    if (this->hCom == INVALID_HANDLE_VALUE) { return; }

    SetCommMask(this->hCom, 0);

    CloseHandle(this->hCom);
    if (FileEvent.hEvent) { CloseHandle(FileEvent.hEvent); }
    FileEvent.hEvent = nullptr;
    this->hCom = INVALID_HANDLE_VALUE;
}

int P_COMImpl::writeCom(const std::vector<char> &data) {
    if (hCom != INVALID_HANDLE_VALUE) {
        OVERLAPPED osWrite = {0};
        DWORD RS_send;
        BOOL fRes;

        // Create this writes OVERLAPPED structure hEvent.
        osWrite.hEvent = CreateEvent(nullptr, true, false, nullptr);
        if (osWrite.hEvent == nullptr) {
            // Error creating overlapped event handle.
            Console::printf(Console::ERROR_MESSAGE, "W Error creating overlapped event handle %lu.\n", GetLastError());
            return -2;
        }

        // Issue write.
        if (!WriteFile(hCom, data.data(), data.size(), &RS_send, &osWrite)) {
            if (GetLastError() != ERROR_IO_PENDING) {
                Console::printf(Console::ERROR_MESSAGE, "W ERROR_IO_PENDING %lu.\n", GetLastError());
                fRes = false;
            } else {
                // Write is pending.
                fRes = (GetOverlappedResult(hCom, &osWrite, &RS_send, true) != 0);
            }
        } else {
            // WriteFile completed immediately.
            fRes = true;
        }

        CloseHandle(osWrite.hEvent);

        if (fRes) {
            communication_log.w_write(data.data(), RS_send);
            Console::printf(Console::DATA_FUNCTION_LOG, "Data write: %.*s\n", (int) RS_send, data.data());

            return static_cast<int>(RS_send);
        } else {
            Console::printf(Console::ERROR_MESSAGE, "W GetOverlappedResult OR WriteFile %lu.\n", GetLastError());
            return -2;
        }

    }
    return 0;
}

int P_COMImpl::receive(std::string &r_data, sf::Time time) {
    DWORD RS_read;
    DWORD dwRes;
    unsigned long Occured;//returns the type of an occured event
    Console::printf(Console::LOG, "receive()\n");
    if (!FileEvent.hEvent) {
        Console::printf(Console::LOG, "CreateEvent()\n");
        FileEvent.hEvent = CreateEvent(nullptr, true, false, nullptr);
        if (FileEvent.hEvent == nullptr) {
            Console::printf(Console::ERROR_MESSAGE, "R Error creating overlapped event handle %lu.\n", GetLastError());
            // Error creating overlapped event handle.
            return -2;
        }
        Console::printf(Console::LOG, "WaitCommEvent()\n");
        if (!WaitCommEvent(hCom, &Occured, &FileEvent)) {
            if (GetLastError() != ERROR_IO_PENDING) {
                CloseHandle(FileEvent.hEvent);
                Console::printf(Console::ERROR_MESSAGE, "R WaitCommEvent overlapped %lu.\n", GetLastError());
                return -2;
            }
        }
    }

    dwRes = WaitForSingleObject(FileEvent.hEvent, time.asMilliseconds());

    Console::printf(Console::LOG, "WaitForSingleObject() %i\n", dwRes);
    switch (dwRes) {
        case WAIT_OBJECT_0: //the requested event happened
        {
            Console::printf(Console::LOG, "WAIT_OBJECT_0\n");
            CloseHandle(FileEvent.hEvent);
            FileEvent.hEvent = nullptr;

            OVERLAPPED ReadState = {0};

            ReadState.hEvent = CreateEvent(nullptr, true, false, nullptr);
            if (ReadState.hEvent == nullptr) {
                // Error creating overlapped event handle.
                Console::printf(Console::ERROR_MESSAGE, "R Error creating overlapped event handle2 %lu.\n",
                                GetLastError());
                return -2;
            }

            COMSTAT statsread;

            ClearCommError(hCom, nullptr, &statsread); //sprawdza status
            tab_receive_buffer.resize(statsread.cbInQue + 1);
            Console::printf(Console::LOG, "ClearCommError %i\n", statsread.cbInQue);
            if (!ReadFile(hCom, tab_receive_buffer.data(), statsread.cbInQue, &RS_read, &ReadState)) {
                Console::printf(Console::LOG, "ReadFile()\n");
                if (GetLastError() != ERROR_IO_PENDING) {
                    CloseHandle(ReadState.hEvent);
                    Console::printf(Console::ERROR_MESSAGE, "R ERROR_IO_PENDING %lu.\n", GetLastError());
                    return -2;
                } else {
                    // Read is pending.
                    if (!GetOverlappedResult(hCom, &ReadState, &RS_read, true)) {
                        CloseHandle(ReadState.hEvent);
                        Console::printf(Console::ERROR_MESSAGE, "R GetOverlappedResult %lu.\n", GetLastError());
                        return -2;
                    }
                }
            }

            CloseHandle(ReadState.hEvent);
            Console::printf(Console::LOG, "RS_read %i\n", RS_read);

            if (RS_read != 0) {
                communication_log.r_write(tab_receive_buffer.data(), RS_read);
                Console::printf(Console::DATA_FUNCTION_LOG, "Read: %.*s\n", (int) RS_read, tab_receive_buffer.data());
            }

            r_data.append(tab_receive_buffer.data(), RS_read);
        }
            break;
        case WAIT_TIMEOUT://time out
            Console::printf(Console::LOG, "WAIT_TIMEOUT()\n");
            return 0;
        default://error in WaitForSingleObject
            CloseHandle(FileEvent.hEvent);
            FileEvent.hEvent = nullptr;
            Console::printf(Console::ERROR_MESSAGE, "R WaitForSingleObject %lu.\n", GetLastError());
            return -2;
    }

    return RS_read;
}
