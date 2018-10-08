#ifndef PRZELACZNIK_C_H
#define PRZELACZNIK_C_H

#include "Przelacznik.h"


class Przelacznik_c : public Przelacznik {
public:
    Przelacznik_c(std::string nazwa, int32_t id, int32_t flaga);

    virtual ~Przelacznik_c();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
    int32_t flaga;
};

#endif // PRZELACZNIK_C_H
