#include "Com.h"

#if defined(_WIN32)
#include <windows.h>



#define MAX_VALUE_NAME 16383

std::list<std::string> Com::getComList() {
    std::list<std::string> tab;

    HKEY hkCom = HKEY_LOCAL_MACHINE;

    LONG result;
    const char *gdzie = "HARDWARE\\DEVICEMAP\\SERIALCOMM";

    result = RegOpenKeyEx(hkCom, gdzie, 0, KEY_READ, &hkCom);

    if (result != ERROR_SUCCESS) {
        RegCloseKey(hkCom);
        return tab;
    }

    DWORD dwMaxName, dwMaxData, dwMaxData_old, dwMaxIndex;
    result = RegQueryInfoKey(hkCom, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &dwMaxIndex, nullptr,
                             &dwMaxData_old, nullptr, nullptr);

    if (result != ERROR_SUCCESS) {
        RegCloseKey(hkCom);
        return tab;
    }

    char Name[MAX_VALUE_NAME]; //popraw
    char Data[dwMaxData_old]; //popraw

    for (DWORD i = 0; i < dwMaxIndex; ++i) {

        memset(Name, 0, MAX_VALUE_NAME); //zerowanie nazwy
        memset(Data, 0, dwMaxData_old); //zerowanie danych

        dwMaxName = MAX_VALUE_NAME;
        dwMaxData = dwMaxData_old;

        result = RegEnumValue(hkCom, i, Name, &dwMaxName, nullptr, nullptr, (LPBYTE) Data, &dwMaxData);

        if (result != ERROR_SUCCESS) {
            perror("ERROR");
            RegCloseKey(hkCom);
            return tab;
        }

        tab.push_back(std::string(Data)); //nazwa
    }

    RegCloseKey(hkCom);

    tab.sort();
    return tab;
}

#else

#include <regex>
#include "Funkcje_str.h"

std::list<std::string> Com::getComList() {
    std::list<std::string> tab;
    std::regex txt_regex("[a-z]*S[0-9]");
    auto vtab = Game_api::plik::dirF("/dev");

    for (const auto &it : vtab) {

        if (std::regex_match(it, txt_regex)) {
            tab.push_back(it);
        }
    }

    return tab;
}
#endif
