//
// Created by Michal_Marszalek on 04.12.2018.
//

#include <include/Console.h>
#include "P_COMImpl.h"
#include "Console.h"
#include "File_Communication_log.h"
#include <cstdio>
#include <cstring>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

static File_Communication_log communication_log;

// Todo Do implementation
P_COMImpl::P_COMImpl() :
        fd(-1) {}

P_COMImpl::~P_COMImpl() {
    this->close();
}

bool P_COMImpl::connect(const std::string &port, int BaudRate) {
    this->close();
    fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

    termios tty{};
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        return false;
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                     ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
// tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
// tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 1;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

// Set in/out baud rate to be 9600
    if (BaudRate == 9600) {
        cfsetispeed(&tty, B9600);
        cfsetospeed(&tty, B9600);
    } else if (BaudRate == 115200) {
        cfsetispeed(&tty, B115200);
        cfsetospeed(&tty, B115200);
    }

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        return false;
    }

    return true;
}

void P_COMImpl::close() {
    if (fd != -1) { ::close(fd); }
}

int P_COMImpl::writeCom(const std::vector<char> &data) {
    int sended = static_cast<int>(write(fd, data.data(), data.size()));
    communication_log.w_write(data.data(), data.size());
    Console::printf(Console::DATA_FUNCTION_LOG, "Data write: %.*s\n", sended, data.data());
    return sended;
}

int P_COMImpl::receive(std::string &r_data, sf::Time time) {
    static std::vector<char> tym;
    tym.resize(255, 0);

    termios tty{};
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        return -2;
    }
    tty.c_cc[VTIME] = static_cast<cc_t>(time.asSeconds() *
                                        10);    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    if (tty.c_cc[VTIME] == 0) { tty.c_cc[VTIME] = 1; }
    tty.c_cc[VMIN] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        return -2;
    }

    int readed = static_cast<int>(read(fd, tym.data(), tym.size()));

    if (readed == -1 && errno != 11) { return -2; }
    if (readed == -1) { return 0; }

    tym.resize(static_cast<unsigned long>(readed));

    communication_log.r_write(tym.data(), static_cast<uint32_t>(tym.size()));
    Console::printf(Console::DATA_FUNCTION_LOG, "Read: %.*s\n", (int) tym.size(), tym.data());
    r_data.append({tym.data(), tym.size()});
    tym.clear();

    return readed;
}
