//
// Created by Michal_Marszalek on 30.12.2018.
//

#ifndef SMART_CONTROL_TEST_H
#define SMART_CONTROL_TEST_H

#include "P_SIMPLE/Win32/P_COMImpl.h"
#include <list>
#include <map>
#include "Order.h"

class Test {
public:
    Test();

    virtual ~Test();

    bool create(P_COMImpl *);

    void run_test(int);

    std::list<std::string> get_test_list();

private:
    std::map<int, Order> tests;
    P_COMImpl *impl;
};


#endif //SMART_CONTROL_TEST_H
