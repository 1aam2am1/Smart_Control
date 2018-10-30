#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <windows.h>

#undef MessageBox

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

void load(tgui::Gui *, const Argv_options &);

int main(int argc, char **argv) {
    std::shared_ptr<Device> com;

    sf::RenderWindow window;

    Argv_options options;
    options.process(argc, argv);
    window.create(sf::VideoMode(static_cast<uint32_t>(options.getOptions().size.x),
                                static_cast<uint32_t>(options.getOptions().size.y)),
                  "Smart Control " + Version::GIT_TAG + " " +
                  (Version::GIT_DIRTY.empty() ? "" : (Version::GIT_SHA + " " +
                                                      Version::DATE)),
                  sf::Style::Default);
    window.setView(sf::View(sf::FloatRect(0, 0, 1200, 820)));

    if (options.getOptions().console) {
        Console::RedirectIOToConsole();

        Console::printf("Program uruchomiono jako:\n");
        for (int32_t i = 0; i < argc; ++i) {
            Console::printf("    %s\n", argv[i]);
        }
        Console::printf("\n");
    }
    if (options.getOptions().tryb == Argv_options::Options::rs232) {
        Console::printf("Uruchomono program z interfejsem: rs232\n");
        com = std::make_shared<rs232>();
    } else if (options.getOptions().tryb == Argv_options::Options::modbus) {
        Console::printf("Uruchomono program z interfejsem: P_SIMPLE\n");
        com = std::make_shared<P_SIMPLE>();
    } else {
        Console::printf("Uruchomono program z interfejsem: P_SIMPLE_USB\n");
        com = std::make_shared<P_SIMPLE>(1);
    }
    if (options.getOptions().version) {
        Console::printf("Version: %s\n", (Version::GIT_TAG + " " +
                                          Version::GIT_SHA + " " +
                                          Version::DATE).c_str());
    }

    sf::Clock send_clock;
    bool send_bool = false;

    sf::Clock com_refresh_clock;
    bool start_refresh_clock = true;

#if defined(SFML_SYSTEM_WINDOWS)
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
    bool dane = false;
    while (window.isOpen()) {
        {
            sf::Clock clock;
            while (!dane && clock.getElapsedTime() < sf::seconds(1)) {
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        window.close();

                    // Pass the event to all the widgets
                    gui.handleEvent(event, false);

                    dane = true;
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
                        case Event::Connected: {
                            sf::Color color;

                            color = (com_event.type == Event::Close) ? sf::Color::Red : color;
                            color = (com_event.type == Event::Open) ? sf::Color::Yellow : color;
                            color = (com_event.type == Event::Create) ? sf::Color::Red : color;
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

                    dane = true;
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

                    dane = true;
                    start_refresh_clock = false;
                }

                if (send_bool && (options.getOptions().tryb == Argv_options::Options::modbus ||
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
            dane = false;
        }

        // The callback loop
        tgui::Callback callback;
        while (gui.pollCallback(callback)) //nie callback.value mozliwa zmiana przed callbakiem
        {
            if (callback.id == 1 && callback.trigger == Menu::COMChanged) {
                sf::String str = gui.get<tgui::ComboBox>("COM", true)->getSelectedItem();

                if (str != "-") {
                    if (!com->connect(str)) {
                        gui.get<tgui::ComboBox>("COM", true)->setSelectedItem("-");
                    }
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
                gui.get<Logi>("logi", true)->save("Zapis.txt");
            }
            if (callback.id == 1 && callback.trigger == Menu::GetCalendarData) {
                com->getCalendarDataSignal();
            }
        }

        window.clear(sf::Color(176, 224, 230));


        // Draw all created widgets
        gui.draw(false);

        window.display();
    }

    fflush(stdout);

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
        modbus->setText("RS232");
    } else if (op.getOptions().tryb == Argv_options::Options::modbus) {
        modbus->setText("MODBUS");
    } else {
        modbus->setText("USB");
    }
}

