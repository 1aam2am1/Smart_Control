#include <utility>

#include "Przelacznik_c.h"
#include "Funkcje_str.h"

Przelacznik_c::Przelacznik_c(std::string _nazwa, int32_t _id, int32_t _flaga)
        : nazwa(std::move(_nazwa)), id(_id), flaga(_flaga) {

}

Przelacznik_c::~Przelacznik_c() = default;

void Przelacznik_c::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }

    gui->get<tgui::CheckBox>(nazwa)->setChecked((i->second & flaga) != 0);

}
