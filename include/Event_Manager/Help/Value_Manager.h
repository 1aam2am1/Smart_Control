//
// Created by Michal_Marszalek on 01.04.2019.
//

#ifndef SMART_CONTROL_VALUE_MANAGER_H
#define SMART_CONTROL_VALUE_MANAGER_H

#include <list>
#include <cstdint>

class Value_Manager {
public:
    void set(int);                          ///< Set value

    int get();                              ///< Get value

    virtual std::list<uint8_t> get_message() = 0;       ///< Get message to create

    bool was_changed();

    void null_change();

    virtual bool change(uint8_t *tab, uint32_t size) = 0;

private:
    int value;
    bool changed;
};


#endif //SMART_CONTROL_VALUE_MANAGER_H
