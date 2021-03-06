#include "Argv_options.h"
#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "Funkcje_str.h"
#include "Console.h"

bool Argv_options::process(int argc, char **argv) {
    const option long_options[] =
            {
                    {"geometry",      required_argument, nullptr, 'g'},
                    {"rs232",         no_argument,       nullptr, 'r'},
                    {"modbus",        no_argument,       nullptr, 'm'},
                    {"modbus_usb",    no_argument,       nullptr, 'u'},
                    {"console",       no_argument,       nullptr, 'c'},
                    {"debug_message", no_argument,       nullptr, 'd'},
                    {"help",          no_argument,       nullptr, 'h'},
                    {"version",       no_argument,       nullptr, 'v'},
                    {nullptr,         0,                 nullptr, 0}
            };
    int opcje_kolidujace_rs232_modbus = 0;
    int options_long_string_length = 0;

    optind = 0; //set to 0 because of multiple processing

    std::string getopt_long_short_options;
    getopt_long_short_options.reserve((sizeof(long_options) / sizeof(option)) * 2);

    for (uint32_t i = 0; long_options[i].name != nullptr || long_options[i].has_arg != 0 ||
                         long_options[i].flag != nullptr || long_options[i].val != 0; ++i) {
        getopt_long_short_options.push_back(static_cast<char>(long_options[i].val));
        if (long_options[i].has_arg != no_argument) {
            getopt_long_short_options.push_back(':');
        }

        options_long_string_length = std::max<int>(options_long_string_length, strlen(long_options[i].name));
    }

    while (true) {
        int option_index = 0;

        int c = getopt_long(argc, argv, getopt_long_short_options.c_str(),
                            long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c) {
            case 'g': {
                std::string str = optarg;
                if (str.find('x') != std::string::npos) {
                    int x = Game_api::convertString(Game_api::wypisz(str, 'x'));
                    int y = Game_api::convertString(str);
                    options.size = sf::Vector2i(x, y);
                } else if (str.find('X') != std::string::npos) {
                    int x = Game_api::convertString(Game_api::wypisz(str, 'X'));
                    int y = Game_api::convertString(str);
                    options.size = sf::Vector2i(x, y);
                } else {
                    Console::printf(Console::ERROR_MESSAGE, "Option -g as --geometry required widthXheight\n");
                }
            }
                break;
            case 'r':
            case 'm':
            case 'u': {
                opcje_kolidujace_rs232_modbus += 1;
                if (opcje_kolidujace_rs232_modbus >= 2) {
                    Console::printf(Console::ERROR_MESSAGE,
                                    "Options -r -m or -u as --rs232 --modbus --modbus_usb can use only one time\n");
                }
                if (c == 'r') { options.tryb = Argv_options::Options::rs232; }
                if (c == 'm') { options.tryb = Argv_options::Options::modbus; }
                if (c == 'u') { options.tryb = Argv_options::Options::modbus_usb; }
            }
                break;
            case 'c':
                options.console = true;
                break;
            case 'd':
                options.debug_message = true;
                break;
            case 'v':
                options.version = true;
                break;
            case 'h':
                Console::printf(Console::ERROR_MESSAGE, "Options:\n");
                for (uint32_t i = 0; long_options[i].name != nullptr || long_options[i].has_arg != 0 ||
                                     long_options[i].flag != nullptr || long_options[i].val != 0; ++i) {
                    Console::printf(Console::ERROR_MESSAGE, " - %-*s %s\n", options_long_string_length + 2,
                                    long_options[i].name,
                                    long_options[i].has_arg ? "Arg" : "");
                }
#ifndef _WIN32
                Console::printf(Console::ERROR_MESSAGE, "\nLinux console option use xterm\n");
#endif
                exit(EXIT_FAILURE);
            case '?':
                Console::printf(Console::ERROR_MESSAGE, "ERROR Bad Option\n");
                exit(EXIT_FAILURE);
            default:
                Console::printf(Console::ERROR_MESSAGE, "ERROR Bad Option\n");
                return false;
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        Console::printf(Console::ERROR_MESSAGE, "non-option ARGV-elements: ");
        while (optind < argc)
            Console::printf(Console::ERROR_MESSAGE, "%s ", argv[optind++]);
        Console::printf(Console::ERROR_MESSAGE, "\n");
    }

    return true;
}

Argv_options::Options Argv_options::getOptions() const {
    return options;
}

void Argv_options::setOptions(Argv_options::Options op) {
    this->options = op;
}
