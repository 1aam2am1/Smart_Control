#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <TGUI/Gui.hpp>
#include <map>

class Parser {
public:
    Parser();

    ~Parser();

    std::map<int, std::string> parseReceivedData(std::string);

    std::string parseToSendData(const std::map<int, int> &);

protected:

private:
};

#endif // PARSER_H
