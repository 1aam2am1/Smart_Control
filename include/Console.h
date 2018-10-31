#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdio>
#include <string>
#include <vector>

namespace Console {
    void RedirectIOToConsole();

    __attribute__ ((deprecated)) void printf(const char *, ...);

    enum Message_level {
        NONE = 0,                //< Write nothing in the console
        MESSAGE = 1,                //< Simple message for user read
        FUNCTION_LOG = 1 << 1,           //< Name of function executed
        DATA_FUNCTION_LOG = 1 << 2,           //< Data of function executed
        LOG = 1 << 3,           //< Data for communication
        ERROR_MESSAGE = 1 << 4,           //< Error in communication
        ALL = (1 << 5) - 1
    };

    void setMessage_level(Message_level);

    void printf(Message_level, const char *, ...);

    class Printf_block {
    private:
        Printf_block();

        std::string message;
        std::vector<char> v;
    public:
        static Printf_block beginWrite();

        __attribute__ ((deprecated)) Printf_block &printf(const char *, ...);

        Printf_block &printf(Message_level, const char *, ...);

        Printf_block endWrite();
    };
}


#endif // CONSOLE_H
