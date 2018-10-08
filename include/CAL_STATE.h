#ifndef CAL_STATE_H
#define CAL_STATE_H

#pragma pack(push, 1)
union CAL_STATE {
    struct {
        unsigned EN_FUNC :1; ///< Can Calendar be activated
        unsigned WORKING :1; ///< Work Operation depends on the settings
        //unsigned IN_CLK :1;
        //unsigned EXT_CLK :1;
        //unsigned ETH_MODULE :1;
        unsigned :6;
    };

    enum f {
        IN_CLK = 1,
        EXT_CLK = 2,
        ETH_MODULE = 4,
    };

    struct {
        unsigned :2;
        f flag :3;
        unsigned :3;
    };

    uint8_t data;
};
#pragma pack(pop)

///static_assert(sizeof(CAL_STATE) == 1, "Size is not correct");///GCC Align is not working

#endif // CAL_STATE_H
