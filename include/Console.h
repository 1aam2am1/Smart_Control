#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdio>
#include <string>

namespace Console {
    void RedirectIOToConsole();

    void printf(const char *, ...);

    void fprintf(FILE *, const char *, ...);

    class Printf_block {
    private:
        Printf_block();

        std::string message;
    public:
        static Printf_block beginWrite();

        Printf_block &printf(const char *, ...);

        Printf_block endWrite();
    };
}


#endif // CONSOLE_H
