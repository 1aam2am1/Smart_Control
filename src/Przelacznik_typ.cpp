#include <utility>

#include "Przelacznik_typ.h"
#include "Funkcje_str.h"

Przelacznik_typ::Przelacznik_typ(std::string _nazwa, int32_t _id)
        : nazwa(std::move(_nazwa)), id(_id) {

}

Przelacznik_typ::~Przelacznik_typ() = default;

void Przelacznik_typ::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }

    static const std::string tab[] = {"normal", "reczny", "temp_err",
                                      "bypass", "odmraz_przerwa", "odmraz",
                                      "odmraz_praca", "odmraz_standard", "odmraz_przerwa_dogrz",
                                      "dogrze", "wychladzanie", "wychladzanie_temp_err", "wychladzanie_reczny"};

    if (static_cast<uint32_t>(i->second) > (sizeof(tab) / sizeof(std::string))) {
        //printf("Brak takiego trybu\n");
        gui->get<tgui::EditBox>(nazwa)->setText("--brak--");
    } else {
        gui->get<tgui::EditBox>(nazwa)->setText(tab[i->second]);
    }
}
