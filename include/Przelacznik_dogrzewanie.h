#ifndef PRZELACZNIK_DOGRZEWANIE_H
#define PRZELACZNIK_DOGRZEWANIE_H

#include "Przelacznik.h"


class Przelacznik_dogrzewanie : public Przelacznik {
public:
    Przelacznik_dogrzewanie(std::string nazwa, int32_t id, int32_t flaga);

    virtual ~Przelacznik_dogrzewanie();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
    int32_t flaga;
};

#endif // PRZELACZNIK_DOGRZEWANIE_H
