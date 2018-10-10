#include "Modes_window.h"
#include "TGUI/TGUI.hpp"
#include "Funkcje_str.h"
#include "WidgetSingleton.h"

Modes_window::Modes_window()
        : modes{{"Tryb boost",                       1,  60,  "60m"},
                {"Wietrzenie",                       1,  12,  "12h"},
                {"Sen",                              1,  12,  "12h"},
                {"Urlop",                            1,  30,  "30d"},
                //{"Ograniczenie wydajnosci centrali", 50, 100, "100%"}
} {
    this->setSize(315, 50 * modes.size());
}

Modes_window::~Modes_window() = default;

Modes_data_struct Modes_window::getChanged() {
    Modes_data_struct result;
    result.boost_value = Game_api::convertString(this->get<tgui::EditBox>("e0")->getText());
    result.boost_en = this->get<tgui::Checkbox>("c0")->isChecked();

    result.wierzenie_value = Game_api::convertString(this->get<tgui::EditBox>("e1")->getText());
    result.wierzenie_en = this->get<tgui::Checkbox>("c1")->isChecked();

    result.sen_value = Game_api::convertString(this->get<tgui::EditBox>("e2")->getText());
    result.sen_en = this->get<tgui::Checkbox>("c2")->isChecked();

    result.urlop_value = Game_api::convertString(this->get<tgui::EditBox>("e3")->getText());
    result.urlop_en = this->get<tgui::Checkbox>("c3")->isChecked();
/*
    result.max_went_value = Game_api::convertString(this->get<tgui::EditBox>("e4")->getText());
    result.max_went_en = this->get<tgui::Checkbox>("c4")->isChecked();
*/

    return result;
}

void Modes_window::setModesData(const Modes_data_struct &data) {
    this->unbindGlobalCallback();

    for (uint32_t j = 0; j < modes.size(); ++j) {
        uint32_t value = data.data[j] & 0b01111111;

        this->get<tgui::EditBox>("e" + Game_api::convertInt(j))->
                setText(Game_api::convertInt(value));

        this->get<tgui::Slider>("s" + Game_api::convertInt(j))->
                setValue(value - modes[j].minimum);

        {
            auto ch = this->get<tgui::Checkbox>("c" + Game_api::convertInt(j));

            if (data.data[j] & 0b10000000) {
                ch->check();
            } else {
                ch->uncheck();
            }
        }
    }

    this->bindGlobalCallback(std::bind(&Modes_window::callback, this, std::placeholders::_1));
}

void Modes_window::initialize(Container *const container) {
    this->Panel::initialize(container);
    this->bindGlobalCallback(std::bind(&Modes_window::callback, this, std::placeholders::_1));
    this->setBackgroundColor(sf::Color::White);


    for (uint32_t j = 0; j < modes.size(); ++j) {
        tgui::Label::Ptr name = WidgetSingleton<tgui::Label>::get(*this);
        //name->load(THEME_CONFIG_FILE);
        name->setText(modes[j].name);
        name->setPosition(5, 5 + j * 50);
        name->setTextColor(sf::Color(0, 0, 0, 255));
        name->setTextSize(12);

        tgui::Checkbox::Ptr ch = WidgetSingleton<tgui::Checkbox>::get(*this, "c" + Game_api::convertInt(j));
        ch->setPosition(5, 25 + j * 50);
        ch->setSize(12, 12);
        ch->bindCallback(tgui::Checkbox::Checked | tgui::Checkbox::Unchecked);
        ch->setCallbackId(100 + j);
/*
        tgui::Label::Ptr minimum = WidgetSingleton<tgui::Label>::get(*this);
        //minimum->load(THEME_CONFIG_FILE);
        minimum->setText(Game_api::convertInt(scroll[j].minimum));
        minimum->setPosition(5, 25 + j * 50);
        minimum->setTextColor(sf::Color(0, 0, 0, 255));
        minimum->setTextSize(12);
*/
        tgui::Slider::Ptr scr = WidgetSingleton<tgui::Slider>::get(*this, "s" + Game_api::convertInt(j));
        //scr->load(THEME_CONFIG_FILE);
        scr->setPosition(35, 25 + j * 50);
        scr->setSize(240, 14);
        scr->setMaximum(modes[j].maximum - modes[j].minimum);
        scr->setMinimum(0);
        scr->bindCallback(tgui::Slider::ValueChanged);
        scr->setCallbackId(200 + j);

        tgui::Label::Ptr maximum = WidgetSingleton<tgui::Label>::get(*this);
        //maximum->load(THEME_CONFIG_FILE);
        maximum->setText(modes[j].max_opis);
        maximum->setPosition(280, 25 + j * 50);
        maximum->setTextColor(sf::Color(0, 0, 0, 255));
        maximum->setTextSize(12);

        tgui::EditBox::Ptr box = WidgetSingleton<tgui::EditBox>::get(*this, "e" + Game_api::convertInt(j));
        //box->load(THEME_CONFIG_FILE);
        box->setPosition(275, 5 + j * 50);
        box->setSize(37, 15);
        box->setNumbersOnly(true);
        box->setText(Game_api::convertInt(modes[j].minimum));
        box->bindCallback(tgui::EditBox::TextChanged);
        box->setCallbackId(300 + j);
    }
}

void Modes_window::callback(const tgui::Callback &callback) {
    if (callback.id / 100 == 1) ///checkbox
    {

    }
    if (callback.id / 100 == 2) ///slider
    {
        uint32_t scroll_id = callback.id - 200;
        this->get<tgui::EditBox>("e" + Game_api::convertInt(scroll_id), true)->
                setText(Game_api::convertInt(callback.value + modes[scroll_id].minimum));
    }
    if (callback.id / 100 == 3) ///editbox
    {
        uint32_t scroll_id = callback.id - 300;
        this->get<tgui::Slider>("s" + Game_api::convertInt(scroll_id), true)->
                setValue(Game_api::convertString(callback.text) - modes[scroll_id].minimum);
    }

    m_Callback.trigger = Modes_window::ValueChanged;
    addCallback();
}
