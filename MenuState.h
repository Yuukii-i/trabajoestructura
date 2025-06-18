#pragma once
#include "State.h"
#include "StateManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include "hPersonaje.h"
#include <SFML/Audio.hpp>
class MenuState : public State {
public:
    MenuState(sf::RenderWindow& window, StateManager& stateManager,
        std::vector<Personaje*> personajes, Personaje* (&tablero)[8][8],
        std::queue<Personaje*> orden_Turno);

    void handleEvent(sf::Event& event) override;
    void update() override;
    void render() override;
    bool shouldExit() const override { return m_exit; }
    const float anchoBarra = 40.f;
    bool debeSalir() const;
    void reproducirCancionMenuAleatoria();

private:
    sf::RenderWindow& m_window;
    StateManager& m_stateManager;
    std::vector<Personaje*> m_personajes;
    Personaje* (&m_tablero)[8][8];
    std::queue<Personaje*> m_ordenTurno;
    std::vector<sf::Text> m_options;
    sf::Font m_font;
    int m_selectedIndex;
    bool m_exit;
    sf::Texture m_logo;
    sf::Sprite m_logoSprite;
    sf::Texture m_fondo;
    sf::Sprite m_fondoSprite;
    sf::Music m_music;
    std::vector<std::string> menuThemes;
};
