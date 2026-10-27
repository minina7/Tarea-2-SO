#ifndef SERVIDOR_HPP
#define SERVIDOR_HPP

#include <pthread.h>
#include <semaphore.h>
#include <vector>

#include "configuracion.hpp"
#include "partida.hpp"

using namespace std;

class Servidor {
private:
    struct DatosCliente {
        Servidor* servidor;
        int jugador;
    };

    struct DatosPartida {
        Servidor* servidor;
        int jugadorPuma;
        int jugadorPerros;
        int numeroPartida;
    };

    int cantidadJugadores;
    int maxMesas;

    Configuracion configuracion;

    sem_t semaforoMesas;

    pthread_mutex_t mutexEstado;
    pthread_mutex_t mutexEmparejamiento;

    int mesasActivas;
    int siguientePartida;

    vector<int> jugadoresEsperando;

    vector<Partida*> partidasActivas;

    vector<pthread_t> hilosPartidas;
    vector<int> partidaCreada;
    vector<DatosPartida> datosPartidas;

    static void* ejecutarCliente(void* datos);
    static void* ejecutarPartida(void* datos);

    void registrarCliente(int jugador);

public:
    Servidor(
        int jugadores,
        int mesas,
        const Configuracion& config
    );

    ~Servidor();

    void iniciar();

    bool reportarFalloPuma(
        int numeroPartida
    );

    bool reportarFalloPerros(
        int numeroPartida
    );

    int obtenerPartidaActiva();

    int obtenerMesasActivas();
};

#endif