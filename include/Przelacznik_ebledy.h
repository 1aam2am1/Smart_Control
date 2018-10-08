#ifndef PRZELACZNIK_EBLEDY_H
#define PRZELACZNIK_EBLEDY_H

#include "Przelacznik.h"


class Przelacznik_ebledy : public Przelacznik {
public:
    Przelacznik_ebledy(std::string nazwa, int32_t id, int32_t flaga);

    virtual ~Przelacznik_ebledy();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
    int32_t flaga;
};

#endif // PRZELACZNIK_EBLEDY_H
