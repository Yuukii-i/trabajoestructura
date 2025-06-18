#include "State.h"
#include "StateManager.h"
#include <SFML/Graphics.hpp>
#include "hPersonaje.h"
#include <vector>
#include <queue>
#include <SFML/Audio.hpp>
#include <list>
#include "Skill.h"
#include "GameStateSim.h"
#include "IAUtils.h"
struct CompararVelocidad {
    bool operator()(Personaje* a, Personaje* b) {
        return a->getVelocidad() < b->getVelocidad();
    }
};
enum class ActionMenuState {
    Open,
    Attack,
    Move,
    Defend,
    Skill_Select,
    Skill_ConfirmTarget
};



class GameState : public State {
public:
    GameState(sf::RenderWindow& window, StateManager& stateManager,
        std::vector<Personaje*> personajes, Personaje* (&tablero)[8][8],
        std::queue<Personaje*> orden_Turno);
    void handleEvent(sf::Event& event) override;
    void update() override;
    void render() override;
    void reproducirCancionDeCombateAleatoria();
    void inicializarOrdenTurnos();
    void dibujarBarras(Personaje* p, sf::RenderWindow& window);
    Personaje* m_personajeEnTurno = nullptr;
    void pasarTurno();
    sf::RectangleShape m_targetCursor;
    std::pair<int, int> m_targetPos;
    void crearNumeroFlotante(int valor, sf::Vector2f posicion);
    void usarSkill(Skill* skill, Personaje* lanzador, std::vector<Personaje*> objetivos);
    std::vector<Personaje*> getObjetivosPorEfecto(const std::string& efecto, Personaje* lanzador);
    void cargarSkillsDelPersonaje(Personaje* p);
    GameStateSim crearSimulacion() const;
    int evaluarEstado(const GameStateSim& estado);
    int minimax(GameStateSim estado, int profundidad, bool esMaximizador, int alfa, int beta);
    GameStateSim obtenerMejorAccion(const GameStateSim& estado);
    void actualizarEstadoConSimulacion(const GameStateSim& simulado);
    void verificarFinDelJuego();
    void handleGameOverInput(sf::Event& event);
    void handleActionMenuInput(sf::Event& event);
    void handleAttackInput(sf::Event& event);
    void handleSkillSelectInput(sf::Event& event);
    void handleSkillConfirmTargetInput(sf::Event& event);
    void actualizarObjetivosValidosBasico();
private:
    bool m_finDelJuego = false;
    std::string m_ganador;
    sf::Vector2f tableroAPantalla(std::pair<int, int> posTablero);
    sf::RenderWindow& m_window;
    StateManager& m_stateManager;
    std::vector<Personaje*> m_personajes;
    Personaje* (&m_tablero)[8][8];
    std::queue<Personaje*> m_ordenTurno;
    sf::RectangleShape m_grid[8][8];
    sf::Music music;
    std::vector<std::string> m_combatSongs;
    sf::Font b_font;
    std::vector<sf::Text> m_actionOptions;
    int m_selectedOptionIndex = 0;
    int m_selectedSkillIndex;
    std::vector<sf::Text> m_skillOptions;
    sf::Text m_skillDescription;
    Skill* m_habilidadSeleccionada;
    ActionMenuState m_currentActionMenu;
    std::vector<Personaje*> m_objetivosValidos;
    int m_selectedTargetIndex = 0;
    std::string formatearDescripcion(const std::string& texto, unsigned int maxWidth, const sf::Font& font, unsigned int characterSize);
};
