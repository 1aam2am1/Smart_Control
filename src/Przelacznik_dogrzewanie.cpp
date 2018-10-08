#include <utility>

#include "Przelacznik_dogrzewanie.h"
#include "Funkcje_str.h"

Przelacznik_dogrzewanie::Przelacznik_dogrzewanie(std::string _nazwa, int32_t _id, int32_t _flaga)
        : nazwa(std::move(_nazwa)), id(_id), flaga(_flaga) {

}

Przelacznik_dogrzewanie::~Przelacznik_dogrzewanie() = default;

void Przelacznik_dogrzewanie::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }

    static const std::string tab[2] = {"nieaktywne", "aktywne"};

    gui->get<tgui::EditBox>(nazwa)->setText(tab[(i->second & flaga ? 1 : 0)]);

}
