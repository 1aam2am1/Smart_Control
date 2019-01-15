//
// Created by Michal_Marszalek on 15.01.2019.
//

#include "P_SIMPLE/P_SIMPLE_Message.h"

P_SIMPLE_Imp::P_SIMPLE_Message::P_SIMPLE_Message() :
        header{} {

}

int P_SIMPLE_Imp::P_SIMPLE_Message::create(const std::vector<char> &str, P_SIMPLE_Message::Ptr result) {
    return create(str, std::move(result), check_type::All);
}

int P_SIMPLE_Imp::P_SIMPLE_Message::create(const std::string &str, P_SIMPLE_Message::Ptr result) {
    return create(str, std::move(result), check_type::All);
}

void P_SIMPLE_Imp::P_SIMPLE_Message::parse_protocol_message(std::vector<char> &protocol_message) {
    protocol_message.resize(6u + 2 * length + 1); ///66 00 len {com-dane 0C}=len \r

    CRC = crc8(header + 2, length); ///66 00 {len com-dane}=len crc \r
    header[3 + length] = CRC; ///66 00 len {com-dane OC}=len \r

    //write += sprintf(result.data() + write, "%02hX", i);
    for (uint32_t i = 0; i < 3 + length; ++i) { ///66 00 len {com-dane OC}=len \r
        static const char *digits = "0123456789ABCDEF";

        protocol_message[2 * i + 0] = digits[header[i] >> 4];
        protocol_message[2 * i + 1] = digits[header[i] & 0xf];
    }

    protocol_message.back() = '\r';
}
