#ifndef MODES_WINDOW_H
#define MODES_WINDOW_H

#include <TGUI/Widgets/Panel.hpp>

#include <Modes_data_struct.h>

class Modes_window : public tgui::Panel {
public:
    typedef std::shared_ptr<Modes_window> Ptr;

    Modes_window();

    virtual ~Modes_window();

    void change(const std::map<int, int> &);

    std::map<int, int> getChanged();

    void setModesData(const Modes_data_struct &);

    Modes_data_struct getChangedModes();

protected:
    tgui::Signal &getSignal(std::string signalName) override;

    void initialize();

public:
    tgui::Signal onValueChange = {"ValueChanged"};

private:
    bool callback_block;

    struct Modes {
        std::string name;
        int32_t minimum;
        int32_t maximum;
        std::string min_opis;
        std::string max_opis;
    };

    const std::vector<Modes> modes;
};

#endif // MODES_WINDOW_H
