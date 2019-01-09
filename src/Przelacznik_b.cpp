#include <utility>

#include "Przelacznik_b.h"
#include "Funkcje_str.h"

Przelacznik_b::Przelacznik_b(std::string _nazwa, int32_t _id, int32_t _flaga)
        : nazwa(std::move(_nazwa)), id(_id), flaga(_flaga) {

}

Przelacznik_b::~Przelacznik_b() = default;

void Przelacznik_b::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }
    gui->get<tgui::Button>(nazwa)->getRenderer()->setTextColor(
            sf::Color(static_cast<sf::Uint8>(255 * (i->second & flaga ? 1 : 0)), 0, 0, 255));
}
