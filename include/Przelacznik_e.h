#ifndef PRZELACZNIK_E_H
#define PRZELACZNIK_E_H

#include "Przelacznik.h"


class Przelacznik_e : public Przelacznik {
public:
    Przelacznik_e(std::string nazwa, int32_t id);

    virtual ~Przelacznik_e();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
};

#endif // PRZELACZNIK_E_H
