#ifndef MENU_H
#define MENU_H

#include <TGUI/Panel.hpp>
#include <TGUI/SharedWidgetPtr.hpp>
#include <map>
#include <Calendar.h>
#include <Modes_window.h>
#include <Date.h>

class Menu : public tgui::Panel {
public:
    typedef tgui::SharedWidgetPtr<Menu> Ptr;

    Menu();

    virtual ~Menu();

    void change(const std::map<int, int> &);

    std::map<int, int> getChanged();

    Calendar *getCalendar();

    Modes_window *getModes();

    Date *getDate();

protected:
    virtual void initialize(Container *const container) override;

public:
    enum Main_windowCallbacks {
        ValueChanged = PanelCallbacksCount * 1,     ///< Value changed
        COMChanged = PanelCallbacksCount * 2,     ///< COM changed
        MODBUSChanged = PanelCallbacksCount * 4,     ///< MODBUS changed
        SaveLogs = PanelCallbacksCount * 8,     ///< Save changed
        GetCalendarData = PanelCallbacksCount * 16,     ///< Get Calendar Data
        AllMenuCallbacks = PanelCallbacksCount * 32 - 1, ///< All triggers defined in Button and its base classes
        MenuCallbacksCount = PanelCallbacksCount * 32
    };

private:
    void callback(const tgui::Callback &);
};

#endif // MENU_H
