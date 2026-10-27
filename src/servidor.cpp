#include "servidor.hpp"

#include <iostream>
#include <cerrno>

#include "partida.hpp"
#include "senales.hpp"

using namespace std;

Servidor::Servidor(
    int jugadores,
    int mesas,
    const Configuracion& config
) {
    cantidadJugadores =
        jugadores;

    maxMesas =
        mesas;

    configuracion =
        config;

    mesasActivas = 0;

    sem_init(
        &semaforoMesas,
        0,
        maxMesas
    );

    pthread_mutex_init(
        &mutexEstado,
        nullptr
    );
}

Servidor::~Servidor() {
    sem_destroy(
        &semaforoMesas
    );

    pthread_mutex_destroy(
        &mutexEstado
    );
}

void* Servidor::ejecutarPartida(
    void* datos
) {
    DatosPartida* info =
        static_cast<DatosPartida*>(
            datos
        );

    Servidor* servidor =
        info->servidor;

    bool obtuvoMesa = false;

    while (!obtuvoMesa) {
        if (
            interrupcionSolicitada()
        ) {
            return nullptr;
        }

        int resultado =
            sem_wait(
                &servidor->semaforoMesas
            );

        if (resultado == 0) {
            obtuvoMesa = true;
        }
        else if (
            errno == EINTR
        ) {
            if (
                interrupcionSolicitada()
            ) {
                return nullptr;
            }
        }
        else {
            return nullptr;
        }
    }

    if (
        interrupcionSolicitada()
    ) {
        sem_post(
            &servidor->semaforoMesas
        );

        return nullptr;
    }

    pthread_mutex_lock(
        &servidor->mutexEstado
    );

    servidor->mesasActivas++;

    cout
        << "\nPartida "
        << info->numeroPartida
        << " comienza";

    cout
        << " | Puma: jugador "
        << info->jugadorPuma;

    cout
        << " | Perros: jugador "
        << info->jugadorPerros;

    cout
        << " | Mesas activas: "
        << servidor->mesasActivas
        << "/"
        << servidor->maxMesas
        << "\n";

    pthread_mutex_unlock(
        &servidor->mutexEstado
    );

    Partida partida(
        servidor->configuracion.posicionPuma,
        servidor->configuracion.posicionesPerros,
        servidor->configuracion.retardoMin,
        servidor->configuracion.retardoMax,
        servidor->configuracion.semilla +
            info->numeroPartida * 1000,
        info->numeroPartida
    );

    if (
        interrupcionSolicitada()
    ) {
        partida.solicitarDetencion();
    }

    partida.iniciar();

    bool interrumpida =
        partida.fueInterrumpida();

    pthread_mutex_lock(
        &servidor->mutexEstado
    );

    servidor->mesasActivas--;

    if (interrumpida) {
        cout
            << "\nPartida "
            << info->numeroPartida
            << " detenida por SIGINT";
    }
    else {
        cout
            << "\nPartida "
            << info->numeroPartida
            << " termino";
    }

    cout
        << " | Mesas activas: "
        << servidor->mesasActivas
        << "/"
        << servidor->maxMesas
        << "\n";

    pthread_mutex_unlock(
        &servidor->mutexEstado
    );

    sem_post(
        &servidor->semaforoMesas
    );

    return nullptr;
}

void Servidor::iniciar() {
    int cantidadPartidas =
        cantidadJugadores / 2;

    vector<pthread_t> hilos(
        cantidadPartidas
    );

    vector<bool> hilosCreados(
        cantidadPartidas,
        false
    );

    vector<DatosPartida> datos(
        cantidadPartidas
    );

    cout
        << "Jugadores: "
        << cantidadJugadores
        << "\n";

    cout
        << "Maximo de mesas: "
        << maxMesas
        << "\n";

    cout
        << "Partidas a realizar: "
        << cantidadPartidas
        << "\n";

    for (
        int i = 0;
        i < cantidadPartidas;
        i++
    ) {
        if (
            interrupcionSolicitada()
        ) {
            break;
        }

        datos[i].servidor =
            this;

        datos[i].jugadorPuma =
            i * 2 + 1;

        datos[i].jugadorPerros =
            i * 2 + 2;

        datos[i].numeroPartida =
            i + 1;

        int resultado =
            pthread_create(
                &hilos[i],
                nullptr,
                ejecutarPartida,
                &datos[i]
            );

        if (resultado == 0) {
            hilosCreados[i] =
                true;
        }
        else {
            cout
                << "Error: no se pudo crear la partida "
                << i + 1
                << ".\n";
        }
    }

    for (
        int i = 0;
        i < cantidadPartidas;
        i++
    ) {
        if (
            hilosCreados[i]
        ) {
            pthread_join(
                hilos[i],
                nullptr
            );
        }
    }

    if (
        interrupcionSolicitada()
    ) {
        cout
            << "\nServidor detenido por SIGINT.\n";

        return;
    }

    if (
        cantidadJugadores % 2 != 0
    ) {
        cout
            << "\nEl jugador "
            << cantidadJugadores
            << " quedo sin pareja.\n";
    }

    cout
        << "\nTodas las partidas terminaron.\n";
}