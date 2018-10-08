#ifndef PRZELACZNIK_SEKMIN_H
#define PRZELACZNIK_SEKMIN_H

#include "Przelacznik.h"


class Przelacznik_sekmin : public Przelacznik {
public:
    Przelacznik_sekmin(std::string nazwa, int32_t id);

    virtual ~Przelacznik_sekmin();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
};

#endif // PRZELACZNIK_SEKMIN_H
