#ifndef PRZELACZNIK_V_H
#define PRZELACZNIK_V_H

#include "Przelacznik.h"


class Przelacznik_v : public Przelacznik {
public:
    Przelacznik_v(std::string nazwa, int32_t id);

    virtual ~Przelacznik_v();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
};

#endif // PRZELACZNIK_V_H
