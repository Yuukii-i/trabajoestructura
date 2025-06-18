//(Jaime Araya/21652246-k),(Rodrigo Cortes/21594212-0, Ingenieria Civil en Computacion e Informatica
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <sstream>
#include <iostream>
#include "hPersonaje.h"
#include "hHabilidad.h"
#include "hBuff.h"
#include <queue>
#include "StateManager.h"
#include "MenuState.h"
#include <SFML/Audio.hpp>
#include "Skill.h"
std::queue<Personaje*> calcularOrden(std::vector<Personaje*> personajes) {
	std::queue<Personaje*> ordenTurno;


	// Ordenar personajes por velocidad
	
	std::sort(personajes.begin(), personajes.end(), [](Personaje* a, Personaje* b) {
		return a->getVelocidad() > b->getVelocidad();
		});
	// Agregar personajes al orden de turno
	for (Personaje* personaje : personajes) {
		ordenTurno.push(personaje);
	}
	return ordenTurno;
}
void ubicarPersonaje(std::vector<Personaje*> personajes,Personaje* tablero[8][8]) {
	for (Personaje* person : personajes) {
		if (person != nullptr) {
			std::pair<int, int> posicion = person->getPosicion();
			tablero[posicion.second][posicion.first] = person;
		}
	}
}
std::vector<Personaje*> crearPersonajes() {
	std::vector<Personaje*> personajes;

	std::vector<Habilidad*> habilidadesP1;

	habilidadesP1.push_back(new Skill("Arranque de esquizofrenia", "Esquizofrenia", "Desenfrenado ataque de esquizofrenia que afecta a todos los enemigos, con poca probabilidad", 5, "Total", 1.5, 4));

	habilidadesP1.push_back(new Skill("Amigo imaginario", "Esquizofrenia", "Llama a un compañero imaginaro que tiene un 10% de probabilidad de  golpearte a ti ", 10, "Single", 1.2, 8));

	habilidadesP1.push_back(new Buff("Toma tus pastillas", "Curación", "El usuario finalmente entra en razón y toma sus medicinas para la ezquisofrenia recuperando 20 de vida y disminuye su velocidad un 5%", 15));

	habilidadesP1.push_back(new Buff("Paranoia", "Debuff", "Las voces en tu cabeza han hablado y te dicen el punto debil del enemigo quitandole 30% de defenza", 30));

	Personaje* p1 = new Personaje("Jhon el esquizofrenico", "Esquizofrenia", "J1", 150, 20, 5, 1, 50, false, true, true, habilidadesP1, std::make_pair(0, 1), 50, "assets/images/jhon.png");
	personajes.push_back(p1);

	std::vector<Habilidad*> habilidadesP2;

	habilidadesP2.push_back(new Skill("Lanza cuchilla", "Depresivo", "Lanza cuchillas al enemigo con un 20% de prob de dejar sangrado al enemigo ", 5, "Single", 1.3, 9));

	habilidadesP2.push_back(new Buff("Corte de venas", "Curación de equipo", "cura a los aliados 20 puntos de vida a cambio del 10% total de la tuya", 30));

	habilidadesP2.push_back(new Buff("Soga", "Skipeo", "Cuelga al siguiente enemigo impidiendole atacar por 1 turno", 25));

	habilidadesP2.push_back(new Buff("Sacrificio", "Curación de equipo", " revive a un aliado a cambio de tu vida", 30));

	Personaje* p2 = new Personaje("Hollow Knight", "Depresivo", "J1", 100, 20, 0, 20, 120, false, true, true, habilidadesP2, std::make_pair(0, 3), 120, "assets/images/Hollow.png");
	personajes.push_back(p2);

	std::vector<Habilidad*> habilidadesP3;

	habilidadesP3.push_back(new Buff("Trauma de guerra", "Mejora de daño", "Recuerda su pasado lo cual le da fuerza pero le quita años de vida", 15));

	habilidadesP3.push_back(new Skill("Ametralla", "Trauma", "Dispara una ametralladora que golpea a todos los enemigos", 15, "Multi", 1.0, 5));

	habilidadesP3.push_back(new Skill("Granada de fragmentacion", "Trauma", "Lanza una granada que golpea a todos los enemigos con daño grave", 35, "Multi", 2.4, 9));

	habilidadesP3.push_back(new Buff("Bomba de humo", "Debuff", "Crea una nube de humo que disminuye la velocidad del enemigo en un 10%", 20));

	Personaje* p3 = new Personaje("Umbra Bellator", "Trauma", "J1", 120, 40, 100, 3, 45, false, true, true, habilidadesP3, std::make_pair(0, 5), 45, "assets/images/Soldado.png");
	personajes.push_back(p3);

	std::vector<Habilidad*> habilidadesP4;

	habilidadesP4.push_back(new Buff("¿Que estaba haciendo?", "Curación", "olvida que fuiste golpeado y recupera 30 pts de vida", 30));

	habilidadesP4.push_back(new Buff("Quien eras tu?", "Debuff enemigo", "hace olvidar al enemigo quien eras tu y baja un 20% de defensa", 20));

	habilidadesP4.push_back(new Skill("QUIEN ERES??!!!", "Olvidado", "El usuario no sabe quien esta enfrente y ataca desenfrenadamente a todos haciendo poco daño", 7, "Total", 0.8, 5));

	habilidadesP4.push_back(new Skill("Dios donde estoy", "Olvidado", "El usuario no recuerda donde esta y ataca a todos los enemigos a su alrededor con mucho daño", 20, "Multi", 1.6, 9));

	Personaje* p4 = new Personaje("K", "Olvidado", "J1", 100, 16, 15, 2, 50, false, true, true, habilidadesP4, std::make_pair(0, 7), 50, "assets/images/K.png");
	personajes.push_back(p4);

	std::vector<Habilidad*> habilidadesP5;

	habilidadesP5.push_back(new Skill("Demonios internos", "Esquizofrenia", "Ataca con demonios internos", 10, "Single", 1.3, 8));

	habilidadesP5.push_back(new Skill("Baja lealtad", "Esquizofrenia", "Ataca a un enemigo con un 20% de probabilidad de dejarlo aturdido", 15, "Single", 1.6, 7));

	habilidadesP5.push_back(new Buff("Desconfianza", "Debuff", "El enemigo no confia en ti y baja su defensa un 20%", 20));

	habilidadesP5.push_back(new Buff("Desesperacion", "Debuff", "El enemigo se siente desesperado y baja su velocidad un 20%", 20));


	Personaje* p5 = new Personaje("Sans", "Esquizofrenia", "J2", 90, 19, 5, 9, 50, false, true, true, habilidadesP5, std::make_pair(7, 1), 50, "assets/images/sans.png");
	personajes.push_back(p5);

	std::vector<Habilidad*> habilidadesP6;

	habilidadesP6.push_back(new Skill("Corte de venas", "Depresivo", "Corta las venas del enemigo y le hace mucho daño", 10, "Single", 1.8, 9));

	habilidadesP6.push_back(new Skill("Ataque de sangre", "Depresivo", "Ataca al enemigo con sangre y le hace daño medio", 5, "Single", 1.2, 9));

	habilidadesP6.push_back(new Buff("Desesperacion", "Curación", "Sacrifica 20 de vida y cura a tus compañeros un 20%", 20));

	habilidadesP6.push_back(new Buff("Miedo", "Debuff", "Le metes miedo al enemigo quitandole un 25% de defensa", 20));

	Personaje* p6 = new Personaje("Cuadro", "Depresivo", "J2", 200, 13, 0, 7, 120, false, true, true, habilidadesP6, std::make_pair(7, 3), 120, "assets/images/joker.png");
	personajes.push_back(p6);

	std::vector<Habilidad*> habilidadesP7;

	habilidadesP7.push_back(new Skill("Disparo de escopeta", "Trauma", "Dispara una escopeta a quemarropa y hace daño grave", 30, "Single", 2.0, 8));

	habilidadesP7.push_back(new Skill("Granada de fragmentacion", "Trauma", "Lanza una granada que explota a todos los enemigos haciendo daño grave", 35, "Multi", 2.5, 9));

	habilidadesP7.push_back(new Buff("Bomba de humo", "Debuff", "Crea una nube de humo que disminuye la velocidad del enemigo en un 10%", 20));

	habilidadesP7.push_back(new Buff("Trauma de guerra", "Mejora de daño", "Recuerda su pasado lo cual le da fuerza pero le quita años de vida", 15));

	Personaje* p7 = new Personaje("Stanli", "Trauma", "J2", 120, 16, 10, 3, 45, false, true, true, habilidadesP7, std::make_pair(7, 5), 45, "assets/images/stanli.png");
	personajes.push_back(p7);

	std::vector<Habilidad*> habilidadesP8;

	habilidadesP8.push_back(new Buff("¿Que estaba haciendo?", "Curación", "olvida que fuiste golpeado y recupera 30 pts de vida", 30));

	habilidadesP8.push_back(new Buff("Quien eras tu?", "Debuff enemigo", "hace olvidar al enemigo quien eras tu y baja un 20% de defensa", 20));

	habilidadesP8.push_back(new Buff("Lo recuerdas?", "Olvidado", "El usuario recuerda quien es y recupera 30% de velocidad", 20));

	habilidadesP8.push_back(new Skill("QUIEN ERES??!!!", "Olvidado", "El usuario no sabe quien esta enfrente y ataca desenfrenadamente a todos los enemigos", 10, "Multi", 0.8, 7));

	Personaje* p8 = new Personaje("J", "Olvidado", "J2", 140, 15, 15, 2, 50, false, true, true, habilidadesP8, std::make_pair(7, 7), 50, "assets/images/J.png");
	personajes.push_back(p8);

	return personajes;
}
int main() {
	std::vector<Personaje*> personajes = crearPersonajes();
	Personaje* tablero[8][8];
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			tablero[i][j] = nullptr;
		}
	}

	ubicarPersonaje(personajes, tablero);
	std::queue<Personaje*> orden_Turno = calcularOrden(personajes);

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Take Your Meds");

	StateManager stateManager;
	stateManager.setState(std::make_unique<MenuState>(window, stateManager, personajes, tablero, orden_Turno));
	window.setFramerateLimit(60);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			stateManager.getState()->handleEvent(event);
		}

		MenuState* menu = dynamic_cast<MenuState*>(stateManager.getState());
		if (menu && menu->debeSalir()) {
			window.close();
			break;
		}

		stateManager.getState()->update();
		stateManager.getState()->render();
	}

	return 0;
}

