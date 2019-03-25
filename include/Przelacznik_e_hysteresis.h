#ifndef PRZELACZNIK_E_HYSTERESIS_H
#define PRZELACZNIK_E_HYSTERESIS_H

#include "Przelacznik.h"


class Przelacznik_e_hysteresis : public Przelacznik {
public:
    Przelacznik_e_hysteresis(std::string nazwa, int32_t id, int32_t max, int32_t min);

    virtual ~Przelacznik_e_hysteresis();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
    int32_t min;
    int32_t max;
};

#endif // PRZELACZNIK_E_HYSTERESIS_H
