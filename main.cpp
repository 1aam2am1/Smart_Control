#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>

#if defined(_WIN32)
#include <windows.h>

#undef MessageBox
#endif
#ifdef __linux__

#include <unistd.h>

#endif

#include <memory>
#include "icona.h"
#include "Logi.h"
#include "Menu.h"
#include "Argv_options.h"
#include "Device.h"
#include "rs232/rs232.h"
#include "P_SIMPLE/P_SIMPLE.h"
#include "Funkcje_str.h"
#include "Com.h"
#include "WidgetSingleton.h"
#include "Console.h"
#include "version.h"
#include "Asynchronous_write.h"

struct Callback_data {
    Argv_options op;
    std::shared_ptr<Device> com;
    sf::Clock send_clock;
    bool send_bool = false;

    sf::Clock com_refresh_clock;
    bool start_refresh_clock = true;


    bool com_signal_block = false;
};

void load(tgui::Gui *, Callback_data &);

int main(int argc, char **argv) {
    Callback_data callback_data;

    callback_data.op.process(argc, argv);

    Console::setMessage_level(Console::ALL & ~(!callback_data.op.getOptions().debug_message ? Console::LOG : 0));

    if (callback_data.op.getOptions().console) {
        Console::RedirectIOToConsole();

        Console::printf(Console::MESSAGE, "Program uruchomiono jako:\n");
        for (int32_t i = 0; i < argc; ++i) {
            Console::printf(Console::MESSAGE, "    %s\n", argv[i]);
        }
        Console::printf(Console::MESSAGE, "\n");
    }
    if (callback_data.op.getOptions().tryb == Argv_options::Options::rs232) {
        Console::printf(Console::MESSAGE, "Uruchomiono program z interfejsem: USB_RS232\n");
        callback_data.com = std::make_shared<rs232>();
    } else if (callback_data.op.getOptions().tryb == Argv_options::Options::modbus) {
        Console::printf(Console::MESSAGE, "Uruchomiono program z interfejsem: RS485\n");
        callback_data.com = std::make_shared<P_SIMPLE>();
    } else {
        Console::printf(Console::MESSAGE, "Uruchomiono program z interfejsem: USB_RS485\n");
        callback_data.com = std::make_shared<P_SIMPLE>(1);
    }
    if (callback_data.op.getOptions().version) {
        Console::printf(Console::MESSAGE, "Version: %s\n", (Version::GIT_TAG + " " +
                                                            Version::GIT_SHA + " " +
                                                            Version::DATE).c_str());
    }
#ifdef __linux__
    Console::printf(Console::MESSAGE, "We need root privileges to open rs232 port %i\n", setuid(0));

#endif // _linux_

    sf::RenderWindow window;
    window.create(sf::VideoMode(static_cast<uint32_t>(callback_data.op.getOptions().size.x),
                                static_cast<uint32_t>(callback_data.op.getOptions().size.y)),
                  "Smart Control " + Version::GIT_TAG + " " +
                  (Version::GIT_DIRTY.empty() ? "" : (Version::GIT_SHA + " " +
                                                      Version::DATE)),
                  sf::Style::Default);
    window.setView(sf::View(sf::FloatRect(0, 0, 1200, 820)));


#if defined(_WIN32)
    HICON hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_IKONA));
    if (hIcon) {
        SendMessage(window.getSystemHandle(), WM_SETICON, ICON_BIG, (LPARAM) hIcon);
        SendMessage(window.getSystemHandle(), WM_SETICON, ICON_SMALL, (LPARAM) hIcon);
        DestroyIcon(hIcon);
    }
#endif
    tgui::Gui gui(window);

    ///load(&gui, options);

    sf::Event event{};
    Event com_event{};
    bool event_flag = false;
    while (window.isOpen()) {
        {
            sf::Clock clock;
            while (!event_flag && clock.getElapsedTime() < sf::seconds(1)) {
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        window.close();

                    // Pass the event to all the widgets
                    if (gui.handleEvent(event)) { event_flag = true; }

                }
                while (callback_data.com->pollEvent(com_event)) {
                    switch (com_event.type) {
                        case Event::Close:
                        case Event::Create:
                            load(&gui, callback_data);
                            callback_data.send_bool = false;
                            callback_data.start_refresh_clock = true;
                            ///break;
                            continue;
                        case Event::Open:
                        case Event::DisConnected:
                        case Event::Connected: {
                            sf::Color color;

                            color = (com_event.type == Event::Close) ? sf::Color::Red : color;
                            color = (com_event.type == Event::Open) ? sf::Color::Yellow : color;
                            color = (com_event.type == Event::Create) ? sf::Color::Red : color;
                            color = (com_event.type == Event::DisConnected) ? sf::Color::Yellow : color;
                            color = (com_event.type == Event::Connected) ? sf::Color::Green : color;

                            gui.get<tgui::Panel>("STATUS")->getRenderer()->setBackgroundColor(color);
                        }
                            break;
                        case Event::Data: {
                            auto data = callback_data.com->getData();
                            gui.get<Menu>("menu")->change(data);
                        }
                            break;
                        case Event::TimeData: {
                            gui.get<Menu>("menu")->getDate()->setDateData(callback_data.com->getDateData());
                        }
                            break;
                        case Event::ModesData: {
                            gui.get<Menu>("menu")->getModes()->setModesData(callback_data.com->getModesData());
                        }
                            break;
                        case Event::CalendarData: {
                            auto data = callback_data.com->getCalendarData();
                            gui.get<Menu>("menu")->getCalendar()->setCAL_STATE(callback_data.com->getCAL_STATE());
                            gui.get<Menu>("menu")->getCalendar()->setActiveDays(
                                    callback_data.com->getCalendarActiveDays());
                            gui.get<Menu>("menu")->getCalendar()->change(data);
                        }
                            break;
                        case Event::Reset: {
                            tgui::MessageBox::Ptr box = WidgetSingleton<tgui::MessageBox>::get(*gui.getContainer());

                            box->setText("A reset signal was received");
                            box->addButton("Ok");
                            box->setPosition((sf::Vector2f(window.getView().getSize()) - box->getFullSize()) / 2.f);
                            box->connect("ButtonPressed",
                                         [box]() -> void { box->getParent()->remove(box); });
                        }
                            break;
                    }

                    event_flag = true;
                }
                if (callback_data.start_refresh_clock ||
                    callback_data.com_refresh_clock.getElapsedTime() > sf::seconds(5)) {
                    tgui::ComboBox::Ptr list_com = gui.get<tgui::ComboBox>("COM");
                    auto i = Com::getComList();
                    callback_data.com_signal_block = true;
                    sf::String wybrany = list_com->getSelectedItem();

                    list_com->removeAllItems();
                    list_com->addItem("-");

                    for (std::list<std::string>::const_iterator j = i.begin(); j != i.end(); ++j) {
                        list_com->addItem(*j);
                    }

                    if (!list_com->setSelectedItem(wybrany)) {
                        callback_data.com->close();
                    }

                    callback_data.com_refresh_clock.restart();

                    callback_data.com_signal_block = false;
                    event_flag = true;
                    callback_data.start_refresh_clock = false;
                }

                if (callback_data.send_bool && (callback_data.op.getOptions().tryb != Argv_options::Options::rs232 ||
                                                callback_data.send_clock.getElapsedTime() > sf::seconds(0.5f))) {
                    auto i = gui.get<Menu>("menu")->getChanged();
                    auto data = gui.get<Menu>("menu")->getCalendar()->getChanged();
                    auto data1 = gui.get<Menu>("menu")->getModes()->getChangedModes();
                    auto data2 = gui.get<Menu>("menu")->getDate()->getChanged();

                    callback_data.com->toSendData(i);
                    callback_data.com->sendCalendarData(data);
                    callback_data.com->sendModesData(data1);
                    callback_data.com->sendDateData(data2);
                    callback_data.send_bool = false;
                }

                sf::sleep(sf::milliseconds(20));
            }///main loop
            event_flag = false;
        }

        window.clear(sf::Color(176, 224, 230));


        // Draw all created widgets
        gui.draw();

        window.display();
    }

    callback_data.com->close();
    Asynchronous_write::getSingleton().wait_to_write();

    return EXIT_SUCCESS;
}

void load(tgui::Gui *gui, Callback_data &data) {
    gui->removeAllWidgets();
    /*static sf::Font font; ///White build in font
    static bool loaded_font = false;

    if (!loaded_font) {
        font.loadFromFile("data/main.ttf");
        gui->setFont(font);
        loaded_font = true;
    }*/

    Menu::Ptr menu = WidgetSingleton<Menu>::get(*gui->getContainer(), "menu");
    menu->setPosition(0, 0);
    menu->connect("ValueChanged", [&send_clock = data.send_clock, &send_bool = data.send_bool]() {
        send_clock.restart();
        send_bool = true;
    });
    menu->connect("COMChanged", [&com = data.com, &com_signal_block = data.com_signal_block, gui]() {
        if (com_signal_block) { return; }
        com_signal_block = true;
        sf::String str = gui->get<tgui::ComboBox>("COM")->getSelectedItem();

        if (str != "-") {
#ifdef _WIN32
            if (!com->connect(str)) {
                gui->get<tgui::ComboBox>("COM")->setSelectedItem("-");
            }
#endif // _WIN32
#ifdef __linux__
            if (!com->connect("/dev/" + str)) {
                        gui->get<tgui::ComboBox>("COM")->setSelectedItem("-");
                    }
#endif // _linux_
        } else {
            com->close();
        }
        com_signal_block = false;
    });
    menu->connect("MODBUSChanged", [&com = data.com, &op = data.op]() {
        auto options = op.getOptions();

        switch (options.tryb) {
            case Argv_options::Options::rs232:
                options.tryb = Argv_options::Options::modbus;
                com = std::make_shared<P_SIMPLE>();
                break;
            case Argv_options::Options::modbus:
                options.tryb = Argv_options::Options::modbus_usb;
                com = std::make_shared<P_SIMPLE>(1);
                break;
            case Argv_options::Options::modbus_usb:
                options.tryb = Argv_options::Options::rs232;
                com = std::make_shared<rs232>();
                break;
        }

        op.setOptions(options);
    });
    menu->connect("SaveLogs", [gui]() {
        const std::string log_file_name = "Zapis.txt";
        gui->get<Logi>("logi")->save(log_file_name);
    });
    menu->connect("GetCalendarData", [&com = data.com]() { com->getCalendarDataSignal(); });

    auto modbus = gui->get<tgui::CheckBox>("MODBUS");
    modbus->setChecked(true);
    if (data.op.getOptions().tryb == Argv_options::Options::rs232) {
        modbus->setText("USB_RS232");
    } else if (data.op.getOptions().tryb == Argv_options::Options::modbus) {
        modbus->setText("RS485");
    } else {
        modbus->setText("USB_RS485");
    }
}

