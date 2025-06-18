#include "hBuff.h"

Buff::Buff(std::string nombre, std::string tipobuff, std::string descripcion,int costeMana):Habilidad(nombre,tipobuff,descripcion) {
	this->costeMana = costeMana;
	

}
Buff::~Buff(){}
void Buff::usarHabilidad(Personaje* atacante,std::vector<Personaje*> personajes,std::queue<Personaje*> turnos){
	if (atacante->getMana() >= costeMana) {
		atacante->setMana(atacante->getMana() - costeMana);
		if (this->Habilidad::getNombre() == "Desconfianza") {
			for (Personaje* personaje : personajes) {
				if (personaje != nullptr && personaje->getControl() != atacante->getControl()) {
					personaje->setDefensa(personaje->getDefensa() * 0.8);
					std::cout << "El enemigo no confia en ti y baja su defensa un 20%" << std::endl;
				}
			}
		}
		else if (this->Habilidad::getNombre() == "Desesperacion") {
			for (Personaje* personaje : personajes) {
				if (personaje != nullptr && personaje->getControl() != atacante->getControl()) {
					personaje->setVelocidad(personaje->getVelocidad() * 0.8);
					std::cout << "El enemigo se siente desesperado y baja su velocidad un 20%" << std::endl;
				}
			}
		}
		else if (this->Habilidad::getNombre() == "¿Que estaba haciendo?") {
			if (atacante->getVidaActual() == atacante->getVidaMax()) {
				std::cout << "No puedes recuperar vida" << std::endl;
			}
			else if (atacante->getVidaActual() + 30 > atacante->getVidaMax()) {
				atacante->setVidaActual(atacante->getVidaMax());
			}
			else {
				atacante->setVidaActual(atacante->getVidaActual() + 30);
				std::cout << "Recuperas 30 pts de vida" << std::endl;
			}
		}
		else if (this->Habilidad::getNombre() == "Quien eras tu?") {
			for (Personaje* personaje : personajes) {
				if (personaje != nullptr && personaje->getControl() != atacante->getControl()) {
					personaje->setDefensa(personaje->getDefensa() * 0.8);
					std::cout << "El enemigo no confia en ti y baja su defensa un 20%" << std::endl;
				}
			}
		}
		else if (this->Habilidad::getNombre() == "Lo recuerdas?") {
			atacante->setVelocidad(atacante->getVelocidad() + 30);
			std::cout << "Recuperas 30 pts de velocidad" << std::endl;

		}
		else if (this->Habilidad::getNombre() == "Toma tus pastillas") {
			if (atacante->getVidaMax() > atacante->getVidaActual() + 20) {
				atacante->setVidaActual(atacante->getVidaActual() + 20);
				atacante->setVelocidad(atacante->getVelocidad() * 0.95);
				std::cout << "Recuperas 20 pts de vida y pierdes 5% de velocidad" << std::endl;
			}
			else if (atacante->getVidaMax() < atacante->getVidaActual() + 20) {
				atacante->setVidaActual(atacante->getVidaMax());
				atacante->setVelocidad(atacante->getVelocidad() * 0.95);
				std::cout << "Recuperas 20 pts de vida y pierdes 5% de velocidad" << std::endl;
			}
			else {
				std::cout << "No puedes recuperar vida" << std::endl;
			}
		}
		else if (this->Habilidad::getNombre() == "Paranoia") {
			for (Personaje* personaje : personajes) {
				if (personaje != nullptr && personaje->getControl() != atacante->getControl()) {
					personaje->setDefensa(personaje->getDefensa() * 0.7);
					std::cout << "descubres el punto debil del enemigo -30% de defensa" << std::endl;
				}
			}
		}
		else if (this->Habilidad::getNombre() == "Corte de venas") {
			atacante->setVidaActual(atacante->getVidaActual() - atacante->getVidaMax() * 0.10);
			for (Personaje* personaje : personajes) {
				if (personaje != nullptr && personaje->getControl() != atacante->getControl()) {
					if (personaje->getVidaActual() + 20 > personaje->getVidaMax()) {
						personaje->setVidaActual(personaje->getVidaMax());
					}
					else if (personaje->getVidaActual() + 20 < personaje->getVidaMax()) {
						personaje->setVidaActual(personaje->getVidaActual() + 20);
					}
					else {
						std::cout << "Este personaje no puede recuperar vida" << std::endl;
					}
				}
			}
		}
		else if (this->Habilidad::getNombre() == "Soga") {
			for (Personaje* personaje : personajes) {
				if (personaje != nullptr && personaje->getControl() != atacante->getControl()) {
					personaje->setAturdido(true);
					std::cout << "El enemigo a quedado colgado" << std::endl;
					break;
				}
			}

		}
		else if (this->Habilidad::getNombre() == "Sacrificio") {
			for (Personaje* personaje : personajes) {
				if (personaje != nullptr && personaje->getControl() == atacante->getControl()) {
					personaje->setVivo(true);
					personaje->setVidaActual(personaje->getVidaMax());

					atacante->setVidaActual(0);
					atacante->setVivo(false);

					break;
				}
			}

		}
		else if (this->Habilidad::getNombre() == "Trauma de guerra") {
			if (atacante->getVidaActual() - 10 > 0) {
				atacante->setAtaque(atacante->getAtaque() + 10);
				atacante->setVidaActual(atacante->getVidaActual() - 10);
			}
			else {
				std::cout << "No puedes sacrificar vida" << std::endl;
			}
		}
		else if (this->Habilidad::getNombre() == "Bomba de humo") {
			for (Personaje* personaje : personajes) {
				if (personaje != nullptr && personaje->getControl() != atacante->getControl()) {
					personaje->setVelocidad(personaje->getVelocidad() * 0.9);
					std::cout << "El enemigo se siente mareado y baja su velocidad un 10%" << std::endl;
				}
			}
		}
		else if (this->Habilidad::getNombre() == "Miedo") {
			for (Personaje* personaje : personajes) {
				if (personaje != nullptr && personaje->getControl() != atacante->getControl()) {
					personaje->setDefensa(personaje->getDefensa() * 0.8);
					std::cout << "Le metes miedo al enemigo quitandole un 20% de defensa" << std::endl;
				}
			}
		}

		
	
	}
	else {
		std::cout << "No tienes suficiente mana" << std::endl;
	}

}
