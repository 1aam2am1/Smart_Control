#include "Main_window.h"
#include "TGUI/TGUI.hpp"
#include "Funkcje_str.h"
#include "WidgetSingleton.h"
#include "Sliders.h"
#include <memory>
#include "Przelacznik_b.h"
#include "Przelacznik_c.h"
#include "Przelacznik_e.h"
#include "Przelacznik_v.h"
#include "Przelacznik_el.h"
#include "Przelacznik_s.h"
#include "Przelacznik_odmrazanie.h"
#include "Przelacznik_dogrzewanie.h"
#include "Przelacznik_typ.h"
#include "Przelacznik_ebledy.h"
#include "Przelacznik_sekmin.h"
#include "Console.h"
#include "Logi.h"

Main_window::Main_window() = default;

Main_window::~Main_window() = default;

void Main_window::change(const std::map<int, int> &dane) {
    this->unbindGlobalCallback();

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

             std::make_shared<Przelacznik_c>("wej_1", 25, 1 << 0), ///checkbox
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
             std::make_shared<Przelacznik_v>("soft_1", 32),
             std::make_shared<Przelacznik_sekmin>("czas_1", 40),
             std::make_shared<Przelacznik_sekmin>("przer_1", 41),

             std::make_shared<Przelacznik_e>("gp_moc", 42), ///lewa
             std::make_shared<Przelacznik_e>("tmp_1", 20),
             std::make_shared<Przelacznik_e>("wilg_1", 35),
             std::make_shared<Przelacznik_e>("tmp_3", 22),
             std::make_shared<Przelacznik_e>("wilg_3", 36),

             std::make_shared<Przelacznik_el>("p_moc", std::vector<int32_t>{42, 43},
                                              std::vector<int32_t>{27, 29}), ///srodek
             std::make_shared<Przelacznik_e>("odz_1", 37),
             std::make_shared<Przelacznik_e>("odz_2", 38),
             std::make_shared<Przelacznik_e>("moc_s1", 26),
             std::make_shared<Przelacznik_e>("obr_s1", 27),
             std::make_shared<Przelacznik_e>("moc_s2", 28),
             std::make_shared<Przelacznik_e>("obr_s2", 29),

             std::make_shared<Przelacznik_e>("tmp_2", 21), ///prawa
             std::make_shared<Przelacznik_e>("wilg_2", 34),
             std::make_shared<Przelacznik_e>("gw_moc", 43),
             std::make_shared<Przelacznik_e>("tmp_4", 23),
             std::make_shared<Przelacznik_e>("wilg_4", 33)
            };

    for (auto &&wsk : tab) {
        wsk->change(dane, this);
    }

    this->bindGlobalCallback(std::bind(&Main_window::callback, this, std::placeholders::_1));
}

std::map<int, int> Main_window::getChanged() {
    uint32_t flaga = 0;
    for (uint32_t i = 8; i > 0; --i) {
        flaga <<= 1;
        flaga |= (this->get<tgui::Button>("przy_" + Game_api::convertInt(i))->getTextColor() == sf::Color::Red);
    }
    this->get<tgui::Button>("przy_8")->setTextColor(sf::Color::Black);///zmiana koloru resetu

    auto i = this->get<Sliders>("sli")->getChanged();
    i[13] = flaga;

    return i;
}

void Main_window::initialize(Container *const container) {
    this->Panel::initialize(container);
    this->bindGlobalCallback(std::bind(&Main_window::callback, this, std::placeholders::_1));

    this->loadWidgetsFromFile("data/form.txt");
    this->setBackgroundColor(sf::Color::Transparent);

    Sliders::Ptr sli(*this, "sli");
    sli->setPosition(860, 10);
    sli->setSize(330, 450);
    sli->setBackgroundColor(sf::Color::White);
    sli->setCallbackId(20);
    sli->bindCallback(Sliders::ValueChanged);

    Logi::Ptr logi(*this, "logi");
    logi->setPosition(10, 470);
    logi->setSize(1180, 320);
    logi->setBackgroundColor(sf::Color::White);


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
}

void Main_window::callback(const tgui::Callback &callback) {
    if (callback.id == 1) {
        m_Callback.trigger = Main_window::COMChanged;
    }
    if (callback.id >= 2 && callback.id <= 9) ///przyciski
    {
        auto *button = dynamic_cast<tgui::Button *>(callback.widget);
        sf::Color c = button->getTextColor();
        if (c == sf::Color::Black) { button->setTextColor(sf::Color::Red); }
        else { button->setTextColor(sf::Color::Black); }

        m_Callback.trigger = Main_window::ValueChanged;
    }
    if (callback.id == 10) ///logi zapisz
    {
        m_Callback.trigger = Main_window::SaveLogs;
    }
    if (callback.id == 11) ///modbus change
    {
        m_Callback.checked = callback.checked;
        m_Callback.trigger = Main_window::MODBUSChanged;
    }
    if (callback.id == 20) ///slider
    {
        m_Callback.trigger = Main_window::ValueChanged;
    }
    Console::printf("Callback id: %i, value: %i, text: %s\n", callback.id, callback.value,
                    callback.text.toAnsiString().c_str());
    addCallback();
}
