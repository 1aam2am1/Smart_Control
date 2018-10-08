#ifndef ACTION_DATA_STRUCT_H
#define ACTION_DATA_STRUCT_H


#pragma pack(push, 1)
union Action_data_struct {
    struct {
        unsigned moc_wywiew :7;
        unsigned en :1;
        unsigned moc_nawiew :7;
        unsigned max_moc :1;
        unsigned time :11;
        unsigned hour :5;
        unsigned minutes :6;
        unsigned :2;
        unsigned temp :4;
        unsigned :4;
    };

    uint8_t data[6];
};
#pragma pack(pop)

///static_assert(sizeof(Action_data_struct) == 6, "Size is not correct");///GCC Align is not working

inline bool operator==(const Action_data_struct left, const Action_data_struct right) {
    for (uint8_t i = 0; i < 6; ++i) {
        if (left.data[i] != right.data[i]) {
            return false;
        }
    }

    return true;
}

#endif // ACTION_DATA_STRUCT_H
