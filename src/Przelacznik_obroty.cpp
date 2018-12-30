#include <utility>

#include "Przelacznik_obroty.h"
#include "Funkcje_str.h"

Przelacznik_obroty::Przelacznik_obroty(std::string _nazwa, int32_t _id)
        : nazwa(std::move(_nazwa)), id(_id) {

}

Przelacznik_obroty::~Przelacznik_obroty() = default;

void Przelacznik_obroty::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }

    gui->get<tgui::EditBox>(nazwa)->setText(Game_api::convertInt(i->second * 60));
}

