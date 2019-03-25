#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <TGUI/Panel.hpp>
#include <TGUI/SharedWidgetPtr.hpp>
#include <map>

class Main_window : public tgui::Panel {
public:
    typedef tgui::SharedWidgetPtr<Main_window> Ptr;

    Main_window();

    virtual ~Main_window();

    void change(const std::map<int, int> &);

    std::map<int, int> getChanged();

protected:
    virtual void initialize(Container *const) override;

public:
    enum Main_windowCallbacks {
        ValueChanged = PanelCallbacksCount * 1,     ///< Value changed
        COMChanged = PanelCallbacksCount * 2,     ///< COM changed
        MODBUSChanged = PanelCallbacksCount * 4,     ///< MODBUS changed
        SaveLogs = PanelCallbacksCount * 8,     ///< Save changed
        OldVersion = PanelCallbacksCount * 16,  ///< Old version
        AllMain_windowCallbacks = PanelCallbacksCount * 32 - 1, ///< All triggers defined in Button and its base classes
        Main_windowCallbacksCount = PanelCallbacksCount * 32
    };

private:
    void callback(const tgui::Callback &);
};

#endif // MAIN_WINDOW_H
