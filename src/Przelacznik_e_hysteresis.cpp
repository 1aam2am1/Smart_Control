#include <utility>

#include "Przelacznik_e_hysteresis.h"
#include "Funkcje_str.h"

Przelacznik_e_hysteresis::Przelacznik_e_hysteresis(std::string _nazwa, int32_t _id, int32_t _max, int32_t _min)
        : nazwa(std::move(_nazwa)), id(_id), min(_min), max(_max) {

}

Przelacznik_e_hysteresis::~Przelacznik_e_hysteresis() = default;

void Przelacznik_e_hysteresis::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }

    int32_t value = i->second;
    if (value > max) {
        value = max;
    } else if (value < min) {
        gui->get<tgui::EditBox>(nazwa)->setText("");
        return;
    } else if (value < min + 2 && gui->get<tgui::EditBox>(nazwa)->getText() == "") {
        return;
    }
    gui->get<tgui::EditBox>(nazwa)->setText(Game_api::convertInt(value));
}

