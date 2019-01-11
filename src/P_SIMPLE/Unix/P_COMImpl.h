//
// Created by Michal_Marszalek on 04.12.2018.
//

#ifndef SMART_CONTROL_P_COMIMPL_H
#define SMART_CONTROL_P_COMIMPL_H

#include <string>
#include <SFML/System/Time.hpp>
#include <vector>

class P_COMImpl {
public:
    P_COMImpl();

    virtual ~P_COMImpl();

    bool connect(const std::string &, int = 9600);

    void close();

    int writeCom(const std::vector<char> &);

    int receive(std::string &r_data, sf::Time time);

private:
    int fd;
};


#endif //SMART_CONTROL_P_COMIMPL_H
