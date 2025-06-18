#pragma once
#include <iostream>
#include <vector>
#include "hHabilidad.h"
class Personaje
{
private:
	int vidaMax,vidaActual, ataque, defensa, velocidad, mana, manaMax;
	std::string nombre,tipo,control;
	std::vector<Habilidad*> habilidades;
	std::pair<int, int> posicion;// x,y
	bool sangrando,vivo, aturdido;
public:
	Personaje(std::string nombre,std::string tipo,std::string control,int vidaMax, int ataque, int defensa, int velocidad, int mana,bool sangrando,bool vivo,bool aturdido, std::vector<Habilidad*> habilidades, std::pair<int, int> posicion, int manaMax);
	~Personaje();
	void moverse();
	bool atacar(Personaje* objetivo);

	
	// Getters
	std::string getNombre();
	std::string getTipo();
	std::string getControl();
	int getVidaMax();
	int getVidaActual();
	int getAtaque();
	int getDefensa();
	int getVelocidad();
	int getMana();
	std::vector<Habilidad*> getHabilidades();
	std::pair<int, int> getPosicion();
	bool getSangrando();
	bool getVivo();
	bool getAturdido();
	int getManaMax();

	// Setters
	void setNombre(std::string nombre);
	void setTipo(std::string tipo);
	void setControl(std::string control);
	void setVidaMax(int vidaMax);
	void setVidaActual(int vidaActual);
	void setAtaque(int ataque);
	void setDefensa(int defensa);
	void setVelocidad(int velocidad);
	void setMana(int mana);
	void setHabilidades(std::vector<Habilidad*> habilidades);
	void setPosicion(std::pair<int, int> posicion);
	void setSangrando(bool sangrando);
	void setVivo(bool vivo);
	void setAturdido(bool aturdido);
	


		
	
};

