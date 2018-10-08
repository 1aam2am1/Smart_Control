#ifndef ARGV_OPTIONS_H
#define ARGV_OPTIONS_H

#include <SFML/System/Vector2.hpp>


class Argv_options {
public:
    Argv_options() = default;

    virtual ~Argv_options() = default;

    bool process(int argc, char **argv);

    struct Options {
        sf::Vector2i size = sf::Vector2i(1200, 800);
        bool rs232 = true;
        bool console = false;
        bool version = false;
    };

    Options getOptions() const;

    void setOptions(Options);

protected:

private:
    Options options;
};

#endif // ARGV_OPTIONS_H
