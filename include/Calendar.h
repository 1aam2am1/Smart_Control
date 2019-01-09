#ifndef CALENDAR_H
#define CALENDAR_H

#include <TGUI/Widgets/Panel.hpp>

#include <map>
#include <Action_data_struct.h>
#include <CAL_STATE.h>

class Calendar : public tgui::Panel {
public:
    typedef std::shared_ptr<Calendar> Ptr;

    Calendar();

    virtual ~Calendar();

    void setCAL_STATE(const CAL_STATE &);

    void setActiveDays(uint8_t);

    void change(const std::map<int, std::vector<Action_data_struct>> &);

    std::pair<uint8_t, std::map<int, std::vector<Action_data_struct>>> getChanged();

protected:
    tgui::Signal &getSignal(std::string signalName) override;

    void initialize();

public:
    tgui::Signal onValueChange = {"ValueChanged"};
    tgui::Signal onGetData = {"GetData"};

private:
    void callback(const int);
};

#endif // CALENDAR_H
