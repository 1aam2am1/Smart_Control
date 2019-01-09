#include <utility>

#include "Przelacznik_s.h"
#include "Funkcje_str.h"

Przelacznik_s::Przelacznik_s(std::string _nazwa, int32_t _id, int32_t _flaga)
        : nazwa(std::move(_nazwa)), id(_id), flaga(_flaga) {

}

Przelacznik_s::~Przelacznik_s() = default;

void Przelacznik_s::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }
    static const std::tuple<std::string, sf::Color> tab[2] = {std::make_tuple("Zamkniete", sf::Color::Black),
                                                              std::make_tuple("Otwarte", sf::Color::Red)};
    int32_t j = (i->second & flaga) ? 1 : 0;
    gui->get<tgui::EditBox>(nazwa)->setText(std::get<0>(tab[j]));
    gui->get<tgui::EditBox>(nazwa)->getRenderer()->setTextColor(std::get<1>(tab[j]));
}
