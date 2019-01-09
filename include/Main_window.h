#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <TGUI/Widgets/Panel.hpp>

#include <map>

///TODO: Crete virtual vunction getsignal in all signal based classes
class Main_window : public tgui::Panel {
public:
    typedef std::shared_ptr<Main_window> Ptr;

    Main_window();

    virtual ~Main_window();

    void change(const std::map<int, int> &);

    std::map<int, int> getChanged();

protected:
    tgui::Signal &getSignal(std::string signalName) override;

    void initialize();

public:
    tgui::Signal onValueChange = {"ValueChanged"};
    tgui::Signal onCOMChange = {"COMChanged"};
    tgui::Signal onMODBUSChange = {"MODBUSChanged"};
    tgui::Signal onSaveLogs = {"SaveLogs"};

private:
    bool callback_block;
};

#endif // MAIN_WINDOW_H
