#include "hPersonaje.h"
#include <SFML/Graphics.hpp>
#include "hHabilidad.h"
Personaje::Personaje(std::string nombre,std::string tipo,std::string control, int vidaMax, int ataque, int defensa, int velocidad, int mana,bool sangrando ,bool vivo,bool aturdido ,std::vector<Habilidad*> habilidades,std::pair<int, int> posicion, int manaMax) {
	this->nombre = nombre;
	this->tipo = tipo;
	this->control = control;
	this->vidaMax = vidaMax;
	this->vidaActual = vidaMax;
	this->ataque = ataque;
	this->defensa = defensa;
	this->velocidad = velocidad;
	this->mana = mana;
	this->manaMax = manaMax;
	this->habilidades = habilidades;	
	this->posicion = posicion;
	this->sangrando = sangrando;
	this->vivo = vivo;
	this->aturdido = aturdido;

	
}
Personaje::~Personaje(){}
bool Personaje::atacar(Personaje* objetivo) {

	if (objetivo != nullptr) {
		if (!objetivo->getVivo()) return false;
		std::cout << "ATACANTE: " << this->getNombre() << " ATAQUE: " << this->getAtaque() << std::endl;
		std::cout << "OBJETIVO: " << objetivo->getNombre() << " DEFENSA: " << objetivo->getDefensa() << std::endl;

		if (this->getControl() == objetivo->getControl()) {
			std::cout << "�No puedes atacar a un compa�ero de equipo!" << std::endl;
			return false;
		}

		int da�o = this->ataque - objetivo->getDefensa();
		if (da�o < 0) da�o = 0;

		int nuevaVida = objetivo->getVidaActual() - da�o;
		if (nuevaVida <= 0) {
			objetivo->setVidaActual(0);
			objetivo->setVivo(false);
			std::cout << objetivo->getNombre() << " ha sido derrotado." << std::endl;
		}
		else {
			objetivo->setVidaActual(nuevaVida);
			std::cout << this->getNombre() << " atac� a " << objetivo->getNombre()
				<< " causando " << da�o << " de da�o. Vida restante: " << nuevaVida << std::endl;
		}

		return true;
	}
	return false;
}

// Getters
std::string Personaje::getNombre() {
	return nombre;
}
std::string Personaje::getTipo() {
	return tipo;
}
std::string Personaje::getControl() {
	return control;
}
int Personaje::getVidaMax() {
	return vidaMax;
}
int Personaje::getVidaActual() {
	return vidaActual;
}
int Personaje::getAtaque() {
	return ataque;
}
int Personaje::getDefensa() {
	return defensa;
}
int Personaje::getVelocidad() {
	return velocidad;
}
int Personaje::getMana() {
	return mana;
}
std::vector<Habilidad*> Personaje::getHabilidades() {
	return habilidades;
}
std::pair<int, int> Personaje::getPosicion() {
	return posicion;
}
bool Personaje::getSangrando() {
	return sangrando;
}
bool Personaje::getVivo() {
	return vivo;
}
bool Personaje::getAturdido() {
	return aturdido;
}
int Personaje::getManaMax(){
	return manaMax;
}

// Setters
void Personaje::setNombre(std::string nombre) {
	this->nombre = nombre;
}
void Personaje::setTipo(std::string tipo) {
	this->tipo = tipo;
}
void Personaje::setControl(std::string control) {
	this->control = control;
}
void Personaje::setVidaMax(int vidaMax) {
	this->vidaMax = vidaMax;
}
void Personaje::setVidaActual(int vidaActual) {
	this->vidaActual = vidaActual;
}
void Personaje::setAtaque(int ataque) {
	this->ataque = ataque;
}
void Personaje::setDefensa(int defensa) {
	this->defensa = defensa;
}
void Personaje::setVelocidad(int velocidad) {
	this->velocidad = velocidad;
}
void Personaje::setMana(int mana) {
	this->mana = mana;
}
void Personaje::setHabilidades(std::vector<Habilidad*> habilidades) {
	this->habilidades = habilidades;
}
void Personaje::setPosicion(std::pair<int, int> posicion) {
	this->posicion = posicion;
}
void Personaje::setSangrando(bool sangrando) {
	this->sangrando = sangrando;
}
void Personaje::setVivo(bool vivo) {
	this->vivo = vivo;
}
void Personaje::setAturdido(bool aturdido) {
	this->aturdido = aturdido;
}