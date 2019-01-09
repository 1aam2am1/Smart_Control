#ifndef ACTION_H
#define ACTION_H

#include <TGUI/Widgets/Panel.hpp>
#include <map>
#include <Action_data_struct.h>

class Action : public tgui::Panel {
public:
    typedef std::shared_ptr<Action> Ptr;

    Action();

    virtual ~Action();

    void setData(const Action_data_struct &);

    Action_data_struct getData();

protected:
    tgui::Signal &getSignal(std::string signalName) override;

    void initialize();

public:
    tgui::Signal onValueChange = {"ValueChanged"};
    tgui::Signal onDelete = {"Deleted"};

private:
    bool callback_block;

    void callback(const int);

    void checkData();

    Action_data_struct result;
};

#endif // ACTION_H
