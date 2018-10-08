#ifndef DATE_DATA_STRUCT_H
#define DATE_DATA_STRUCT_H


#pragma pack(push, 1)
union Date_data_struct {
    struct {
        unsigned minutes :7;
        unsigned batery :1;
        unsigned hours :6;
        unsigned :2;
        unsigned days :6;
        unsigned :2;
        unsigned weekends :3;
        unsigned :5;
        unsigned months :5;
        unsigned :2;
        unsigned century :1;
        unsigned years :8;
    };

    uint8_t data[6];
};
#pragma pack(pop)

inline uint32_t getminutes(const Date_data_struct &data) {
    return (data.minutes & 0b1111) + ((data.minutes & 0b1110000) >> 4) * 10;
}

inline void setminutes(Date_data_struct &data, uint32_t m) { data.minutes = ((m / 10) << 4) + m % 10; }

inline uint32_t gethours(const Date_data_struct &data) {
    return (data.hours & 0b1111) + ((data.hours & 0b110000) >> 4) * 10;
}

inline void sethours(Date_data_struct &data, uint32_t m) { data.hours = ((m / 10) << 4) + m % 10; }

inline uint32_t getdays(const Date_data_struct &data) {
    return (data.days & 0b1111) + ((data.days & 0b110000) >> 4) * 10;
}

inline void setdays(Date_data_struct &data, uint32_t m) { data.days = ((m / 10) << 4) + m % 10; }

inline uint32_t getmonth(const Date_data_struct &data) {
    return (data.months & 0b1111) + ((data.months & 0b10000) >> 4) * 10;
}

inline void setmonth(Date_data_struct &data, uint32_t m) { data.months = ((m / 10) << 4) + m % 10; }

inline uint32_t getyear(const Date_data_struct &data) {
    return (data.years & 0b1111) + ((data.years & 0b11110000) >> 4) * 10 + data.century * 100;
}

inline void setyear(Date_data_struct &data, uint32_t m) {
    data.century = m / 100;
    m = m % 100;
    data.years = ((m / 10) << 4) + m % 10;
}


#endif // DATE_DATA_STRUCT_H
