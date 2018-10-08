#ifndef PRZELACZNIK_B_H
#define PRZELACZNIK_B_H

#include "Przelacznik.h"


class Przelacznik_b : public Przelacznik {
public:
    Przelacznik_b(std::string nazwa, int32_t id, int32_t flaga);

    virtual ~Przelacznik_b();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
    int32_t flaga;
};

#endif // PRZELACZNIK_B_H
