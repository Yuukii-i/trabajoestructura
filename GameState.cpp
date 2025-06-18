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
    float cellSize = 50.f;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            m_grid[i][j].setSize(sf::Vector2f(cellSize, cellSize));
            m_grid[i][j].setPosition(j * cellSize, i * cellSize);
            m_grid[i][j].setFillColor(sf::Color(255, 255, 255, 40));
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

    // Verificar si es el turno de la IA
    if (m_personajeEnTurno != nullptr && m_personajeEnTurno->getControl() == "J2") {

        GameStateSim estadoSimulado = crearSimulacion();

        // Asegúrate de que la función recibe GameStateSim
        GameStateSim mejorEstado = obtenerMejorAccion(estadoSimulado);
        
        // Actualizar el estado real del juego con el mejor estado calculado por la IA
        actualizarEstadoConSimulacion(mejorEstado);

        // Pasar al siguiente turno
        pasarTurno();
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
    sf::Texture texturaFondo;
    if (!texturaFondo.loadFromFile("assets/images/fondo.jpg")) {
        std::cerr << "No se pudo cargar el fondo\n";
    }
    sf::Sprite fondo;
    fondo.setTexture(texturaFondo);

    // Suponiendo que la ventana mide 800x600
    sf::Vector2u tamañoVentana = m_window.getSize();
    sf::Vector2u tamañoImagen = texturaFondo.getSize();

    float escalaX = static_cast<float>(tamañoVentana.x) / tamañoImagen.x;
    float escalaY = static_cast<float>(tamañoVentana.y) / tamañoImagen.y;

    fondo.setScale(escalaX, escalaY);
    m_window.draw(fondo);
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
        sf::Texture personajeTexture;
        if (!personajeTexture.loadFromFile(p->getSprite())) {
            std::cerr << "Error al cargar sprite\n";
        }

        sf::Sprite sprite(personajeTexture);

        sf::Vector2u spriteSize = personajeTexture.getSize();



        float escalaX = 50.f / spriteSize.x;
        float escalaY = 50.f / spriteSize.y;

        sprite.setScale(escalaX, escalaY);

        sprite.setPosition(p->getPosicion().first * 50.f, p->getPosicion().second * 50.f);

        m_window.draw(sprite);
        sf::Vector2f posicion = sprite.getPosition();
        float x = posicion.x;
        float y = posicion.y;

        // Flechita de turno
        if (p == m_personajeEnTurno) {
            sf::CircleShape flecha(5.f, 3);
            flecha.setFillColor(sf::Color::Black);
            flecha.setRotation(180.f);
            flecha.setPosition(x + 15.f, y - 10.f);
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
            nombreText.setPosition(x + 80.f, y + 80.f);

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

GameStateSim GameState::crearSimulacion() const {
    GameStateSim sim;

    for (size_t i = 0; i < m_personajes.size(); ++i) {
        const Personaje* p = m_personajes[i];
        if (!p) continue;

        SimPersonaje simP(
            static_cast<int>(i),              // id
            p->getVidaActual(),                       // hp actual
            p->getVidaMax(),                    // hp máximo
            p->getVelocidad(),                // velocidad
            p->getAtaque(),                 // daño básico
            p->getVivo(),                     // ¿sigue vivo?
            (p->getControl() == "J1" ? 0 : 1)
            // 0 = jugador, 1 = IA
        );

        sim.personajes.push_back(simP);
    }

    // Determinar quién tiene el turno actualmente
    // Suponiendo que tu turno actual está en `m_personajeEnTurno`
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
        return evaluarEstado(estado);  // Evaluamos el estado al llegar a una hoja o al límite de profundidad
    }

    std::vector<GameStateSim> siguientesEstados = generarAcciones(estado);

    if (esMaximizador) {
        int mejorPuntuacion = -INT_MAX;
        for (const auto& siguienteEstado : siguientesEstados) {
            int puntuacion = minimax(siguienteEstado, profundidad - 1, false, alfa, beta); // Es turno del jugador
            mejorPuntuacion = std::max(mejorPuntuacion, puntuacion);
            alfa = std::max(alfa, mejorPuntuacion);  // Actualizamos alfa

            if (beta <= alfa) {
                break;  // Poda: si el valor de beta es menor o igual a alfa, cortamos la rama
            }
        }
        return mejorPuntuacion;
    }
    else {
        int mejorPuntuacion = INT_MAX;
        for (const auto& siguienteEstado : siguientesEstados) {
            int puntuacion = minimax(siguienteEstado, profundidad - 1, true, alfa, beta); // Es turno de la IA
            mejorPuntuacion = std::min(mejorPuntuacion, puntuacion);
            beta = std::min(beta, mejorPuntuacion);  // Actualizamos beta

            if (beta <= alfa) {
                break;  // Poda: si el valor de beta es menor o igual a alfa, cortamos la rama
            }
        }
        return mejorPuntuacion;
    }
}

GameStateSim GameState::obtenerMejorAccion(const GameStateSim& estado) {
    // Generamos todos los posibles estados de acción del estado actual
    std::vector<GameStateSim> siguientesEstados = generarAcciones(estado);

    // Inicializamos la puntuación más baja posible
    int mejorPuntuacion = -INT_MAX;
    GameStateSim mejorEstado;

    // Inicializamos los valores de alfa y beta para la poda alpha-beta
    int alfa = -INT_MAX;
    int beta = INT_MAX;

    // Evaluamos todos los posibles siguientes estados
    for (const auto& siguienteEstado : siguientesEstados) {
        // Llamamos al algoritmo minimax con poda para obtener la puntuación del siguiente estado
        int puntuacion = minimax(siguienteEstado, 3, false, alfa, beta); // Es el turno de la IA (minimizar)

        // Si la puntuación es mejor que la actual, actualizamos el mejor estado
        if (puntuacion > mejorPuntuacion) {
            mejorPuntuacion = puntuacion;
            mejorEstado = siguienteEstado;
        }
    }

    // Devolvemos el mejor estado encontrado
    return mejorEstado;
}

int GameState::evaluarEstado(const GameStateSim& estado) {
    int puntuacion = 0;

    // Variables para sumar/penalizar según el número de personajes vivos
    int personajesIAVivos = 0;
    int personajesJugadorVivos = 0;

    // Recorremos todos los personajes del estado
    for (const auto& p : estado.personajes) {
        if (!p.vivo) continue;  // Si el personaje está muerto, lo ignoramos

        // Para la IA (control == 1)
        if (p.control == 1) {
            personajesIAVivos++;
            puntuacion += p.hp;  // Sumar vida de los personajes vivos de la IA

            // Premiar por mana disponible (si el personaje tiene mana, considera usarlo)
            // Supongo que tienes algún atributo para el mana de los personajes
            int mana = 100;  // Reemplaza esto por el atributo real de mana del personaje
            puntuacion += mana * 0.5; // Añadir valor proporcional al mana

            // Sumar puntos por la velocidad de los personajes
            puntuacion += p.velocidad * 2;  // Cuanto más rápido, mejor

            // Penalizar personajes de la IA con poco HP
            if (p.hp < (p.maxHp * 0.25)) {
                puntuacion -= 20;  // Penaliza si su vida es muy baja
            }

            // Agregar valor si el personaje tiene un alto daño
            puntuacion += p.dañoBase * 1.5;  // Ajusta el valor según sea necesario

        }
        // Para el jugador (control == 0)
        else if (p.control == 0) {
            personajesJugadorVivos++;

            // Penalizar por vida del jugador (la IA quiere que el jugador tenga poca vida)
            puntuacion -= p.hp;

            // Penalizar personajes del jugador con alta velocidad
            puntuacion -= p.velocidad * 1.5;  // Si el jugador es rápido, es una desventaja para la IA

            // Penalizar personajes enemigos con mucho daño (son más peligrosos para la IA)
            puntuacion -= p.dañoBase * 2;  // Ajusta según lo que consideres adecuado

            // Penalizar personajes enemigos con alta defensa (si tienes defensa, añádela aquí)
            int defensa = 10;  // Reemplaza esto por el atributo real de defensa
            puntuacion -= defensa * 1.5;  // Penaliza a los enemigos con alta defensa
        }
    }

    // Factor final que premia tener más personajes vivos
    puntuacion += (personajesIAVivos - personajesJugadorVivos) * 50;  // Ajusta el valor de la diferencia de personajes vivos

    return puntuacion;
}

void GameState::actualizarEstadoConSimulacion(const GameStateSim& simulado) {
    // Recorremos los personajes en el estado simulado
    for (size_t i = 0; i < simulado.personajes.size(); ++i) {
        const SimPersonaje& pSimulado = simulado.personajes[i];
        Personaje* pReal = m_personajes[i];  // El personaje real en el juego

        if (!pReal) continue;

        // Actualizar atributos según el estado simulado
        pReal->setVidaActual(pSimulado.hp);      // Actualizamos la vida
        pReal->setVelocidad(pSimulado.velocidad); // Actualizamos la velocidad
        pReal->setAtaque(pSimulado.dañoBase);  // Actualizamos el daño básico

        // Actualizar si el personaje está vivo o muerto
        if (pSimulado.vivo) {
            pReal->setVivo(true);  // Método ficticio para revivir si estaba muerto
        }
        else {
            pReal->setVivo(false);  // Método ficticio para matar al personaje si estaba vivo
        }

        // Asegurarse de que el control (jugador/IA) sigue siendo correcto
        pReal->setControl(pSimulado.control == 0 ? "J1" : "J2");
    }

    // Actualizamos el turno real
    m_personajeEnTurno = m_personajes[simulado.indiceTurnoActual];
}

