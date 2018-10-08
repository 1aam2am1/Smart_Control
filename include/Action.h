#ifndef ACTION_H
#define ACTION_H

#include <TGUI/Panel.hpp>
#include <TGUI/SharedWidgetPtr.hpp>
#include <map>
#include <Action_data_struct.h>

class Action : public tgui::Panel {
public:
    typedef tgui::SharedWidgetPtr<Action> Ptr;

    Action();

    virtual ~Action();

    void setData(const Action_data_struct &);

    Action_data_struct getData();

protected:
    virtual void initialize(Container *const) override;

public:
    enum SlidersCallbacks {
        ValueChanged = PanelCallbacksCount * 1,     ///< Value changed
        Delete = PanelCallbacksCount * 2,     ///< Delete event
        AllSlidersCallbacks = PanelCallbacksCount * 4 - 1, ///< All triggers defined in Button and its base classes
        SlidersCallbacksCount = PanelCallbacksCount * 4
    };

private:
    bool callback_b;

    void callback(const tgui::Callback &);

    void checkData();

    Action_data_struct result;
};

#endif // ACTION_H
