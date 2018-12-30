#ifndef PRZELACZNIK_OBROTY_H
#define PRZELACZNIK_OBROTY_H

#include "Przelacznik.h"


class Przelacznik_obroty : public Przelacznik {
public:
    Przelacznik_obroty(std::string nazwa, int32_t id);

    virtual ~Przelacznik_obroty();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
};

#endif // PRZELACZNIK_OBROTY_H
