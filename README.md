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
./torneo N M
```

Por ejemplo:

```bash
./torneo 8 2
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

Actualmente el programa ya recibe `N` y `M`. La lectura de `config.txt` todavia no se encuentra implementada.

El programa se compila con C++17 utilizando las opciones:

`g++ -Wall -Wextra -std=c++17 -pthread`

## Funciones implementadas

### Main

Actualmente el `main` recibe los argumentos `N` y `M`.

Se comprueba que:

- `N` sea al menos 2;
- `M` sea mayor que 0.

Luego se crea el servidor encargado de organizar las partidas del torneo.

Mas adelante se agregara la lectura del archivo opcional `config.txt`.

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

### Servidor

La clase `Servidor` se encarga actualmente de organizar las partidas del torneo.

- Recibe la cantidad total de jugadores `N`.
- Recibe la cantidad maxima de mesas simultaneas `M`.
- Los jugadores son emparejados de dos en dos.
- Cada pareja ejecuta una partida en un thread independiente.
- Se utiliza un semaforo `sem_t` para limitar la cantidad de mesas activas.
- Si todas las mesas estan ocupadas, las siguientes partidas quedan esperando hasta que una mesa se libere.
- Al terminar una partida se libera el cupo de la mesa utilizando `sem_post()`.
- Se mantiene un contador de mesas activas protegido mediante un mutex.
- El servidor espera la finalizacion de todas las partidas utilizando `pthread_join()`.
- Si la cantidad de jugadores es impar, el ultimo jugador queda sin pareja.

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
- Se utiliza un semaforo para representar la cantidad de mesas disponibles.
- Antes de comenzar una partida se utiliza `sem_wait()` y al terminar se utiliza `sem_post()`.
- De esta forma nunca pueden existir mas de `M` mesas activas al mismo tiempo.
- Las partidas que no encuentran una mesa disponible quedan esperando sin realizar espera activa.

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
- deteccion de victoria durante una partida concurrente;
- ejecucion de varias partidas;
- prueba con 4 jugadores y 1 mesa;
- prueba con 4 jugadores y 2 mesas;
- prueba con 8 jugadores y 2 mesas;
- comprobacion de que nunca existan mas de `M` mesas activas;
- liberacion de una mesa al finalizar una partida;
- inicio de una nueva partida cuando queda una mesa disponible.

## Estado actual

Actualmente se encuentra implementada la logica basica del juego, las partidas concurrentes y el control de varias mesas.

Cada ficha se ejecuta mediante un thread y el tablero compartido de cada partida se protege utilizando un mutex global.

El servidor puede crear varias partidas a partir de `N` jugadores y utiliza un semaforo para impedir que funcionen mas de `M` mesas al mismo tiempo.

Todavia falta implementar:

- lectura de `config.txt`;
- configuracion de retardos mediante archivo;
- configuracion de posiciones iniciales;
- semilla aleatoria configurable;
- Juez de cada mesa;
- manejo de desconexiones;
- manejo de `SIGINT`;
- logs de las partidas;
- pruebas de estres y concurrencia.

## Limitaciones

- El archivo `config.txt` todavia no se utiliza.
- Los retardos se mantienen actualmente entre 50 y 300 ms.
- Las posiciones iniciales de las fichas se encuentran definidas directamente en el codigo.
- La semilla aleatoria todavia no se puede configurar desde un archivo.
- Todavia no se encuentra implementado el manejo de desconexiones.
- Todavia no se encuentra implementado el manejo de `SIGINT`.
- La sincronizacion interna de cada partida utiliza un mutex global para todo el tablero.