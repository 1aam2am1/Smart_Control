#include "Modes_window.h"
#include "TGUI/TGUI.hpp"
#include "Funkcje_str.h"
#include "WidgetSingleton.h"

Modes_window::Modes_window()
        : modes{{"Tryb boost",                          1,  60,  "1m",  "60m"},
                {"Wietrzenie",                          1,  12,  "1h",  "12h"},
                {"Sen",                                 1,  12,  "1h",  "12h"},
                {"Kominek",                             1,  60,  "1m",  "60m"},
                {"Ograniczenie wydajnosci centrali",    50, 100, "50%", "100%"},
                {"Ograniczenie wentylatora nawiewnego", 50, 100, "50%", "100%"},
                {"Ograniczenie wentylatora wywiewnego", 50, 100, "50%", "100%"},
} {
    this->setSize(315, 350);
}

Modes_window::~Modes_window() = default;

void Modes_window::change(const std::map<int, int> &data) {
    this->unbindGlobalCallback();
    this->bindGlobalCallback([](const tgui::Callback &) -> void {});

    auto it = data.find(59);
    if (it != data.end()) {
        this->get<tgui::EditBox>("e5")->
                setText(Game_api::convertInt(it->second));

        this->get<tgui::Slider>("s5")->
                setValue(static_cast<unsigned int>(it->second - modes[5].minimum));
    }

    it = data.find(60);
    if (it != data.end()) {
        this->get<tgui::EditBox>("e6")->
                setText(Game_api::convertInt(it->second));

        this->get<tgui::Slider>("s6")->
                setValue(static_cast<unsigned int>(it->second - modes[5].minimum));
    }

    this->bindGlobalCallback(std::bind(&Modes_window::callback, this, std::placeholders::_1));
}

std::map<int, int> Modes_window::getChanged() {
    std::map<int, int> result;

    result[59] = Game_api::convertString(this->get<tgui::EditBox>("e5")->getText());
    result[60] = Game_api::convertString(this->get<tgui::EditBox>("e6")->getText());

    return result;
}

Modes_data_struct Modes_window::getChangedModes() {
    Modes_data_struct result;
    result.boost_value = Game_api::convertString(this->get<tgui::EditBox>("e0")->getText());
    result.boost_en = this->get<tgui::CheckBox>("c0")->isChecked();

    result.wierzenie_value = Game_api::convertString(this->get<tgui::EditBox>("e1")->getText());
    result.wierzenie_en = this->get<tgui::CheckBox>("c1")->isChecked();

    result.sen_value = Game_api::convertString(this->get<tgui::EditBox>("e2")->getText());
    result.sen_en = this->get<tgui::CheckBox>("c2")->isChecked();

    result.urlop_value = Game_api::convertString(this->get<tgui::EditBox>("e3")->getText());
    result.urlop_en = this->get<tgui::CheckBox>("c3")->isChecked();

    result.max_went_value = Game_api::convertString(this->get<tgui::EditBox>("e4")->getText());
    result.max_went_en = this->get<tgui::CheckBox>("c4")->isChecked();


    return result;
}

void Modes_window::setModesData(const Modes_data_struct &data) {
    callback_block = true;

    for (uint32_t j = 0; j < 5; ++j) {
        uint32_t value = data.data[j] & 0b01111111;

        this->get<tgui::EditBox>("e" + Game_api::convertInt(j))->
                setText(Game_api::convertInt(value));

        this->get<tgui::Slider>("s" + Game_api::convertInt(j))->
                setValue(value - modes[j].minimum);

        {
            auto ch = this->get<tgui::CheckBox>("c" + Game_api::convertInt(j));

            ch->setChecked((data.data[j] & 0b10000000) != 0);
        }
    }

    callback_block = false;
}

tgui::Signal &Modes_window::getSignal(std::string signalName) {
    if (signalName == tgui::toLower(onValueChange.getName()))
        return onValueChange;
    else
        return tgui::Panel::getSignal(std::move(signalName));
}


void Modes_window::initialize() {
    this->getRenderer()->setBackgroundColor(sf::Color::White);


    for (uint32_t j = 0; j < modes.size(); ++j) {
        tgui::Label::Ptr name = WidgetSingleton<tgui::Label>::get(*this);
        //name->load(THEME_CONFIG_FILE);
        name->setText(modes[j].name);
        name->setPosition(5, 5 + j * 50);
        name->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
        name->setTextSize(12);

        tgui::CheckBox::Ptr ch = WidgetSingleton<tgui::CheckBox>::get(*this, "c" + Game_api::convertInt(j));
        ch->setPosition(5, 25 + j * 50);
        ch->setSize(12, 12);
        if (j == 5 || j == 6) {
            ch->hide();
        }

        tgui::Label::Ptr minimum = WidgetSingleton<tgui::Label>::get(*this);
        //minimum->load(THEME_CONFIG_FILE);
        minimum->setText(modes[j].min_opis);
        minimum->setPosition(20, 25 + j * 50);
        minimum->setTextColor(sf::Color(0, 0, 0, 255));
        minimum->setTextSize(12);

        tgui::Slider::Ptr scr = WidgetSingleton<tgui::Slider>::get(*this, "s" + Game_api::convertInt(j));
        tgui::EditBox::Ptr box = WidgetSingleton<tgui::EditBox>::get(*this, "e" + Game_api::convertInt(j));
        //scr->load(THEME_CONFIG_FILE);
        scr->setPosition(55, 25 + j * 50);
        scr->setSize(220, 14);
        scr->setMaximum(modes[j].maximum - modes[j].minimum);
        scr->setMinimum(0);
        scr->connect("ValueChanged", [&, box, scroll_id = j](float value) {
            if (callback_block) { return; }
            box->setText(Game_api::convertInt(value + modes[scroll_id].minimum));

            onValueChange.emit(this);
        });

        tgui::Label::Ptr maximum = WidgetSingleton<tgui::Label>::get(*this);
        //maximum->load(THEME_CONFIG_FILE);
        maximum->setText(modes[j].max_opis);
        maximum->setPosition(280, 25 + j * 50);
        maximum->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
        maximum->setTextSize(12);


        //box->load(THEME_CONFIG_FILE);
        box->setPosition(275, 5 + j * 50);
        box->setSize(37, 15);
        box->setInputValidator(tgui::EditBox::Validator::Int);
        box->setText(Game_api::convertInt(modes[j].minimum));
        box->connect("TextChanged", [&, scr, scroll_id = j](const sf::String &text) {
            if (callback_block) { return; }
            scr->setValue(Game_api::convertString(text) - modes[scroll_id].minimum);

            onValueChange.emit(this);
        });
    }
}
