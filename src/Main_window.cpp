#include "Main_window.h"
#include "TGUI/TGUI.hpp"
#include "Funkcje_str.h"
#include "Sliders.h"
#include <memory>
#include "WidgetSingleton.h"
#include "Przelacznik_b.h"
#include "Przelacznik_c.h"
#include "Przelacznik_e.h"
#include "Przelacznik_e_hysteresis.h"
#include "Przelacznik_v.h"
#include "Przelacznik_el.h"
#include "Przelacznik_s.h"
#include "Przelacznik_odmrazanie.h"
#include "Przelacznik_dogrzewanie.h"
#include "Przelacznik_typ.h"
#include "Przelacznik_ebledy.h"
#include "Przelacznik_sekmin.h"
#include "Przelacznik_obroty.h"
#include "Console.h"
#include "Logi.h"

Main_window::Main_window() :
        callback_block(false) {
    initialize();
}

Main_window::~Main_window() = default;

void Main_window::change(const std::map<int, int> &dane) {
    callback_block = true;

    this->get<Sliders>("sli")->change(dane);
    this->get<Logi>("logi")->change(dane);

    ///statyczne obiekty ktore beda ustawialy wszystkie przelaczniki
    const static std::shared_ptr<const Przelacznik> tab[] =
            {std::make_shared<Przelacznik_b>("przy_1", 13, 1 << 0), ///przyciski
             std::make_shared<Przelacznik_b>("przy_2", 13, 1 << 1),
             std::make_shared<Przelacznik_b>("przy_3", 13, 1 << 2),
             std::make_shared<Przelacznik_b>("przy_4", 13, 1 << 3),
             std::make_shared<Przelacznik_b>("przy_5", 13, 1 << 4),
             std::make_shared<Przelacznik_b>("przy_6", 13, 1 << 5),
             std::make_shared<Przelacznik_b>("przy_7", 13, 1 << 6),
             std::make_shared<Przelacznik_b>("przy_8", 13, 1 << 7),

             std::make_shared<Przelacznik_ebledy>("tmp_1", 24, 1 << 0), ///bledy
             std::make_shared<Przelacznik_ebledy>("wilg_1", 24, 1 << 1),
             std::make_shared<Przelacznik_ebledy>("tmp_2", 24, 1 << 2),
             std::make_shared<Przelacznik_ebledy>("wilg_2", 24, 1 << 3),
             std::make_shared<Przelacznik_ebledy>("tmp_3", 24, 1 << 4),
             std::make_shared<Przelacznik_ebledy>("wilg_3", 24, 1 << 5),
             std::make_shared<Przelacznik_ebledy>("tmp_4", 24, 1 << 6),
             std::make_shared<Przelacznik_ebledy>("wilg_4", 24, 1 << 7),

             std::make_shared<Przelacznik_c>("wej_1", 25, 1 << 0), ///CheckBox
             std::make_shared<Przelacznik_c>("wej_2", 25, 1 << 1),
             std::make_shared<Przelacznik_c>("wej_3", 25, 1 << 2),
             std::make_shared<Przelacznik_c>("wej_4", 25, 1 << 3),
             std::make_shared<Przelacznik_c>("wej_5", 31, 1 << 3),
             std::make_shared<Przelacznik_c>("wej_6", 31, 1 << 4),
             std::make_shared<Przelacznik_c>("wej_7", 31, 1 << 5),
             std::make_shared<Przelacznik_c>("wej_8", 31, 1 << 6),

             std::make_shared<Przelacznik_s>("serw_1", 31, 1 << 0), ///editbox
             std::make_shared<Przelacznik_s>("serw_2", 31, 1 << 1),
             std::make_shared<Przelacznik_s>("serw_3", 31, 1 << 2),
             std::make_shared<Przelacznik_typ>("tryb_1", 39),
             std::make_shared<Przelacznik_odmrazanie>("odm_1", 30), ///brak flag
             std::make_shared<Przelacznik_dogrzewanie>("dog_1", 30, 1 << 7),
             std::make_shared<Przelacznik_v>("soft_1", 32, 58),
             std::make_shared<Przelacznik_sekmin>("czas_1", 40),
             std::make_shared<Przelacznik_sekmin>("przer_1", 41),

             std::make_shared<Przelacznik_e>("gp_moc", 42), ///lewa
             std::make_shared<Przelacznik_e>("tmp_1", 20),
             std::make_shared<Przelacznik_e>("wilg_1", 35),
             std::make_shared<Przelacznik_e>("tmp_3", 22),
             std::make_shared<Przelacznik_e>("wilg_3", 36),

             std::make_shared<Przelacznik_el>("p_moc", std::vector<int32_t>{42, 43},
                                              std::vector<int32_t>{27, 29}), ///srodek
             std::make_shared<Przelacznik_e_hysteresis>("odz_1", 37, 99, 70),
             std::make_shared<Przelacznik_e>("odz_2", 38),
             std::make_shared<Przelacznik_e>("moc_s1", 26),
             std::make_shared<Przelacznik_obroty>("obr_s1", 27),
             std::make_shared<Przelacznik_e>("moc_s2", 28),
             std::make_shared<Przelacznik_obroty>("obr_s2", 29),

             std::make_shared<Przelacznik_e>("tmp_2", 21), ///prawa
             std::make_shared<Przelacznik_e>("wilg_2", 34),
             std::make_shared<Przelacznik_e>("gw_moc", 43),
             std::make_shared<Przelacznik_e>("tmp_4", 23),
             std::make_shared<Przelacznik_e>("wilg_4", 33)
            };

    callback_block = false;

    for (auto &&wsk : tab) {
        wsk->change(dane, this);
    }
}

std::map<int, int> Main_window::getChanged() {
    uint32_t flaga = 0;
    for (uint32_t i = 8; i > 0; --i) {
        flaga <<= 1;
        flaga |= (this->get<tgui::Button>("przy_" + Game_api::convertInt(i))->getRenderer()->getTextColor() ==
                  sf::Color::Red);
    }
    this->get<tgui::Button>("przy_8")->getRenderer()->setTextColor(sf::Color::Black);///zmiana koloru resetu

    auto i = this->get<Sliders>("sli")->getChanged();
    i[13] = flaga;

    return i;
}

tgui::Signal &Main_window::getSignal(std::string signalName) {
    if (signalName == tgui::toLower(onValueChange.getName()))
        return onValueChange;
    else if (signalName == tgui::toLower(onCOMChange.getName()))
        return onCOMChange;
    else if (signalName == tgui::toLower(onMODBUSChange.getName()))
        return onMODBUSChange;
    else if (signalName == tgui::toLower(onSaveLogs.getName()))
        return onSaveLogs;
    else
        return tgui::Panel::getSignal(std::move(signalName));
}

void Main_window::initialize() {
    callback_block = true;
    this->loadWidgetsFromFile("data/form.txt");
    this->getRenderer()->setBackgroundColor(sf::Color::Transparent);

    this->get<tgui::ComboBox>("COM")->connect("ItemSelected", [&]() {
        if (callback_block) { return; }
        onCOMChange.emit(this);
    });
    this->get<tgui::ComboBox>("COM")->setSelectedItem("-"); ///setselecteditem is not implemented yet

    for (uint32_t i = 1; i < 9; ++i) {
        auto b1 = this->get<tgui::Button>("przy_" + Game_api::convertInt(i));
        b1->connect("Pressed", [&, widget = b1]() {
            if (callback_block) { return; }
            sf::Color c = widget->getRenderer()->getTextColor();
            if (c == sf::Color::Black) { widget->getRenderer()->setTextColor(sf::Color::Red); }
            else { widget->getRenderer()->setTextColor(sf::Color::Black); }

            onValueChange.emit(this);
        });
    }

    this->get<tgui::Button>("log_1")->connect("Pressed", [&]() {
        onSaveLogs.emit(this);
    });

    this->get<tgui::CheckBox>("MODBUS")->connect("Unchecked", [&]() {
        if (callback_block) { return; }
        onMODBUSChange.emit(this);
    });

    Sliders::Ptr sli = WidgetSingleton<Sliders>::get(*this, "sli");
    sli->setPosition(860, 10);
    sli->setSize(330, 450);
    sli->getRenderer()->setBackgroundColor(sf::Color::White);
    sli->connect("ValueChanged", [&]() {
        if (callback_block) { return; }
        onValueChange.emit(this);
    });

    Logi::Ptr logi = WidgetSingleton<Logi>::get(*this, "logi");
    logi->setPosition(10, 470);
    logi->setSize(1180, 320);
    logi->getRenderer()->setBackgroundColor(sf::Color::White);


    logi->registerIDforTracking(0, "Czerpnia *C", 20, sf::Color::Green);
    logi->registerIDforErrors(20, -1, 24, 1 << 0);
    logi->registerIDforTracking(0, "Czerpnia %", 35, sf::Color::Green + sf::Color(50, 50, 50));
    logi->registerIDforErrors(35, -1, 24, 1 << 1);
    logi->registerIDforTracking(0, "Wyrzutnia *C", 22, sf::Color(139, 69, 19));
    logi->registerIDforErrors(22, -1, 24, 1 << 4);
    logi->registerIDforTracking(0, "Wyrzutnia %", 36, sf::Color(139, 69, 19) + sf::Color(50, 50, 50));
    logi->registerIDforErrors(36, -1, 24, 1 << 5);
    logi->registerIDforTracking(0, "Wywiew *C", 21, sf::Color(237, 28, 36));
    logi->registerIDforErrors(21, -1, 24, 1 << 2);
    logi->registerIDforTracking(0, "Wywiew %", 34, sf::Color(237, 28, 36) + sf::Color(50, 50, 50));
    logi->registerIDforErrors(34, -1, 24, 1 << 3);
    logi->registerIDforTracking(0, "Nawiew *C", 23, sf::Color(0, 162, 232));
    logi->registerIDforErrors(23, -1, 24, 1 << 6);
    logi->registerIDforTracking(0, "Nawiew %", 33, sf::Color(0, 162, 232) + sf::Color(50, 50, 50));
    logi->registerIDforErrors(33, -1, 24, 1 << 7);
    logi->registerRangeofTab(0, -20, 100);


    logi->registerIDforTracking(1, "Moc grzalki wstepnej", 42, sf::Color::Blue);
    logi->registerIDforTracking(1, "Moc grzalki wtornej", 43, sf::Color(0, 162, 232));
    logi->registerIDforTracking(1, "Serwo 1", 31, sf::Color::Magenta, 1 << 0);
    logi->registerIDforTracking(1, "Serwo 2", 31, sf::Color::Green, 1 << 1);
    logi->registerIDforTracking(1, "Serwo 3", 31, sf::Color::Red, 1 << 2);
    logi->registerIDforTracking(1, "Odzysk %", 37, sf::Color(139, 69, 19));
    logi->registerIDforTracking(1, "Odzysk kWh", 38, sf::Color(139, 69, 19) + sf::Color(50, 50, 50));
    logi->registerRangeofTab(1, 0, 100);

    logi->updateIDforTracking();

    callback_block = false;
}
