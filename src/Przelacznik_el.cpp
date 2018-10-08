#include <utility>

#include "Przelacznik_el.h"
#include "Funkcje_str.h"

static double moc_od_obrotow(int32_t obroty) {
    if (obroty == 0) { return 0; }

    double result = 0;
    result = pow(obroty, 2) * (0.00000000280730835942661 * obroty - 0.00000355746926477736) +
             0.00732582964032312 * obroty - 0.759107715961406;
    ///= 0,00000000280730835942661x3 - 0,00000355746926477736000x2 + 0,00732582964032312000000x - 0,75910771596140600000000



    return result;
}


Przelacznik_el::Przelacznik_el(std::string _nazwa, std::vector<int32_t> grzalka, std::vector<int32_t> obroty)
        : nazwa(std::move(_nazwa)), id_grzalka(std::move(grzalka)), id_wentylator(std::move(obroty)) {
    //printf("g:\n");
    for (int i : id_grzalka) {
        //printf("%i\n", id_grzalka[i]);
        wartosc_grzalka[i] = 0;
    }
    //printf("o:\n");
    for (int i : id_wentylator) {
        //printf("%i\n", id_wentylator[i]);
        wartosc_obroty[i] = 0;
    }
}

Przelacznik_el::~Przelacznik_el() = default;

void Przelacznik_el::change(const std::map<int, int> &dane, tgui::Container *gui) const {
    bool b = false;
    /*auto i = dane.find(id1);
    if(i != dane.end()){w1 = Game_api::convertString_float(i->second); b=true;}


    i = dane.find(id2);
    if(i != dane.end()){w2 = Game_api::convertString_float(i->second); b=true;}


    if(b)gui->get<tgui::EditBox>(nazwa)->setText(Game_api::convertInt(w1*10+w2*10+10));*/

    for (int i : id_grzalka) {
        auto w = dane.find(i);
        if (w != dane.end()) {
            wartosc_grzalka[w->first] = static_cast<double>(w->second) * 10.0;
            b = true;
        }
    }

    for (int i : id_wentylator) {
        auto w = dane.find(i);
        if (w != dane.end()) {
            wartosc_obroty[w->first] = moc_od_obrotow(w->second);
            b = true;
        }
    }

    if (b) {
        double wartosc = 0.0;
        for (auto &it : wartosc_grzalka) {
            wartosc += it.second;
        }
        for (auto &it : wartosc_obroty) {
            wartosc += it.second;
        }
        gui->get<tgui::EditBox>(nazwa)->setText(Game_api::convertInt(static_cast<int32_t>(wartosc + 5)));
    }
}

