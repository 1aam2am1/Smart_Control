#include "File_Communication_log.h"
#include "Funkcje_str.h"
#include "Console.h"
#include <list>
#include "Asynchronous_write.h"
#include <ctime>

File_Communication_log::File_Communication_log(std::string dir)
        : directory(dir),
          start(true),
          file(nullptr),
          open_time({0, 0, 0, 0, 0, 0, 0, 0, 0}) {
    this->open();
    this->time_write();
}

File_Communication_log::~File_Communication_log() {
    if (file != nullptr) {
        fclose(file);
        file = nullptr;
    }
}

void File_Communication_log::write(const char *str, uint32_t i) {
    std::unique_lock<std::mutex> lock(this->mutex);

    this->open();
    this->time_write();

    auto r = this->parse(str, i);

    Asynchronous_write::getSingleton().add({file, r});
/*
    fprintf(file, "%.*s", (int)r.size(), r.c_str());

    fflush(file);*/
}

void File_Communication_log::r_write(const char *str, uint32_t i) {
    std::unique_lock<std::mutex> lock(this->mutex);

    this->open();
    this->time_write();

    if (last_write == std::string(str, i)) {
        last_write.clear();
        return;
    }

    auto r = this->parse(str, i);

    Asynchronous_write::getSingleton().add({file, r});
    /*
    fprintf(file, "%.*s", (int)r.size(), r.c_str());

    fflush(file);*/
}

void File_Communication_log::w_write(const char *str, uint32_t i) {
    std::unique_lock<std::mutex> lock(this->mutex);

    this->open();
    this->time_write();

    last_write = {str, i};

    auto r = this->parse(str, i);

    Asynchronous_write::getSingleton().add({file, r});
    /*
    fprintf(file, "%.*s", (int)r.size(), r.c_str());

    fflush(file);*/
}

void File_Communication_log::open() {
    time_t now;
    tm buf;

    time(&now);
#if defined(_WIN32)
    gmtime_s(&buf, &now);
#else
    gmtime_r(&now, &buf);
#endif

    if (open_time.tm_yday == buf.tm_yday && open_time.tm_mon == buf.tm_mon && open_time.tm_year == buf.tm_year &&
        file != nullptr) { return; }

    if (file != nullptr) {
        fclose(file);
        file = nullptr;
    }

    if (!Game_api::plik::mkdir(directory)) {
        Console::printf(Console::ERROR_MESSAGE, "File_communication directory cannot be created");
    }

    char buffer[125];

    if (directory.back() != '/') {
        strftime(buffer, 125, "/%d.%m.%Y.txt", &buf);
    } else {
        strftime(buffer, 125, "%d.%m.%Y.txt", &buf);
    }

    file = fopen((directory + buffer).c_str(), "a");

    if (file == nullptr) {
        Console::printf(Console::ERROR_MESSAGE, "File_communication file cannot be opened");
        file = stdout;
    }
    open_time = buf;
}

void File_Communication_log::time_write() {
    if (clock.getElapsedTime() < sf::seconds(60) && !start) { return; }
    else {
        clock.restart();
        start = false;
    }

    time_t now;
    tm buf;

    time(&now);
#if defined(_WIN32)
    gmtime_s(&buf, &now);
#else
    gmtime_r(&now, &buf);
#endif

    char buffer[80];
    if (new_line) {
        strftime(buffer, 80, "\n<%d.%m.%Y %H:%M:%S>\n", &buf);
        new_line = false;
    } else {
        strftime(buffer, 80, "<%d.%m.%Y %H:%M:%S>\n", &buf);
    }


    Asynchronous_write::getSingleton().add({file, buffer});
    /*
    fprintf(file, "<%s>\n", buffer);

    fflush(file);*/
}
