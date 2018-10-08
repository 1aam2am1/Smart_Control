#ifndef SCROLLBARS_H
#define SCROLLBARS_H

#include <TGUI/Panel.hpp>
#include <TGUI/SharedWidgetPtr.hpp>
#include <map>

class Sliders : public tgui::Panel {
public:
    typedef tgui::SharedWidgetPtr<Sliders> Ptr;

    Sliders();

    virtual ~Sliders();

    void change(const std::map<int, int> &);

    std::map<int, int> getChanged();

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

    struct Scroll {
        std::string name;
        int32_t minimum;
        int32_t maximum;
        int32_t id;
    };

    const std::vector<Scroll> scroll;
};

#endif // SCROLLBARS_H
