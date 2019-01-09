#ifndef SCROLLBARS_H
#define SCROLLBARS_H

#include <TGUI/Widgets/Panel.hpp>

#include <map>

class Sliders : public tgui::Panel {
public:
    typedef std::shared_ptr<Sliders> Ptr;

    Sliders();

    virtual ~Sliders();

    void change(const std::map<int, int> &);

    std::map<int, int> getChanged();

protected:
    tgui::Signal &getSignal(std::string signalName) override;

    void initialize();

public:
    tgui::Signal onValueChange = {"ValueChanged"};

private:
    bool callback_block;

    struct Scroll {
        std::string name;
        int32_t minimum;
        int32_t maximum;
        int32_t id;
    };

    const std::vector<Scroll> scroll;
};

#endif // SCROLLBARS_H
