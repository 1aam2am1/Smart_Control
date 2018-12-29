#include "Device.h"
#include <SFML/System/Lock.hpp>

Device::Device() = default;

Device::~Device() = default;

bool Device::pollEvent(Event &e) {
    sf::Lock lock(mutex);

    if (!event.empty()) {
        e = event.front();
        event.pop();
        return true;
    }

    return false;
}
