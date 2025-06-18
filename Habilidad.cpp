#include "hHabilidad.h"
#include <iostream>

Habilidad::Habilidad(std::string nombre, std::string tipo,std::string descripcion) {
	this->nombre = nombre;
	this->tipo = tipo;
	this->descripcion = descripcion;
}
Habilidad::~Habilidad() {}
void Habilidad::usarHabilidad(int ile){
}


std::string Habilidad::getNombre() {
	return nombre;

}
std::string Habilidad::getTipo() {
	return tipo;

}
std::string Habilidad::getDescripcion() {
	return descripcion;
}