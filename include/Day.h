#ifndef DAY_H
#define DAY_H

#include <TGUI/Panel.hpp>
#include <TGUI/SharedWidgetPtr.hpp>
#include <map>
#include <Action_data_struct.h>

class Day : public tgui::Panel {
public:
    typedef tgui::SharedWidgetPtr<Day> Ptr;

    Day();

    virtual ~Day();

    void setActive(bool);

    void change(const std::vector<Action_data_struct> &);

    std::pair<bool, std::vector<Action_data_struct>> getChanged();

    void setName(const std::string &);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \internal
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void mouseWheelMoved(int delta, int x, int y);

    void addAction(const Action_data_struct &);

protected:
    virtual void initialize(Container *const) override;

    void calculateScrollbar();

    void delAction(tgui::Widget *);

public:
    enum SlidersCallbacks {
        ValueChanged = PanelCallbacksCount * 1,     ///< Value changed
        AllSlidersCallbacks = PanelCallbacksCount * 2 - 1, ///< All triggers defined in Button and its base classes
        SlidersCallbacksCount = PanelCallbacksCount * 2
    };

private:
    void callback(const tgui::Callback &);

    uint32_t action_cout;
    int32_t old_value_scrollbar;
};

#endif // DAY_H
