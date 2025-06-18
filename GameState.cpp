#include "GameState.h"
#include "MenuState.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <iostream>
GameState::GameState(sf::RenderWindow& window, StateManager& stateManager,
    std::vector<Personaje*> personajes, Personaje* (&tablero)[8][8],
    std::queue<Personaje*> orden_Turno)
    : m_window(window),
    m_stateManager(stateManager),
    m_personajes(personajes),
    m_tablero(tablero),
    m_ordenTurno(orden_Turno)

{
    float cellSize = 50.f;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            m_grid[i][j].setSize(sf::Vector2f(cellSize, cellSize));
            m_grid[i][j].setPosition(j * cellSize, i * cellSize);
            m_grid[i][j].setFillColor(sf::Color::White);
            m_grid[i][j].setOutlineThickness(1.f);
            m_grid[i][j].setOutlineColor(sf::Color::Black);
        }
    }
    if (!b_font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cout << "Error en fuente" << std::endl;
    }
    std::vector<std::string> options = { "Atacar", "Defenderse", "Habilidad" };
    for (int i = 0; i < options.size(); ++i) {
        sf::Text text;
        text.setFont(b_font);
        text.setString(options[i]);
        text.setCharacterSize(24);
        text.setPosition(420.f, 50.f + i * 40.f);
        text.setFillColor(sf::Color::White);
        m_actionOptions.push_back(text);
    }
    m_currentActionMenu = ActionMenuState::Open;

    m_combatSongs = {
    "assets/sounds/bgm/battle1.ogg",
    "assets/sounds/bgm/battle2.ogg",
    "assets/sounds/bgm/battle3.ogg",
    "assets/sounds/bgm/battle4.ogg"
    };
    m_targetCursor.setSize(sf::Vector2f(50.f, 50.f));
    m_targetCursor.setFillColor(sf::Color::Transparent);
    m_targetCursor.setOutlineThickness(2.f);
    m_targetCursor.setOutlineColor(sf::Color::Red);
    m_targetPos = { 0, 0 };

    reproducirCancionDeCombateAleatoria();
    inicializarOrdenTurnos();

}

void GameState::handleEvent(sf::Event& event) {
    if (m_finDelJuego) {
        handleGameOverInput(event);
        return;
    }

    if (event.type != sf::Event::KeyPressed) return;

    switch (m_currentActionMenu) {
    case ActionMenuState::Open:
        handleActionMenuInput(event);
        break;
    case ActionMenuState::Attack:
        handleAttackInput(event);
        break;
    case ActionMenuState::Skill_Select:
        handleSkillSelectInput(event);
        break;
    case ActionMenuState::Skill_ConfirmTarget:
        handleSkillConfirmTargetInput(event);
        break;
    default:
        break;
    }
}
// Maneja la entrada cuando el juego ha terminado
void GameState::handleGameOverInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        m_window.close();
    }
}

// Menú principal de acciones (Atacar, Defender, Habilidad)
void GameState::handleActionMenuInput(sf::Event& event) {
    if (event.key.code == sf::Keyboard::Down && m_selectedOptionIndex < m_actionOptions.size() - 1)
        m_selectedOptionIndex++;
    else if (event.key.code == sf::Keyboard::Up && m_selectedOptionIndex > 0)
        m_selectedOptionIndex--;
    else if (event.key.code == sf::Keyboard::Enter) {
        switch (m_selectedOptionIndex) {
        case 0: m_currentActionMenu = ActionMenuState::Attack;
            actualizarObjetivosValidosBasico();
            break;
        case 1: m_currentActionMenu = ActionMenuState::Defend; break;
        case 2:
            m_currentActionMenu = ActionMenuState::Skill_Select;
            cargarSkillsDelPersonaje(m_personajeEnTurno);
            break;
        }
        std::cout << "Seleccionaste opción: " << m_selectedOptionIndex << std::endl;
    }
    else if (event.key.code == sf::Keyboard::Escape) {
        m_stateManager.setState(std::make_unique<MenuState>(m_window, m_stateManager, m_personajes, m_tablero, m_ordenTurno));
    }
}

// Entrada en modo ataque: mover cursor y atacar
void GameState::handleAttackInput(sf::Event& event) {
    if (m_objetivosValidos.empty()) return;

    if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Down) {
        m_selectedTargetIndex = (m_selectedTargetIndex + 1) % m_objetivosValidos.size();
    }
    else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Up) {
        m_selectedTargetIndex = (m_selectedTargetIndex - 1 + m_objetivosValidos.size()) % m_objetivosValidos.size();
    }

    if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Down ||
        event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Up) {
        Personaje* target = m_objetivosValidos[m_selectedTargetIndex];
        m_targetPos = target->getPosicion();
    }

    else if (event.key.code == sf::Keyboard::Enter) {
        Personaje* objetivo = m_objetivosValidos[m_selectedTargetIndex];
        if (objetivo && objetivo != m_personajeEnTurno) {
            if (m_personajeEnTurno->atacar(objetivo)) {
                verificarFinDelJuego();
                pasarTurno();
            }
            m_currentActionMenu = ActionMenuState::Open;
        }
    }
    else if (event.key.code == sf::Keyboard::Escape) {
        m_currentActionMenu = ActionMenuState::Open;
    }
}


// Entrada en menú de selección de habilidad
void GameState::handleSkillSelectInput(sf::Event& event) {
    if (event.key.code == sf::Keyboard::Down && m_selectedSkillIndex < m_skillOptions.size() - 1)
        m_selectedSkillIndex++;
    else if (event.key.code == sf::Keyboard::Up && m_selectedSkillIndex > 0)
        m_selectedSkillIndex--;
    else if (event.key.code == sf::Keyboard::Enter) {
        std::vector<Habilidad*> habilidades = m_personajeEnTurno->getHabilidades();
        int indexSkillReal = 0;

        for (int i = 0; i < habilidades.size(); ++i) {
            Skill* s = dynamic_cast<Skill*>(habilidades[i]);
            if (!s) continue;
            if (indexSkillReal == m_selectedSkillIndex) {
                m_habilidadSeleccionada = s;
                break;
            }
            indexSkillReal++;
        }

        if (m_habilidadSeleccionada) {
            if (m_habilidadSeleccionada->getEfecto() == "Single") {
                m_objetivosValidos.clear();
                for (Personaje* p : m_personajes) {
                    if (p && p->getVivo() && p->getControl() != m_personajeEnTurno->getControl()) {
                        m_objetivosValidos.push_back(p);
                    }
                }
                std::cout << "[DEBUG] Objetivos válidos encontrados: " << m_objetivosValidos.size() << std::endl;
                m_selectedTargetIndex = 0;
                m_currentActionMenu = ActionMenuState::Skill_ConfirmTarget;
            }

            else {
                auto objetivos = getObjetivosPorEfecto(m_habilidadSeleccionada->getEfecto(), m_personajeEnTurno);
                usarSkill(m_habilidadSeleccionada, m_personajeEnTurno, objetivos);
                verificarFinDelJuego();
                pasarTurno();
                m_currentActionMenu = ActionMenuState::Open;
            }
        }
    }
    else if (event.key.code == sf::Keyboard::Escape) {
        m_currentActionMenu = ActionMenuState::Open;
    }
}

// Confirmación del objetivo para skills tipo "Single"
void GameState::handleSkillConfirmTargetInput(sf::Event& event) {
    if (m_objetivosValidos.empty()) return;

    if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Down) {
        m_selectedTargetIndex = (m_selectedTargetIndex + 1) % m_objetivosValidos.size();
    }
    else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Up) {
        m_selectedTargetIndex--;
        if (m_selectedTargetIndex < 0) m_selectedTargetIndex = m_objetivosValidos.size() - 1;
    }
    else if (event.key.code == sf::Keyboard::Enter) {
        Personaje* objetivo = m_objetivosValidos[m_selectedTargetIndex];
        if (objetivo) {
            usarSkill(m_habilidadSeleccionada, m_personajeEnTurno, { objetivo });
            verificarFinDelJuego();
            pasarTurno();
            m_currentActionMenu = ActionMenuState::Open;
            m_habilidadSeleccionada = nullptr;
            m_objetivosValidos.clear();
        }
    }
    else if (event.key.code == sf::Keyboard::Escape) {
        m_currentActionMenu = ActionMenuState::Skill_Select;
        m_objetivosValidos.clear();
    }
}
void GameState::actualizarObjetivosValidosBasico() {
    m_objetivosValidos.clear();
    for (Personaje* p : m_personajes) {
        if (p && p->getVivo() && p->getControl() != m_personajeEnTurno->getControl()) {
            m_objetivosValidos.push_back(p);
        }
    }
    m_selectedTargetIndex = 0;

    if (!m_objetivosValidos.empty()) {
        m_targetPos = m_objetivosValidos[m_selectedTargetIndex]->getPosicion();
    }
}

void GameState::cargarSkillsDelPersonaje(Personaje* p) {
    m_skillOptions.clear();

    std::vector<Habilidad*> habilidades = p->getHabilidades();
    m_selectedSkillIndex = 0;

    for (int i = 0; i < habilidades.size(); ++i) {
        Skill* skill = dynamic_cast<Skill*>(habilidades[i]);
        if (!skill) continue;

        sf::Text text;
        text.setFont(b_font);
        text.setString(skill->getNombre());
        text.setCharacterSize(20);
        text.setPosition(420.f, 50.f + i * 30.f);
        text.setFillColor(sf::Color::White);
        m_skillOptions.push_back(text);
    }

    m_skillDescription.setFont(b_font);
    m_skillDescription.setCharacterSize(16);
    m_skillDescription.setFillColor(sf::Color::White);
    m_skillDescription.setPosition(420.f, 300.f);
}


void GameState::update() {
   
    for (int i = 0; i < m_actionOptions.size(); ++i) {
        if (i == m_selectedOptionIndex)
            m_actionOptions[i].setFillColor(sf::Color::Yellow);
        else
            m_actionOptions[i].setFillColor(sf::Color::White);
    }
}

void GameState::verificarFinDelJuego() {
    bool j1Vivo = false;
    bool j2Vivo = false;

    for (auto p : m_personajes) {
        if (p->getVivo()) {
            if (p->getControl() == "J1")
                j1Vivo = true;
            else if (p->getControl() == "J2")
                j2Vivo = true;
        }
    }

    if (!j1Vivo || !j2Vivo) {
        m_finDelJuego = true;
        m_ganador = j1Vivo ? "Jugador 1 (J1)" : "Jugador 2 (J2)";
    }
}

void GameState::pasarTurno() {
    verificarFinDelJuego();
    if (!m_ordenTurno.empty()) {
        m_ordenTurno.pop();
    }

    if (m_ordenTurno.empty()) {
        
        inicializarOrdenTurnos();
    }

    if (!m_ordenTurno.empty()) {
        m_personajeEnTurno = m_ordenTurno.front();
    }
    else {
        m_personajeEnTurno = nullptr;
    }
}

void GameState::inicializarOrdenTurnos(){
    while (!m_ordenTurno.empty()) m_ordenTurno.pop();
    std::vector<Personaje*> temp;
    for (auto p : m_personajes) {
        if (p != nullptr)
            temp.push_back(p);
    }

    std::sort(temp.begin(), temp.end(), [](Personaje* a, Personaje* b) {
        return a->getVelocidad() > b->getVelocidad();
        });

    for (auto p : temp) {
        m_ordenTurno.push(p);
    }

    if (!m_ordenTurno.empty())
        m_personajeEnTurno = m_ordenTurno.front();
    else
        m_personajeEnTurno = nullptr;
}


void GameState::dibujarBarras(Personaje* p, sf::RenderWindow& window) {
    if (p == nullptr) return;

    float posX = (p->getPosicion().first) * 50.f;
    float posY = (p->getPosicion().second) * 50.f;

    const float anchoBarra = 40.f;
    const float altoBarra = 5.f;
    const float offsetVidaY = -12.f;
    const float offsetManaY = -5.f;

    // ===== Barra de Vida =====
    float vidaRatio = (float)p->getVidaActual() / p->getVidaMax();
    sf::RectangleShape fondoVida(sf::Vector2f(anchoBarra, altoBarra));
    fondoVida.setFillColor(sf::Color(50, 50, 50));
    fondoVida.setPosition(posX + 5.f, posY + offsetVidaY);

    sf::RectangleShape barraVida(sf::Vector2f(anchoBarra * vidaRatio, altoBarra));
    barraVida.setFillColor(sf::Color::Green);
    barraVida.setPosition(posX + 5.f, posY + offsetVidaY);

    window.draw(fondoVida);
    window.draw(barraVida);

    // ===== Barra de Mana =====
    float manaRatio = (float)p->getMana() / p->getManaMax();
    sf::RectangleShape fondoMana(sf::Vector2f(anchoBarra, altoBarra));
    fondoMana.setFillColor(sf::Color(50, 50, 50));
    fondoMana.setPosition(posX + 5.f, posY + offsetManaY);

    sf::RectangleShape barraMana(sf::Vector2f(anchoBarra * manaRatio, altoBarra));
    barraMana.setFillColor(sf::Color::Blue);
    barraMana.setPosition(posX + 5.f, posY + offsetManaY);

    window.draw(fondoMana);
    window.draw(barraMana);
}


void GameState::render() {
    m_window.clear();

        if (m_currentActionMenu == ActionMenuState::Skill_ConfirmTarget)
    m_targetCursor.setOutlineColor(sf::Color::Blue);
else
    m_targetCursor.setOutlineColor(sf::Color::Red);

    // Dibujar grid
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            m_window.draw(m_grid[i][j]);
    if (m_currentActionMenu == ActionMenuState::Attack) {
        m_targetCursor.setPosition(m_targetPos.first * 50.f, m_targetPos.second * 50.f);
        m_window.draw(m_targetCursor);
    }
    else if (m_currentActionMenu == ActionMenuState::Skill_ConfirmTarget) {
        if (!m_objetivosValidos.empty()) {
            // Validar índice
            if (m_selectedTargetIndex >= 0 && m_selectedTargetIndex < static_cast<int>(m_objetivosValidos.size())) {
                Personaje* objetivo = m_objetivosValidos[m_selectedTargetIndex];
                if (objetivo) {
                    auto pos = objetivo->getPosicion();
                    m_targetCursor.setPosition(pos.first * 50.f, pos.second * 50.f);
                    m_window.draw(m_targetCursor);
                }
            }
            else {
                std::cerr << "[ERROR] Índice de objetivo inválido: " << m_selectedTargetIndex << std::endl;
            }
        }
        else {
            std::cerr << "[INFO] No hay objetivos válidos para esta habilidad.\n";
        }
    }

    // Dibujar personajes
    for (auto p : m_personajes) {
        if (!p->getVivo()) continue;

        sf::CircleShape circle(20.f);

        if (p->getControl() == "J1")
            circle.setFillColor(sf::Color::Green);// J1
        else
            circle.setFillColor(sf::Color::Red);// J2


        circle.setPosition((p->getPosicion().first) * 50.f, (p->getPosicion().second) * 50.f);
        m_window.draw(circle);


        // Flechita de turno
        if (p == m_personajeEnTurno) {
            sf::CircleShape flecha(5.f, 3);
            flecha.setFillColor(sf::Color::Black);
            flecha.setRotation(180.f);
            flecha.setPosition(circle.getPosition().x + 15.f, circle.getPosition().y - 10.f);
            m_window.draw(flecha);
        }
        if (m_personajeEnTurno != nullptr) {
            sf::Text turnoText;
            turnoText.setFont(b_font);
            turnoText.setCharacterSize(18);
            turnoText.setFillColor(sf::Color::White);
            turnoText.setString("Turno de: " + m_personajeEnTurno->getNombre());
            turnoText.setPosition(10.f, 410.f);
            m_window.draw(turnoText);
            sf::Text nombreText;
            nombreText.setFont(b_font);
            nombreText.setCharacterSize(10); 
            nombreText.setFillColor(sf::Color::Black);

            std::string nombre = p->getNombre();
            if (nombre.size() > 8) { 
                nombre = nombre.substr(0, 4) + "\n" + nombre.substr(4, 4);
            }
            nombreText.setString(nombre);

            sf::FloatRect bounds = nombreText.getLocalBounds();
            nombreText.setOrigin(bounds.width / 2, bounds.height / 2);
            nombreText.setPosition(circle.getPosition().x + 20.f, circle.getPosition().y + 20.f);

            m_window.draw(nombreText);

        }

        dibujarBarras(p, m_window);
    }
  
    if (m_currentActionMenu == ActionMenuState::Skill_Select && m_habilidadSeleccionada) {
        std::string efecto = m_habilidadSeleccionada->getEfecto();
        if (efecto == "Multi" || efecto == "Total") {
            std::vector<Personaje*> objetivos = getObjetivosPorEfecto(efecto, m_personajeEnTurno);
            for (Personaje* obj : objetivos) {
                sf::RectangleShape marco(sf::Vector2f(50.f, 50.f));
                marco.setPosition(obj->getPosicion().first * 50.f, obj->getPosicion().second * 50.f);
                marco.setFillColor(sf::Color::Transparent);
                marco.setOutlineThickness(2.f);
                marco.setOutlineColor(sf::Color::Magenta);
                m_window.draw(marco);
            }
        }
    }


    // Dibujar menú de acciones
    if (m_currentActionMenu == ActionMenuState::Open) {
        for (const auto& option : m_actionOptions) {
            m_window.draw(option);
        }
    }
    if (m_currentActionMenu == ActionMenuState::Skill_Select) {
        for (int i = 0; i < m_skillOptions.size(); ++i) {
            if (i == m_selectedSkillIndex)
                m_skillOptions[i].setFillColor(sf::Color::Yellow);
            else
                m_skillOptions[i].setFillColor(sf::Color::White);

            m_window.draw(m_skillOptions[i]);
        }

        // Mostrar descripción
        std::vector<Habilidad*> habilidades = m_personajeEnTurno->getHabilidades();
        int indexSkillReal = 0;
        for (int i = 0; i < habilidades.size(); ++i) {
            Skill* s = dynamic_cast<Skill*>(habilidades[i]);
            if (!s) continue;
            if (indexSkillReal == m_selectedSkillIndex) {
                std::string textoFormateado = formatearDescripcion(s->getDescripcion(), 350, b_font, 16);
                m_skillDescription.setString(textoFormateado);

                break;
            }
            indexSkillReal++;
        }

        m_window.draw(m_skillDescription);
    }

        if (m_finDelJuego) {
            sf::Text textoFin;
            textoFin.setFont(b_font);
            textoFin.setCharacterSize(30);
            textoFin.setFillColor(sf::Color::Red);
            textoFin.setString("¡" + m_ganador + " ha ganado!\nPresiona Enter para salir");
            textoFin.setPosition(150, 250);

            m_window.draw(textoFin);
        }
    m_window.display();
}
std::string GameState::formatearDescripcion(const std::string& texto, unsigned int maxWidth, const sf::Font& font, unsigned int characterSize) {
    std::string resultado;
    std::string palabra;
    sf::Text testText;
    testText.setFont(font);
    testText.setCharacterSize(characterSize);

    for (size_t i = 0; i < texto.size(); ++i) {
        char c = texto[i];
        palabra += c;

        // Si encontramos un espacio o fin de texto
        if (c == ' ' || i == texto.size() - 1) {
            sf::Text temp(resultado + palabra, font, characterSize);
            if (temp.getLocalBounds().width > maxWidth) {
                resultado += "\n" + palabra;
            }
            else {
                resultado += palabra;
            }
            palabra.clear();
        }
    }

    return resultado;
}

void GameState::usarSkill(Skill* skill, Personaje* lanzador, std::vector<Personaje*> objetivos) {
    if (lanzador->getMana() < skill->getCoste()) {
        std::cout << "No hay suficiente maná para usar " << skill->getNombre() << std::endl;
        return;
    }

    lanzador->setMana(lanzador->getMana() - skill->getCoste());
    std::cout << lanzador->getNombre() << " usó " << skill->getNombre() << "!\n";

    for (Personaje* objetivo : objetivos) {
        if (!objetivo || !objetivo->getVivo()) continue;

        // Tirada de acierto
        int tirada = rand() % 10 + 1;
        if (tirada > skill->getAcierto()) {
            std::cout << "¡La habilidad falló contra " << objetivo->getNombre() << "!\n";
            continue;
        }

        // Calcular daño
        float danio = lanzador->getAtaque() * skill->getPotencia();
        int danioFinal = static_cast<int>(danio) - objetivo->getDefensa();
        if (danioFinal < 0) danioFinal = 0;

        objetivo->setVidaActual(objetivo->getVidaActual() - danioFinal);
        std::cout << objetivo->getNombre() << " recibió " << danioFinal << " de daño!\n";

        if (objetivo->getVidaActual() <= 0) {
            objetivo->setVivo(false);
            std::cout << objetivo->getNombre() << " ha sido derrotado!\n";
        }
    }
}

std::vector<Personaje*> GameState::getObjetivosPorEfecto(const std::string& efecto, Personaje* lanzador) {
    std::vector<Personaje*> objetivos;

    for (Personaje* p : m_personajes) {
        if (!p || !p->getVivo()) continue;

        if (efecto == "Single") {
        }
        else if (efecto == "Multi") {
            if (p->getControl() != lanzador->getControl()) {
                objetivos.push_back(p);
            }
        }
        else if (efecto == "Total") {
            objetivos.push_back(p);
        }
    }

    return objetivos;
}



void GameState::reproducirCancionDeCombateAleatoria() {
    if (m_combatSongs.empty()) return;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, m_combatSongs.size() - 1);

    std::string cancionElegida = m_combatSongs[distrib(gen)];

    music.stop();
    if (!music.openFromFile(cancionElegida)) {
        std::cout << "No se pudo cargar " << cancionElegida << std::endl;
        return;
    }
    music.setLoop(true);
    music.setVolume(10.f);
    music.play();
    

}

