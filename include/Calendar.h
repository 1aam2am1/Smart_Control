#ifndef CALENDAR_H
#define CALENDAR_H

#include <TGUI/Panel.hpp>
#include <TGUI/SharedWidgetPtr.hpp>
#include <map>
#include <Action_data_struct.h>
#include <CAL_STATE.h>

class Calendar : public tgui::Panel {
public:
    typedef tgui::SharedWidgetPtr<Calendar> Ptr;

    Calendar();

    virtual ~Calendar();

    void setCAL_STATE(const CAL_STATE &);

    void setActiveDays(uint8_t);

    void change(const std::map<int, std::vector<Action_data_struct>> &);

    std::pair<uint8_t, std::map<int, std::vector<Action_data_struct>>> getChanged();

protected:
    virtual void initialize(Container *const container) override;

public:
    enum SlidersCallbacks {
        ValueChanged = PanelCallbacksCount * 1,     ///< Value changed
        GetData = PanelCallbacksCount * 2,     ///< Need Data
        AllSlidersCallbacks = PanelCallbacksCount * 4 - 1, ///< All triggers defined in Button and its base classes
        SlidersCallbacksCount = PanelCallbacksCount * 4
    };

private:
    void callback(const tgui::Callback &);
};

#endif // CALENDAR_H
