#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>
#include <unistd.h>

#include "juego.hpp"

using namespace std;

struct DatosEstres {
    Juego* juego;
    pthread_mutex_t* mutex;
    int indice;
    unsigned int seed;
};

void* ejecutarEstres(void* datos) {
    DatosEstres* info =
        static_cast<DatosEstres*>(datos);

    for (int i = 0; i < 1000; i++) {
        int origen =
            rand_r(&info->seed) %
            Tablero::TOTAL_CASILLAS;

        int destino =
            rand_r(&info->seed) %
            Tablero::TOTAL_CASILLAS;

        pthread_mutex_lock(
            info->mutex
        );

        if (
            info->juego
                ->obtenerTablero()
                .obtenerFicha(origen) == 'D'
        ) {
            info->juego->moverPerro(
                origen,
                destino
            );
        }

        pthread_mutex_unlock(
            info->mutex
        );

        usleep(100);
    }

    return nullptr;
}

int main() {
    cout << "Prueba de estres\n";

    Juego juego;

    Tablero& tablero =
        juego.obtenerTablero();

    vector<int> perros = {
        0, 1, 2, 3, 4,
        5, 6, 7, 8, 9,
        11, 13
    };

    for (int posicion : perros) {
        if (!tablero.colocarFicha(posicion, 'D')) {
            cout << "ERROR: no se pudo preparar el tablero.\n";
            return 1;
        }
    }

    pthread_mutex_t mutex;

    pthread_mutex_init(
        &mutex,
        nullptr
    );

    const int cantidadHilos = 12;

    vector<pthread_t> hilos(
        cantidadHilos
    );

    vector<DatosEstres> datos(
        cantidadHilos
    );

    for (int i = 0; i < cantidadHilos; i++) {
        datos[i].juego = &juego;
        datos[i].mutex = &mutex;
        datos[i].indice = i;
        datos[i].seed =
            1000 + i;

        pthread_create(
            &hilos[i],
            nullptr,
            ejecutarEstres,
            &datos[i]
        );
    }

    for (pthread_t hilo : hilos) {
        pthread_join(
            hilo,
            nullptr
        );
    }

    pthread_mutex_destroy(
        &mutex
    );

    int cantidadPerros = 0;

    for (
        int i = 0;
        i < Tablero::TOTAL_CASILLAS;
        i++
    ) {
        if (
            tablero.obtenerFicha(i) == 'D'
        ) {
            cantidadPerros++;
        }
    }

    if (cantidadPerros != 12) {
        cout << "ERROR: cantidad de Perros incorrecta: "
             << cantidadPerros
             << "\n";

        return 1;
    }

    cout << "OK: se mantienen los 12 Perros despues del estres.\n";
    cout << "test_estres finalizado correctamente.\n";

    return 0;
}