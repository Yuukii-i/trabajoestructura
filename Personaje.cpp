#include "hPersonaje.h"
#include <SFML/Graphics.hpp>
#include "hHabilidad.h"
Personaje::Personaje(std::string nombre,std::string tipo,std::string control, int vidaMax, int ataque, int defensa, int velocidad, int mana,bool sangrando ,bool vivo,bool aturdido ,std::vector<Habilidad*> habilidades,std::pair<int, int> posicion, int manaMax, std::string sprite) {
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
	this->sprite = sprite;
	
}
Personaje::~Personaje(){}
bool Personaje::atacar(Personaje* objetivo) {

	if (objetivo != nullptr) {
		if (!objetivo->getVivo()) return false;
		std::cout << "ATACANTE: " << this->getNombre() << " ATAQUE: " << this->getAtaque() << std::endl;
		std::cout << "OBJETIVO: " << objetivo->getNombre() << " DEFENSA: " << objetivo->getDefensa() << std::endl;

		if (this->getControl() == objetivo->getControl()) {
			std::cout << "¡No puedes atacar a un compañero de equipo!" << std::endl;
			return false;
		}

		int daño = this->ataque - objetivo->getDefensa();
		if (daño < 0) daño = 0;

		int nuevaVida = objetivo->getVidaActual() - daño;
		if (nuevaVida <= 0) {
			objetivo->setVidaActual(0);
			objetivo->setVivo(false);
			std::cout << objetivo->getNombre() << " ha sido derrotado." << std::endl;
		}
		else {
			objetivo->setVidaActual(nuevaVida);
			std::cout << this->getNombre() << " atacó a " << objetivo->getNombre()
				<< " causando " << daño << " de daño. Vida restante: " << nuevaVida << std::endl;
		}

		return true;
	}
	return false;
}

// Getters
std::string Personaje::getNombre() const {
	return nombre;
}
std::string Personaje::getTipo() const {
	return tipo;
}
std::string Personaje::getControl() const {
	return control;
}
int Personaje::getVidaMax() const {
	return vidaMax;
}
int Personaje::getVidaActual() const {
	return vidaActual;
}
int Personaje::getAtaque() const {
	return ataque;
}
int Personaje::getDefensa() const {
	return defensa;
}
int Personaje::getVelocidad() const {
	return velocidad;
}
int Personaje::getMana() const {
	return mana;
}
std::vector<Habilidad*> Personaje::getHabilidades() const {
	return habilidades;
}
std::pair<int, int> Personaje::getPosicion() const {
	return posicion;
}
bool Personaje::getSangrando() const {
	return sangrando;
}
bool Personaje::getVivo() const {
	return vivo;
}
bool Personaje::getAturdido() const {
	return aturdido;
}
int Personaje::getManaMax() const{
	return manaMax;
}
std::string Personaje::getSprite() {
	return sprite;
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