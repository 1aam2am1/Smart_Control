#ifndef PRZELACZNIK_H
#define PRZELACZNIK_H

#include <TGUI/TGUI.hpp>


class Przelacznik {
public:
    Przelacznik();

    virtual ~Przelacznik();

    virtual void change(const std::map<int, int> &, tgui::Container *) const = 0;

protected:

private:
};

#endif // PRZELACZNIK_H
