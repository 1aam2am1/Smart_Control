#include "Device.h"
#include <SFML/System/Sleep.hpp>

Device::Device() = default;

Device::~Device() = default;

bool Device::pollEvent(Event &e) {
    if (!event.empty()) {
        e = event.front();
        event.pop();
        return true;
    }

    return false;
}
