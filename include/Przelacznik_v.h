#ifndef PRZELACZNIK_V_H
#define PRZELACZNIK_V_H

#include "Przelacznik.h"


class Przelacznik_v : public Przelacznik {
public:
    Przelacznik_v(std::string nazwa, int32_t id, int32_t id2); ///<16bit value id1id2

    virtual ~Przelacznik_v();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id1;
    int32_t id2;
};

#endif // PRZELACZNIK_V_H
