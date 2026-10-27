#ifndef PARTIDA_HPP
#define PARTIDA_HPP

#include <pthread.h>
#include <vector>

#include "juego.hpp"

using namespace std;

class Partida {
private:
    struct DatosHilo {
        Partida* partida;
        int indice;
        unsigned int seed;
    };

    Juego juego;

    pthread_mutex_t mutexTablero;
    pthread_cond_t condEstado;

    int posicionPuma;

    vector<int> posicionesPerros;

    int retardoMin;
    int retardoMax;

    unsigned int semillaBase;

    bool terminada;
    bool estadoPendiente;

    static void* ejecutarPuma(void* datos);
    static void* ejecutarPerro(void* datos);
    static void* ejecutarJuez(void* datos);

    int obtenerRetardo(unsigned int& seed);

    bool moverPumaAleatorio(unsigned int& seed);

    bool moverPerroAleatorio(
        int indice,
        unsigned int& seed
    );

public:
    Partida(
        int posicionInicialPuma,
        const vector<int>& posicionesInicialesPerros,
        int retardoMinimo,
        int retardoMaximo,
        unsigned int semilla
    );

    ~Partida();

    void iniciar();
};

#endif