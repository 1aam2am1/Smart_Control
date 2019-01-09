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
    initialize();
}

Day::~Day() = default;

void Day::setActive(bool flag) {
    auto b1 = this->get<tgui::Button>("t1");
    b1->getRenderer()->setTextColor(sf::Color(0, 255 * flag, 0, 255));

    if (flag) {
        this->getRenderer()->setBackgroundColor(sf::Color::Green);
    } else {
        this->getRenderer()->setBackgroundColor(sf::Color::White);
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

    result.first = (this->get<tgui::Button>("t1")->getRenderer()->getTextColor() == sf::Color::Green);

    for (auto &&it : this->get<tgui::Panel>("p0")->getWidgets()) {
        result.second.emplace_back(dynamic_cast<Action *>(it.get())->getData());
    }

    return result;
}

void Day::setName(const std::string &name) {
    this->get<tgui::Label>("name")->setText(name);
}

bool Day::mouseWheelScrolled(float delta, tgui::Vector2f pos) {
    return this->get<tgui::Scrollbar>("scr")->mouseWheelScrolled(delta, pos);
}

void Day::addAction(const Action_data_struct &data) {
    if (action_cout >= max_action_count) { return; }

    Action::Ptr action = WidgetSingleton<Action>::get(*(this->get<tgui::Panel>("p0")),
                                                      "a" + Game_api::convertInt(action_cout));
    action->setPosition(0, (action->getSize().y + 10) * action_cout - this->old_value_scrollbar);
    action->setData(data);
    action->connect("ValueChanged", [&]() { onValueChange.emit(this); });
    action->connect("Deleted", [&, widget = action]() {
        this->delAction(widget);
        onValueChange.emit(this);
    });

    ++action_cout;

    this->calculateScrollbar();
}

tgui::Signal &Day::getSignal(std::string signalName) {
    if (signalName == tgui::toLower(onValueChange.getName()))
        return onValueChange;
    else
        return tgui::Panel::getSignal(std::move(signalName));
}

void Day::initialize() {
    this->getRenderer()->setBackgroundColor(sf::Color::White);

    tgui::Label::Ptr name = WidgetSingleton<tgui::Label>::get(*this, "name");
    //name->load(THEME_CONFIG_FILE);
    name->setPosition(5, 5);
    name->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
    name->setTextSize(12);

    tgui::Button::Ptr b1 = WidgetSingleton<tgui::Button>::get(*this, "t0");
    b1->setSize(16, 16);
    b1->setTextSize(12);
    b1->setPosition(80, 1);
    b1->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
    b1->setText("+");
    b1->connect("Pressed", [&]() { this->addAction({}); });

    tgui::Button::Ptr b2 = WidgetSingleton<tgui::Button>::get(*this, "t1");
    b2->setSize(65, 16);
    b2->setTextSize(12);
    b2->setPosition(95, 1);
    b2->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
    b2->setText("Aktywny");
    b2->connect("Pressed", [&]() {
        auto b1 = this->get<tgui::Button>("t1");
        b1->getRenderer()->setTextColor(
                sf::Color(0, 255 * (b1->getRenderer()->getTextColor() == sf::Color::Black ? 1 : 0), 0, 255));

        if (b1->getRenderer()->getTextColor() == sf::Color::Black) {
            this->getRenderer()->setBackgroundColor(sf::Color::White);
        } else {
            this->getRenderer()->setBackgroundColor(sf::Color::Green);
        }

        onValueChange.emit(this);
    });

    tgui::Scrollbar::Ptr scrollbar = WidgetSingleton<tgui::Scrollbar>::get(*this, "scr");
    //scrollbar->load(THEME_CONFIG_FILE);
    scrollbar->setPosition(140, 20);
    scrollbar->setSize(20, 740);
    scrollbar->setViewportSize(15);
    scrollbar->connect("ValueChanged", [&](uint32_t value) {
        const std::vector<Widget::Ptr> &widgets = this->get<tgui::Panel>("p0")->getWidgets();

        for (auto &&val: widgets) {
            val->setPosition(0, val->getPosition().y + old_value_scrollbar - value);
        }
        old_value_scrollbar = value;
    });

    tgui::Panel::Ptr p1 = WidgetSingleton<tgui::Panel>::get(*this, "p0");
    p1->setPosition(10, 20);
    p1->setSize(120, 740);
    p1->getRenderer()->setBackgroundColor(sf::Color::Transparent);


    this->calculateScrollbar();
}

void Day::calculateScrollbar() {
    tgui::Scrollbar::Ptr scrollbar = this->get<tgui::Scrollbar>("scr");
    scrollbar->setViewportSize(740);///rozmiar jednej strony


    int32_t diff = 210 * this->action_cout;


    int32_t value = scrollbar->getValue();
    int32_t smaxx = scrollbar->getMaximum();

    scrollbar->setMaximum(diff);///rozmiar wszystkich danych

    if (value + 740 >= smaxx) {
        scrollbar->setValue(scrollbar->getMaximum() - 740);
    }
}

void Day::delAction(const tgui::Widget::Ptr &widget) {
    std::vector<Action_data_struct> data;
    data.reserve(max_action_count);

    for (auto &&it : this->get<tgui::Panel>("p0")->getWidgets()) {
        if (it != widget) {
            data.emplace_back(dynamic_cast<Action *>(it.get())->getData());
        }
    }

    this->get<tgui::Panel>("p0")->removeAllWidgets();

    this->action_cout = 0;

    for (auto &&it : data) {
        this->addAction(it);
    }
}

