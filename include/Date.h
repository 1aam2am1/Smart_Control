#ifndef DATE_H
#define DATE_H

#include <TGUI/Widgets/Panel.hpp>

#include <Date_data_struct.h>

class Date : public tgui::Panel {
public:
    typedef std::shared_ptr<Date> Ptr;

    Date();

    virtual ~Date();

    void setDateData(const Date_data_struct &);

    Date_data_struct getChanged();

protected:
    tgui::Signal &getSignal(std::string signalName) override;

    void initialize();

public:
    tgui::Signal onValueChange = {"ValueChanged"};

private:
    bool callback_block;

    void callback(const int);

    Date_data_struct result;
};

#endif // DATE_H
