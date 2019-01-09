//
// Created by Michal_Marszalek on 30.12.2018.
//

#include <include/Console.h>
#include <algorithm>
#include <string>
#include "Test.h"
#include "Console.h"
#include "Funkcje_str.h"

Test::Test()
        : impl(nullptr) {
    tests[1] = [&]() -> bool {
        bool result = true;
        for (const auto &i : tests) {
            if (i.first != 1) {
                bool re;
                result &= (re = i.second());
                Console::printf(Console::MESSAGE, "%i: %s\n", i.first, re?"true":"false");
            }
        }
        return result;
    };
    tests.find(1)->second.help = "Run all tests";

    tests[2] = [&]() -> bool {
        for (int i = 0; i < 10; i++) {
            std::string result;
            impl->receive(result, sf::milliseconds(120));
            for (auto &c : result) {
                c = static_cast<char>(std::toupper(c));
            }
            if (result == "66000201CF") {
                return true;
            }
        }
        return false;
    };
    tests.find(2)->second.help = "Check of message to scan devices";

    tests[3] = [&]() -> bool {
        for (int i = 0; i < 5; i++) {
            writeCom({"550a03200025"});
            std::string result;
            impl->receive(result, sf::milliseconds(60));
            for (auto &c : result) {
                c = static_cast<char>(std::toupper(c));
            }
            if (!result.empty()) {
                return false;
            }
        }
        for (int i = 0; i < 2; i++) {
            std::string result;
            impl->receive(result, sf::milliseconds(120));
            for (auto &c : result) {
                c = static_cast<char>(std::toupper(c));
            }
            if (result == "66000201CF") {
                return true;
            }
        }
        return false;
    };
    tests.find(3)->second.help = "Check of message to scan devices when we send messages sometimes";

    tests[4] = [&]() -> bool {
        writeCom("55 1f 03 20 00 25"); ///simple messsage
        for (int i = 0; i < 10; i++) {
            std::string result;
            impl->receive(result, sf::milliseconds(120));
            for (auto &c : result) {
                c = static_cast<char>(std::toupper(c));
            }
            if (result[2] & 0x10) {
                writeCom("55 1f 03 10 00 08"); ///message was ok
                return true;
            }
        }
        return false;
    };
    tests.find(4)->second.help = "Check if app want data if it is first connected";
}

Test::~Test() = default;

bool Test::create(P_SIMPLE_DEVICE *i) {

    return (this->impl = i) != nullptr;
}

void Test::run_test(int i) {
    const auto it = tests.find(i);
    if (it != tests.end()) {
        Console::printf(Console::MESSAGE, "%i: %s\n", it->first, it->second()?"true":"false");
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
    y result.emplace_back("1: Run all tests");
    y result.emplace_back("2: Check of message to scan devices");
    y result.emplace_back("3: Check of message to scan devices when we send messages sometimes");
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

int Test::writeCom(const std::string &data) {
    std::vector<char> v;
    v.resize(data.size());

    {
        uint32_t j = 0;
        for (uint32_t i = 0; i < data.size(); ++i) {
            if (data[i] != ' ') {
                v[j] = data[i];
                ++j;
            }
        }
        v.resize(j);
    }

    if (v.empty() || v.back() != '\r') { v.push_back('\r'); }

    return impl->writeCom(v);
}
