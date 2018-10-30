#include "Console.h"
#include "windows.h"
#include <cstdio>
#include <fcntl.h>
#include <fstream>
#include <include/Console.h>

#include "Asynchronous_write.h"


// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

BOOL CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
        // Handle the CTRL-C signal.
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT:
            FreeConsole();
            return (TRUE);

        default:
            return FALSE;
    }
}

void Console::RedirectIOToConsole() {
    CONSOLE_SCREEN_BUFFER_INFO coninfo;

// allocate a console for this app
    AllocConsole();

// set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);
    freopen("CON", "r", stdin);
    freopen("CON", "w", stderr);

// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
// point to console as well
    std::ios::sync_with_stdio();

    SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE);

    HWND hwnd = GetConsoleWindow();
    if (hwnd != nullptr) {
        HMENU hMenu = GetSystemMenu(hwnd, FALSE);
        if (hMenu != nullptr) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
    }
}

void Console::printf(const char *str, ...) {
    Asynchronous_write::message m;
    m.file = stdout;

    std::vector<char> v(256);

    va_list v1;
    va_start(v1, str);

    while (vsnprintf(&v[0], v.size(), str, v1) < 0) {
        v.resize(v.size() * 2);
    }

    m.str = &v[0];

    va_end(v1);

    Asynchronous_write::getSingleton().add(m);
}

static Console::Message_level message_level;

void Console::setMessage_level(Message_level level) {
    message_level = level;
}

void Console::printf(Message_level level, const char *str, ...) {
    if (!(message_level | level)) { return; }

    Asynchronous_write::message m;
    m.file = stdout;

    std::vector<char> v(256);

    va_list v1;
    va_start(v1, str);

    while (vsnprintf(&v[0], v.size(), str, v1) < 0) {
        v.resize(v.size() * 2);
    }

    m.str = &v[0];

    va_end(v1);

    Asynchronous_write::getSingleton().add(m);
}

Console::Printf_block::Printf_block()
        : v(256) {}

Console::Printf_block Console::Printf_block::beginWrite() {
    return {};
}

Console::Printf_block &Console::Printf_block::printf(const char *str, ...) {
    va_list v1;
    va_start(v1, str);

    while (vsnprintf(&v[0], v.size(), str, v1) < 0) {
        v.resize(v.size() * 2);
    }

    va_end(v1);

    message += &v[0];

    return *this;
}

Console::Printf_block &Console::Printf_block::printf(Console::Message_level level, const char *str, ...) {
    if (!(message_level | level)) { return *this; }

    va_list v1;
    va_start(v1, str);

    while (vsnprintf(&v[0], v.size(), str, v1) < 0) {
        v.resize(v.size() * 2);
    }

    va_end(v1);

    message += &v[0];

    return *this;
}

Console::Printf_block Console::Printf_block::endWrite() {
    Asynchronous_write::message m;
    m.file = stdout;
    m.str = std::move(this->message);
    this->message.clear();
    Asynchronous_write::getSingleton().add(m);

    return *this;
}
