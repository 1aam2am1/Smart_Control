#ifndef DATE_H
#define DATE_H

#include <TGUI/Panel.hpp>
#include <TGUI/SharedWidgetPtr.hpp>
#include <Date_data_struct.h>

class Date : public tgui::Panel {
public:
    typedef tgui::SharedWidgetPtr<Date> Ptr;

    Date();

    virtual ~Date();

    void setDateData(const Date_data_struct &);

    Date_data_struct getChanged();

protected:
    virtual void initialize(Container *const) override;

public:
    enum SlidersCallbacks {
        ValueChanged = PanelCallbacksCount * 1,     ///< Value changed
        AllSlidersCallbacks = PanelCallbacksCount * 2 - 1, ///< All triggers defined in Button and its base classes
        SlidersCallbacksCount = PanelCallbacksCount * 2
    };

private:
    void callback(const tgui::Callback &);

    Date_data_struct result;
};

#endif // DATE_H
