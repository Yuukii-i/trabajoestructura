#pragma once
#include "hHabilidad.h"
#include "hPersonaje.h"
#include <vector>
#include <queue>
class Buff :
    public Habilidad
{
private:
    int costeMana;

public:
    Buff(std::string nombre, std::string tipobuff,std::string descripcion,int costeMana);
    ~Buff();
    void usarHabilidad(Personaje* atacante,std::vector<Personaje*> personajes,std::queue<Personaje*> orden);
    
};

