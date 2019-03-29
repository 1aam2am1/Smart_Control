#include "Przelacznik_v.h"
#include "Funkcje_str.h"
#include "version.h"
#include <utility>

Przelacznik_v::Przelacznik_v(std::string _nazwa, int32_t _id1, int32_t _id2)
        : nazwa(std::move(std::move(_nazwa))), id1(_id1), id2(_id2) {

}

Przelacznik_v::~Przelacznik_v() = default;

void Przelacznik_v::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id1);
    if (i == dane.end()) { return; }

    uint32_t value = static_cast<uint8_t>(i->second);

    i = dane.find(id2);
    if (i != dane.end()) {
        value = (value << 8) | static_cast<uint8_t>(i->second);
    }

    std::string str = Game_api::convertInt(value);

    if (str.size() < 4) {
        str.insert(0, 4 - str.size(), '0');
    }

    str.insert(str.size() - 3, ".");
    gui->get<tgui::EditBox>(nazwa)->setText(str);

    if (value < Game_api::convertString(DeviceVersion::number)) {
        tgui::Callback callback;
        callback.id = 30;
        callback.widget = gui->get<tgui::EditBox>(nazwa).get();

        gui->get<tgui::EditBox>(nazwa)->getParent()->addChildCallback(callback);
    }
}
