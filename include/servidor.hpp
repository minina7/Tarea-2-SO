#ifndef SERVIDOR_HPP
#define SERVIDOR_HPP

#include <pthread.h>
#include <semaphore.h>
#include <vector>

using namespace std;


class Servidor {
private:

    struct DatosPartida {
        Servidor* servidor;
        int jugadorPuma;
        int jugadorPerros;
        int numeroPartida;
    };

    int cantidadJugadores;
    int maxMesas;

    sem_t semaforoMesas;

    pthread_mutex_t mutexEstado;

    int mesasActivas;

    static void* ejecutarPartida(void* datos);

public:

    Servidor(int jugadores, int mesas);

    ~Servidor();

    void iniciar();
};

#endif