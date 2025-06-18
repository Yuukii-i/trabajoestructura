#ifndef IA_UTILS_H
#define IA_UTILS_H

#include "GameStateSim.h"
#include <vector>

// Devuelve una lista de posibles estados resultantes de las acciones del personaje actual
std::vector<GameStateSim> generarAcciones(const GameStateSim& estado);
int evaluarEstado(const GameStateSim& estado);
#endif // IA_UTILS_H
