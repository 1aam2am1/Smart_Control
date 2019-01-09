#include "Logi.h"
#include <SFML/OpenGL.hpp>
#include <map>
#include <ctime>
#include <cmath>
#include "Funkcje_str.h"
#include "TGUI/TGUI.hpp"
#include <TGUI/Clipping.hpp>
#include "WidgetSingleton.h"

// TODO (Michal_Marszalek#1#03/30/17): time_t na chrono

///Tu statyczne logi
struct xy {
    time_t time;
    int16_t wartosc;
};

struct ID {
    int32_t id;
    int32_t bit;
};

bool operator<(const ID &left, const ID &right) {
    if (left.id < right.id) {
        return true;
    } else if (left.id == right.id) {
        if (left.bit < right.bit) {
            return true;
        }
    }

    return false;
}

struct ErrorFlag {
    int32_t error_id;
    int32_t error_bit;
    bool error;
};

static std::map<ID, std::vector<xy>> dane; ///id,bit->dane
static std::map<ID, ErrorFlag> errors; ///id,bit->error,id,bit,stara_wartosc_bledu

Logi::Logi() {
    initialize();
}

Logi::~Logi() = default;

void Logi::save(std::string str) const {
    FILE *file;

    file = fopen(str.c_str(), "w");

    std::list<std::tuple<std::vector<xy>::iterator, std::vector<xy> *>> tym_tab;///tym lista


    fprintf(file, "Zapis\n");
    fprintf(file, "time");
    for (const auto &i : tab_nazwy) ///tablica stron
    {
        for (const auto &j : i.second) ///lista tego co zbieram
        {
            fprintf(file, "%c%i", 0x09, std::get<0>(j));///id
            if (std::get<3>(j) != -1) { fprintf(file, "&%i", std::get<3>(j)); }///bit
            fprintf(file, "-\"%2s\"", std::get<1>(j).c_str());///nazwa

            auto &obiekt = dane[{std::get<0>(j), std::get<3>(j)}];///obiekt z danymi

            tym_tab.emplace_back(obiekt.begin(), &obiekt);
        }

    }
    fprintf(file, "\n");

    if (tym_tab.empty()) {
        fclose(file);
        return;
    }

    time_t najmniejszy;
    bool wstawiony = false;
    for (auto &i : tym_tab) {
        if (std::get<0>(i) != std::get<1>(i)->end()) {
            if (!wstawiony) {
                najmniejszy = std::get<0>(i)->time;
                wstawiony = true;
            } else {
                najmniejszy = std::min(najmniejszy, std::get<0>(i)->time);
            }
        }
    }

    while (true) {
        ///sprawdzam czy wszystkie iteratory sa rowne koniec
        bool koniec = true;
        for (auto &j : tym_tab) {
            if (std::get<0>(j) != std::get<1>(j)->end()) {
                koniec = false;
            }
        }
        if (koniec) { break; }

        ///zapisuje czas
        char buffer[80];
        strftime(buffer, 80, "%d.%m.%Y %H:%M:%S", gmtime(&najmniejszy));
        fprintf(file, "%s", buffer);

        ///zapisuje wszystkie temperatury do czasu
        for (auto &j : tym_tab) {
            if (std::get<0>(j) != std::get<1>(j)->end()) {
                if (std::get<0>(j)->time <= najmniejszy) {
                    fprintf(file, "%c%hi", 0x09, std::get<0>(j)->wartosc);

                    continue;
                }
            }

            fprintf(file, "%c ", 0x09);

        }
        fprintf(file, "\n");

        ///szukam nastepnego najmniejszego
        time_t nowy_najmniejszy = najmniejszy;
        wstawiony = false;
        for (auto &i : tym_tab) {
            if (std::get<0>(i) != std::get<1>(i)->end())///jezeli nie koniec to + 1
            {
                auto it = std::get<0>(i);

                if (it->time <= najmniejszy) {
                    ++it;
                }

                if (it != std::get<1>(i)->end()) {
                    if (!wstawiony) {
                        nowy_najmniejszy = it->time;
                        wstawiony = true;
                    } else {
                        nowy_najmniejszy = std::min(nowy_najmniejszy, it->time);
                    }
                }
            }
        }
        if (!wstawiony) { nowy_najmniejszy += 1; }
        najmniejszy = nowy_najmniejszy;

        ///przesuwam wszystkie nastepne iteratowy rowne najmniejszy
        for (auto &i : tym_tab) {
            if (std::get<0>(i) != std::get<1>(i)->end())///jezeli nie koniec to + 1
            {
                if ((std::get<0>(i) + 1) != std::get<1>(i)->end())///jezeli nie koniec to moge zobaczyc
                {
                    if ((std::get<0>(i) + 1)->time <= najmniejszy) {
                        ++std::get<0>(i); ///ustawiam nowa wartosc wskaznika
                    }
                } else {
                    if (std::get<0>(i)->time < najmniejszy) {
                        ++std::get<0>(i);///przenosze wsk na koniec
                    }
                }
            }
        }

    }


    fclose(file);
}

void Logi::change(const std::map<int, int> &ch) {
    const time_t present_time = time(nullptr);
    for (auto &i : dane) {
        const auto &infromation = ch.find(i.first.id);
        if (infromation != ch.end())///znaleziono id w danych dostanych
        {
            xy tym;
            tym.time = present_time;

            int32_t flaga = i.first.bit;
            if (flaga != -1) {
                tym.wartosc = ((infromation->second & flaga) ? 1 : 0) * 100;
            } else {
                tym.wartosc = infromation->second;
            }

            auto it = errors.find({i.first.id, flaga});
            if (it != errors.end()) ///zmaleziono id w bledach
            {
                ///sprawdzanie czy blad wystapil
                const auto &error_inf = ch.find(it->second.error_id);
                if (error_inf != ch.end()) ///zmalezono dana bledu
                {
                    it->second.error = error_inf->second & it->second.error_bit;
                }

                tym.wartosc = it->second.error ? 0 : tym.wartosc;
            }

            if (i.second.size() >= 2) {
                if (i.second.rbegin()->wartosc == tym.wartosc
                    && (++(i.second.rbegin()))->wartosc == tym.wartosc) {
                    i.second.back().time = tym.time;
                    continue;
                }
            }

            i.second.push_back(tym);
        }
    }

    calculateScrollbar();
}

void Logi::registerIDforTracking(int32_t tab, std::string nazwa, int32_t id, sf::Color color, int32_t bit) {
    tab_nazwy[tab].push_back(std::make_tuple(id, nazwa, color, bit));

    if (tab_range.find(tab) == tab_range.end()) {
        tab_range[tab] = std::make_tuple(-30, 70);
    }

    //tab_nazwy[tab].unique();

    //dane[std::make_tuple(id, bit)];
    dane[{id, bit}].push_back({time(nullptr), 0});
}

void Logi::registerRangeofTab(int32_t tab, int32_t minimum, int32_t maximum) {
    tab_range[tab] = std::make_tuple(minimum, maximum);
}

void Logi::registerIDforErrors(int32_t id, int32_t bit, int32_t error_id, int32_t error_bit) {
    errors[{id, bit}] = {error_id, error_bit, false};
}

void Logi::updateIDforTracking() {
    this->remove(this->get("t_panel"));

    tgui::Panel::Ptr p1 = WidgetSingleton<tgui::Panel>::get(*this, "t_panel");
    p1->setPosition(890, 10);
    p1->setSize(280, 16);
    p1->getRenderer()->setBackgroundColor(sf::Color::Transparent);

    int32_t j = 0;
    for (auto i = tab_nazwy.begin(); i != tab_nazwy.end(); ++i, ++j) {
        tgui::Button::Ptr button = WidgetSingleton<tgui::Button>::get(*p1, "t" + Game_api::convertInt(j));
        //button->load(THEME_CONFIG_FILE);
        button->setSize(95, 16);
        button->setPosition(j * 105, 0);
        button->setText("Tab " + Game_api::convertInt(i->first));
        button->connect("Pressed", [&, id = j, widget = button]() {

            int32_t j = 0;
            for (auto i = tab_nazwy.begin(); i != tab_nazwy.end(); ++i, ++j) {
                this->get<tgui::Button>("t" + Game_api::convertInt(j))->getRenderer()->setTextColor(
                        sf::Color(0, 0, 0, 255));
                this->get<tgui::Panel>("b" + Game_api::convertInt(j))->setVisible(false);
            }
            std::dynamic_pointer_cast<tgui::Button>(widget)->getRenderer()->setTextColor(sf::Color(255, 0, 0, 255));
            this->get<tgui::Panel>("b" + Game_api::convertInt(id))->setVisible(true);

            ///usun i dodaj labels

            onValueChange.emit(this);
        });



        ///tworze panele nazwy w zakladkach
        this->remove(this->get("b" + Game_api::convertInt(j)));

        tgui::Panel::Ptr p2 = WidgetSingleton<tgui::Panel>::get(*this, "b" + Game_api::convertInt(j));
        p2->setPosition(890, 30);
        p2->setSize(200, 280);
        p2->getRenderer()->setBackgroundColor(sf::Color::Transparent);

        int32_t z = 0;
        for (auto k : i->second) ///lista id w tab_nazwy
        {
            tgui::CheckBox::Ptr ch = WidgetSingleton<tgui::CheckBox>::get(*p2,
                                                                          "ch" + Game_api::convertInt(std::get<0>(k)) +
                                                                          "-"
                                                                          + Game_api::convertInt(std::get<3>(k)));
            //ch->load(THEME_CONFIG_FILE);
            ch->setPosition(0, 0 + z * 25);
            ch->setSize(12, 12);
            ch->setText(std::get<1>(k));
            ch->getRenderer()->setTextColor(std::get<2>(k));
            ch->setTextSize(12);
            ch->setChecked(true);

            ++z;
        }

        p2->setVisible(false);
    }

    this->get<tgui::Button>("t0")->onPress.emit(this->get("t0").get(), "");
}

tgui::Signal &Logi::getSignal(std::string signalName) {
    if (signalName == tgui::toLower(onValueChange.getName()))
        return onValueChange;
    else
        return tgui::Panel::getSignal(std::move(signalName));
}

void Logi::initialize() {
    tgui::Scrollbar::Ptr scrollbar = WidgetSingleton<tgui::Scrollbar>::get(*this, "scr");
    //scrollbar->load(THEME_CONFIG_FILE);
    scrollbar->setPosition(10, 10);
    scrollbar->setSize(855, 20);

    calculateScrollbar();

    const static std::vector<std::tuple<std::string, float>> wartosci = {
            std::make_tuple("1m", 0.1),
            std::make_tuple("10m", 1),
            std::make_tuple("30m", 3),
            std::make_tuple("1h", 6),
            std::make_tuple("12h", 72),
            std::make_tuple("2d", 288),
            std::make_tuple("4d", 576)};
    for (uint32_t i = 0; i < wartosci.size(); ++i) {
        tgui::Button::Ptr button = WidgetSingleton<tgui::Button>::get(*this, "time" + Game_api::convertInt(i));
        //button->load(THEME_CONFIG_FILE);
        button->setSize(95, 16);
        button->setPosition(1090, 30 + i * 25);
        button->setText(std::get<0>(wartosci[i]));
        button->connect("Pressed", [this, i, button]() -> void {
            czas = std::get<1>(wartosci[i]);
            for (int32_t j = 0; j < 6; ++j) {
                this->get<tgui::Button>(
                        "time" + Game_api::convertInt(j))->getRenderer()->setTextColor(
                        sf::Color(0, 0, 0, 255));
            }

            button->getRenderer()->setTextColor(sf::Color::Red);

            this->calculateScrollbar();
        });

        if (std::get<1>(wartosci[i]) == this->czas) { button->getRenderer()->setTextColor(sf::Color::Red); }
    }
}

void Logi::update(sf::Time elapsedTime) {
    calculateScrollbar();
}

void Logi::calculateScrollbar() {
    tgui::Scrollbar::Ptr scrollbar = this->get<tgui::Scrollbar>("scr");
    scrollbar->setViewportSize(15);///rozmiar jednej strony


    time_t minn = time(nullptr);
    time_t maxx = time(nullptr);

    for (auto it : dane) {
        if (!std::get<1>(it).empty()) {
            minn = std::min(minn, std::get<1>(it).front().time);
            maxx = std::max(maxx, std::get<1>(it).back().time);
        }
    }

    float diff = (maxx - minn);
    diff /= this->czas * 40;


    int32_t value = scrollbar->getValue();
    int32_t smaxx = scrollbar->getMaximum();

    scrollbar->setMaximum(std::ceil(diff));///rozmiar wszystkich danych

    if (value + 15 >= smaxx) {
        scrollbar->setValue(scrollbar->getMaximum() - 15);
    }

}

void Logi::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    states.transform.translate(getPosition());

    // Draw the borders
    if (m_bordersCached != tgui::Borders{0}) {
        drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
        states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
    }

    // Draw the background
    const sf::Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                    getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    drawRectangleShape(target, states, innerSize, m_backgroundColorCached);

    states.transform.translate(m_paddingCached.getLeft(), m_paddingCached.getTop());
    const sf::Vector2f contentSize = {innerSize.x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                      innerSize.y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

    // Draw the child widgets
    const tgui::Clipping clipping{target, states, {}, contentSize};
    drawWidgetContainer(&target, states);

    //Draw log
    drawLogi(target, states);
}

void Logi::drawLogi(sf::RenderTarget &target, sf::RenderStates states) const {
    states.transform.translate(10, 30); //pozycja rysowania
    sf::Vector2f size(getSize().x - 300, getSize().y - 30); ///powierzchnia rysowania

    sf::Vector2i topLeftPosition = target.mapCoordsToPixel(states.transform.transformPoint({}));
    sf::Vector2i bottomRightPosition = target.mapCoordsToPixel(states.transform.transformPoint(size));

    // Get the old clipping area
    GLint m_scissor[4];
    glGetIntegerv(GL_SCISSOR_BOX, m_scissor);

    // Calculate the clipping area
    GLint scissorLeft = std::max(static_cast<GLint>(topLeftPosition.x), m_scissor[0]);
    GLint scissorTop = std::max(static_cast<GLint>(topLeftPosition.y),
                                static_cast<GLint>(target.getSize().y) - m_scissor[1] - m_scissor[3]);
    GLint scissorRight = std::min(static_cast<GLint>(bottomRightPosition.x), m_scissor[0] + m_scissor[2]);
    GLint scissorBottom = std::min(static_cast<GLint>(bottomRightPosition.y),
                                   static_cast<GLint>(target.getSize().y) - m_scissor[1]);

    // If the object outside the window then don't draw anything
    if (scissorRight < scissorLeft)
        scissorRight = scissorLeft;
    else if (scissorBottom < scissorTop)
        scissorTop = scissorBottom;

    // Set the clipping area
    glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

    states.transform.translate(0, 10); ///obnizamy rysowanie
    size.y -= 20; ///zmniejszamy rozmiar rysowania

    ///obliczamy strone
    int32_t strona = -1;
    int32_t j = 0;
    for (auto i = tab_nazwy.begin(); i != tab_nazwy.end(); ++i, ++j) ///aktywna strona
    {
        if (this->get<tgui::Button>("t" + Game_api::convertInt(j))->getRenderer()->getTextColor() == sf::Color::Red) {
            strona = i->first;
        }
    }


    sf::Text text;
    text.setFont(*m_fontCached.getFont());
    sf::RenderStates st = states;
    sf::VertexArray tab(sf::Lines, 2);
    tab[0].position = sf::Vector2f(1, 0);
    tab[1].position = sf::Vector2f(size.x - 25, 0);
    tab[0].color = sf::Color::Black;
    tab[1].color = sf::Color::Black;

    text.setCharacterSize(10);
    text.setFillColor(sf::Color::Black);
    text.setPosition(size.x - 20, -7);

    ///pozime
    sf::Vector2f scala((size.x - 25) / 15, size.y / 10);///scala rysowania wykresu

    int32_t temperature_max = 70;
    int32_t temperature_min = -30;

    if (strona != -1 && tab_nazwy.find(strona) != tab_nazwy.end()) {
        if (tab_range.find(strona) != tab_range.end()) {
            temperature_min = std::get<0>(tab_range.find(strona)->second);
            temperature_max = std::get<1>(tab_range.find(strona)->second);
        }
    }

    float temperature_scale;

    temperature_scale = static_cast<float>(temperature_max - temperature_min) / 10.0f;

    for (int i = 0; i < 11; ++i) //cale
    {
        text.setString(Game_api::convertInt(temperature_max - i * temperature_scale)); //od 70 do -30

        target.draw(text, st);

        target.draw(tab, st);

        if (i == 7) //linia 0
        {
            st.transform.translate(0, 1);
            text.setString(text.getString() + "*C");
            target.draw(tab, st);
            st.transform.translate(0, -1);
        }

        st.transform.translate(0, scala.y);
    }

    tab[0].position = sf::Vector2f(1, 0);
    tab[1].position = sf::Vector2f(1, size.y);
    tab[0].color = sf::Color::Black;
    tab[1].color = sf::Color::Black;

    st = states;

    ///pionowe
    text.setPosition(20, 20);
    text.setRotation(90);
    text.setCharacterSize(16);
    text.setStyle(sf::Text::Bold);

    tgui::Scrollbar::Ptr scrollbar = this->get<tgui::Scrollbar>("scr");
    int32_t przesuniecie = (static_cast<int32_t>(scrollbar->getValue()) + 15 - scrollbar->getMaximum());
    if (scrollbar->getMaximum() < 15) { przesuniecie = 0; }

    for (int i = 0; i < 16; ++i) {
        target.draw(tab, st);
        if (!(i % 3 || i == 0 || i == 15)) {
            time_t tym = time(nullptr) + przesuniecie * 40 * this->czas -
                         (15 - i) * 40 * this->czas;///(ilosc dzialek - dzialka) * wsp

            tm z = *localtime(&tym);
            //text.setString(Game_api::convertInt(z.tm_hour) + ":" + Game_api::convertInt(z.tm_min));
            char buf[80];
            strftime(buf, 80, "%d.%m.%Y %H:%M:%S", &z);
            //printf(buf);
            text.setString(buf);
            target.draw(text, st);

        }

        st.transform.translate(scala.x, 0);
    }


    time_t min_czas;
    time_t max_czas;

    min_czas = time(nullptr) - 15 * 40 * this->czas + przesuniecie * 40 * this->czas;
    max_czas = time(nullptr) + przesuniecie * 40 * this->czas;

    ///wykres
    ///Nowy scizor wykresu
    {
        bottomRightPosition = target.mapCoordsToPixel(states.transform.transformPoint(size.x - 25, size.y));
        scissorRight = std::min(static_cast<GLint>(bottomRightPosition.x), m_scissor[0] + m_scissor[2]);
        // If the object outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft,
                  scissorBottom - scissorTop);
    }


    if (strona != -1 && tab_nazwy.find(strona) != tab_nazwy.end())
        for (const auto &it : tab_nazwy.find(strona)->second) ///jakie wykresy
        {
            if (!this->get<tgui::CheckBox>("ch" + Game_api::convertInt(std::get<0>(it)) + "-"
                                           + Game_api::convertInt(std::get<3>(it)))->isChecked()) { continue; }

            const auto &obj = dane[{std::get<0>(it), std::get<3>(it)}];

            st = states;

            sf::VertexArray vertex_tab;
            sf::Vertex vertex;

            vertex_tab.setPrimitiveType(sf::LinesStrip);

            for (auto d = obj.begin(); d != obj.end(); ++d) ///dane wykresu
            {
                auto tym = d;
                if (++tym != obj.end()) { if (d->time < min_czas && tym->time < min_czas) { continue; }}
                tym = d;
                if (tym != obj.begin()) {
                    --tym;
                    if (d->time > max_czas && tym->time > max_czas) { continue; }
                }

                vertex.position.x =
                        (size.x - 25) - (time(nullptr) - d->time) * scala.x / (40.0 * this->czas) -
                        przesuniecie * scala.x;

                vertex.position.y = ((static_cast<float>(temperature_max) - d->wartosc) / temperature_scale) * scala.y;


                if (vertex.position.y > size.y) { vertex.position.y = size.y; }
                if (vertex.position.y < 0) { vertex.position.y = 0; }

                vertex.color = std::get<2>(it);

                vertex_tab.append(vertex);
            }
            if (vertex.position.x < (size.x - 25)) {
                vertex.position.x = (size.x - 25);
                vertex_tab.append(vertex);
            }

            target.draw(vertex_tab, st);
            st.transform.translate(0, -1);
            target.draw(vertex_tab, st);
        }

    // Reset the old clipping area
    glScissor(m_scissor[0], m_scissor[1], m_scissor[2], m_scissor[3]);
}

