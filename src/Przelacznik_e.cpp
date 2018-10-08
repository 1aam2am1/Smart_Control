#include <utility>

#include "Przelacznik_e.h"
#include "Funkcje_str.h"

Przelacznik_e::Przelacznik_e(std::string _nazwa, int32_t _id)
        : nazwa(std::move(_nazwa)), id(_id) {

}

Przelacznik_e::~Przelacznik_e() = default;

void Przelacznik_e::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }

    gui->get<tgui::EditBox>(nazwa)->setText(Game_api::convertInt(i->second));
}

