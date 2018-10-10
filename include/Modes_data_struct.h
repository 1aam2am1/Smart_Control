#ifndef MODES_DATA_STRUCT_H
#define MODES_DATA_STRUCT_H

#include <cstdint>


#pragma pack(push, 1)
union Modes_data_struct {
    struct {
        unsigned boost_value :7;
        unsigned boost_en :1;
        unsigned wierzenie_value :7;
        unsigned wierzenie_en :1;
        unsigned sen_value :7;
        unsigned sen_en :1;
        unsigned urlop_value :7;
        unsigned urlop_en :1;
        unsigned max_went_value :7;
        unsigned max_went_en :1;
        //unsigned owc_value :7;
        //unsigned owc_en :1;
    };

    uint8_t data[5];
};
#pragma pack(pop)

#endif // MODES_DATA_STRUCT_H
