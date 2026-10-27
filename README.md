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

La forma de ejecucion es:

```bash
./torneo N M [config.txt]
```

- `N`: cantidad total de jugadores
- `M`: cantidad maxima de mesas que pueden funcionar al mismo tiempo

El archivo `config.txt` es opcional. Si no se entrega, el programa utiliza valores por defecto.

El archivo permite configurar:

- retardo minimo entre movimientos
- retardo maximo entre movimientos
- semilla utilizada para los movimientos aleatorios
- posicion inicial del Puma
- posiciones iniciales de los 12 Perros

Ejemplo de `config.txt`:

```text
retardo_min=50
retardo_max=300
semilla=12345
puma=12
perros=0,1,2,3,4,5,6,7,8,9,11,13
```

Para ejecutar utilizando el archivo de configuracion:

```bash
./torneo 8 2 config.txt
```

Actualmente el programa recibe `N`, `M` y opcionalmente un archivo `config.txt`.

El programa se compila con C++17 utilizando las opciones:

`g++ -Wall -Wextra -std=c++17 -pthread`

## Funciones implementadas

### Main

Actualmente el `main` recibe los argumentos `N` y `M`.

Se comprueba que:

- `N` sea al menos 2
- `M` sea mayor que 0

Luego se crea el servidor encargado de organizar las partidas del torneo.

El `main` tambien permite recibir opcionalmente un archivo `config.txt`.

Si el archivo es entregado, se carga y valida antes de iniciar el torneo. Si no se entrega, se utilizan los valores por defecto.

Antes de iniciar el servidor tambien se instala el manejador de `SIGINT` para permitir un cierre ordenado al presionar `Ctrl+C`.

### Tablero

El tablero se encuentra implementado utilizando un grafo.

Se utilizan 28 posiciones en total:

- 25 posiciones para la cuadricula principal de 5x5
- 3 posiciones para el trono

Cada posicion puede contener:

- `.` si esta vacia
- `P` si contiene al Puma
- `D` si contiene un Perro

Actualmente el tablero permite:

- comprobar si una posicion es valida
- comprobar si una posicion esta libre
- revisar si dos posiciones son adyacentes
- colocar fichas
- mover fichas entre posiciones adyacentes
- realizar una captura
- mostrar el estado del tablero
- mostrar las posiciones vecinas de una casilla

Las conexiones entre las posiciones se guardan en una lista de vecinos.

### Juego

La clase `Juego` se encarga de las reglas del Komikan y utiliza internamente un objeto `Tablero`.

Actualmente se encuentra implementado:

- movimiento del Puma
- movimiento de los Perros
- captura de un Perro por parte del Puma
- contador de Perros capturados
- comprobacion de victoria del Puma
- comprobacion de movimientos disponibles del Puma
- comprobacion de victoria de los Perros

Para realizar una captura se comprueba que:

- en la posicion inicial exista un Puma
- exista un Perro entre el origen y el destino
- la posicion de destino este libre
- el salto mantenga la misma direccion

Cada vez que una captura es valida se aumenta el contador de Perros capturados.

El Puma gana cuando alcanza 6 Perros capturados.

Los Perros ganan cuando el Puma no tiene movimientos normales ni capturas disponibles.

### Partida concurrente

- Se crea un thread para el Puma y un thread para cada Perro
- La partida utiliza 1 Puma y 12 Perros
- Cada ficha intenta moverse de forma independiente
- Antes de cada movimiento se utiliza un retardo aleatorio
- El rango de retardos puede definirse mediante `config.txt`
- El tablero es compartido por todos los threads de la partida
- Se utiliza un `pthread_mutex_t` para proteger el acceso al tablero
- Solo un thread puede revisar y modificar el tablero al mismo tiempo
- Esto evita que dos fichas ocupen una misma posicion de forma simultanea
- El Puma puede realizar movimientos normales y capturar Perros durante la ejecucion concurrente
- Cuando un Perro es capturado, su thread deja de participar en la partida
- Cada partida crea un thread adicional encargado de funcionar como Juez
- Los threads notifican al Juez cuando existe un nuevo estado que debe revisar
- Cuando el Juez detecta una condicion de victoria, abandono o interrupcion, la partida se marca como terminada
- Se utiliza `pthread_join()` para esperar correctamente la finalizacion de los threads

### Juez

Cada partida posee un thread de Juez encargado de revisar el estado del juego.

El Juez comprueba:

- si el Puma alcanzo 6 Perros capturados
- si el Puma se encuentra completamente acorralado
- si alguno de los jugadores reporto un abandono o fallo
- si el servidor solicito detener la partida
- cuando debe finalizar la partida

Para comunicarse con los threads de las fichas se utiliza una variable de condicion `pthread_cond_t`.

Cuando una ficha realiza un movimiento valido:

- se marca que existe un nuevo estado pendiente
- se notifica al Juez mediante `pthread_cond_signal()` o `pthread_cond_broadcast()`
- el Juez revisa las condiciones de victoria

Mientras no existe un nuevo estado para revisar, el Juez queda esperando mediante `pthread_cond_wait()` sin realizar espera activa.

Si el Puma abandona, el Juez declara ganadores a los Perros.

Si el jugador de los Perros abandona, el Juez declara ganador al Puma.

Si se solicita una interrupcion mediante `SIGINT`, el Juez termina la partida sin declarar un ganador.

Cuando la partida debe finalizar, se notifica a los demas threads para que puedan terminar correctamente.

### Servidor

La clase `Servidor` se encarga actualmente de organizar las partidas del torneo.

- Recibe la cantidad total de jugadores `N`
- Recibe la cantidad maxima de mesas simultaneas `M`
- Los jugadores son emparejados de dos en dos
- Cada pareja ejecuta una partida en un thread independiente
- Se utiliza un semaforo `sem_t` para limitar la cantidad de mesas activas
- Si todas las mesas estan ocupadas, las siguientes partidas quedan esperando hasta que una mesa se libere
- Al terminar una partida se libera el cupo de la mesa utilizando `sem_post()`
- Se mantiene un contador de mesas activas protegido mediante un mutex
- El servidor espera la finalizacion de todas las partidas utilizando `pthread_join()`
- Si la cantidad de jugadores es impar, el ultimo jugador queda sin pareja
- Si se recibe `SIGINT`, se evita iniciar nuevas partidas
- Las partidas activas son detenidas de manera ordenada
- Al finalizar se liberan los recursos de sincronizacion utilizados por el servidor

### Configuracion

La clase `Configuracion` se encarga de cargar y validar los valores del archivo opcional `config.txt`.

Actualmente permite configurar:

- retardo minimo
- retardo maximo
- semilla aleatoria
- posicion inicial del Puma
- posiciones iniciales de los 12 Perros

Si no se entrega un archivo de configuracion, se utilizan valores por defecto.

Tambien se comprueba que:

- el retardo minimo no sea negativo
- el retardo maximo sea mayor o igual que el minimo
- la posicion del Puma sea valida
- las posiciones de los Perros sean validas
- existan exactamente 12 Perros
- no existan dos fichas utilizando la misma posicion
- los valores del archivo tengan un formato valido

### Senales

El manejo de señales se encuentra separado del resto del programa.

Se instala un manejador para `SIGINT` que permite detectar cuando el usuario presiona `Ctrl+C`.

El manejador solamente registra que se solicito una interrupcion. Luego el servidor y las partidas revisan este estado y realizan el cierre de forma ordenada.

Durante el cierre:

- se dejan de iniciar nuevas partidas
- se notifica a las partidas activas
- los Jueces terminan las partidas interrumpidas
- los threads finalizan
- las mesas son liberadas
- el servidor espera el termino de los threads creados
- las primitivas de sincronizacion se destruyen al finalizar sus objetos

## Decisiones de diseño

- El tablero se representa como un grafo porque ademas de la cuadricula de 5x5 tambien existe el trono
- Cada posicion guarda una lista de posiciones vecinas para poder revisar los movimientos de manera mas simple
- Las posiciones de la cuadricula se numeran desde `0` hasta `24`
- Las posiciones `25`, `26` y `27` corresponden al trono
- Se utilizan los caracteres `P`, `D` y `.` para representar las fichas y posiciones vacias
- La clase `Tablero` se encarga solamente de guardar y modificar el estado de las posiciones
- La clase `Juego` se encuentra separada del tablero para mantener las reglas del Komikan aparte de la representacion del tablero
- Para revisar los saltos del Puma se utilizan coordenadas para comprobar que el movimiento continue en la misma direccion
- Para la primera version concurrente se utiliza un mutex global por partida para proteger todo el tablero
- Se eligio un mutex global porque el tablero es pequeno y permite evitar condiciones de carrera sin agregar demasiada complejidad
- Aunque este metodo limita que dos movimientos se modifiquen al mismo tiempo, mantiene el estado del tablero consistente y reduce el riesgo de deadlocks
- Cada ficha realiza el retardo fuera de la seccion critica y solo toma el mutex cuando necesita revisar o modificar el tablero
- Se utiliza una variable de condicion `pthread_cond_t` para comunicar los movimientos de las fichas al Juez
- El Juez espera mediante `pthread_cond_wait()` mientras no existe un nuevo estado que revisar
- Esto permite que el Juez permanezca bloqueado sin utilizar espera activa
- Los abandonos se reportan al Juez utilizando el mismo mecanismo de sincronizacion de la partida
- Cuando se detecta un abandono, el Juez finaliza la partida y declara ganador al jugador contrario
- Se utiliza un semaforo para representar la cantidad de mesas disponibles
- Antes de comenzar una partida se utiliza `sem_wait()` y al terminar se utiliza `sem_post()`
- De esta forma nunca pueden existir mas de `M` mesas activas al mismo tiempo
- Las partidas que no encuentran una mesa disponible quedan esperando sin realizar espera activa
- La configuracion se encuentra separada en una clase propia para no mezclar la lectura del archivo con la logica del torneo
- Si no se entrega `config.txt`, el programa mantiene valores por defecto
- Cada partida recibe una semilla derivada de la semilla configurada y de su numero de partida
- Antes de iniciar el torneo se validan los valores del archivo para evitar posiciones invalidas, fichas repetidas o rangos de retardo incorrectos
- El manejador de `SIGINT` solamente modifica una bandera para mantener el manejo de la señal simple
- El cierre real se realiza fuera del manejador de la señal
- Las partidas comprueban la interrupcion y notifican al Juez para finalizar de forma ordenada
- El servidor espera la finalizacion de los threads antes de terminar

## Pruebas

Actualmente se han realizado pruebas para:

- creacion del tablero vacio
- colocacion del Puma
- colocacion de Perros
- movimiento normal entre posiciones adyacentes
- captura horizontal valida
- eliminacion del Perro despues de una captura
- aumento del contador de Perros capturados
- rechazo de una captura que cambia de direccion
- rechazo del movimiento de un Perro hacia una casilla ocupada por el Puma
- deteccion de un Puma completamente acorralado
- victoria del Puma al capturar 6 Perros
- compilacion con `-Wall -Wextra -std=c++17 -pthread`
- ejecucion concurrente del Puma y los 12 Perros
- movimientos realizados por distintos threads
- retardos aleatorios entre movimientos
- capturas del Puma durante una partida concurrente
- proteccion del tablero mediante mutex
- finalizacion de los threads al terminar la partida
- deteccion de victoria durante una partida concurrente
- ejecucion del Juez en un thread independiente
- deteccion de victoria del Puma por parte del Juez
- finalizacion de los threads despues de que el Juez detecta un ganador
- ejecucion simultanea de un Juez independiente por cada partida
- comunicacion entre las fichas y el Juez mediante `pthread_cond_t`
- ejecucion de varias partidas
- prueba con 4 jugadores y 1 mesa
- prueba con 4 jugadores y 2 mesas
- prueba con 8 jugadores y 2 mesas
- comprobacion de que nunca existan mas de `M` mesas activas
- liberacion de una mesa al finalizar una partida
- inicio de una nueva partida cuando queda una mesa disponible
- ejecucion sin `config.txt` utilizando valores por defecto
- carga correcta de `config.txt`
- cambio del rango de retardos mediante el archivo
- cambio de la semilla aleatoria mediante el archivo
- validacion de una configuracion con retardos incorrectos
- rechazo de una configuracion invalida antes de iniciar las partidas
- prueba automatica de victoria del Puma
- prueba automatica de Puma acorralado
- prueba concurrente de colision entre varios Perros intentando ocupar la misma casilla
- comprobacion de que solo una ficha puede ganar una posicion compartida
- prueba de estres con 12 threads realizando operaciones concurrentes
- prueba de estres utilizando ThreadSanitizer
- ejecucion de la prueba de estres con ThreadSanitizer sin condiciones de carrera detectadas
- prueba de abandono del jugador Puma
- prueba de abandono del jugador Perros
- deteccion del abandono por parte del Juez
- declaracion del oponente como ganador por abandono
- finalizacion correcta de los threads despues de un abandono
- interrupcion del servidor mediante `SIGINT`
- notificacion de las partidas activas al presionar `Ctrl+C`
- finalizacion ordenada de los threads durante una interrupcion
- liberacion de las mesas activas despues de `SIGINT`
- comprobacion de que las mesas activas llegan nuevamente a `0`
- cierre ordenado del servidor despues de `SIGINT`

## Estado actual

Actualmente se encuentra implementada la logica basica del juego, las partidas concurrentes, el Juez de cada partida, el control de varias mesas, la lectura del archivo de configuracion, el mecanismo de abandono de jugadores y el manejo de `SIGINT`.

Cada ficha se ejecuta mediante un thread y el tablero compartido de cada partida se protege utilizando un mutex global.

Cada partida posee ademas un thread de Juez que espera mediante una variable de condicion y comprueba las condiciones de victoria cuando se producen cambios en el tablero.

El Juez tambien puede recibir el reporte de abandono de alguno de los jugadores, finalizar la partida y declarar ganador al oponente.

El servidor puede crear varias partidas a partir de `N` jugadores y utiliza un semaforo para impedir que funcionen mas de `M` mesas al mismo tiempo.

El programa tambien puede utilizar un archivo `config.txt` opcional para definir los retardos, la semilla y las posiciones iniciales.

Al recibir `SIGINT`, el programa detiene el inicio de nuevas partidas, termina ordenadamente las partidas activas, espera los threads creados y libera las mesas utilizadas.

Todavia falta implementar:

- logs de las partidas
- integracion final del manejo de fallos con el flujo completo del servidor

## Limitaciones

- El manejo de abandono se encuentra implementado dentro de cada partida, pero falta integrarlo completamente con el flujo del servidor
- Todavia no se generan archivos de log para las partidas
- La sincronizacion interna de cada partida utiliza un mutex global para todo el tablero