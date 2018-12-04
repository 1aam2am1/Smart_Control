//
// Created by Michal_Marszalek on 04.12.2018.
//

#include <include/Console.h>
#include "P_COMImpl.h"
#include "Console.h"
#include "File_Communication_log.h"

static File_Communication_log communication_log;

// Todo Do implementation
P_COMImpl::P_COMImpl() {}

P_COMImpl::~P_COMImpl() {
    this->close();
}

bool P_COMImpl::connect(const std::string &port, int BaudRate) {

}

void P_COMImpl::close() {

}

int P_COMImpl::writeCom(const std::vector<char> &data) {

}

int P_COMImpl::receive(std::string &r_data, sf::Time time) {

}
