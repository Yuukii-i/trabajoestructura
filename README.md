Rodrigo Cortés, 21.594.212-0
Jaime Araya, 21.652.246-K

(ACLARACION, en este repositorio puede que aparescan 2 ramas, la default y la main, la mas actual es la main ya que la otra fue antes de que supieramos que se podia hacer un main y ahi se ven reflejados cambios anteriores)
#Take your meds
Bienvenido a Take your meds, un RPG por turnos Jugador vs Jugador (por ahora) donde podras luchar contra tus demonios internos con una musica de fondo emocionante

##Requisitos
Visual Studio 2022 (recomendable) (Puede que al importar el repositorio puede que  pida descargar algunas dependencias de Visual Studio las cuales no usamos en su mayoria, pero no sabiamos como quitarlas)

##Paso a paso para compilar

Para compilar correctamente este juego (recomendamos usar Visual Studio), probablemente tirara error de que no encuentra la libreria de SFML/grafics...etc, tambien recomendamos exportarlo a Visual Studio desde Github Desktop

1-para solucionarlo hay que ir a propiedades en el proyecto en Propiedades de configuración---> C/C++ ---> General y dentro donde dice directorios de inclucion edicionales remplazar lo que haya con: $(ProjectDir)external\SFML\include y aplicar.

2-Luego tambien en Propiedades de configuracion ---> Vinculador ---> General y dentro donde dice directorios de biblioteca adicionales remplazar lo que haya con:
$(ProjectDir)external\SFML\lib y aplicar

3-Y por ultimo ir a las propiedades del trabajoestructura.cpp y hacer lo mismo que en el paso 1
con eso deberia estar listo y compilable

##Como Jugar
-Al abrir el juego se mostraran 3 opciones, Jugar, configuracion(No funciona por ahora) y salir.
-Usa las flechas del teclado para moverte entre opcion y pulsa enter para seleccionar una opcion.
-Al seleccionar la opcion de "Jugar" se mostrara el tablero con todos los personajes(Verdes para el J1 y rojos para el J2), con el personaje en turno resaltado con un una flecha en su barra de vida y su nombre escrito dentro del circulo. Como tambien 3 opciones, Atacar, Defenderse(aun no funciona) y Habilidad. 
-El usuario debera ponerse de acuerdo con el contricante para decidir cual bando jugar, al ser por turnos, el personaje con mayor velocidad atacara primero, puedes elegir entre un ataque basico que no consume Mana o una habilidad que gastara mana.
-Para atacar debes seleccionar a quien quieres hacerlo (No puedes atacar personajes de tu mismo equipo, a ti mismo ni tampoco espacios en blanco), aunque las habilidades pueden no requerir un objetivo.
-El daño se muestra por consola
-Si te arrepientes y no quieres atacar si no, usar una habilidad o viceversa , pulsa la tecla ESC para volver atras, asi tambien para volver al menu inicial
-El juego se acaba cuando todos los personajes de un equipo hayan muerto

##Estructura del codigo

###Clase Habilidad
Esta clase es padre de la clase Buff y la clase Skill, contiene los atributos (Nombre,tipo y descripcion) contiene tanto los setters como getters

###Clase Buff
Esta clase aun no esta implementada dentro del juego en si,pero sera implementada en futuras actualizaciones (Taller 2), es una clase que hereda de la Clase Habilidad y contiene los atributos (Nombre, tipoBuff,descripcion y coste mana). Dentro contiene metodos para usar el buffo dependiendo del nombre.

###Clase Skill
Esta clase es la principal para las habilidades, y es hija de la clase Hbilidad, contiene los atributos (Nombre,Tipo,descripcion,coste, efecto,potencia y acierto) con sus getters y setters, ademas del void usarSkill

###Clase Personaje
Esta clase es la que representa un personaje, contiene los atributos (nombre, tipo, control, vidaMax, ataque, defensa, velocidad, mana, sangrando , vivo, aturdido , vector<Habilidad*> habilidades, pair<int, int> posicion,manaMax) (El aturdido sera implementado en proximas atualizaciones)

###Clase MenuState
Esta clase es la encargada de inicializar el menu y cargar la fuente, creando las opciones (Jugar, Configuracion y Salir)
-Void Handlevent: Maneja los eventos desde teclado en el menu, ya se pasar al estado GameState o cerrar el juego.
-Void update: Cambia el color de la opcion actualmente selecionada
-void Render: dibuja el menu actual
-Bool Debesalir: devuelve true si el usuario marco la opcion salir

###Clase GameState
Esta clase administra la lógica principal del juego utilizando SFML. Controla el estado del juego, la interacción del usuario, el sistema de turnos, la ejecución de habilidades, la navegación por los menús y el renderizado de los elementos visuales.
-Constructor tiene los siguientes atributos (RenderWindow& window, StateManager& stateManager, vector<Personaje*> personajes, Personaje* (&tablero)[8][8], queue<Personaje*> orden_Turno)

###Clase main trabajoEstructura
En esta clase esta el main que llama a todo el resto de clases para inicializar el juego, tambien contiene 3 metodos.
1-Calcular Orden: Este metodo calcula el orden en que los personajes atacan segun su velocidad y los ordena en una cola para el orden de turno.
2-Ubicar Personaje: Este metodo posiciona cada uno de los personajes en una matriz a traves del pair del personaje
3-Crear Personajes: Este metodo se usa para crear los 8 personajes del juego con sus habilidades y estadisticas 


