#ifndef PRZELACZNIK_EL_H
#define PRZELACZNIK_EL_H

#include "Przelacznik.h"


class Przelacznik_el : public Przelacznik {
public:
    Przelacznik_el(std::string nazwa, std::vector<int32_t> grzalka, std::vector<int32_t> obroty);

    virtual ~Przelacznik_el();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    std::vector<int32_t> id_grzalka;
    std::vector<int32_t> id_wentylator;
    mutable std::map<int32_t, double> wartosc_grzalka;
    mutable std::map<int32_t, double> wartosc_obroty;
};

#endif // PRZELACZNIK_EL_H
