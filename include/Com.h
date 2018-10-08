#ifndef COM_H
#define COM_H

#include <list>
#include <string>

class Com {
public:
    /* Com();
     ~Com();

     bool pollEvent(Event&);

     bool connect(std::string);
     void close();*/
    static std::list<std::string> getComList();
    /*std::string getData(); ///wnetrze od# do;
    bool write(std::string); ///przyjmuje raw tylko wnetrze

private:
    void haveData();
    void isTime();

    HANDLE hCom;
    std::string temponary_data;
    std::queue<Event> event;
    std::queue<std::string> data;

    sf::Clock clock;
    sf::Clock recive_clock;*/
};

#endif // COM_H
