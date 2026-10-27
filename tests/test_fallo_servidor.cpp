#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "servidor.hpp"
#include "configuracion.hpp"

using namespace std;

struct DatosServidor {
    Servidor* servidor;
};

void* ejecutarServidorPrueba(
    void* datos
) {
    DatosServidor* info =
        static_cast<DatosServidor*>(
            datos
        );

    info->servidor->iniciar();

    return nullptr;
}

int esperarPartidaActiva(
    Servidor& servidor,
    int diferenteDe
) {
    for (
        int intento = 0;
        intento < 500;
        intento++
    ) {
        int activa =
            servidor.obtenerPartidaActiva();

        if (
            activa != 0 &&
            activa != diferenteDe
        ) {
            return activa;
        }

        usleep(
            10000
        );
    }

    return 0;
}

int main() {
    cout
        << "Prueba integrada de fallo y liberacion de mesa\n";

    Configuracion config;

    config.retardoMin = 500;
    config.retardoMax = 500;
    config.semilla = 12345;

    Servidor servidor(
        4,
        1,
        config
    );

    DatosServidor datos;

    datos.servidor =
        &servidor;

    pthread_t hiloServidor;

    if (
        pthread_create(
            &hiloServidor,
            nullptr,
            ejecutarServidorPrueba,
            &datos
        ) != 0
    ) {
        cout
            << "ERROR: no se pudo iniciar el servidor.\n";

        return 1;
    }

    int primeraPartida =
        esperarPartidaActiva(
            servidor,
            0
        );

    if (
        primeraPartida == 0
    ) {
        cout
            << "ERROR: ninguna partida obtuvo la mesa.\n";

        return 1;
    }

    cout
        << "OK: partida "
        << primeraPartida
        << " obtuvo la unica mesa.\n";

    if (
        !servidor.reportarFalloPuma(
            primeraPartida
        )
    ) {
        cout
            << "ERROR: el servidor no pudo reportar el fallo del Puma.\n";

        return 1;
    }

    int segundaPartida =
        esperarPartidaActiva(
            servidor,
            primeraPartida
        );

    if (
        segundaPartida == 0
    ) {
        cout
            << "ERROR: la mesa no fue reutilizada despues del fallo.\n";

        return 1;
    }

    cout
        << "OK: partida "
        << segundaPartida
        << " reutilizo la mesa liberada.\n";

    if (
        !servidor.reportarFalloPerros(
            segundaPartida
        )
    ) {
        cout
            << "ERROR: el servidor no pudo reportar el fallo de los Perros.\n";

        return 1;
    }

    pthread_join(
        hiloServidor,
        nullptr
    );

    if (
        servidor.obtenerMesasActivas()
        != 0
    ) {
        cout
            << "ERROR: quedaron mesas activas al finalizar.\n";

        return 1;
    }

    cout
        << "OK: el Juez manejo los fallos reportados por el servidor.\n";

    cout
        << "OK: la mesa fue liberada y reutilizada correctamente.\n";

    cout
        << "OK: no quedaron mesas activas.\n";

    cout
        << "test_fallo_servidor finalizado correctamente.\n";

    return 0;
}