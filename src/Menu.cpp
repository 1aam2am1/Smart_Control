#include "Menu.h"
#include "TGUI/TGUI.hpp"
#include "Funkcje_str.h"
#include "WidgetSingleton.h"
#include "Main_window.h"
#include "Calendar.h"
#include "Console.h"
#include "Date.h"
#include "Modes_window.h"

Menu::Menu() :
        callback_block(false) {
    this->setSize(1200, 820);
    initialize();
}

Menu::~Menu() = default;

void Menu::change(const std::map<int, int> &dane) {
    callback_block = true;

    this->get<Main_window>("p0")->change(dane);
    this->get<Modes_window>("p2")->change(dane);

    callback_block = false;
}

std::map<int, int> Menu::getChanged() {
    std::map<int, int> result;

    result = this->get<Main_window>("p0")->getChanged();
    auto d = this->get<Modes_window>("p2")->getChanged();
    for (const auto it : d) {
        result[it.first] = it.second;
    }

    return result;
}

Calendar *Menu::getCalendar() {
    return this->get<Calendar>("p1").get();
}

Modes_window *Menu::getModes() {
    return this->get<Modes_window>("p2").get();
}

Date *Menu::getDate() {
    return this->get<Date>("date").get();
}

tgui::Signal &Menu::getSignal(std::string signalName) {
    if (signalName == tgui::toLower(onValueChange.getName()))
        return onValueChange;
    else if (signalName == tgui::toLower(onCOMChange.getName()))
        return onCOMChange;
    else if (signalName == tgui::toLower(onMODBUSChange.getName()))
        return onMODBUSChange;
    else if (signalName == tgui::toLower(onSaveLogs.getName()))
        return onSaveLogs;
    else if (signalName == tgui::toLower(onGetCalendarData.getName()))
        return onGetCalendarData;
    else
        return tgui::Panel::getSignal(std::move(signalName));
}

void Menu::initialize() {
    this->getRenderer()->setBackgroundColor(sf::Color::Transparent);

    std::string nazwy[4] = {"Okno glowne", "Kalendarz", "Tryby", "XXX"};

    for (uint32_t i = 0; i < 3; ++i) {
        tgui::Button::Ptr b1 = WidgetSingleton<tgui::Button>::get(*this, "t" + Game_api::convertInt(i));
        //b1->load(THEME_CONFIG_FILE);
        b1->setSize(95, 16);
        b1->setTextSize(12);
        b1->setPosition(i * 95, 0);
        b1->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
        b1->setText(nazwy[i]);
        b1->connect("Pressed", [&, id = i, widget = b1]() {

            for (int32_t it = 0; it < 3; ++it) {
                this->get<tgui::Button>("t" + Game_api::convertInt(it))->getRenderer()->setTextColor(
                        sf::Color(0, 0, 0, 255));
                this->get<tgui::Panel>("p" + Game_api::convertInt(it))->setVisible(false);
            }
            std::dynamic_pointer_cast<tgui::Button>(widget)->getRenderer()->setTextColor(sf::Color(255, 0, 0, 255));
            this->get<tgui::Panel>("p" + Game_api::convertInt(id))->setVisible(true);
        });
    }

    Date::Ptr date = WidgetSingleton<Date>::get(*this, "date");
    date->setPosition(895, 0);
    date->connect("ValueChanged", [&]() {
        if (callback_block) { return; }
        onValueChange.emit(this);
    });


    Main_window::Ptr main_window = WidgetSingleton<Main_window>::get(*this, "p0");
    main_window->setPosition(0, 20);
    main_window->setSize(1200, 800);
    main_window->connect("ValueChanged", [&]() {
        if (callback_block) { return; }
        onValueChange.emit(this);
    });
    main_window->connect("COMChanged", [&]() {
        onCOMChange.emit(this);
    });
    main_window->connect("MODBUSChanged", [&]() {
        if (callback_block) { return; }
        onMODBUSChange.emit(this);
    });
    main_window->connect("SaveLogs", [&]() {
        if (callback_block) { return; }
        onSaveLogs.emit(this);
    });

    Calendar::Ptr calendar = WidgetSingleton<Calendar>::get(*this, "p1");
    calendar->setPosition(0, 20);
    calendar->setSize(1200, 800);
    calendar->connect("ValueChanged", [&]() {
        if (callback_block) { return; }
        onValueChange.emit(this);
    });
    calendar->connect("GetData", [&]() {
        if (callback_block) { return; }
        onGetCalendarData.emit(this);
    });

    Modes_window::Ptr modes = WidgetSingleton<Modes_window>::get(*this, "p2");
    modes->setPosition(0, 20);
    //modes->setSize(1200, 800);
    modes->connect("ValueChanged", [&]() {
        if (callback_block) { return; }
        onValueChange.emit(this);
    });

/*
    Panel::Ptr p3(*this, "p3");
    p3->setSize(1200, 800);
    p3->setPosition(0, 20);
*/
    this->get<tgui::Button>("t0")->onPress.emit(this->get("t0").get(), "");
}
