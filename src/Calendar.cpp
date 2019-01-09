#include "Calendar.h"
#include "TGUI/TGUI.hpp"
#include "Funkcje_str.h"
#include "WidgetSingleton.h"
#include "Day.h"

const static sf::Color green = sf::Color(198, 239, 206);
const static sf::Color red = sf::Color(255, 199, 206);

Calendar::Calendar() {
    initialize();
}

Calendar::~Calendar() = default;

void Calendar::setCAL_STATE(const CAL_STATE &state) {
    std::string message;
    if (!state.EN_FUNC) {
        message = "Brak zegara";
    } else if (state.WORKING) {
        message = "Working: ";
    }
    switch (state.flag) {
        case CAL_STATE::IN_CLK:
            message += "Zegar PCB";
            break;
        case CAL_STATE::EXT_CLK:
            message += "Zegar na plycie rozszerzen";
            break;
        case CAL_STATE::ETH_MODULE:
            message += "Czas z modulu ethernetowego";
            break;
        default:
            break;
    }


    this->get<tgui::Label>("status")->setText(message);
}

void Calendar::setActiveDays(uint8_t flags) {
    auto b1 = this->get<tgui::Button>("aktywny");
    b1->getRenderer()->setTextColor(sf::Color(0, 255 * (flags & 1), 0, 255));
    if (flags & 1) {
        this->getRenderer()->setBackgroundColor(green);
    } else {
        this->getRenderer()->setBackgroundColor(red);
    }

    for (uint32_t i = 0; i < 7; ++i) {
        this->get<Day>("p" + Game_api::convertInt(i))->setActive(flags & (1 << (i + 1)));
    }
}

void Calendar::change(const std::map<int, std::vector<Action_data_struct>> &dane) {
    for (auto &&it : dane) {
        this->get<Day>("p" + Game_api::convertInt(it.first))->change(it.second);
    }

    auto b3 = this->get<tgui::Button>("save");
    b3->setEnabled(false);
}

std::pair<uint8_t, std::map<int, std::vector<Action_data_struct>>> Calendar::getChanged() {
    std::pair<uint8_t, std::map<int, std::vector<Action_data_struct>>> result;

    result.first = (this->get<tgui::Button>("aktywny")->getRenderer()->getTextColor() == sf::Color::Green);

    for (uint32_t i = 0; i < 7; ++i) {
        auto data = this->get<Day>("p" + Game_api::convertInt(i))->getChanged();

        result.first |= data.first << (i + 1);
        result.second[i] = data.second;
    }

    return result;
}

tgui::Signal &Calendar::getSignal(std::string signalName) {
    if (signalName == tgui::toLower(onValueChange.getName()))
        return onValueChange;
    else if (signalName == tgui::toLower(onGetData.getName()))
        return onGetData;
    else
        return tgui::Panel::getSignal(std::move(signalName));
}

void Calendar::initialize() {
    this->getRenderer()->setBackgroundColor(red);

    tgui::Button::Ptr b1 = WidgetSingleton<tgui::Button>::get(*this, "aktywny");
    //b1->load(THEME_CONFIG_FILE);
    b1->setSize(95, 16);
    b1->setTextSize(12);
    b1->setPosition(10, 0);
    b1->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
    b1->setText("Aktywny");
    b1->connect("Pressed", &Calendar::callback, this, 1);

    tgui::Button::Ptr b2 = WidgetSingleton<tgui::Button>::get(*this, "aktualizuj");
    b2->setSize(140, 16);
    b2->setTextSize(12);
    b2->setPosition(110, 0);///105<-
    b2->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
    b2->setText("Odczytaj Kalendarz");
    b2->connect("Pressed", &Calendar::callback, this, 2);

    tgui::Button::Ptr b3 = WidgetSingleton<tgui::Button>::get(*this, "save");
    b3->setSize(140, 16);
    b3->setTextSize(12);
    b3->setPosition(255, 0);
    b3->getRenderer()->setTextColor(sf::Color(0, 0, 0, 255));
    b3->setText("Zapisz Kalendarz");
    b3->connect("Pressed", &Calendar::callback, this, 3);
    b3->setEnabled(false);

    tgui::Label::Ptr status = WidgetSingleton<tgui::Label>::get(*this, "status");
    //status->load(THEME_CONFIG_FILE);
    //status->setText("Tu bedzie text");
    status->setPosition(400, 0);
    status->getRenderer()->setTextColor(sf::Color(255, 0, 0, 255));
    status->setTextSize(14);

    const std::string nazwy[7] = {"Poniedzialek", "Wtorek", "Sroda", "Czwartek", "Piatek", "Sobota", "Niedziela"};

    for (uint32_t i = 0; i < 7; ++i) {
        Day::Ptr day = WidgetSingleton<Day>::get(*this, "p" + Game_api::convertInt(i));
        day->setName(nazwy[i]);
        day->setPosition(10 + i * 170, 20);
        day->connect("ValueChanged", [&]() {
            auto b3 = this->get<tgui::Button>("save");
            b3->setEnabled(true);
        });
    }
}

void Calendar::callback(const int id) {
    if (id == 1) {
        auto b1 = this->get<tgui::Button>("aktywny");
        b1->getRenderer()->setTextColor(
                sf::Color(0, 255 * (b1->getRenderer()->getTextColor() == sf::Color::Black ? 1 : 0), 0, 255));
        if (b1->getRenderer()->getTextColor() == sf::Color::Black) {
            this->getRenderer()->setBackgroundColor(red);
        } else {
            this->getRenderer()->setBackgroundColor(green);
        }
        //m_Callback.trigger = Calendar::ValueChanged;

        auto b3 = this->get<tgui::Button>("save");
        b3->setEnabled(true);
        return;
    }
    if (id == 2) {
        onGetData.emit(this);
    }
    if (id == 3) {
        onValueChange.emit(this);
    }
}
