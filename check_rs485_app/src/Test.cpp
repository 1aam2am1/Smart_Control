//
// Created by Michal_Marszalek on 30.12.2018.
//

#include <include/Console.h>
#include "Test.h"
#include "Console.h"
#include "Funkcje_str.h"

Test::Test()
        : impl(nullptr) {
    tests[1] = [&]() {
        for (const auto &i : tests) {
            if (i.first != 1) { i.second(); }
        }
    };
    tests.find(1)->second.help = "Run all tests";

    tests[2] = [&]() {
        for (int i = 0; i < 200; i++) {
            std::string result;
            impl->receive(result, sf::milliseconds(120));
            //if(result == "66 00 02 01 CF\r") {
            //}
        }
    };
    tests.find(2)->second.help = "Check of message to scan devices";
}

Test::~Test() = default;

bool Test::create(P_COMImpl *i) {

    return (this->impl = i) != nullptr;
}

void Test::run_test(int i) {
    const auto it = tests.find(i);
    if (it != tests.end()) {
        it->second();
        return;
    }
    Console::printf(Console::ERROR_MESSAGE, "The run test don't exists");
}

std::list<std::string> Test::get_test_list() {
    std::list<std::string> result;
    for (const auto &i : tests) {
        result.emplace_back(Game_api::convertInt(i.first) + ": " + i.second.help);
    }
    /*
    result.emplace_back("1: Run all tests");
    result.emplace_back("2: Check of message to scan devices");
    result.emplace_back("3: Check of message to scan devices when we send messages sometimes");
    result.emplace_back("4: Check if app want data if it is first connected");
    result.emplace_back("5: Check app responses for correct data: calendar, time, sliders itp...");
    result.emplace_back("6: Check app response for incorrect crc => we have incorrect crc");
    result.emplace_back("7: Check app response for transmission error information => response has error");
    result.emplace_back("8: Check app response for broadcast information");
    result.emplace_back("9: Check app response for when we need data");
    result.emplace_back("10: Check app response if we send data to other devices");
    result.emplace_back("11: Check app response for incorrect transmission => we send many special symbols");
    result.emplace_back("100: All check from 2-10");
    result.emplace_back("255: Check if app has memory los => we pretend continuous use");
*/
    return result;
}
