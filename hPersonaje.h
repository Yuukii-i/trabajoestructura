#pragma once
#include <iostream>
#include <vector>
#include "hHabilidad.h"
class Personaje
{
private:
	int vidaMax,vidaActual, ataque, defensa, velocidad, mana, manaMax;
	std::string nombre,tipo,control,sprite;
	std::vector<Habilidad*> habilidades;
	std::pair<int, int> posicion;// x,y
	bool sangrando,vivo, aturdido;
public:
	Personaje(std::string nombre,std::string tipo,std::string control,int vidaMax, int ataque, int defensa, int velocidad, int mana,bool sangrando,bool vivo,bool aturdido, std::vector<Habilidad*> habilidades, std::pair<int, int> posicion, int manaMax, std::string sprite);
	~Personaje();
	void moverse();
	bool atacar(Personaje* objetivo);

	
	// Getters
	std::string getNombre() const;
	std::string getTipo() const;
	std::string getControl() const;
	int getVidaMax() const;
	int getVidaActual() const;
	int getAtaque() const;
	int getDefensa() const;
	int getVelocidad() const;
	int getMana() const;
	std::vector<Habilidad*> getHabilidades() const;
	std::pair<int, int> getPosicion() const;
	bool getSangrando() const;
	bool getVivo() const;
	bool getAturdido() const;
	int getManaMax() const;
	std::string getSprite();
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

