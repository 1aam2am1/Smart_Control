//
// Created by Michal_Marszalek on 30.12.2018.
//

#include "Com.h"
#include "P_SIMPLE/Win32/P_COMImpl.h"
#include "Console.h"
#include "Test.h"
#include "Funkcje_str.h"

int main(int argc, char **argv) {
    char cstr[255];
    P_SIMPLE_DEVICE serial_port;
    Test test;

    Console::setMessage_level(Console::ALL & ~Console::LOG);
    while (true) {
        auto i = Com::getComList();

        Console::printf(Console::MESSAGE, "List of com ports:\n");

        for (std::list<std::string>::const_iterator j = i.begin(); j != i.end(); ++j) {
            Console::printf(Console::MESSAGE, " - %s\n", j->c_str());
        }
        Console::printf(Console::MESSAGE, "Rescan: r\nExit: e\n\n");

        scanf("%254s", cstr);

        std::string str = cstr;

        if (str == "e") {
            return 0;
        }
        if (str == "r") {
            continue;
        }

        if (!serial_port.connect(str)) {
            //Console::printf(Console::ERROR_MESSAGE, "Error com port\n");
            continue;
        }

        test.create(&serial_port);

        do {

            Console::printf(Console::MESSAGE, "List of test:\n");

            auto j = test.get_test_list();

            for (const auto &k : j) {
                Console::printf(Console::MESSAGE, " - %s\n", k.c_str());
            }
            Console::printf(Console::MESSAGE, "\n");

            scanf("%254s", cstr);

            str = cstr;

            if (str == "e") {
                return 0;
            }
            if (str == "r") {
                break;
            }

            test.run_test(Game_api::convertString(str));

        } while (true);
    }


    return 0;
}