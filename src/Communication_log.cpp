#include "Communication_log.h"
#include <map>

Communication_log::Communication_log()
        : next_new_symbol(false),
          was_new_line(false) {

}

Communication_log::~Communication_log() = default;

std::string Communication_log::parse(const char *str, uint32_t size) {
    const static std::map<char, std::string> tab{
            {0,   "<NULL>"},
            {1,   "<SOH>"},
            {2,   "<STX>"},
            {3,   "<ETX>"},
            {4,   "<EOT>"},
            {5,   "<ENQ>"},
            {6,   "<ACK>"},
            {7,   "<BEL>"},
            {8,   "<BS>"},
            {9,   "<TAB>"},
            {10,  "<LF>"},
            {11,  "<VT>"},
            {12,  "<FF>"},
            {13,  "<CR>"},
            {14,  "<SO>"},
            {15,  "<SI>"},
            {16,  "<DLE>"},
            {17,  "<DC1>"},
            {18,  "<DC2>"},
            {19,  "<DC3>"},
            {20,  "<DC4>"},
            {21,  "<NAK>"},
            {22,  "<SYN>"},
            {23,  "<ETB>"},
            {24,  "<CAN>"},
            {25,  "<EM>"},
            {26,  "<SUB>"},
            {27,  "<ESC>"},
            {28,  "<FS>"},
            {29,  "<GS>"},
            {30,  "<RS>"},
            {31,  "<US>"},
            {127, "<DEL>"}
    };

    std::string result;

    for (uint32_t i = 0; i < size; ++i) {
        char c = str[i];

        auto it = tab.find(c);

        if (it != tab.end()) {
            if (next_new_symbol && c != '\n') {
                result += "\n";
                next_new_symbol = false;
            }

            result += it->second;

            was_new_line = false;

            if (c == '\r') { next_new_symbol = true; }
            else if (c == '\n') {
                next_new_symbol = false;
                result += "\n";
                was_new_line = true;
            }
        } else {
            if (next_new_symbol) {
                next_new_symbol = false;
                result += "\n";
            }

            result += c;
            was_new_line = false;
        }

    }

    return result;
}
