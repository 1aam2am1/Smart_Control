#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdio>

namespace Console {
    void RedirectIOToConsole();

    void printf(const char *, ...);

    void fprintf(FILE *, const char *, ...);
}


#endif // CONSOLE_H
