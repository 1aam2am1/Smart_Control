#ifndef LOGI_H
#define LOGI_H

#include <TGUI/Panel.hpp>
#include <TGUI/SharedWidgetPtr.hpp>


class Logi : public tgui::Panel {
public:
    typedef tgui::SharedWidgetPtr<Logi> Ptr;

    Logi();

    virtual ~Logi();

    void save(std::string) const;

    void change(const std::map<int, int> &);

    void registerIDforTracking(int32_t tab, std::string nazwa, int32_t id, sf::Color, int32_t bit = -1);

    void registerRangeofTab(int32_t tab, int32_t minimum, int32_t maximum);

    void registerIDforErrors(int32_t id, int32_t bit, int32_t error_id, int32_t error_bit);

    void updateIDforTracking();

protected:
    virtual void initialize(Container *const) override;

public:
    enum LogiCallbacks {
        ValueChanged = PanelCallbacksCount * 1,     ///< Value changed
        AllLogiCallbacks = PanelCallbacksCount * 2 - 1, ///< All triggers defined in Button and its base classes
        LogiCallbacksCount = PanelCallbacksCount * 2
    };

protected:
    virtual void update() override;

    void calculateScrollbar();

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    void drawLogi(sf::RenderTarget &target, sf::RenderStates states) const;

private:
    void callback(const tgui::Callback &);

    std::map<int32_t, std::list<std::tuple<int32_t, std::string, sf::Color, int32_t>>> tab_nazwy;
    ///strona->liste->id,nazwa,kolor,bit
    std::map<int32_t, std::tuple<int32_t, int32_t>> tab_range;
    float czas = 1;
};

#endif // LOGI_H
