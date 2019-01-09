#ifndef WIDGETSINGLETON_H
#define WIDGETSINGLETON_H

#include <TGUI/TGUI.hpp>

//#define THEME_CONFIG_FILE "data/widgets/White.conf"

template<typename T, class Enable = void>
struct WidgetSingleton {
    static std::shared_ptr<T> get(tgui::Container &gui, const sf::String &widgetName = "");
};


template<typename T>
struct WidgetSingleton<T, typename std::enable_if<std::is_base_of<tgui::Widget, T>::value \
/* && !std::is_same<tgui::Panel, T>::value*/>::type> {
    static std::shared_ptr<T> get(tgui::Container &gui, const sf::String &widgetName = "") {

        std::shared_ptr<T> widget = std::make_shared<T>();

        gui.add(widget, widgetName);

        return widget;
    }
};
/*
template<typename T>
struct WidgetSingleton<T, typename std::enable_if<std::is_base_of<tgui::Panel, T>::value>::type> {
    static std::shared_ptr<T> get(tgui::Container &gui, const sf::String &widgetName = "") {
        static std::shared_ptr<T> zmienna;

        std::shared_ptr<T> clone = zmienna->clone();

        gui.add(clone, widgetName);

        return clone;
    }
};
*/
#endif // WIDGETSINGLETON_H
