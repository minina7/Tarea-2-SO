# Tarea 2 - Torneo de Komikan Concurrente

## Sistemas Operativos

Implementación en C++ de un torneo concurrente del juego Komikan.

El programa simula múltiples jugadores y mesas de juego utilizando mecanismos de concurrencia y sincronización de sistemas operativos.

## Compilación

El proyecto utiliza C++17.

Para compilar:

```bash
make
```

También se puede limpiar el ejecutable generado con:

```bash
make clean
```

## Ejecución

La forma general de ejecución será:

```bash
./torneo N M [config.txt]
```

Donde:

- `N`: número total de jugadores.
- `M`: número máximo de mesas que pueden ejecutarse simultáneamente.
- `config.txt`: archivo de configuración opcional.

## Estructura del proyecto

```text
Tarea-2-SO/
├── include/
├── logs/
├── src/
├── tests/
├── .gitignore
├── Makefile
└── README.md
```

### include/

Contiene los archivos de cabecera (`.hpp`) con las declaraciones de clases y funciones.

### src/

Contiene la implementación principal del programa en archivos `.cpp`.

### tests/

Contendrá archivos y pruebas utilizadas para verificar el funcionamiento del sistema.

### logs/

Se utilizará para almacenar registros generados durante las simulaciones.

## Implementación actual

### Tablero

El tablero de Komikan se representa mediante una estructura de posiciones conectadas.

Se utiliza una representación basada en un grafo, donde cada posición del tablero corresponde a un nodo y las conexiones entre posiciones representan movimientos posibles entre casillas adyacentes.

El tablero contiene:

- 25 posiciones correspondientes a la cuadrícula principal de 5x5.
- 3 posiciones correspondientes al trono.
- 28 posiciones en total.

Cada posición puede encontrarse:

- vacía (`.`)
- ocupada por el Puma (`P`)
- ocupada por un Perro (`D`)

Actualmente se encuentran implementadas operaciones para:

- verificar si una posición es válida;
- verificar si una posición está libre;
- determinar si dos posiciones son adyacentes;
- colocar una ficha;
- mover una ficha entre posiciones adyacentes;
- mostrar el estado del tablero.

## Decisiones de diseño

### Representación del tablero

Se decidió representar el tablero como un grafo en lugar de utilizar únicamente una matriz.

Esta decisión facilita la representación del trono, ya que el tablero de Komikan no corresponde solamente a una cuadrícula rectangular.

Cada posición mantiene una lista de sus posiciones vecinas, lo que permite comprobar directamente si un movimiento entre dos posiciones es válido.

Esta representación también facilitará posteriormente la implementación de las reglas del Puma, las capturas y la sincronización de movimientos concurrentes.

## Estado del proyecto

Actualmente se está desarrollando la representación básica del tablero y la validación de movimientos.

Las funcionalidades de concurrencia, emparejamiento de jugadores, mesas, sincronización y manejo de señales serán incorporadas progresivamente.