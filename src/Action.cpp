#include "Action.h"
#include "TGUI/TGUI.hpp"
#include "Funkcje_str.h"
#include "WidgetSingleton.h"

const static sf::Color green = sf::Color(198, 239, 206);
const static sf::Color red = sf::Color(255, 199, 206);


Action::Action() :
        callback_block(false) {
    this->setSize(120, 200);
    this->initialize();
}

Action::~Action() = default;

void Action::setData(const Action_data_struct &dane) {
    this->callback_block = true;

    result = dane;

    this->get<tgui::Button>("t0")->getRenderer()->setTextColor(
            sf::Color(0, static_cast<sf::Uint8>(255 * (dane.en ? 1 : 0)), 0, 255));

    this->getRenderer()->setBackgroundColor(dane.en ? green : red);

    ///Poczatek pracy godzina
    this->get<tgui::EditBox>("eh0")->setText(Game_api::convertInt(dane.hour < 23 ? dane.hour : 23));
    this->get<tgui::EditBox>("em0")->setText(Game_api::convertInt(dane.minutes < 59 ? dane.minutes : 59));

    ///Koniec pracy godzina
    {
        int32_t hour = dane.hour < 23 ? dane.hour : 23;
        int32_t minutes = dane.minutes < 59 ? dane.minutes : 59;

        minutes += dane.time;

        while (minutes >= 60) {
            hour += 1;
            minutes -= 60;
        }
        hour %= 24;

        this->get<tgui::EditBox>("eh1")->setText(Game_api::convertInt(hour));
        this->get<tgui::EditBox>("em1")->setText(Game_api::convertInt(minutes));
    }

    ///Moc wywiew
    this->get<tgui::EditBox>("eh2")->setText(Game_api::convertInt(dane.moc_wywiew));

    ///Moc nawiew
    this->get<tgui::EditBox>("eh3")->setText(Game_api::convertInt(dane.moc_nawiew));

    ///Temp Powietrza
    this->get<tgui::EditBox>("eh4")->setText(Game_api::convertInt(dane.temp + 14));

    ///Temp en
    {
        auto ch = this->get<tgui::CheckBox>("ec4");

        ch->setChecked(dane.temp_en);
    }

    ///150%
    {
        auto ch = this->get<tgui::CheckBox>("150%");

        ch->setChecked(dane.max_moc);
    }

    this->callback_block = false;
}

Action_data_struct Action::getData() {
    int32_t i;

    result.en = (this->get<tgui::Button>("t0")->getRenderer()->getTextColor() == sf::Color::Green);
    i = Game_api::convertString(this->get<tgui::EditBox>("eh0")->getText());
    result.hour = i < 23 ? i : 23;
    i = Game_api::convertString(this->get<tgui::EditBox>("em0")->getText());
    result.minutes = i < 59 ? i : 59;

    {
        i = Game_api::convertString(this->get<tgui::EditBox>("eh1")->getText());
        int32_t hour = i < 23 ? i : 23;
        i = Game_api::convertString(this->get<tgui::EditBox>("em1")->getText());
        int32_t minutes = i < 59 ? i : 59;

        int32_t minutes_start = result.hour * 60 + result.minutes;
        int32_t minutes_stop = hour * 60 + minutes;
        int32_t diff = minutes_stop - minutes_start;

        if (diff <= 0) {
            diff += 1440;
        }

        result.time = diff;
    }

    i = Game_api::convertString(this->get<tgui::EditBox>("eh2")->getText());
    result.moc_wywiew = i < 100 ? i : 100;
    i = Game_api::convertString(this->get<tgui::EditBox>("eh3")->getText());
    result.moc_nawiew = i < 100 ? i : 100;

    i = Game_api::convertString(this->get<tgui::EditBox>("eh4")->getText()) - 14;
    result.temp = i < 12 ? i : 12;

    result.temp_en = this->get<tgui::CheckBox>("ec4")->isChecked();

    result.max_moc = this->get<tgui::CheckBox>("150%")->isChecked();

    return result;
}

tgui::Signal &Action::getSignal(std::string signalName) {
    if (signalName == tgui::toLower(onValueChange.getName()))
        return onValueChange;
    else if (signalName == tgui::toLower(onDelete.getName()))
        return onDelete;
    else
        return tgui::Panel::getSignal(std::move(signalName));
}

void Action::initialize() {
    this->getRenderer()->setBackgroundColor(red);

    tgui::Button::Ptr b1 = WidgetSingleton<tgui::Button>::get(*this, "t0");
    b1->setSize(65, 16);
    b1->setTextSize(12);
    b1->setPosition(1, 1);
    b1->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
    b1->setText("Aktywny");
    b1->connect("Pressed", &Action::callback, this, 1);

    tgui::Button::Ptr b2 = WidgetSingleton<tgui::Button>::get(*this, "t1");
    b2->setSize(16, 16);
    b2->setTextSize(14);
    b2->setPosition(103, 1);
    b2->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
    b2->setText("-");
    b2->connect("Pressed", &Action::callback, this, 2);

    std::string nazwy[5] = {"Start", "Koniec", "Moc wywiew", "Moc nawiew", "Temp Powietrza"};

    for (uint32_t i = 0; i < 5; ++i) {

        tgui::Label::Ptr l1 = WidgetSingleton<tgui::Label>::get(*this);
        //name->load(THEME_CONFIG_FILE);
        l1->setText(nazwy[i]);
        l1->setPosition(5, 20 + i * 30);
        l1->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
        l1->setTextSize(12);

        if (i < 4) {
            tgui::EditBox::Ptr box1 = WidgetSingleton<tgui::EditBox>::get(*this, "eh" + Game_api::convertInt(i));
            //box->load(THEME_CONFIG_FILE);
            box1->setPosition(5, 32 + i * 30);
            box1->setSize(37, 15);
            box1->setInputValidator(tgui::EditBox::Validator::Int);
            if (i != 2 && i != 3) { box1->setMaximumCharacters(2); } else { box1->setMaximumCharacters(3); }
            box1->setAlignment(tgui::EditBox::Alignment::Right);
            //box->setText(Game_api::convertInt(scroll[scroll_id].minimum));
            box1->connect("ReturnKeyPressed", &Action::callback, this, 10 + i * 2);
            box1->connect("Unfocused", &Action::callback, this, 10 + i * 2);
        } else {
            tgui::CheckBox::Ptr ch = WidgetSingleton<tgui::CheckBox>::get(*this, "ec" + Game_api::convertInt(i));
            //ch->load(THEME_CONFIG_FILE);
            ch->setPosition(5, 33 + i * 30);
            ch->setSize(14, 14);
            ch->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
            ch->connect("Checked", &Action::callback, this, 100);
            ch->connect("Unchecked", &Action::callback, this, 100);

            tgui::EditBox::Ptr box1 = WidgetSingleton<tgui::EditBox>::get(*this, "eh" + Game_api::convertInt(i));
            //box->load(THEME_CONFIG_FILE);
            box1->setPosition(25, 32 + i * 30);
            box1->setSize(37, 15);
            box1->setInputValidator(tgui::EditBox::Validator::Int);
            if (i != 2 && i != 3) { box1->setMaximumCharacters(2); } else { box1->setMaximumCharacters(3); }
            box1->setAlignment(tgui::EditBox::Alignment::Right);
            //box->setText(Game_api::convertInt(scroll[scroll_id].minimum));
            box1->connect("ReturnKeyPressed", &Action::callback, this, 10 + i * 2);
            box1->connect("Unfocused", &Action::callback, this, 10 + i * 2);
        }


        if (i < 2) {
            tgui::Label::Ptr colon1 = WidgetSingleton<tgui::Label>::get(*this);
            //name->load(THEME_CONFIG_FILE);
            colon1->setText(":");
            colon1->setPosition(45, 36 + i * 30);
            colon1->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
            colon1->setTextSize(12);
            colon1->getRenderer()->setTextStyle(sf::Text::Bold);

            tgui::EditBox::Ptr box2 = WidgetSingleton<tgui::EditBox>::get(*this, "em" + Game_api::convertInt(i));
            //box->load(THEME_CONFIG_FILE);
            box2->setPosition(50, 32 + i * 30);
            box2->setSize(37, 15);
            box2->setInputValidator(tgui::EditBox::Validator::Int);
            box2->setMaximumCharacters(2);
            //box->setText(Game_api::convertInt(scroll[scroll_id].minimum));
            box2->connect("ReturnKeyPressed", &Action::callback, this, 11 + i * 2);
            box2->connect("Unfocused", &Action::callback, this, 11 + i * 2);
        }
    }

    tgui::CheckBox::Ptr ch = WidgetSingleton<tgui::CheckBox>::get(*this, "150%");
    //ch->load(THEME_CONFIG_FILE);
    ch->setPosition(5, 175);
    ch->setSize(14, 14);
    ch->setText("Tryb boost");
    ch->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
    ch->setTextSize(12);
    ch->connect("Checked", &Action::callback, this, 100);
    ch->connect("Unchecked", &Action::callback, this, 100);

    this->setData({});
}

void Action::callback(const int id) {
    if (callback_block) { return; }

    if (id == 1) {
        auto b1 = this->get<tgui::Button>("t0");
        b1->getRenderer()->setTextColor(
                sf::Color(0, 255 * (b1->getRenderer()->getTextColor() == sf::Color::Black ? 1 : 0), 0, 255));

        this->getRenderer()->setBackgroundColor(b1->getRenderer()->getTextColor() == sf::Color::Black ? red : green);
    } else if (id == 2) {
        onDelete.emit(this);

        return;
    } else if (id == 100) {

    } else ///changed editbox
    {
        this->checkData();
    }

    onValueChange.emit(this);
}

void Action::checkData() {
    this->callback_block = true;

    {
        uint32_t hour = Game_api::convertString(this->get<tgui::EditBox>("eh0")->getText());
        hour = hour < 23 ? hour : 23;

        uint32_t minutes = Game_api::convertString(this->get<tgui::EditBox>("em0")->getText());
        minutes = minutes < 59 ? minutes : 59;

        uint32_t hour_end = Game_api::convertString(this->get<tgui::EditBox>("eh1")->getText());
        hour_end = hour_end < 23 ? hour_end : 23;
        uint32_t minutes_end = Game_api::convertString(this->get<tgui::EditBox>("em1")->getText());
        minutes_end = minutes_end < 59 ? minutes_end : 59;

        if (hour_end < hour || (hour_end == hour && minutes_end <= minutes)) {
            if ((result.hour * 60 + result.minutes + result.time) > 1440) {
                result.time = 0;
            }
            this->setData(result);
        }
    }


    Action_data_struct data = this->getData();
/*
    data.moc_wywiew = (data.moc_wywiew<100)?data.moc_wywiew:100;
    data.moc_nawiew = (data.moc_nawiew<100)?data.moc_nawiew:100;
    data.time = (data.time<1440)?data.time:1440;
    data.hour = (data.hour<23)?data.hour:23;
    data.minutes = (data.minutes<59)?data.minutes:59;
    data.temp = (data.temp<12)?data.temp:12;
*/
    this->setData(data);

    this->callback_block = false;
}
