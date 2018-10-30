#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdio>
#include <string>
#include <vector>

namespace Console {
    void RedirectIOToConsole();

    __attribute__ ((deprecated)) void printf(const char *, ...);

    enum Message_level {
        NONE,               //< Write nothing in the console
        MESSAGE,            //< Simple message for user read
        FUNCTION_LOG,       //< Name of function executed
        LOG,                //< Data for communication
        ERROR_MESSAGE       //< Error in communication
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
