#include "MenuState.h"
#include "GameState.h"

MenuState::MenuState(sf::RenderWindow& window, StateManager& stateManager,
    std::vector<Personaje*> personajes, Personaje* (&tablero)[8][8],
    std::queue<Personaje*> orden_Turno)
    : m_window(window), 
    m_stateManager(stateManager),
    m_personajes(personajes), 
    m_tablero(tablero),
    m_ordenTurno(orden_Turno),
    m_selectedIndex(0), 
    m_exit(false)
{
    m_font.loadFromFile("assets/fonts/arial.ttf");
    std::vector<std::string> optionNames = { "Jugar", "Configuración", "Salir" };
    for (size_t i = 0; i < optionNames.size(); ++i) {
        sf::Text option;
        option.setFont(m_font);
        option.setString(optionNames[i]);
        option.setCharacterSize(40);
        option.setFillColor(i == m_selectedIndex ? sf::Color::Yellow : sf::Color::White);
        option.setPosition(200.f, 150.f + i * 60.f);
        m_options.push_back(option);
    }
}

void MenuState::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up)
            if (m_selectedIndex > 0) --m_selectedIndex;
        if (event.key.code == sf::Keyboard::Down)
            if (m_selectedIndex < m_options.size() - 1) ++m_selectedIndex;

        if (event.key.code == sf::Keyboard::Enter) {
            if (m_selectedIndex == 0) {
    
                m_stateManager.setState(std::make_unique<GameState>(
                    m_window, m_stateManager, m_personajes, m_tablero, m_ordenTurno));
            }
            else if (m_selectedIndex == 2) {
                m_exit = true;
            }
        }
    }
}

void MenuState::update() {
    for (size_t i = 0; i < m_options.size(); ++i)
        m_options[i].setFillColor(i == m_selectedIndex ? sf::Color::Yellow : sf::Color::White);
}

void MenuState::render() {
    m_window.clear();
    for (auto& opt : m_options)
        m_window.draw(opt);
    m_window.display();
}
bool MenuState::debeSalir() const {
    return m_exit;
}
