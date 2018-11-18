#include <utility>

#include "Przelacznik_odmrazanie.h"
#include "Funkcje_str.h"

Przelacznik_odmrazanie::Przelacznik_odmrazanie(std::string _nazwa, int32_t _id)
        : nazwa(std::move(_nazwa)), id(_id) {

}

Przelacznik_odmrazanie::~Przelacznik_odmrazanie() = default;

void Przelacznik_odmrazanie::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }
    static const std::string tab[6] = {"Standard", "SSA-", "SSA+", "Programisty", "SSA++", "SSA+++"};

    std::string wartosc;
    for (uint32_t j = 0; j < 6; ++j) {
        if (i->second & (1 << j)) { wartosc += tab[j]; }
    }

    /*
    if (i->second & 1 << 0) { wartosc = tab[0]; }
    if (i->second & 1 << 1) { wartosc = tab[1]; }
    if (i->second & 1 << 2) { wartosc = tab[2]; }
    if (i->second & 1 << 3) { wartosc = tab[3]; }
    if (i->second & 1 << 4) { wartosc = tab[4]; }
    */

    gui->get<tgui::EditBox>(nazwa)->setText(wartosc);
}
