#include <utility>

#include "Przelacznik_ebledy.h"
#include "Funkcje_str.h"

Przelacznik_ebledy::Przelacznik_ebledy(std::string _nazwa, int32_t _id, int32_t _flaga)
        : nazwa(std::move(_nazwa)), id(_id), flaga(_flaga) {

}

Przelacznik_ebledy::~Przelacznik_ebledy() = default;

void Przelacznik_ebledy::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }

    if (i->second & flaga) {
        gui->get<tgui::EditBox>(nazwa)->getRenderer()->setTextColor(sf::Color::Red);
    } else {
        gui->get<tgui::EditBox>(nazwa)->getRenderer()->setTextColor(sf::Color::Black);
    }
}
