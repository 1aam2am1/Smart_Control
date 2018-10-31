#include <include/Console.h>
#include "Parser.h"
#include "Funkcje_str.h"

Parser::Parser() = default;

Parser::~Parser() = default;

std::map<int, std::string> Parser::parseReceivedData(std::string dane) {
    std::map<int, std::string> result;
    std::string temp;
    std::string value;

    while (dane.size()) {
        temp = Game_api::wypisz(dane, '='); //flaga
        value = Game_api::wypisz(dane, ','); //value
        result[Game_api::convertString(temp)] = value;
    }

    return result;
}

std::string Parser::parseToSendData(const std::map<int, int> &dane) {
    if (static_cast<uint32_t>((dane.rbegin()->first - dane.begin()->first) + 1) != dane.size()) {
        Console::printf(Console::ERROR_MESSAGE, "Brak wszystkich danych z paskow ERROR\n");

        return "";
    }

    std::string result;
    for (auto i = dane.begin(); i != dane.end(); ++i) {
        result += ",";
        result += Game_api::convertInt(i->second);
    }
    result.erase(0, 1); ///pierwszy przecinek

    return result;
}

