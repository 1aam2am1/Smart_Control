#ifndef DAY_H
#define DAY_H

#include <TGUI/Widgets/Panel.hpp>

#include <map>
#include <Action_data_struct.h>

class Day : public tgui::Panel {
public:
    typedef std::shared_ptr<Day> Ptr;

    Day();

    virtual ~Day();

    void setActive(bool);

    void change(const std::vector<Action_data_struct> &);

    std::pair<bool, std::vector<Action_data_struct>> getChanged();

    void setName(const std::string &);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \internal
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool mouseWheelScrolled(float delta, tgui::Vector2f pos) override;

    void addAction(const Action_data_struct &);

protected:
    tgui::Signal &getSignal(std::string signalName) override;

    void initialize();

    void calculateScrollbar();

    void delAction(const tgui::Widget::Ptr &);

public:
    tgui::Signal onValueChange = {"ValueChanged"};

private:
    uint32_t action_cout;
    int32_t old_value_scrollbar;
};

#endif // DAY_H
