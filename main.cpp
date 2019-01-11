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

void load(tgui::Gui *, const Argv_options &);

int main(int argc, char **argv) {
    Argv_options options;
    std::shared_ptr<Device> com;
    const std::string log_file_name = "Zapis.txt";

    options.process(argc, argv);

    Console::setMessage_level(Console::ALL & ~(!options.getOptions().debug_message ? Console::LOG : 0));

    if (options.getOptions().console) {
        Console::RedirectIOToConsole();

        Console::printf(Console::MESSAGE, "Program uruchomiono jako:\n");
        for (int32_t i = 0; i < argc; ++i) {
            Console::printf(Console::MESSAGE, "    %s\n", argv[i]);
        }
        Console::printf(Console::MESSAGE, "\n");
    }
    if (options.getOptions().tryb == Argv_options::Options::rs232) {
        Console::printf(Console::MESSAGE, "Uruchomiono program z interfejsem: USB_RS232\n");
        com = std::make_shared<rs232>();
    } else if (options.getOptions().tryb == Argv_options::Options::modbus) {
        Console::printf(Console::MESSAGE, "Uruchomiono program z interfejsem: RS485\n");
        com = std::make_shared<P_SIMPLE>();
    } else {
        Console::printf(Console::MESSAGE, "Uruchomiono program z interfejsem: USB_RS485\n");
        com = std::make_shared<P_SIMPLE>(1);
    }
    if (options.getOptions().version) {
        Console::printf(Console::MESSAGE, "Version: %s\n", (Version::GIT_TAG + " " +
                                                            Version::GIT_SHA + " " +
                                                            Version::DATE).c_str());
    }
#ifdef __linux__
    Console::printf(Console::MESSAGE, "We need root privileges to open rs232 port %i\n", setuid(0));

#endif // _linux_

    sf::RenderWindow window;
    window.create(sf::VideoMode(static_cast<uint32_t>(options.getOptions().size.x),
                                static_cast<uint32_t>(options.getOptions().size.y)),
                  "Smart Control " + Version::GIT_TAG + " " +
                  (Version::GIT_DIRTY.empty() ? "" : (Version::GIT_SHA + " " +
                                                      Version::DATE)),
                  sf::Style::Default);
    window.setView(sf::View(sf::FloatRect(0, 0, 1200, 820)));

    sf::Clock send_clock;
    bool send_bool = false;

    sf::Clock com_refresh_clock;
    bool start_refresh_clock = true;

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
                    gui.handleEvent(event, false);

                    event_flag = true;
                }
                while (com->pollEvent(com_event)) {
                    switch (com_event.type) {
                        case Event::Close:
                        case Event::Create:
                            load(&gui, options);
                            send_bool = false;
                            start_refresh_clock = true;
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

                            gui.get<tgui::Panel>("STATUS", true)->setBackgroundColor(color);
                        }
                            break;
                        case Event::Data: {
                            auto data = com->getData();
                            gui.get<Menu>("menu")->change(data);
                        }
                            break;
                        case Event::TimeData: {
                            gui.get<Menu>("menu")->getDate()->setDateData(com->getDateData());
                        }
                            break;
                        case Event::ModesData: {
                            gui.get<Menu>("menu")->getModes()->setModesData(com->getModesData());
                        }
                            break;
                        case Event::CalendarData: {
                            auto data = com->getCalendarData();
                            gui.get<Menu>("menu")->getCalendar()->setCAL_STATE(com->getCAL_STATE());
                            gui.get<Menu>("menu")->getCalendar()->setActiveDays(com->getCalendarActiveDays());
                            gui.get<Menu>("menu")->getCalendar()->change(data);
                        }
                            break;
                        case Event::Reset: {
                            tgui::MessageBox::Ptr box = WidgetSingleton<tgui::MessageBox>::get(gui.getContainer());

                            box->setText("A reset signal was received");
                            box->addButton("Ok");
                            box->setPosition((sf::Vector2f(window.getView().getSize()) - box->getFullSize()) / 2.f);
                            box->bindCallbackEx(
                                    [box](const tgui::Callback &) -> void { box->getParent()->remove(box); },
                                    tgui::MessageBox::ButtonClicked);
                        }
                            break;
                    }

                    event_flag = true;
                }
                if (start_refresh_clock || com_refresh_clock.getElapsedTime() > sf::seconds(5)) {
                    tgui::ComboBox::Ptr list_com = gui.get("COM", true);
                    auto i = Com::getComList();
                    sf::String wybrany = list_com->getSelectedItem();

                    list_com->removeAllItems();
                    list_com->addItem("-");

                    for (std::list<std::string>::const_iterator j = i.begin(); j != i.end(); ++j) {
                        list_com->addItem(*j);
                    }

                    if (!list_com->setSelectedItem(wybrany)) {
                        com->close();
                    }

                    com_refresh_clock.restart();

                    event_flag = true;
                    start_refresh_clock = false;
                }

                if (send_bool && (options.getOptions().tryb != Argv_options::Options::rs232 ||
                                  send_clock.getElapsedTime() > sf::seconds(0.5f))) {
                    auto i = gui.get<Menu>("menu")->getChanged();
                    auto data = gui.get<Menu>("menu")->getCalendar()->getChanged();
                    auto data1 = gui.get<Menu>("menu")->getModes()->getChanged();
                    auto data2 = gui.get<Menu>("menu")->getDate()->getChanged();

                    com->toSendData(i);
                    com->sendCalendarData(data);
                    com->sendModesData(data1);
                    com->sendDateData(data2);
                    send_bool = false;
                }

                sf::sleep(sf::milliseconds(20));
            }///main loop
            event_flag = false;
        }

        // The callback loop
        tgui::Callback callback;
        while (gui.pollCallback(callback)) //nie callback.value mozliwa zmiana przed callbakiem
        {
            if (callback.id == 1 && callback.trigger == Menu::COMChanged) {
                sf::String str = gui.get<tgui::ComboBox>("COM", true)->getSelectedItem();

                if (str != "-") {
#ifdef _WIN32
                    if (!com->connect(str)) {
                        gui.get<tgui::ComboBox>("COM", true)->setSelectedItem("-");
                    }
#endif // _WIN32
#ifdef __linux__
                    if (!com->connect("/dev/" + str)) {
                        gui.get<tgui::ComboBox>("COM", true)->setSelectedItem("-");
                    }
#endif // _linux_
                } else {
                    com->close();
                }
            }
            if (callback.id == 1 && callback.trigger == Menu::ValueChanged) {
                send_clock.restart();
                send_bool = true;
            }
            if (callback.id == 1 && callback.trigger == Menu::MODBUSChanged) {
                /*if (callback.checked && options.getOptions().tryb == Argv_options::Options::rs232) {
                    com = std::make_shared<P_SIMPLE>();

                    auto op = options.getOptions();
                    op.tryb = Argv_options::Options::modbus;
                    options.setOptions(op);
                }*/
                auto op = options.getOptions();

                if (!callback.checked) {
                    switch (options.getOptions().tryb) {
                        case Argv_options::Options::rs232:
                            op.tryb = Argv_options::Options::modbus;
                            com = std::make_shared<P_SIMPLE>();
                            break;
                        case Argv_options::Options::modbus:
                            op.tryb = Argv_options::Options::modbus_usb;
                            com = std::make_shared<P_SIMPLE>(1);
                            break;
                        case Argv_options::Options::modbus_usb:
                            op.tryb = Argv_options::Options::rs232;
                            com = std::make_shared<rs232>();
                            break;
                    }
                }

                options.setOptions(op);
            }
            if (callback.id == 1 && callback.trigger == Menu::SaveLogs) {
                gui.get<Logi>("logi", true)->save(log_file_name);
            }
            if (callback.id == 1 && callback.trigger == Menu::GetCalendarData) {
                com->getCalendarDataSignal();
            }

            event_flag = true;
        }

        window.clear(sf::Color(176, 224, 230));


        // Draw all created widgets
        gui.draw(false);

        window.display();
    }

    Asynchronous_write::getSingleton().wait_to_write();

    return EXIT_SUCCESS;
}

void load(tgui::Gui *gui, const Argv_options &op) {
    gui->removeAllWidgets();
    gui->setGlobalFont("data/main.ttf");

    Menu::Ptr menu(*gui, "menu");
    menu->setPosition(0, 0);
    menu->bindCallback(
            Menu::ValueChanged | Menu::COMChanged | Menu::MODBUSChanged | Menu::SaveLogs | Menu::GetCalendarData);
    menu->setCallbackId(1);

    auto modbus = gui->get<tgui::Checkbox>("MODBUS", true);
    modbus->check();
    if (op.getOptions().tryb == Argv_options::Options::rs232) {
        modbus->setText("USB_RS232");
    } else if (op.getOptions().tryb == Argv_options::Options::modbus) {
        modbus->setText("RS485");
    } else {
        modbus->setText("USB_RS485");
    }
}

