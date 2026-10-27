#include "servidor.hpp"

#include <iostream>

#include "partida.hpp"

using namespace std;


Servidor::Servidor(int jugadores, int mesas) {

    cantidadJugadores = jugadores;
    maxMesas = mesas;

    mesasActivas = 0;

    sem_init(&semaforoMesas, 0, maxMesas);

    pthread_mutex_init(&mutexEstado, nullptr);
}


Servidor::~Servidor() {

    sem_destroy(&semaforoMesas);

    pthread_mutex_destroy(&mutexEstado);
}


void* Servidor::ejecutarPartida(void* datos) {

    DatosPartida* info =
        static_cast<DatosPartida*>(datos);

    Servidor* servidor = info->servidor;


    sem_wait(&servidor->semaforoMesas);


    pthread_mutex_lock(&servidor->mutexEstado);

    servidor->mesasActivas++;

    cout << "\nPartida "
         << info->numeroPartida
         << " comienza";

    cout << " | Puma: jugador "
         << info->jugadorPuma;

    cout << " | Perros: jugador "
         << info->jugadorPerros;

    cout << " | Mesas activas: "
         << servidor->mesasActivas
         << "/"
         << servidor->maxMesas
         << "\n";

    pthread_mutex_unlock(&servidor->mutexEstado);


    int posicionPuma = 12;

    vector<int> posicionesPerros = {
        0, 1, 2, 3, 4,
        5, 6, 7, 8, 9,
        11, 13
    };


    Partida partida(
        posicionPuma,
        posicionesPerros,
        50,
        300
    );

    partida.iniciar();


    pthread_mutex_lock(&servidor->mutexEstado);

    servidor->mesasActivas--;

    cout << "\nPartida "
         << info->numeroPartida
         << " termino";

    cout << " | Mesas activas: "
         << servidor->mesasActivas
         << "/"
         << servidor->maxMesas
         << "\n";

    pthread_mutex_unlock(&servidor->mutexEstado);


    sem_post(&servidor->semaforoMesas);


    return nullptr;
}


void Servidor::iniciar() {

    int cantidadPartidas =
        cantidadJugadores / 2;


    vector<pthread_t> hilos(
        cantidadPartidas
    );

    vector<DatosPartida> datos(
        cantidadPartidas
    );


    cout << "Jugadores: "
         << cantidadJugadores
         << "\n";

    cout << "Maximo de mesas: "
         << maxMesas
         << "\n";

    cout << "Partidas a realizar: "
         << cantidadPartidas
         << "\n";


    for (int i = 0;
         i < cantidadPartidas;
         i++) {

        datos[i].servidor = this;

        datos[i].jugadorPuma =
            i * 2 + 1;

        datos[i].jugadorPerros =
            i * 2 + 2;

        datos[i].numeroPartida =
            i + 1;


        pthread_create(
            &hilos[i],
            nullptr,
            ejecutarPartida,
            &datos[i]
        );
    }


    for (pthread_t hilo : hilos) {

        pthread_join(hilo, nullptr);
    }


    if (cantidadJugadores % 2 != 0) {

        cout << "\nEl jugador "
             << cantidadJugadores
             << " quedo sin pareja.\n";
    }


    cout << "\nTodas las partidas terminaron.\n";
}