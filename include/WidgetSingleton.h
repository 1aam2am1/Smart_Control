#ifndef WIDGETSINGLETON_H
#define WIDGETSINGLETON_H

#include <TGUI/TGUI.hpp>

#define THEME_CONFIG_FILE "data/widgets/White.conf"

template<typename T, class Enable = void>
struct WidgetSingleton {
    static tgui::SharedWidgetPtr<T> get(tgui::Container &gui, const sf::String &widgetName = "");
};


template<typename T>
struct WidgetSingleton<T, typename std::enable_if<std::is_base_of<tgui::Widget, T>::value \
 && !std::is_same<tgui::Panel, T>::value>::type> {
    static tgui::SharedWidgetPtr<T> get(tgui::Container &gui, const sf::String &widgetName = "") {
        static bool load = false;
        static tgui::SharedWidgetPtr<T> zmienna;
        if (!load) {
            zmienna->load(THEME_CONFIG_FILE);
            load = true;
        }
        tgui::SharedWidgetPtr<T> clone = zmienna.clone();

        gui.add(clone, widgetName);

        return clone;
    }
};

template<typename T>
struct WidgetSingleton<T, typename std::enable_if<std::is_base_of<tgui::Panel, T>::value>::type> {
    static tgui::SharedWidgetPtr<T> get(tgui::Container &gui, const sf::String &widgetName = "") {
        static tgui::SharedWidgetPtr<T> zmienna;

        tgui::SharedWidgetPtr<T> clone = zmienna.clone();

        gui.add(clone, widgetName);

        return clone;
    }
};

#endif // WIDGETSINGLETON_H
