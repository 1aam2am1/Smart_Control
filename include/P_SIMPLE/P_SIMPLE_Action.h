//
// Created by Michal_Marszalek on 18.01.2019.
//

#ifndef SMART_CONTROL_P_SIMPLE_ACTION_H
#define SMART_CONTROL_P_SIMPLE_ACTION_H

#include "P_SIMPLE/P_SIMPLE_Message.h"
#include <list>
#include <functional>
/*
namespace P_SIMPLE_Imp {

    class P_SIMPLE_Action {
    public:
        P_SIMPLE_Action() = default;

        virtual ~P_SIMPLE_Action() = default;

        /// Execute message that we have get when command and command list
        /// \param message Message to parse
        /// \return false if command wasn't eaten
        /// \return true if command was eaten by function
        virtual bool execute(P_SIMPLE_Message::Ptr message) = 0;

        /// Response for our message to send
        /// \param response Response to send
        /// \return false if we don't have eny response
        /// \return true if we have response to send
        virtual bool get_response(P_SIMPLE_Message::Ptr response) = 0;

        /// Get command list for out action (register this class for these commadns)
        /// \return
        virtual std::list<uint8_t> get_command_list() = 0;

        virtual bool execute_all_messages(P_SIMPLE_Message::Ptr message){};

        void connect(std::function<void()>);

    protected:
        //virtual tgui::Signal &getSignal(std::string signalName);
        std::list<std::function < void()>> data_signal;
    };

}*/

#endif //SMART_CONTROL_P_SIMPLE_ACTION_H
