#ifndef PRZELACZNIK_ODGRZEWANIE_H
#define PRZELACZNIK_ODGRZEWANIE_H

#include "Przelacznik.h"

///brak flag pracuje na stalych wartosciach
class Przelacznik_odmrazanie : public Przelacznik {
public:
    Przelacznik_odmrazanie(std::string nazwa, int32_t id);

    virtual ~Przelacznik_odmrazanie();

    virtual void change(const std::map<int, int> &, tgui::Container *) const override;

private:
    std::string nazwa;
    int32_t id;
};

#endif // PRZELACZNIK_ODGRZEWANIE_H
