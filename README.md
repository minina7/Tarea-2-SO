# Torneo de Komikan Concurrente

Tarea 2, Sistemas Operativos

Programa que simula un torneo concurrente de Komikan, donde varios jugadores son emparejados en mesas y las fichas se mueven al mismo tiempo sobre un tablero compartido.

## Integrante

- Daneza Parraguez

## Compilacion y uso

Para compilar:

```bash
make
```

Para ejecutar:

```bash
./torneo N M
```

Por ejemplo:

```bash
./torneo 8 2
```

La forma completa de ejecucion es:

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

Para ejecutar todas las pruebas automaticas:

```bash
make tests
```

Para eliminar los ejecutables:

```bash
make clean
```

El programa se compila con C++17 utilizando:

```text
g++ -Wall -Wextra -std=c++17 -pthread
```

## Funciones implementadas

### Main

El `main` recibe los argumentos `N`, `M` y opcionalmente un archivo `config.txt`.

Se comprueba que:

- `N` sea al menos 2
- `M` sea mayor que 0
- el archivo de configuracion pueda cargarse correctamente cuando es entregado

Si no se entrega un archivo de configuracion, se utilizan los valores por defecto.

Antes de iniciar el servidor se instala tambien el manejador de `SIGINT` para permitir un cierre ordenado al presionar `Ctrl+C`.

### Tablero

El tablero se encuentra implementado utilizando un grafo.

Se utilizan 28 posiciones en total:

- 25 posiciones para la cuadricula principal de 5x5
- 3 posiciones para el trono

Las posiciones `0` a `24` corresponden a la cuadricula principal.

Las posiciones `25`, `26` y `27` corresponden al trono.

Las conexiones de la cuadricula siguen las lineas horizontales y verticales del tablero, mientras que las posiciones del trono poseen sus conexiones adicionales correspondientes.

Cada posicion puede contener:

- `.` si esta vacia
- `P` si contiene al Puma
- `D` si contiene un Perro

El tablero permite:

- comprobar si una posicion es valida
- comprobar si una posicion esta libre
- revisar si dos posiciones son adyacentes
- colocar fichas
- mover fichas entre posiciones adyacentes
- realizar una captura
- mostrar el estado del tablero
- mostrar las posiciones vecinas de una casilla

Las conexiones entre las posiciones se guardan mediante listas de vecinos.

### Juego

La clase `Juego` se encarga de las reglas del Komikan y utiliza internamente un objeto `Tablero`.

Se encuentra implementado:

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
- las posiciones involucradas correspondan a conexiones validas del tablero

Cada vez que una captura es valida se aumenta el contador de Perros capturados.

El Puma gana cuando alcanza 6 Perros capturados.

Los Perros ganan cuando el Puma queda completamente acorralado y no posee movimientos ni capturas validas.

### Partida concurrente

Cada partida funciona de manera independiente.

- Se crea un thread para el Puma
- Se crea un thread para cada uno de los 12 Perros
- Cada ficha intenta moverse de manera independiente
- Antes de cada intento de movimiento se utiliza un retardo aleatorio
- El rango de retardos puede configurarse mediante `config.txt`
- El tablero es compartido por todos los threads de la partida
- Se utiliza un `pthread_mutex_t` para proteger el acceso al tablero
- Solo un thread puede revisar y modificar el tablero dentro de la seccion critica
- Esto evita que dos fichas ocupen simultaneamente una misma posicion
- El Puma puede moverse normalmente o capturar Perros
- Cuando un Perro es capturado, su thread deja de participar
- Cada partida posee un thread adicional que funciona como Juez
- Los threads notifican al Juez cuando existe un nuevo estado que revisar
- Cuando se detecta victoria, abandono o interrupcion, la partida se marca como terminada
- Se utiliza `pthread_join()` para esperar correctamente la finalizacion de los threads

### Juez

Cada partida posee un thread de Juez encargado de revisar el estado del juego.

El Juez comprueba:

- si el Puma alcanzo 6 Perros capturados
- si el Puma se encuentra completamente acorralado
- si el jugador Puma abandono o fallo
- si el jugador Perros abandono o fallo
- si el servidor solicito detener la partida mediante `SIGINT`
- cuando debe finalizar la partida

Para comunicarse con los threads de las fichas se utiliza una variable de condicion `pthread_cond_t`.

Cuando una ficha realiza un movimiento valido:

- se marca que existe un nuevo estado pendiente
- se utiliza `pthread_cond_broadcast()` para despertar a los threads que esperan un cambio
- el Juez revisa las condiciones de finalizacion

Mientras no existe un nuevo estado para revisar, el Juez queda esperando mediante `pthread_cond_wait()` sin realizar espera activa.

Si el Puma abandona o falla, el Juez declara ganadores a los Perros.

Si el jugador de los Perros abandona o falla, el Juez declara ganador al Puma.

Si se solicita una interrupcion mediante `SIGINT`, el Juez termina la partida sin declarar un ganador.

Cuando la partida debe finalizar, se despierta a los demas threads para que puedan terminar correctamente.

### Servidor

La clase `Servidor` se encarga de organizar las partidas del torneo.

- Recibe la cantidad total de jugadores `N`
- Recibe la cantidad maxima de mesas simultaneas `M`
- Los jugadores son emparejados de dos en dos
- A cada pareja se le asigna un jugador Puma y un jugador Perros
- Cada pareja ejecuta una partida en un thread independiente
- Se utiliza un semaforo `sem_t` para limitar la cantidad de mesas activas
- Si todas las mesas estan ocupadas, las siguientes partidas quedan esperando mediante `sem_wait()`
- Al terminar una partida se libera la mesa mediante `sem_post()`
- Se mantiene un contador de mesas activas protegido por un mutex
- El servidor espera la finalizacion de todas las partidas mediante `pthread_join()`
- Si la cantidad de jugadores es impar, el ultimo jugador queda sin pareja

El servidor mantiene tambien una referencia a las partidas que se encuentran activas.

Esto permite:

- identificar una partida activa
- reportar un fallo del jugador Puma
- reportar un fallo del jugador Perros
- notificar el abandono a la partida correspondiente
- permitir que el Juez declare ganador al oponente
- liberar nuevamente la mesa cuando termina la partida
- permitir que otra partida reutilice la mesa liberada

Si se recibe `SIGINT`, el servidor evita continuar iniciando partidas y espera que las partidas activas finalicen de manera ordenada.

### Configuracion

La clase `Configuracion` se encarga de cargar y validar los valores del archivo opcional `config.txt`.

Permite configurar:

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

El manejador solamente modifica una bandera indicando que se solicito una interrupcion.

El cierre real se realiza fuera del manejador de señal.

Durante el cierre:

- se evita iniciar nuevas partidas
- las partidas activas detectan la interrupcion
- se notifica al Juez
- los Jueces terminan las partidas interrumpidas
- los threads finalizan
- las mesas son liberadas
- el servidor espera el termino de los threads creados
- los mutexes, variables de condicion y semaforos son destruidos al finalizar sus objetos

### Logs

Cada partida genera un archivo de log independiente dentro del directorio `logs`.

Los archivos utilizan el formato:

```text
logs/partida_NUMERO.txt
```

Por ejemplo:

```text
logs/partida_1.txt
logs/partida_2.txt
logs/partida_3.txt
```

Cada log puede registrar:

- inicio de la partida
- numero de partida
- semilla utilizada
- posicion inicial del Puma
- inicio de los threads
- movimientos realizados por los Perros
- movimientos realizados por el Puma
- capturas realizadas por el Puma
- abandono de alguno de los jugadores
- fallos reportados
- interrupcion mediante `SIGINT`
- decision final del Juez
- finalizacion de la partida
- ganador de la partida

Cada partida utiliza su propio archivo, evitando mezclar el registro de distintas mesas.

El archivo se cierra al finalizar la partida.

## Decisiones de diseño

- El tablero se representa como un grafo porque ademas de la cuadricula principal existe el trono
- Cada posicion mantiene una lista de posiciones vecinas
- Las posiciones de la cuadricula se numeran desde `0` hasta `24`
- Las posiciones `25`, `26` y `27` corresponden al trono
- Se utilizan los caracteres `P`, `D` y `.` para representar las fichas y posiciones vacias
- La clase `Tablero` se encarga solamente de almacenar y modificar el estado del tablero
- La clase `Juego` mantiene separadas las reglas del Komikan de la representacion del tablero
- Para comprobar los saltos del Puma se utilizan coordenadas para verificar que el movimiento continue en la misma direccion
- Se utiliza un mutex global por partida para proteger el tablero
- Se eligio un mutex global porque el tablero es pequeño y permite mantener una solucion simple y segura
- Aunque el mutex global limita que varios movimientos modifiquen simultaneamente una misma partida, evita estados inconsistentes y simplifica la prevencion de condiciones de carrera
- Cada ficha realiza el retardo fuera de la seccion critica
- El mutex solamente se toma cuando es necesario revisar o modificar el tablero
- Se utiliza `pthread_cond_t` para comunicar cambios de estado al Juez
- El Juez utiliza `pthread_cond_wait()` para esperar sin realizar espera activa
- Se utiliza `pthread_cond_broadcast()` cuando cambia un estado que puede afectar a varios threads
- Los abandonos y fallos utilizan el mismo mecanismo de sincronizacion de la partida
- El servidor mantiene referencias a las partidas activas para poder reportar fallos
- Cuando se detecta un abandono, el Juez finaliza la partida y declara ganador al jugador contrario
- La mesa se libera despues de terminar la partida incluso cuando esta termina por abandono
- Se utiliza un semaforo para representar la cantidad de mesas disponibles
- Antes de utilizar una mesa se ejecuta `sem_wait()`
- Al liberar una mesa se ejecuta `sem_post()`
- De esta forma nunca pueden existir mas de `M` mesas activas simultaneamente
- Las partidas que no encuentran una mesa disponible quedan esperando de forma pasiva
- La configuracion se encuentra separada en una clase propia
- Si no se entrega `config.txt`, se utilizan valores por defecto
- Cada partida recibe una semilla derivada de la semilla configurada y de su numero de partida
- La semilla permite repetir escenarios de prueba
- Antes de iniciar el torneo se validan los valores de configuracion
- El manejador de `SIGINT` solamente modifica una bandera
- El cierre real se realiza fuera del manejador de señal
- El servidor espera la finalizacion de todos los threads creados antes de terminar
- Cada partida posee su propio archivo de log para evitar mezclar eventos entre mesas concurrentes

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
- finalizacion de los threads al terminar una partida
- deteccion de victoria durante una partida concurrente
- ejecucion del Juez en un thread independiente
- deteccion de victoria del Puma por parte del Juez
- deteccion de victoria de los Perros
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
- prueba concurrente de colision entre varios Perros intentando ocupar una misma posicion
- comprobacion de que solamente una ficha puede ganar una posicion compartida
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
- creacion automatica del directorio de logs
- creacion de un archivo de log independiente por partida
- registro de movimientos dentro del log
- registro de capturas dentro del log
- registro del ganador dentro del log
- registro de abandonos dentro del log
- comprobacion automatica del contenido de un archivo de log
- ejecucion simultanea de cuatro partidas con cuatro archivos de log separados
- reporte de fallo del Puma desde el servidor
- reporte de fallo del jugador Perros desde el servidor
- deteccion del fallo por parte del Juez
- liberacion de una mesa despues de un fallo
- reutilizacion de una mesa liberada por otra partida
- comprobacion de que no quedan mesas activas despues de las partidas

Las pruebas automaticas principales pueden ejecutarse mediante:

```bash
make tests
```

Actualmente se incluyen:

```text
test_juez
test_colision
test_estres
test_abandono
test_logs
test_fallo_servidor
```

Para compilar la prueba de estres con ThreadSanitizer:

```bash
make test_estres_tsan
```

En el entorno virtual utilizado fue necesario ejecutar ThreadSanitizer desactivando temporalmente ASLR:

```bash
setarch "$(uname -m)" -R ./test_estres_tsan
```

## Estado actual

Actualmente se encuentra implementada la logica del juego, las partidas concurrentes, el Juez de cada partida, el control de varias mesas, la lectura del archivo de configuracion, el manejo de abandono y fallos, el manejo de `SIGINT` y la generacion de logs independientes.

Cada ficha se ejecuta mediante un thread y el tablero compartido de cada partida se protege utilizando un mutex global.

Cada partida posee un Juez que espera mediante una variable de condicion y comprueba las condiciones de victoria cuando cambia el estado del tablero.

El Juez tambien puede detectar abandonos reportados y declarar ganador al oponente.

El servidor mantiene las partidas activas y puede reportar fallos de cualquiera de los dos jugadores.

Cuando una partida termina por victoria, abandono, fallo o interrupcion, sus threads son finalizados y la mesa vuelve a quedar disponible.

El servidor utiliza un semaforo para impedir que funcionen mas de `M` mesas simultaneamente.

El programa permite utilizar un archivo `config.txt` opcional para definir retardos, semilla y posiciones iniciales.

Cada partida genera su propio archivo de log dentro del directorio `logs`.

Al recibir `SIGINT`, el programa evita continuar iniciando partidas, termina ordenadamente las partidas activas, espera los threads creados y libera las mesas utilizadas.

La implementacion funcional principal se encuentra terminada. Antes de la entrega queda realizar la bateria final de pruebas, revisar los requisitos de la pauta y preparar el informe solicitado.

## Limitaciones

- La sincronizacion interna de cada partida utiliza un mutex global para todo el tablero
- El mutex global simplifica la seguridad del estado compartido, pero reduce el paralelismo posible dentro de una misma partida
- Los jugadores se representan dentro de la simulacion mediante threads y estructuras internas del servidor
- El mecanismo de fallos se integra mediante reportes del servidor hacia la partida activa
- Los archivos de log registran los eventos principales de cada partida, pero no almacenan una representacion completa del tablero despues de cada movimiento