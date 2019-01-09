#ifndef ORDER_H
#define ORDER_H

#include <functional>


struct Order {
    typedef bool Func_type_return;
    typedef std::function<Func_type_return()> Func_type;

    Func_type order = []()->bool {return false;};

    inline Func_type_return operator()() const {
        return order();
    }

    inline Order &operator=(const Func_type &right) {
        order = right;
        return *this;
    }

    std::string help = "Pomoc dla tej funkcji jest jeszcze niegotowa";
};

#endif // ORDER_H
