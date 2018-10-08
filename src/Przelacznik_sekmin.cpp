#include <utility>

#include "Przelacznik_sekmin.h"
#include "Funkcje_str.h"

Przelacznik_sekmin::Przelacznik_sekmin(std::string _nazwa, int32_t _id)
        : nazwa(std::move(_nazwa)), id(_id) {

}

Przelacznik_sekmin::~Przelacznik_sekmin() = default;

void Przelacznik_sekmin::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    auto i = dane.find(id);
    if (i == dane.end()) { return; }
    time_t tym = i->second;
    tm z = *gmtime(&tym);

    char buf[80];
    strftime(buf, 80, "%M:%S", &z);

    gui->get<tgui::EditBox>(nazwa)->setText(buf);
}
