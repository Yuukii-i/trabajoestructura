#include "MenuState.h"
#include "GameState.h"
#include <cstdlib>  
#include <ctime>

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
        option.setPosition(40.f, 269.f + i * 60.f);
        m_options.push_back(option);
    }
    menuThemes = {
        "assets/sounds/menubgm/mbgm1.ogg",
        "assets/sounds/menubgm/mbgm2.ogg",
        "assets/sounds/menubgm/mbgm3.ogg"
    };
    if (!m_logo.loadFromFile("assets/images/logo.png")) {
        std::cerr << "No se pudo cargar el logo" << std::endl;
    }
    else {
        m_logoSprite.setTexture(m_logo);
        m_logoSprite.setPosition(50.f, 50.f); // Ajusta la posición según lo necesites
    }
    if (!m_fondo.loadFromFile("assets/images/fondoMenu.png")) {
    std::cerr<< "No se pudo cargar el fondo" << std::endl;
    }
    else {
        m_fondoSprite.setTexture(m_fondo);
        sf::Vector2u textureSize = m_fondo.getSize();
        sf::Vector2u windowSize = m_window.getSize();

        float scaleX = static_cast<float>(windowSize.x-10) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        m_fondoSprite.setScale(scaleX, scaleY);
    }
    reproducirCancionMenuAleatoria();
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
    m_window.draw(m_fondoSprite);
    m_window.draw(m_logoSprite);
    m_logoSprite.setScale(0.3f, 0.3f); // Reduce a la mitad
    m_logoSprite.setPosition(0.f, -10.f);

    for (auto& opt : m_options)
        m_window.draw(opt);
    m_window.display();
}
bool MenuState::debeSalir() const {
    return m_exit;
}
void MenuState::reproducirCancionMenuAleatoria() {
    if (menuThemes.empty()) return;

    std::srand(static_cast<unsigned>(std::time(nullptr))); // Semilla para aleatoriedad

    int index = std::rand() % menuThemes.size(); // Índice aleatorio

    if (!m_music.openFromFile(menuThemes[index])) {
        std::cerr << "No se pudo reproducir la canción: " << menuThemes[index] << std::endl;
        return;
    }

    m_music.setLoop(true); // Para que se repita indefinidamente
    m_music.play();
}