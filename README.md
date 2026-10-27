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

Actualmente el `main` se utiliza para probar la logica basica del juego.

Se realizan pruebas para:

- colocar el Puma y los Perros en el tablero;
- realizar una captura valida;
- rechazar una captura invalida;
- comprobar que un Perro no pueda ocupar la posicion del Puma;
- comprobar cuando el Puma queda completamente acorralado.

Mas adelante el `main` sera adaptado para recibir los argumentos `N`, `M` y el archivo de configuracion.

### Tablero

El tablero se encuentra implementado utilizando un grafo.

Se utilizan 28 posiciones en total:

- 25 posiciones para la cuadricula principal de 5x5;
- 3 posiciones para el trono.

Cada posicion puede contener:

- `.` si esta vacia;
- `P` si contiene al Puma;
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

## Decisiones de diseño

- El tablero se representa como un grafo porque ademas de la cuadricula de 5x5 tambien existe el trono.
- Cada posicion guarda una lista de posiciones vecinas para poder revisar los movimientos de manera mas simple.
- Las posiciones de la cuadricula se numeran desde `0` hasta `24`.
- Las posiciones `25`, `26` y `27` corresponden al trono.
- Se utilizan los caracteres `P`, `D` y `.` para representar las fichas y posiciones vacias.
- La clase `Tablero` se encarga solamente de guardar y modificar el estado de las posiciones.
- La clase `Juego` se encuentra separada del tablero para mantener las reglas del Komikan aparte de la representacion del tablero.
- Para revisar los saltos del Puma se utilizan coordenadas para comprobar que el movimiento continue en la misma direccion.
- Por ahora la logica del juego funciona de manera secuencial. La concurrencia se agregara despues de terminar y probar las reglas principales.

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
- compilacion con `-Wall -Wextra -std=c++17`.
- victoria del puma al capturar 6 perros

## Estado actual

Actualmente se encuentra implementada la estructura basica del tablero y las reglas principales de movimiento y captura.

Todavia falta implementar:

- disposicion inicial completa de las 12 fichas de Perro y el Puma;
- prueba de victoria del Puma con 6 capturas;
- lectura de `config.txt`;
- retardos aleatorios entre movimientos;
- threads para las fichas;
- sincronizacion del tablero;
- mutexes, semaforos y otras primitivas POSIX;
- jugadores concurrentes;
- emparejamiento de jugadores;
- limite de mesas activas;
- Juez de cada mesa;
- manejo de desconexiones;
- manejo de `SIGINT`;
- logs de las partidas;
- pruebas de concurrencia y estres.

## Limitaciones

- La version actual todavia no utiliza concurrencia.
- El programa actual se utiliza principalmente para probar la logica del tablero y las reglas basicas.
- La entrada `./torneo N M [config.txt]` todavia no esta implementada.
- Todavia no se simula una partida completa con las 13 fichas funcionando al mismo tiempo.