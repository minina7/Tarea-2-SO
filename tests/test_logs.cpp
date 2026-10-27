#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>

#include "partida.hpp"

using namespace std;

struct DatosLog {
    Partida* partida;
};

void* ejecutarPartidaLog(
    void* datos
) {
    DatosLog* info =
        static_cast<DatosLog*>(
            datos
        );

    info->partida->iniciar();

    return nullptr;
}

int main() {
    cout
        << "Prueba de logs\n";

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
        77777,
        9999
    );

    DatosLog datos;

    datos.partida =
        &partida;

    pthread_t hilo;

    if (
        pthread_create(
            &hilo,
            nullptr,
            ejecutarPartidaLog,
            &datos
        ) != 0
    ) {
        cout
            << "ERROR: no se pudo iniciar la partida.\n";

        return 1;
    }

    usleep(
        100000
    );

    partida.reportarAbandonoPuma();

    pthread_join(
        hilo,
        nullptr
    );

    ifstream archivo(
        "logs/partida_9999.txt"
    );

    if (!archivo.is_open()) {
        cout
            << "ERROR: no se creo el archivo de log.\n";

        return 1;
    }

    string linea;

    bool encontroInicio = false;
    bool encontroAbandono = false;
    bool encontroGanador = false;
    bool encontroFinal = false;

    while (
        getline(
            archivo,
            linea
        )
    ) {
        if (
            linea.find(
                "Partida 9999 iniciada"
            ) != string::npos
        ) {
            encontroInicio = true;
        }

        if (
            linea.find(
                "Abandono reportado: Puma"
            ) != string::npos
        ) {
            encontroAbandono = true;
        }

        if (
            linea.find(
                "Los Perros ganan por abandono del Puma"
            ) != string::npos
        ) {
            encontroGanador = true;
        }

        if (
            linea.find(
                "Partida finalizada"
            ) != string::npos
        ) {
            encontroFinal = true;
        }
    }

    archivo.close();

    if (
        !encontroInicio ||
        !encontroAbandono ||
        !encontroGanador ||
        !encontroFinal
    ) {
        cout
            << "ERROR: el log no contiene todos los eventos esperados.\n";

        return 1;
    }

    cout
        << "OK: archivo de log creado correctamente.\n";

    cout
        << "OK: eventos principales registrados correctamente.\n";

    cout
        << "test_logs finalizado correctamente.\n";

    return 0;
}