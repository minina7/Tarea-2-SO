#include <iostream>
#include <pthread.h>
#include <vector>
#include <unistd.h>

#include "partida.hpp"

using namespace std;

struct DatosPrueba {
    Partida* partida;
};

void* ejecutarPartidaPrueba(void* datos) {
    DatosPrueba* info =
        static_cast<DatosPrueba*>(datos);

    info->partida->iniciar();

    return nullptr;
}

bool probarAbandonoPuma() {
    vector<int> perros = {
        0, 1, 2, 3, 4,
        5, 6, 7, 8, 9,
        11, 13
    };

    Partida partida(
        12,
        perros,
        500,
        500,
        12345
    );

    DatosPrueba datos;

    datos.partida = &partida;

    pthread_t hilo;

    if (
        pthread_create(
            &hilo,
            nullptr,
            ejecutarPartidaPrueba,
            &datos
        ) != 0
    ) {
        return false;
    }

    usleep(100000);

    partida.reportarAbandonoPuma();

    pthread_join(
        hilo,
        nullptr
    );

    return
        partida.obtenerGanador() == 2 &&
        partida.terminoPorAbandono();
}

bool probarAbandonoPerros() {
    vector<int> perros = {
        0, 1, 2, 3, 4,
        5, 6, 7, 8, 9,
        11, 13
    };

    Partida partida(
        12,
        perros,
        500,
        500,
        54321
    );

    DatosPrueba datos;

    datos.partida = &partida;

    pthread_t hilo;

    if (
        pthread_create(
            &hilo,
            nullptr,
            ejecutarPartidaPrueba,
            &datos
        ) != 0
    ) {
        return false;
    }

    usleep(100000);

    partida.reportarAbandonoPerros();

    pthread_join(
        hilo,
        nullptr
    );

    return
        partida.obtenerGanador() == 1 &&
        partida.terminoPorAbandono();
}

int main() {
    cout << "Prueba de abandono\n";

    if (!probarAbandonoPuma()) {
        cout
            << "ERROR: no se manejo correctamente el abandono del Puma.\n";

        return 1;
    }

    cout
        << "OK: abandono del Puma detectado.\n";

    if (!probarAbandonoPerros()) {
        cout
            << "ERROR: no se manejo correctamente el abandono de los Perros.\n";

        return 1;
    }

    cout
        << "OK: abandono de los Perros detectado.\n";

    cout
        << "test_abandono finalizado correctamente.\n";

    return 0;
}