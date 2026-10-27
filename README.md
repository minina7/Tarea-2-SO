# Torneo de Komikan Concurrente

Tarea 2, Sistemas Operativos

Programa que debe simular un torneo concurrente de Komikan, donde varios jugadores son emparejados en mesas y las fichas se mueven al mismo tiempo sobre un tablero compartido.

## Integrante

- Daneza Parraguez

## Compilacion y uso

Para compilar:

```bash
make
```

Para ejecutar la version actual:

```bash
./torneo
```

Para eliminar el ejecutable:

```bash
make clean
```

La forma final de ejecucion pedida para la tarea sera:

```bash
./torneo N M [config.txt]
```

- `N`: cantidad total de jugadores.
- `M`: cantidad maxima de mesas que pueden funcionar al mismo tiempo.
- `config.txt`: archivo de configuracion opcional.

El programa se compila con C++17 utilizando las opciones:

`g++ -Wall -Wextra -std=c++17 -pthread`

## Funciones implementadas

### Main

Actualmente el `main` se utiliza para iniciar una partida concurrente de prueba.

- Define la posicion inicial del Puma.
- Define las posiciones iniciales de los 12 Perros.
- Crea una partida con retardos entre 50 y 300 ms.
- Inicia la ejecucion concurrente de las fichas.

Mas adelante el `main` sera adaptado para recibir los argumentos `N`, `M` y el archivo de configuracion.

### Tablero

El tablero se encuentra implementado utilizando un grafo.

Se utilizan 28 posiciones en total:

- 25 posiciones para la cuadricula principal de 5x5.
- 3 posiciones para el trono.

Cada posicion puede contener:

- `.` si esta vacia.
- `P` si contiene al Puma.
- `D` si contiene un Perro.

Actualmente el tablero permite:

- comprobar si una posicion es valida;
- comprobar si una posicion esta libre;
- revisar si dos posiciones son adyacentes;
- colocar fichas;
- mover fichas entre posiciones adyacentes;
- realizar una captura;
- mostrar el estado del tablero;
- mostrar las posiciones vecinas de una casilla.

Las conexiones entre las posiciones se guardan en una lista de vecinos.

### Juego

La clase `Juego` se encarga de las reglas del Komikan y utiliza internamente un objeto `Tablero`.

Actualmente se encuentra implementado:

- movimiento del Puma;
- movimiento de los Perros;
- captura de un Perro por parte del Puma;
- contador de Perros capturados;
- comprobacion de victoria del Puma;
- comprobacion de movimientos disponibles del Puma;
- comprobacion de victoria de los Perros.

Para realizar una captura se comprueba que:

- en la posicion inicial exista un Puma;
- exista un Perro entre el origen y el destino;
- la posicion de destino este libre;
- el salto mantenga la misma direccion.

Cada vez que una captura es valida se aumenta el contador de Perros capturados.

El Puma gana cuando alcanza 6 Perros capturados.

Los Perros ganan cuando el Puma no tiene movimientos normales ni capturas disponibles.

### Partida concurrente

- Se crea un thread para el Puma y un thread para cada Perro.
- La partida utiliza 1 Puma y 12 Perros.
- Cada ficha intenta moverse de forma independiente.
- Antes de cada movimiento se utiliza un retardo aleatorio entre 50 y 300 ms.
- El tablero es compartido por todos los threads de la partida.
- Se utiliza un `pthread_mutex_t` para proteger el acceso al tablero.
- Solo un thread puede revisar y modificar el tablero al mismo tiempo.
- Esto evita que dos fichas ocupen una misma posicion de forma simultanea.
- El Puma puede realizar movimientos normales y capturar Perros durante la ejecucion concurrente.
- Cuando un Perro es capturado, su thread deja de participar en la partida.
- Cuando se detecta una condicion de victoria, los threads terminan.
- Se utiliza `pthread_join()` para esperar correctamente la finalizacion de los threads.

## Decisiones de diseño

- El tablero se representa como un grafo porque ademas de la cuadricula de 5x5 tambien existe el trono.
- Cada posicion guarda una lista de posiciones vecinas para poder revisar los movimientos de manera mas simple.
- Las posiciones de la cuadricula se numeran desde `0` hasta `24`.
- Las posiciones `25`, `26` y `27` corresponden al trono.
- Se utilizan los caracteres `P`, `D` y `.` para representar las fichas y posiciones vacias.
- La clase `Tablero` se encarga solamente de guardar y modificar el estado de las posiciones.
- La clase `Juego` se encuentra separada del tablero para mantener las reglas del Komikan aparte de la representacion del tablero.
- Para revisar los saltos del Puma se utilizan coordenadas para comprobar que el movimiento continue en la misma direccion.
- Para la primera version concurrente se utiliza un mutex global por partida para proteger todo el tablero.
- Se eligio un mutex global porque el tablero es pequeno y permite evitar condiciones de carrera sin agregar demasiada complejidad.
- Aunque este metodo limita que dos movimientos se modifiquen al mismo tiempo, mantiene el estado del tablero consistente y reduce el riesgo de deadlocks.
- Cada ficha realiza el retardo fuera de la seccion critica y solo toma el mutex cuando necesita revisar o modificar el tablero.

## Pruebas

Actualmente se han realizado pruebas para:

- creacion del tablero vacio;
- colocacion del Puma;
- colocacion de Perros;
- movimiento normal entre posiciones adyacentes;
- captura horizontal valida;
- eliminacion del Perro despues de una captura;
- aumento del contador de Perros capturados;
- rechazo de una captura que cambia de direccion;
- rechazo del movimiento de un Perro hacia una casilla ocupada por el Puma;
- deteccion de un Puma completamente acorralado;
- victoria del Puma al capturar 6 Perros;
- compilacion con `-Wall -Wextra -std=c++17 -pthread`;
- ejecucion concurrente del Puma y los 12 Perros;
- movimientos realizados por distintos threads;
- retardos aleatorios entre movimientos;
- capturas del Puma durante una partida concurrente;
- proteccion del tablero mediante mutex;
- finalizacion de los threads al terminar la partida;
- deteccion de victoria durante una partida concurrente.

## Estado actual

Actualmente se encuentra implementada la logica basica del juego y una partida concurrente.

Cada ficha se ejecuta mediante un thread y el tablero compartido se protege utilizando un mutex global.

Todavia falta implementar:

- lectura de `config.txt`;
- servidor y llegada de jugadores;
- emparejamiento de jugadores;
- varias mesas funcionando al mismo tiempo;
- limite maximo de `M` mesas;
- semaforo para controlar las mesas disponibles;
- Juez de cada mesa;
- manejo de desconexiones;
- manejo de `SIGINT`;
- logs de las partidas;
- pruebas de estres y concurrencia.

## Limitaciones

- Actualmente se simula una sola partida concurrente.
- Todavia no se encuentra implementado el servidor de jugadores.
- El limite de `M` mesas todavia no esta implementado.
- El archivo `config.txt` todavia no se utiliza.
- La sincronizacion actual utiliza un mutex global para todo el tablero.