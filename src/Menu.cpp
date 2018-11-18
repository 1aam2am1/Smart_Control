#include "Menu.h"
#include "TGUI/TGUI.hpp"
#include "Funkcje_str.h"
#include "WidgetSingleton.h"
#include "Main_window.h"
#include "Calendar.h"
#include "Console.h"
#include "Date.h"
#include "Modes_window.h"

Menu::Menu() {
    this->setSize(1200, 820);
}

Menu::~Menu() = default;

void Menu::change(const std::map<int, int> &dane) {
    this->unbindGlobalCallback();

    this->get<Main_window>("p0")->change(dane);

    this->bindGlobalCallback(std::bind(&Menu::callback, this, std::placeholders::_1));
}

std::map<int, int> Menu::getChanged() {
    std::map<int, int> result;

    result = this->get<Main_window>("p0")->getChanged();

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

void Menu::initialize(Container *const container) {
    this->Panel::initialize(container);
    this->bindGlobalCallback(std::bind(&Menu::callback, this, std::placeholders::_1));
    this->setBackgroundColor(sf::Color::Transparent);

    std::string nazwy[4] = {"Okno glowne", "Kalendarz", "Tryby", "XXX"};

    for (uint32_t i = 0; i < 3; ++i) {
        tgui::Button::Ptr b1 = WidgetSingleton<tgui::Button>::get(*this, "t" + Game_api::convertInt(i));
        //b1->load(THEME_CONFIG_FILE);
        b1->setSize(95, 16);
        b1->setTextSize(12);
        b1->setPosition(i * 95, 0);
        b1->setTextColor(sf::Color(0, 0, 0, 255));
        b1->setText(nazwy[i]);
        b1->bindCallback(tgui::Button::LeftMouseClicked);
        b1->setCallbackId(100 + i);
    }

    Date::Ptr date(*this, "date");
    date->setPosition(895, 0);
    date->bindCallback(Date::ValueChanged);
    date->setCallbackId(1);


    Main_window::Ptr main_window(*this, "p0");
    main_window->setPosition(0, 20);
    main_window->setSize(1200, 800);
    main_window->bindCallback(
            Main_window::ValueChanged | Main_window::COMChanged | Main_window::MODBUSChanged | Main_window::SaveLogs);
    main_window->setCallbackId(200);

    Calendar::Ptr calendar(*this, "p1");
    calendar->setPosition(0, 20);
    calendar->setSize(1200, 800);
    calendar->bindCallback(Calendar::ValueChanged | Calendar::GetData);
    calendar->setCallbackId(300);

    Modes_window::Ptr modes(*this, "p2");
    modes->setPosition(0, 20);
    //modes->setSize(1200, 800);
    modes->bindCallback(Modes_window::ValueChanged);
    modes->setCallbackId(400);

/*
    Panel::Ptr p3(*this, "p3");
    p3->setSize(1200, 800);
    p3->setPosition(0, 20);
*/
    tgui::Callback call;
    call.id = 100;
    call.widget = this->get("t0").get();

    callback(call);
}

void Menu::callback(const tgui::Callback &callback) {
    if (callback.id == 1) ///date
    {
        m_Callback.trigger = Menu::ValueChanged;
        addCallback();
    }
    if (callback.id / 100 == 1) ///tab
    {
        ///kolorowanie tab
        ///howanie i pokazywanie p...
        for (int32_t i = 0; i < 3; ++i) {
            this->get<tgui::Button>("t" + Game_api::convertInt(i))->setTextColor(sf::Color(0, 0, 0, 255));
            this->get<tgui::Panel>("p" + Game_api::convertInt(i))->hide();
        }
        dynamic_cast<tgui::Button *>(callback.widget)->setTextColor(sf::Color(255, 0, 0, 255));
        this->get<tgui::Panel>("p" + Game_api::convertInt(callback.id % 100))->show();
    }
    if (callback.id == 200) ///main window
    {
        switch (callback.trigger) {
            case Main_window::ValueChanged:
                m_Callback.trigger = Menu::ValueChanged;
                break;
            case Main_window::COMChanged:
                m_Callback.trigger = Menu::COMChanged;
                break;
            case Main_window::MODBUSChanged:
                m_Callback.checked = callback.checked;
                m_Callback.trigger = Menu::MODBUSChanged;
                break;
            case Main_window::SaveLogs:
                m_Callback.trigger = Menu::SaveLogs;
                break;
            default:
                Console::printf(Console::ERROR_MESSAGE, "Something goes wrong menu main_window trigger case\n");
                return;
        }
        addCallback();
    }
    if (callback.id == 300) ///calendar
    {
        switch (callback.trigger) {
            case Calendar::ValueChanged:
                m_Callback.trigger = Menu::ValueChanged;
                break;
            case Calendar::GetData:
                m_Callback.trigger = Menu::GetCalendarData;
                break;
            default:
                Console::printf(Console::ERROR_MESSAGE, "Something goes wrong menu calendar trigger case\n");
                return;
        }
        addCallback();
    }
    if (callback.id == 400) ///modes
    {
        m_Callback.trigger = Menu::ValueChanged;
        addCallback();
    }
}
