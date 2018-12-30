#ifndef ORDER_H
#define ORDER_H

#include <functional>


struct Order {
    std::function<void()> order = []() {};

    inline void operator()() const {
        return order();
    }

    inline Order &operator=(const std::function<void()> &prawy) {
        order = prawy;
        return *this;
    }

    std::string help = "Pomoc dla tej funkcji jest jeszcze niegotowa";
};

#endif // ORDER_H
