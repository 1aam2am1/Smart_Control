//
// Created by Michal_Marszalek on 15.01.2019.
//

#ifndef SMART_CONTROL_P_SIMPLE_MESSAGE_H
#define SMART_CONTROL_P_SIMPLE_MESSAGE_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "crc.h"

namespace P_SIMPLE_Imp {

    class P_SIMPLE_Message {
    public:
        typedef std::shared_ptr<P_SIMPLE_Message> Ptr;

        P_SIMPLE_Message();

#pragma pack(push, 1)
        union {
            struct {
                uint8_t start_bit;
                uint8_t address;
                uint8_t length; //max 255
                uint8_t command;
                uint8_t data[253]; //> size = length - 2
                uint8_t CRC;
            };
            struct {
                unsigned :3;
                uint8_t ret_data[254]; //> size = length - 2
                unsigned :1;
            };
            uint8_t header[258];
        };
#pragma pack(pop)

        /// Translate protocol_message and validate it
        /// \param str String to validate and translate
        /// \param message Message that was validated and made human readable
        /// \return -1 crc error
        /// \return 0 error in length this is not all needed data
        /// \return 1 package is successfully  created
        static int create(const std::vector<char> &str, const P_SIMPLE_Message::Ptr &message);

        /// Translate protocol_message and validate it
        /// \param str String to validate and translate
        /// \param message Message that was validated and made human readable
        /// \return -1 crc error
        /// \return 0 error in length this is not all needed data
        /// \return 1 package is successfully  created
        static int create(const std::string &str, const P_SIMPLE_Message::Ptr &message);

        class check_type {
        public:
            enum {
                None = 0,
                Min_Size = 1 << 1,
                Max_Size = 1 << 2,
                Size = Min_Size | Max_Size,
                Digit = 1 << 3,
                Start_bit = 1 << 4,
                Stop_bit = 1 << 5,
                CRC = 1 << 6,
                All = (1 << 7) - 1
            };
        };

        /// Translate protocol_message and validate it
        /// \param str String to validate and translate
        /// \param result Message that was validated and made human readable
        /// \param type What to check?
        /// \return -1 crc error
        /// \return 0 error in length this is not all needed data => There was error
        /// \return 1 package is successfully  created
        template<typename T>
        static int create(const T &str, const P_SIMPLE_Message::Ptr &result, uint32_t type);

        /// Parse and create valid protocol_message from data
        /// \param str Returned protocol message to send by serial port
        void parse_protocol_message(std::vector<char> &str);
    };

    template<typename T>
    int P_SIMPLE_Message::create(const T &str, const P_SIMPLE_Message::Ptr &result, uint32_t type) {
        if (result.get() == nullptr) { return 0; }
        if ((type & check_type::Min_Size) && str.size() < 11) { return 0; }///55 1f len fun crc \r
        if ((type & check_type::Max_Size) && str.size() > (258 * 2 + 1)) { return 0; }
        for (uint32_t i = 0; (type & check_type::Digit) && (i < (str.size() - 1)); ++i) {
            if (!isxdigit(str[i])) { return 0; }///all are hex or transmission error
        }
        if ((type & check_type::Start_bit) && (str[0] != '5' || str[1] != '5') &&
            (str[0] != '6' || str[1] != '6')) { return 0; }///header ok?

        if ((type & check_type::Stop_bit) && str.back() != '\r') { return 0; }///\r in the end?

        /**Beginning of calculating the data**/

        for (uint32_t i = 0; i < ((str.size() - 1) / 2); ++i) {
            //uint16_t wartosc;///hhx => warning uint8_t :/
            //sscanf(str.data() + 2 * i, "%2hx", &wartosc);///co dwa znaki
            //make it faster
            uint8_t value;
            static const auto hex_to_dec = [](const char *x) -> uint8_t {
                auto b = static_cast<uint8_t>(*x);
                uint8_t maskLetter = ((uint8_t('9') - b) >> 31);
                uint8_t maskSmall = ((uint8_t('Z') - b) >> 31);
                uint8_t offset =
                        uint8_t('0') + (maskLetter & uint8_t('A' - '0' - 10)) + (maskSmall & uint8_t('a' - 'A'));
                return b - offset;
            };
            value = (hex_to_dec(str.data() + 2 * i) * uint8_t(16)) + hex_to_dec(str.data() + 2 * i + 1);


            result->header[i] = value;
            printf("%i -> %02hX\n", i, value);
        }

        if (3u + result->length != ((str.size() - 1) / 2)) {
            return 0;
        }///55 adres rozmiar + dane w tym crc

        result->CRC = result->header[2 + result->length]; ///66 00 len {com-dane OC}=len \r

        if ((type & check_type::CRC) &&
            crc8(result->header + 2, result->length) != result->CRC) { return -1; }///suma crc

        return 1;
    }

}

#endif //SMART_CONTROL_P_SIMPLE_MESSAGE_H
