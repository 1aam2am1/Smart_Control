#include "Day.h"
#include "TGUI/TGUI.hpp"
#include "Funkcje_str.h"
#include "WidgetSingleton.h"
#include "Action.h"

constexpr const int &max_action_count = 5;

Day::Day() :
        action_cout(0),
        old_value_scrollbar(0) {
    this->setSize(160, 770);
}

Day::~Day() = default;

void Day::setActive(bool flag) {
    auto b1 = this->get<tgui::Button>("t1");
    b1->setTextColor(sf::Color(0, 255 * flag, 0, 255));

    if (flag) {
        this->setBackgroundColor(sf::Color::Green);
    } else {
        this->setBackgroundColor(sf::Color::White);
    }
}

void Day::change(const std::vector<Action_data_struct> &dane) {
    this->get<tgui::Panel>("p0")->removeAllWidgets();

    this->action_cout = 0;

    for (auto &&it : dane) {
        this->addAction(it);
    }
}

std::pair<bool, std::vector<Action_data_struct>> Day::getChanged() {
    std::pair<bool, std::vector<Action_data_struct>> result;

    result.first = (this->get<tgui::Button>("t1")->getTextColor() == sf::Color::Green);

    for (auto &&it : this->get<tgui::Panel>("p0")->getWidgets()) {
        result.second.emplace_back(dynamic_cast<Action *>(it.get())->getData());
    }

    return result;
}

void Day::setName(const std::string &name) {
    this->get<tgui::Label>("name")->setText(name);
}

void Day::mouseWheelMoved(int delta, int x, int y) {
    this->get<tgui::Scrollbar>("scr")->mouseWheelMoved(delta, x, y);
}

void Day::addAction(const Action_data_struct &data) {
    if (action_cout >= max_action_count) { return; }

    Action::Ptr action(*(this->get<tgui::Panel>("p0")), "a" + Game_api::convertInt(action_cout));
    action->setPosition(0, (action->getSize().y + 10) * action_cout - this->old_value_scrollbar);
    action->setData(data);
    action->bindCallback(Action::ValueChanged | Action::Delete);
    action->setCallbackId(4);

    ++action_cout;

    this->calculateScrollbar();
}

void Day::initialize(Container *const container) {
    this->Panel::initialize(container);
    this->bindGlobalCallback(std::bind(&Day::callback, this, std::placeholders::_1));
    this->setBackgroundColor(sf::Color::White);

    tgui::Label::Ptr name = WidgetSingleton<tgui::Label>::get(*this, "name");
    //name->load(THEME_CONFIG_FILE);
    name->setPosition(5, 5);
    name->setTextColor(sf::Color(0, 0, 0, 255));
    name->setTextSize(12);

    tgui::Button::Ptr b1 = WidgetSingleton<tgui::Button>::get(*this, "t0");
    b1->setSize(16, 16);
    b1->setTextSize(12);
    b1->setPosition(80, 1);
    b1->setTextColor(sf::Color(0, 0, 0, 255));
    b1->setText("+");
    b1->bindCallback(tgui::Button::LeftMouseClicked);
    b1->setCallbackId(1);

    tgui::Button::Ptr b2 = WidgetSingleton<tgui::Button>::get(*this, "t1");
    b2->setSize(65, 16);
    b2->setTextSize(12);
    b2->setPosition(95, 1);
    b2->setTextColor(sf::Color(0, 0, 0, 255));
    b2->setText("Aktywny");
    b2->bindCallback(tgui::Button::LeftMouseClicked);
    b2->setCallbackId(2);

    tgui::Scrollbar::Ptr scrollbar = WidgetSingleton<tgui::Scrollbar>::get(*this, "scr");
    //scrollbar->load(THEME_CONFIG_FILE);
    scrollbar->setVerticalScroll(true);
    scrollbar->setPosition(140, 20);
    scrollbar->setSize(20, 740);
    scrollbar->setArrowScrollAmount(15);
    scrollbar->bindCallback(tgui::Scrollbar::ValueChanged);
    scrollbar->setCallbackId(3);

    tgui::Panel::Ptr p1 = WidgetSingleton<tgui::Panel>::get(*this, "p0");
    p1->setPosition(10, 20);
    p1->setSize(120, 740);
    p1->setBackgroundColor(sf::Color::Transparent);


    this->calculateScrollbar();
}

void Day::calculateScrollbar() {
    tgui::Scrollbar::Ptr scrollbar = this->get<tgui::Scrollbar>("scr");
    scrollbar->setLowValue(740);///rozmiar jednej strony


    int32_t diff = 210 * this->action_cout;


    int32_t value = scrollbar->getValue();
    int32_t smaxx = scrollbar->getMaximum();

    scrollbar->setMaximum(diff);///rozmiar wszystkich danych

    if (value + 740 >= smaxx) {
        scrollbar->setValue(scrollbar->getMaximum() - 740);
    }
}

void Day::delAction(tgui::Widget *widget) {
    std::vector<Action_data_struct> data;
    data.reserve(max_action_count);

    for (auto &&it : this->get<tgui::Panel>("p0")->getWidgets()) {
        if (it.get() != widget) {
            data.emplace_back(dynamic_cast<Action *>(it.get())->getData());
        }
    }

    this->get<tgui::Panel>("p0")->removeAllWidgets();

    this->action_cout = 0;

    for (auto &&it : data) {
        this->addAction(it);
    }
}

void Day::callback(const tgui::Callback &callback) {
    if (callback.id == 1) ///add action
    {
        this->addAction({});
    }
    if (callback.id == 2) ///aktywny
    {
        auto b1 = this->get<tgui::Button>("t1");
        b1->setTextColor(sf::Color(0, 255 * (b1->getTextColor() == sf::Color::Black ? 1 : 0), 0, 255));

        if (b1->getTextColor() == sf::Color::Black) {
            this->setBackgroundColor(sf::Color::White);
        } else {
            this->setBackgroundColor(sf::Color::Green);
        }

        m_Callback.trigger = Day::ValueChanged;
        addCallback();
    }
    if (callback.id == 3) ///scrollbar
    {
        const std::vector<Widget::Ptr> &widgets = this->get<tgui::Panel>("p0")->getWidgets();

        for (auto &&val: widgets) {
            val->move(0, old_value_scrollbar - callback.value);
        }
        old_value_scrollbar = callback.value;
    }
    if (callback.id == 4) ///value changed or delete
    {
        if (callback.trigger == Action::Delete) {
            this->delAction(callback.widget);
        }

        m_Callback.trigger = Day::ValueChanged;
        addCallback();
    }
}
