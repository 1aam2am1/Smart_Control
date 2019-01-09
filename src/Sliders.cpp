#include "Sliders.h"
#include "TGUI/TGUI.hpp"
#include "Funkcje_str.h"
#include "WidgetSingleton.h"

Sliders::Sliders()
        : scroll{{"Max temperatura",            45,  55,  0},
                 {"Bypass czerpnia (minimum)",  14,  20,  2},
                 {"Bypass wyrzutnia (maximum)", 21,  26,  3},
                 {"Odmrazanie",                 -5,  5,   4},
                 {"Dogrzewanie",                16,  28,  5},
                 {"Czas odmrazania",            1,   60,  14},
                 {"Przerwa odmrazania",         1,   180, 15},
                 {"Moc odmrazania",             0,   100, 17},

                 {"Predkosc serwa",             1,   10,  6},
                 {"S. wyrzut zamkniete",        0,   250, 7},
                 {"S. wyrzut otwarte",          0,   250, 8},
                 {"S. nawiew zamkniete",     0,   250, 9},
                 {"S. nawiew otwarte",       0,   250, 10},
                 {"S. bypas zamkniete",      0,   250, 11},
                 {"S. bypas otwarte",        0,   250, 12},
                 {"",                        0,   0,   -1},

                 {"Moc wentylacji (reczny)",    0,   100, 1},
                 {"Odmrazanie standard",        -40, 40,  16},
                 {"Moc wentylacji (modbus)",    0,   100, 46},
                 {"Cholodzenie",                13,  26,  -1}
} {

}


Sliders::~Sliders() = default;

void Sliders::change(const std::map<int, int> &dane) {
    callback_block = true;
    for (auto i : dane) {
        for (uint32_t j = 0; j < scroll.size(); ++j) {
            if (i.first == scroll[j].id) {
                int value = i.second;

                if (scroll[j].minimum < 0) {
                    if ((value >> 8)) { ///16bit
                        value = static_cast<int16_t>(value);
                    } else { ///8bit
                        value = static_cast<int8_t>(value);
                    }
                }

                this->get<tgui::EditBox>("e" + Game_api::convertInt(j))->
                        setText(Game_api::convertInt(value));

                this->get<tgui::Slider>("s" + Game_api::convertInt(j))->
                        setValue(value - scroll[j].minimum);

                break;
            }
        }
    }
    callback_block = false;
}

std::map<int, int> Sliders::getChanged() {
    std::map<int, int> result;

    for (uint32_t j = 0; j < scroll.size(); ++j) {
        if (scroll[j].name.empty()) { continue; }
        if (scroll[j].id == -1) { continue; }

        sf::String str = this->get<tgui::EditBox>("e" + Game_api::convertInt(j))->getText();

        int32_t value = Game_api::convertString(str);

        result[scroll[j].id] = value;
    }

    return result;
}

tgui::Signal &Sliders::getSignal(std::string signalName) {
    if (signalName == tgui::toLower(onValueChange.getName()))
        return onValueChange;
    else
        return tgui::Panel::getSignal(std::move(signalName));
}

void Sliders::initialize() {

    const std::string nazwy[3] = {"Ustawienia", "Serwa", "Ust. dodatkowe"};

    for (uint32_t i = 0; i < 3; ++i) {
        tgui::Button::Ptr b1 = WidgetSingleton<tgui::Button>::get(*this, "t" + Game_api::convertInt(i));
        //b1->load(THEME_CONFIG_FILE);
        b1->setSize(95, 16);
        b1->setTextSize(12);
        b1->setPosition(10 + i * 105, 10);
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

    for (uint32_t i = 0; i < 3; ++i) {
        tgui::Panel::Ptr p1 = WidgetSingleton<tgui::Panel>::get(*this, "p" + Game_api::convertInt(i));
        p1->setPosition(10, 40);
        p1->setSize(315, 400);
        p1->getRenderer()->setBackgroundColor(sf::Color::Transparent);

        for (uint32_t j = 0; j < 8 && (j + i * 8 < scroll.size()); ++j) {
            uint32_t scroll_id = j + i * 8;

            if (scroll[scroll_id].name.empty()) { continue; }

            tgui::Label::Ptr name = WidgetSingleton<tgui::Label>::get(*p1);
            //name->load(THEME_CONFIG_FILE);
            name->setText(scroll[scroll_id].name);
            name->setPosition(5, 5 + j * 50);
            name->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
            name->setTextSize(12);

            tgui::Label::Ptr minimum = WidgetSingleton<tgui::Label>::get(*p1);
            //minimum->load(THEME_CONFIG_FILE);
            minimum->setText(Game_api::convertInt(scroll[scroll_id].minimum));
            minimum->setPosition(5, 25 + j * 50);
            minimum->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
            minimum->setTextSize(12);

            tgui::Slider::Ptr scr = WidgetSingleton<tgui::Slider>::get(*p1, "s" + Game_api::convertInt(scroll_id));
            tgui::EditBox::Ptr box = WidgetSingleton<tgui::EditBox>::get(*p1, "e" + Game_api::convertInt(scroll_id));
            //scr->load(THEME_CONFIG_FILE);
            scr->setPosition(35, 25 + j * 50);
            scr->setSize(240, 14);
            scr->setMaximum(scroll[scroll_id].maximum - scroll[scroll_id].minimum);
            scr->setMinimum(0);
            scr->connect("ValueChanged", [&, box, scroll_id = j](float value) {
                if (callback_block) { return; }
                box->setText(Game_api::convertInt(value + scroll[scroll_id].minimum));

                onValueChange.emit(this);
            });

            tgui::Label::Ptr maximum = WidgetSingleton<tgui::Label>::get(*p1);
            //maximum->load(THEME_CONFIG_FILE);
            maximum->setText(Game_api::convertInt(scroll[scroll_id].maximum));
            maximum->setPosition(280, 25 + j * 50);
            maximum->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
            maximum->setTextSize(12);

            //box->load(THEME_CONFIG_FILE);
            box->setPosition(275, 5 + j * 50);
            box->setSize(37, 15);
            box->setInputValidator(tgui::EditBox::Validator::Int);
            box->setText(Game_api::convertInt(scroll[scroll_id].minimum));
            box->connect("TextChanged", [&, scr, scroll_id = j](const sf::String &text) {
                if (callback_block) { return; }
                scr->setValue(Game_api::convertString(text) - scroll[scroll_id].minimum);

                onValueChange.emit(this);
            });
        }
        p1->setVisible(false);
    }
    this->get<tgui::Button>("t0")->onPress.emit(this->get("t0").get(), "");
}


