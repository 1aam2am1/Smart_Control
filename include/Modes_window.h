#ifndef MODES_WINDOW_H
#define MODES_WINDOW_H

#include <TGUI/Panel.hpp>
#include <TGUI/SharedWidgetPtr.hpp>
#include <Modes_data_struct.h>

class Modes_window : public tgui::Panel {
public:
    typedef tgui::SharedWidgetPtr<Modes_window> Ptr;

    Modes_window();

    virtual ~Modes_window();

    void setModesData(const Modes_data_struct &);

    Modes_data_struct getChanged();

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

    struct Modes {
        std::string name;
        int32_t minimum;
        int32_t maximum;
        std::string max_opis;
    };

    const std::vector<Modes> modes;
};

#endif // MODES_WINDOW_H
