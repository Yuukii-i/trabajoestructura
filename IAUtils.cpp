#include "IAUtils.h"

std::vector<GameStateSim> generarAcciones(const GameStateSim& estadoActual) {
    std::vector<GameStateSim> posiblesEstados;

    const SimPersonaje& atacante = estadoActual.personajes[estadoActual.indiceTurnoActual];

    if (!atacante.vivo) {
        GameStateSim siguiente = estadoActual;
        siguiente.avanzarTurno();
        posiblesEstados.push_back(siguiente);
        return posiblesEstados;
    }

    for (size_t i = 0; i < estadoActual.personajes.size(); ++i) {
        const SimPersonaje& objetivo = estadoActual.personajes[i];

        if (objetivo.vivo && objetivo.control != atacante.control) {
            GameStateSim nuevoEstado = estadoActual;

            SimPersonaje& objetivoEnEstado = nuevoEstado.personajes[i];
            objetivoEnEstado.hp -= atacante.dañoBase;
            if (objetivoEnEstado.hp <= 0) {
                objetivoEnEstado.hp = 0;
                objetivoEnEstado.vivo = false;
            }

            nuevoEstado.avanzarTurno();

            posiblesEstados.push_back(nuevoEstado);
        }
    }

    return posiblesEstados;
}
