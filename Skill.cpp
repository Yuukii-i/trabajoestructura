#include "Skill.h"
#include <iostream>

Skill::Skill(std::string nombre, std::string tipo, std::string descripcion,int coste, std::string efecto, float potencia, int acierto):Habilidad(nombre,tipo,descripcion) {
	this->nombre = nombre;
	this->tipo = tipo;
	this->descripcion = descripcion;
	this->coste = coste;
	this->efecto = efecto;
	this->potencia = potencia;
	this->acierto = acierto;

}
Skill::~Skill() {}
void Skill::usarSkill(int ile) {
}
float Skill::getPotencia() {
	return potencia;
}
int Skill::getAcierto() {
	return acierto;
}
std::string Skill::getEfecto() {
	return efecto;
}
int Skill::getCoste() {
	return coste;
}
std::string Skill::getNombre() {
	return nombre;

}
std::string Skill::getTipo() {
	return tipo;

}
std::string Skill::getDescripcion() {
	return descripcion;
}