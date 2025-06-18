#include "GameStateSim.h"

// Implementaci�n del constructor de SimPersonaje.
SimPersonaje::SimPersonaje(int id_, int hp_, int maxHp_, int vel_, int dmg_, bool vivo_, int ctrl_)
    : id(id_), hp(hp_), maxHp(maxHp_), velocidad(vel_),
    da�oBase(dmg_), vivo(vivo_), control(ctrl_), posicion(0) {
}

// Implementaci�n de la funci�n `estaTerminado()`.
bool GameStateSim::estaTerminado() const {
    bool iaViva = false;
    bool jugadorVivo = false;

    for (const auto& p : personajes) {
        if (!p.vivo) continue;
        if (p.control == 1) iaViva = true;
        else if (p.control == 0) jugadorVivo = true;
    }

    return !(iaViva && jugadorVivo);
}

// Implementaci�n de la funci�n `avanzarTurno()`.
void GameStateSim::avanzarTurno() {
    int total = personajes.size();
    for (int i = 1; i <= total; ++i) {
        int idx = (indiceTurnoActual + i) % total;
        if (personajes[idx].vivo) {
            indiceTurnoActual = idx;
            return;
        }
    }
}
