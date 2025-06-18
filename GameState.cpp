#include "GameState.h"
#include "MenuState.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <iostream>
#include "GameStateSim.h"
#include "IAUtils.h"

GameState::GameState(sf::RenderWindow& window, StateManager& stateManager,
    std::vector<Personaje*> personajes, Personaje* (&tablero)[8][8],
    std::queue<Personaje*> orden_Turno)
    : m_window(window),
    m_stateManager(stateManager),
    m_personajes(personajes),
    m_tablero(tablero),
    m_ordenTurno(orden_Turno)

{
    if (!b_font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cout << "Error en fuente" << std::endl;
    }
    std::vector<std::string> options = { "Atacar", "Habilidad" };
    for (int i = 0; i < options.size(); ++i) {
        sf::Text text;
        text.setFont(b_font);
        text.setString(options[i]);
        text.setCharacterSize(32);
        text.setPosition(420.f, 50.f + i * 80.f);
        text.setFillColor(sf::Color::White);
        m_actionOptions.push_back(text);
    }
    m_currentActionMenu = ActionMenuState::Open;
    
    if (!texturaFondo.loadFromFile("assets/images/fondo.jpg")) {
        std::cerr << "No se pudo cargar el fondo\n";
    }

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
    int indexAliado = 0;
    int indexEnemigo = 0;
    float offsetAliado = 70.f;
    float offsetEnemigo = 70.f;

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
        case 2: m_currentActionMenu = ActionMenuState::Defend; break;
        case 1:
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
    m_skillDescription.setCharacterSize(22);
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

    // Verificar si es el turno de la IA
    if (m_personajeEnTurno != nullptr && m_personajeEnTurno->getControl() == "J2") {

        GameStateSim estadoSimulado = crearSimulacion();

        GameStateSim mejorEstado = obtenerMejorAccion(estadoSimulado);

        //aplicar daños
        actualizarEstadoConSimulacion(mejorEstado);

        //Terminar turno
        pasarTurno();
    }

}

void GameState::inicializarOrdenTurnos() {
    while (!m_ordenTurno.empty()) m_ordenTurno.pop();

    std::vector<Personaje*> vivos;
    for (auto p : m_personajes) {
        if (p && p->getVivo()) {
            vivos.push_back(p);
        }
    }

    std::sort(vivos.begin(), vivos.end(), [](Personaje* a, Personaje* b) {
        return a->getVelocidad() > b->getVelocidad();
    });

    for (auto p : vivos) {
        m_ordenTurno.push(p);
    }

    if (!m_ordenTurno.empty())
        m_personajeEnTurno = m_ordenTurno.front();
    else
        m_personajeEnTurno = nullptr;
}




void GameState::render() {
    m_window.clear();

    // 1. Fondo
    sf::Sprite fondo(texturaFondo);
    sf::Vector2u windowSize = m_window.getSize();
    sf::Vector2u imageSize = texturaFondo.getSize();
    fondo.setScale(
        float(windowSize.x) / imageSize.x,
        float(windowSize.y) / imageSize.y
    );
    m_window.draw(fondo);

    // 2. Dibujar personajes
    int idxAliado = 0, idxEnemigo = 0;
    const float baseY = 100.f, offsetY = 80.f;
    const float xAliado = 100.f, xEnemigo = windowSize.x - 180.f;

    std::map<Personaje*, sf::Vector2f> spritePositions;

    for (auto p : m_personajes) {
        if (!p->getVivo()) continue;

        sf::Texture tex;
        if (!tex.loadFromFile(p->getSprite())) continue;
        sf::Sprite sprite(tex);
        sf::Vector2u ts = tex.getSize();
        sprite.setScale(68.f / ts.x, 68.f / ts.y);

        float x = (p->getControl() == "J1") ? xAliado : xEnemigo;
        float y = baseY + ((p->getControl() == "J1") ? idxAliado++ : idxEnemigo++) * offsetY;
        sprite.setPosition(x, y);
        m_window.draw(sprite);

        spritePositions[p] = { x, y };

        // Vida encima (solo para enemigos)
        if (p->getControl() != "J1") {
            float vidaRatio = static_cast<float>(p->getVidaActual()) / p->getVidaMax();
            float barWidth = 48.f, barHeight = 6.f;

            sf::RectangleShape bg(sf::Vector2f(barWidth, barHeight));
            bg.setFillColor(sf::Color(50, 50, 50));
            bg.setPosition(x, y - 10.f);

            sf::RectangleShape fill(sf::Vector2f(barWidth * vidaRatio, barHeight));
            fill.setFillColor(sf::Color::Red);
            fill.setPosition(x, y - 10.f);

            m_window.draw(bg);
            m_window.draw(fill);
        }
    }
    // 2.5. Indicador de turno actual
    if (m_personajeEnTurno && m_personajeEnTurno->getControl() == "J1") {
        auto it = spritePositions.find(m_personajeEnTurno);
        if (it != spritePositions.end()) {
            sf::Vector2f pos = it->second;

            
            float spriteHeight = 68.f;

            // Flecha
            sf::CircleShape flecha(10.f, 3);
            flecha.setFillColor(sf::Color::Blue);
            flecha.setOrigin(10.f, 10.f);

            
            flecha.setPosition(pos.x + 24.f, pos.y - 12.f);
            flecha.setRotation(0.f);

            m_window.draw(flecha);
        }
    }



    // 3. Dibujar cursor si se está seleccionando objetivo
    if ((m_currentActionMenu == ActionMenuState::Attack ||
        m_currentActionMenu == ActionMenuState::Skill_ConfirmTarget) &&
        !m_objetivosValidos.empty())
    {
        Personaje* target = m_objetivosValidos[m_selectedTargetIndex];
        auto pos = spritePositions[target];
        sf::RectangleShape marco({ 48.f, 48.f });
        marco.setPosition(pos);
        marco.setFillColor(sf::Color::Transparent);
        marco.setOutlineColor((m_currentActionMenu == ActionMenuState::Skill_ConfirmTarget) ? sf::Color::Blue : sf::Color::Red);
        marco.setOutlineThickness(3.f);
        m_window.draw(marco);
    }

    // 4. Menús
    float mx = 30.f, my = windowSize.y - 180.f;
    if (m_currentActionMenu == ActionMenuState::Open) {
        for (size_t i = 0; i < m_actionOptions.size(); ++i) {
            sf::Text txt = m_actionOptions[i];
            txt.setCharacterSize(38);
            txt.setPosition(mx, my + i * 40.f);
            m_window.draw(txt);
        }
    }
    else if (m_currentActionMenu == ActionMenuState::Skill_Select) {
        for (size_t i = 0; i < m_skillOptions.size(); ++i) {
            sf::Text txt = m_skillOptions[i];
            txt.setCharacterSize(28);
            txt.setPosition(mx, my + i * 25.f);
            if ((int)i == m_selectedSkillIndex) txt.setFillColor(sf::Color::Yellow);
            m_window.draw(txt);
        }
        m_skillDescription.setCharacterSize(14);
        m_window.draw(m_skillDescription);
    }

    // 5. HUD compactado para aliados abajo derecha
    auto dibujarHUD = [&](Personaje* p, int idx) {
        float bx = windowSize.x - 280.f;
        float by = windowSize.y - 160.f + idx * 50.f;

        sf::Text name;
        name.setFont(b_font);
        name.setString(p->getNombre());
        name.setCharacterSize(16);
        name.setFillColor(sf::Color::White);
        name.setPosition(bx, by);
        m_window.draw(name);

        float barWidth = 180.f;
        float barHeight = 8.f;

        // Vida
        float vidaRatio = static_cast<float>(p->getVidaActual()) / p->getVidaMax();
        sf::RectangleShape vidaBg(sf::Vector2f(barWidth, barHeight));
        vidaBg.setFillColor(sf::Color(50, 50, 50));
        vidaBg.setPosition(bx, by + 20.f);

        sf::RectangleShape vidaFill(sf::Vector2f(barWidth * vidaRatio, barHeight));
        vidaFill.setFillColor(sf::Color::Green);
        vidaFill.setPosition(bx, by + 20.f);

        // Maná
        float manaRatio = static_cast<float>(p->getMana()) / p->getManaMax();
        sf::RectangleShape manaBg(sf::Vector2f(barWidth, barHeight));
        manaBg.setFillColor(sf::Color(50, 50, 50));
        manaBg.setPosition(bx, by + 34.f);

        sf::RectangleShape manaFill(sf::Vector2f(barWidth * manaRatio, barHeight));
        manaFill.setFillColor(sf::Color::Blue);
        manaFill.setPosition(bx, by + 34.f);

        m_window.draw(vidaBg); m_window.draw(vidaFill);
        m_window.draw(manaBg); m_window.draw(manaFill);
        };

    int idx = 0;
    for (auto p : m_personajes)
        if (p->getVivo() && p->getControl() == "J1")
            dibujarHUD(p, idx++);

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

GameStateSim GameState::crearSimulacion() const {
    GameStateSim sim;

    for (size_t i = 0; i < m_personajes.size(); ++i) {
        const Personaje* p = m_personajes[i];
        if (!p) continue;

        SimPersonaje simP(
            static_cast<int>(i),           
            p->getVidaActual(),           
            p->getVidaMax(),       
            p->getVelocidad(),        
            p->getAtaque(),   
            p->getVivo(),         
            (p->getControl() == "J1" ? 0 : 1)
           
        );

        sim.personajes.push_back(simP);
    }

    for (size_t i = 0; i < sim.personajes.size(); ++i) {
        if (m_personajes[i] == m_personajeEnTurno) {
            sim.indiceTurnoActual = static_cast<int>(i);
            break;
        }
    }

    return sim;
}

int GameState::minimax(GameStateSim estado, int profundidad, bool esMaximizador, int alfa, int beta) {
    if (estado.estaTerminado() || profundidad == 0) {
        return evaluarEstado(estado);
    }

    std::vector<GameStateSim> siguientesEstados = generarAcciones(estado);

    if (esMaximizador) {
        int mejorPuntuacion = -INT_MAX;
        for (const auto& siguienteEstado : siguientesEstados) {
            int puntuacion = minimax(siguienteEstado, profundidad - 1, false, alfa, beta);
            mejorPuntuacion = std::max(mejorPuntuacion, puntuacion);
            alfa = std::max(alfa, mejorPuntuacion);

            if (beta <= alfa) {
                break;
            }
        }
        return mejorPuntuacion;
    }
    else {
        int mejorPuntuacion = INT_MAX;
        for (const auto& siguienteEstado : siguientesEstados) {
            int puntuacion = minimax(siguienteEstado, profundidad - 1, true, alfa, beta);
            mejorPuntuacion = std::min(mejorPuntuacion, puntuacion);
            beta = std::min(beta, mejorPuntuacion);

            if (beta <= alfa) {
                break;
            }
        }
        return mejorPuntuacion;
    }
}

GameStateSim GameState::obtenerMejorAccion(const GameStateSim& estado) {
   
    std::vector<GameStateSim> siguientesEstados = generarAcciones(estado);

    
    int mejorPuntuacion = -INT_MAX;
    GameStateSim mejorEstado;

    
    int alfa = -INT_MAX;
    int beta = INT_MAX;

    
    for (const auto& siguienteEstado : siguientesEstados) {
        
        int puntuacion = minimax(siguienteEstado, 3, false, alfa, beta);

       
        if (puntuacion > mejorPuntuacion) {
            mejorPuntuacion = puntuacion;
            mejorEstado = siguienteEstado;
        }
    }

  
    return mejorEstado;
}

int GameState::evaluarEstado(const GameStateSim& estado) {
    int puntuacion = 0;

    int personajesIAVivos = 0;
    int personajesJugadorVivos = 0;

    
    for (const auto& p : estado.personajes) {
        if (!p.vivo) continue;  

        if (p.control == 1) {
            personajesIAVivos++;
            puntuacion += p.hp;

            int mana = 100;
            puntuacion += mana * 0.5;

            puntuacion += p.velocidad * 2;

            if (p.hp < (p.maxHp * 0.25)) {
                puntuacion -= 20; 
            }

            puntuacion += p.dañoBase * 1.5;  

        }
        // Para el jugador (control == 0)
        else if (p.control == 0) {
            personajesJugadorVivos++;
            puntuacion -= p.hp;
            puntuacion -= p.velocidad * 1.5;
            puntuacion -= p.dañoBase * 2;

            int defensa = 10;  
            puntuacion -= defensa * 1.5;
        }
    }

    
    puntuacion += (personajesIAVivos - personajesJugadorVivos) * 50;

    return puntuacion;
}

void GameState::actualizarEstadoConSimulacion(const GameStateSim& simulado) {
    
    for (size_t i = 0; i < simulado.personajes.size(); ++i) {
        const SimPersonaje& pSimulado = simulado.personajes[i];
        Personaje* pReal = m_personajes[i]; 

        if (!pReal) continue;

        
        pReal->setVidaActual(pSimulado.hp);      
        pReal->setVelocidad(pSimulado.velocidad);
        pReal->setAtaque(pSimulado.dañoBase);

        if (pSimulado.vivo) {
            pReal->setVivo(true);
        }
        else {
            pReal->setVivo(false); 
        }

        pReal->setControl(pSimulado.control == 0 ? "J1" : "J2");
    }

    m_personajeEnTurno = m_personajes[simulado.indiceTurnoActual];
}

