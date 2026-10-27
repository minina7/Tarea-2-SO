#include <iostream>
#include <pthread.h>
#include <vector>
#include <unistd.h>

#include "juego.hpp"

using namespace std;

struct DatosColision {
    Juego* juego;
    pthread_mutex_t* mutex;
    int origen;
    int destino;
    int* exitos;
};

void* intentarMovimiento(void* datos) {
    DatosColision* info =
        static_cast<DatosColision*>(datos);

    usleep(10000);

    pthread_mutex_lock(info->mutex);

    bool movio =
        info->juego->moverPerro(
            info->origen,
            info->destino
        );

    if (movio) {
        (*info->exitos)++;
    }

    pthread_mutex_unlock(info->mutex);

    return nullptr;
}

int main() {
    cout << "Prueba de colision\n";

    Juego juego;

    Tablero& tablero =
        juego.obtenerTablero();

    vector<int> origenes = {
        7,
        11,
        13,
        17
    };

    for (int posicion : origenes) {
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

    vector<pthread_t> hilos(
        origenes.size()
    );

    vector<DatosColision> datos(
        origenes.size()
    );

    int exitos = 0;

    for (
        int i = 0;
        i < static_cast<int>(origenes.size());
        i++
    ) {
        datos[i].juego = &juego;
        datos[i].mutex = &mutex;
        datos[i].origen = origenes[i];
        datos[i].destino = 12;
        datos[i].exitos = &exitos;

        pthread_create(
            &hilos[i],
            nullptr,
            intentarMovimiento,
            &datos[i]
        );
    }

    for (pthread_t hilo : hilos) {
        pthread_join(
            hilo,
            nullptr
        );
    }

    int origenesVacios = 0;

    for (int posicion : origenes) {
        if (tablero.estaLibre(posicion)) {
            origenesVacios++;
        }
    }

    pthread_mutex_destroy(&mutex);

    if (exitos != 1) {
        cout << "ERROR: "
             << exitos
             << " Perros lograron ocupar la posicion 12.\n";

        return 1;
    }

    if (tablero.obtenerFicha(12) != 'D') {
        cout << "ERROR: la posicion 12 no contiene un Perro.\n";
        return 1;
    }

    if (origenesVacios != 1) {
        cout << "ERROR: el estado final del tablero es incorrecto.\n";
        return 1;
    }

    cout << "OK: solo un Perro logro ocupar la posicion 12.\n";

    cout << "test_colision finalizado correctamente.\n";

    return 0;
}