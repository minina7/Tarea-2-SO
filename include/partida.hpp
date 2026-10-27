#ifndef PARTIDA_HPP
#define PARTIDA_HPP

#include <pthread.h>
#include <vector>
#include <fstream>
#include <string>

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

    int numeroPartida;

    bool terminada;
    bool estadoPendiente;

    bool abandonoPuma;
    bool abandonoPerros;

    bool interrupcionServidor;

    int ganador;
    bool finalizoPorAbandono;

    ofstream archivoLog;

    static void* ejecutarPuma(void* datos);
    static void* ejecutarPerro(void* datos);
    static void* ejecutarJuez(void* datos);

    int obtenerRetardo(unsigned int& seed);

    bool moverPumaAleatorio(
        unsigned int& seed
    );

    bool moverPerroAleatorio(
        int indice,
        unsigned int& seed
    );

    void registrarLog(
        const string& mensaje
    );

public:
    Partida(
        int posicionInicialPuma,
        const vector<int>& posicionesInicialesPerros,
        int retardoMinimo,
        int retardoMaximo,
        unsigned int semilla,
        int numero
    );

    ~Partida();

    void iniciar();

    void reportarAbandonoPuma();
    void reportarAbandonoPerros();

    void solicitarDetencion();

    int obtenerGanador();

    bool terminoPorAbandono();
    bool fueInterrumpida();
};

#endif