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

    int posicionPuma;

    vector<int> posicionesPerros;

    int retardoMin;
    int retardoMax;

    unsigned int semillaBase;

    bool terminada;

    static void* ejecutarPuma(void* datos);
    static void* ejecutarPerro(void* datos);

    int obtenerRetardo(unsigned int& seed);

    bool moverPumaAleatorio(unsigned int& seed);

    bool moverPerroAleatorio(
        int indice,
        unsigned int& seed
    );

    void revisarFinPartida();

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