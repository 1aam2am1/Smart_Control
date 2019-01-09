#ifndef MENU_H
#define MENU_H

#include <TGUI/Widgets/Panel.hpp>

#include <map>
#include <Calendar.h>
#include <Modes_window.h>
#include <Date.h>

class Menu : public tgui::Panel {
public:
    typedef std::shared_ptr<Menu> Ptr;

    Menu();

    virtual ~Menu();

    void change(const std::map<int, int> &);

    std::map<int, int> getChanged();

    Calendar *getCalendar();

    Modes_window *getModes();

    Date *getDate();

protected:
    tgui::Signal &getSignal(std::string signalName) override;

    void initialize();

public:
    tgui::Signal onValueChange = {"ValueChanged"};
    tgui::Signal onCOMChange = {"COMChanged"};
    tgui::Signal onMODBUSChange = {"MODBUSChanged"};
    tgui::Signal onSaveLogs = {"SaveLogs"};
    tgui::Signal onGetCalendarData = {"GetCalendarData"};

private:
    bool callback_block;
};

#endif // MENU_H
