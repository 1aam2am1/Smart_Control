#ifndef PRZELACZNIK_S_H
#define PRZELACZNIK_S_H

#include "Przelacznik.h"


class Przelacznik_s : public Przelacznik {
public:
    Przelacznik_s(std::string nazwa, int32_t id, int32_t flaga);

    virtual ~Przelacznik_s();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
    int32_t flaga;
};

#endif // PRZELACZNIK_S_H
