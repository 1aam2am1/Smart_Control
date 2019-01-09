#include "Date.h"
#include "TGUI/TGUI.hpp"
#include "Funkcje_str.h"
#include "WidgetSingleton.h"

Date::Date() :
        callback_block(false) {
    this->setSize(300, 15);
    initialize();
}

Date::~Date() = default;

void Date::setDateData(const Date_data_struct &date) {
    callback_block = true;

    this->result = date;

    this->get<tgui::EditBox>("h")->setText(Game_api::convertInt(gethours(date)));
    this->get<tgui::EditBox>("m")->setText(Game_api::convertInt(getminutes(date)));
    this->get<tgui::ComboBox>("day")->setSelectedItem(date.weekends);

    callback_block = false;
}

Date_data_struct Date::getChanged() {
    sethours(result, Game_api::convertString(this->get<tgui::EditBox>("h")->getText()));
    setminutes(result, Game_api::convertString(this->get<tgui::EditBox>("m")->getText()));
    result.weekends = this->get<tgui::ComboBox>("day")->getSelectedItemIndex();

    return result;
}

tgui::Signal &Date::getSignal(std::string signalName) {
    if (signalName == tgui::toLower(onValueChange.getName()))
        return onValueChange;
    else
        return tgui::Panel::getSignal(std::move(signalName));
}

void Date::initialize() {
    this->getRenderer()->setBackgroundColor(sf::Color::Transparent);

    tgui::EditBox::Ptr hour = WidgetSingleton<tgui::EditBox>::get(*this, "h");
    //hour->load(THEME_CONFIG_FILE);
    hour->setPosition(0, 0);
    hour->setSize(37, 15);
    hour->setInputValidator(tgui::EditBox::Validator::Int);
    hour->setMaximumCharacters(2);
    hour->setText("0");
    hour->connect("TextChanged", &Date::callback, this, 1);

    tgui::Label::Ptr colon1 = WidgetSingleton<tgui::Label>::get(*this);
    //name->load(THEME_CONFIG_FILE);
    colon1->setText(":");
    colon1->setPosition(40, 4);
    colon1->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
    colon1->setTextSize(12);
    colon1->getRenderer()->setTextStyle(sf::Text::Bold);

    tgui::EditBox::Ptr minets = WidgetSingleton<tgui::EditBox>::get(*this, "m");
    //minets->load(THEME_CONFIG_FILE);
    minets->setPosition(45, 0);
    minets->setSize(37, 15);
    minets->setInputValidator(tgui::EditBox::Validator::Int);
    minets->setMaximumCharacters(2);
    minets->setText("0");
    minets->connect("TextChanged", &Date::callback, this, 2);

    const std::string nazwy[7] = {"Poniedzialek", "Wtorek", "Sroda", "Czwartek", "Piatek", "Sobota", "Niedziela"};

    tgui::ComboBox::Ptr day = WidgetSingleton<tgui::ComboBox>::get(*this, "day");
    day->setPosition(90, 0);
    day->setSize(100, 15);
    day->connect("ItemSelected", &Date::callback, this, 3);

    for (const auto &i : nazwy) {
        day->addItem(i);
    }

    tgui::Button::Ptr b1 = WidgetSingleton<tgui::Button>::get(*this, "b1");
    //b1->load(THEME_CONFIG_FILE);
    b1->setSize(95, 16);
    b1->setTextSize(12);
    b1->setPosition(200, 0);
    b1->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
    b1->setText("Ustaw zegar");
    b1->connect("Pressed", &Date::callback, this, 4);

}
// TODO (Michał Marszałek#1#09/23/18): Zabezpiecz przed 25 godzina

void Date::callback(const int id) {
    if (callback_block) { return; }

    if (id == 4) ///date
    {
        onValueChange.emit(this);
    }
}
