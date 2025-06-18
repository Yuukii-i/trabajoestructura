#pragma once
#include <iostream>
#include "hPersonaje.h"
#include <vector>
#include <random>
class Personaje;
class Skill: public Habilidad
{
private:
	std::string nombre,tipo,descripcion,efecto;
	int coste, acierto;
	float potencia;

public:
	Skill(std::string nombre, std::string tipo, std::string descripcion, int coste, std::string efecto, float potencia, int acierto);
	~Skill();
	bool acertar(int probabilidad);
	void usarSkill(int ile);
	float getPotencia();
	int getAcierto();
	std::string getEfecto();
	std::string getNombre();
	std::string getTipo();
	std::string getDescripcion();
	int getCoste();



};

