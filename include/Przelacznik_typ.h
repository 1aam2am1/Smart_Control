#ifndef PRZELACZNIK_TYP_H
#define PRZELACZNIK_TYP_H

#include "Przelacznik.h"


class Przelacznik_typ : public Przelacznik {
public:
    Przelacznik_typ(std::string nazwa, int32_t id);

    virtual ~Przelacznik_typ();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
};

#endif // PRZELACZNIK_TYP_H
