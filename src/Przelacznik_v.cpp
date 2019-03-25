#include "Przelacznik_v.h"
#include "Funkcje_str.h"
#include "version.h"
#include <utility>

Przelacznik_v::Przelacznik_v(std::string _nazwa, int32_t _id)
        : nazwa(std::move(std::move(_nazwa))), id(_id) {

}

Przelacznik_v::~Przelacznik_v() = default;

void Przelacznik_v::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }

    std::string str = Game_api::convertInt(i->second);

    if (str.size() < 4) {
        str.insert(0, 4 - str.size(), '0');
    }

    str.insert(str.size() - 3, ".");
    gui->get<tgui::EditBox>(nazwa)->setText(str);

    if (i->second < Game_api::convertString(DeviceVersion::number)) {
        tgui::Callback callback;
        callback.id = 30;
        callback.widget = gui->get<tgui::EditBox>(nazwa).get();

        gui->get<tgui::EditBox>(nazwa)->getParent()->addChildCallback(callback);
    }
}
