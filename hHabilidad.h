#pragma once
#include <iostream>
class Habilidad
{
private:
	std::string nombre;
	std::string tipo,descripcion;
public:
	Habilidad(std::string nombre, std::string tipo, std::string descripcion);
	~Habilidad();
	virtual void usarHabilidad(int ile);
	std::string getNombre();
	std::string getTipo();
	std::string getDescripcion();

	

};

