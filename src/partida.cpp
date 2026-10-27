#include "partida.hpp"

#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace std;


Partida::Partida(
    int posicionInicialPuma,
    const vector<int>& posicionesInicialesPerros,
    int retardoMinimo,
    int retardoMaximo
) {

    posicionPuma = posicionInicialPuma;
    posicionesPerros = posicionesInicialesPerros;

    retardoMin = retardoMinimo;
    retardoMax = retardoMaximo;

    terminada = false;

    pthread_mutex_init(&mutexTablero, nullptr);

    Tablero& tablero = juego.obtenerTablero();

    tablero.colocarFicha(posicionPuma, 'P');

    for (int posicion : posicionesPerros) {
        tablero.colocarFicha(posicion, 'D');
    }
}


Partida::~Partida() {

    pthread_mutex_destroy(&mutexTablero);
}


int Partida::obtenerRetardo(unsigned int& seed) {

    int rango = retardoMax - retardoMin + 1;

    return retardoMin + (rand_r(&seed) % rango);
}


void Partida::revisarFinPartida() {

    if (juego.victoriaPuma()) {

        cout << "\nEl Puma gana la partida.\n";
        terminada = true;

        return;
    }


    if (juego.victoriaPerros()) {

        cout << "\nLos Perros ganan la partida.\n";
        terminada = true;
    }
}


bool Partida::moverPerroAleatorio(
    int indice,
    unsigned int& seed
) {

    if (indice < 0 ||
        indice >= static_cast<int>(posicionesPerros.size())) {

        return false;
    }


    int origen = posicionesPerros[indice];

    if (origen < 0) {
        return false;
    }


    Tablero& tablero = juego.obtenerTablero();


    if (tablero.obtenerFicha(origen) != 'D') {

        posicionesPerros[indice] = -1;
        return false;
    }


    vector<int> destinos;


    for (int destino = 0;
         destino < Tablero::TOTAL_CASILLAS;
         destino++) {

        if (tablero.estaLibre(destino) &&
            tablero.sonAdyacentes(origen, destino)) {

            destinos.push_back(destino);
        }
    }


    if (destinos.empty()) {
        return false;
    }


    int elegido = rand_r(&seed) % destinos.size();

    int destino = destinos[elegido];


    if (!juego.moverPerro(origen, destino)) {
        return false;
    }


    posicionesPerros[indice] = destino;

    cout << "Perro " << indice + 1
         << ": " << origen
         << " -> " << destino
         << "\n";

    return true;
}


bool Partida::moverPumaAleatorio(unsigned int& seed) {

    Tablero& tablero = juego.obtenerTablero();


    struct Movimiento {
        bool captura;
        int perro;
        int destino;
    };


    vector<Movimiento> movimientos;


    for (int destino = 0;
         destino < Tablero::TOTAL_CASILLAS;
         destino++) {

        if (tablero.estaLibre(destino) &&
            tablero.sonAdyacentes(posicionPuma, destino)) {

            movimientos.push_back({
                false,
                -1,
                destino
            });
        }
    }


    for (int perro = 0;
         perro < Tablero::TOTAL_CASILLAS;
         perro++) {

        if (tablero.obtenerFicha(perro) != 'D') {
            continue;
        }


        for (int destino = 0;
             destino < Tablero::TOTAL_CASILLAS;
             destino++) {

            if (juego.puedeCapturarPerro(
                    posicionPuma,
                    perro,
                    destino)) {

                movimientos.push_back({
                    true,
                    perro,
                    destino
                });
            }
        }
    }


    if (movimientos.empty()) {
        return false;
    }


    int elegido = rand_r(&seed) % movimientos.size();

    Movimiento movimiento = movimientos[elegido];

    int origen = posicionPuma;


    if (movimiento.captura) {

        int indiceCapturado = -1;

        for (int i = 0;
             i < static_cast<int>(posicionesPerros.size());
             i++) {

            if (posicionesPerros[i] == movimiento.perro) {

                indiceCapturado = i;
                break;
            }
        }


        if (!juego.capturarPerro(
                origen,
                movimiento.perro,
                movimiento.destino)) {

            return false;
        }


        if (indiceCapturado != -1) {
            posicionesPerros[indiceCapturado] = -1;
        }


        posicionPuma = movimiento.destino;


        cout << "Puma: "
             << origen
             << " -> "
             << movimiento.destino
             << " captura Perro en "
             << movimiento.perro
             << "\n";

    } else {

        if (!juego.moverPuma(
                origen,
                movimiento.destino)) {

            return false;
        }


        posicionPuma = movimiento.destino;


        cout << "Puma: "
             << origen
             << " -> "
             << movimiento.destino
             << "\n";
    }


    return true;
}


void* Partida::ejecutarPuma(void* datos) {

    DatosHilo* info = static_cast<DatosHilo*>(datos);

    Partida* partida = info->partida;

    unsigned int seed = info->seed;


    while (true) {

        int retardo = partida->obtenerRetardo(seed);

        usleep(retardo * 1000);


        pthread_mutex_lock(&partida->mutexTablero);


        if (partida->terminada) {

            pthread_mutex_unlock(&partida->mutexTablero);
            break;
        }


        bool movimiento = partida->moverPumaAleatorio(seed);


        if (movimiento) {

            partida->juego.obtenerTablero().mostrar();

            partida->revisarFinPartida();
        }


        pthread_mutex_unlock(&partida->mutexTablero);
    }


    return nullptr;
}


void* Partida::ejecutarPerro(void* datos) {

    DatosHilo* info = static_cast<DatosHilo*>(datos);

    Partida* partida = info->partida;

    int indice = info->indice;

    unsigned int seed = info->seed;


    while (true) {

        int retardo = partida->obtenerRetardo(seed);

        usleep(retardo * 1000);


        pthread_mutex_lock(&partida->mutexTablero);


        if (partida->terminada) {

            pthread_mutex_unlock(&partida->mutexTablero);
            break;
        }


        if (partida->posicionesPerros[indice] == -1) {

            pthread_mutex_unlock(&partida->mutexTablero);
            break;
        }


        bool movimiento =
            partida->moverPerroAleatorio(indice, seed);


        if (movimiento) {

            partida->juego.obtenerTablero().mostrar();

            partida->revisarFinPartida();
        }


        pthread_mutex_unlock(&partida->mutexTablero);
    }


    return nullptr;
}


void Partida::iniciar() {

    pthread_t hiloPuma;

    vector<pthread_t> hilosPerros(
        posicionesPerros.size()
    );


    DatosHilo datosPuma;

    datosPuma.partida = this;
    datosPuma.indice = -1;
    datosPuma.seed = 100;


    vector<DatosHilo> datosPerros(
        posicionesPerros.size()
    );


    cout << "\nEstado inicial de la partida:\n";

    juego.obtenerTablero().mostrar();


    pthread_create(
        &hiloPuma,
        nullptr,
        ejecutarPuma,
        &datosPuma
    );


    for (int i = 0;
         i < static_cast<int>(posicionesPerros.size());
         i++) {

        datosPerros[i].partida = this;
        datosPerros[i].indice = i;
        datosPerros[i].seed = 200 + i;


        pthread_create(
            &hilosPerros[i],
            nullptr,
            ejecutarPerro,
            &datosPerros[i]
        );
    }


    pthread_join(hiloPuma, nullptr);


    for (pthread_t hilo : hilosPerros) {
        pthread_join(hilo, nullptr);
    }


    cout << "\nPartida finalizada.\n";
}