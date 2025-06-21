#ifndef GAME_STATE_SIM_H
#define GAME_STATE_SIM_H

#include <vector>

// Estructura que representa a un personaje simulado.
struct SimPersonaje {
    int id;
    int hp;
    int maxHp;
    int velocidad;
    int dañoBase;
    bool vivo;
    int control;
    int posicion; 

    SimPersonaje(int id_, int hp_, int maxHp_, int vel_, int dmg_, bool vivo_, int ctrl_);
};

// Estructura que representa el estado simulado del juego.
struct GameStateSim {
    std::vector<SimPersonaje> personajes;
    int indiceTurnoActual = 0;

    bool estaTerminado() const; 
    void avanzarTurno();         
};

#endif
