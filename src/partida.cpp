#include "partida.hpp"

#include <iostream>
#include <cstdlib>
#include <unistd.h>

using namespace std;

Partida::Partida(
    int posicionInicialPuma,
    const vector<int>& posicionesInicialesPerros,
    int retardoMinimo,
    int retardoMaximo,
    unsigned int semilla
) {
    posicionPuma = posicionInicialPuma;
    posicionesPerros = posicionesInicialesPerros;

    retardoMin = retardoMinimo;
    retardoMax = retardoMaximo;

    semillaBase = semilla;

    terminada = false;
    estadoPendiente = true;

    abandonoPuma = false;
    abandonoPerros = false;

    ganador = 0;
    finalizoPorAbandono = false;

    pthread_mutex_init(&mutexTablero, nullptr);
    pthread_cond_init(&condEstado, nullptr);

    juego.obtenerTablero().colocarFicha(
        posicionPuma,
        'P'
    );

    for (int posicion : posicionesPerros) {
        juego.obtenerTablero().colocarFicha(
            posicion,
            'D'
        );
    }
}

Partida::~Partida() {
    pthread_cond_destroy(&condEstado);
    pthread_mutex_destroy(&mutexTablero);
}

int Partida::obtenerRetardo(
    unsigned int& seed
) {
    int diferencia =
        retardoMax - retardoMin + 1;

    return retardoMin +
        static_cast<int>(
            rand_r(&seed) % diferencia
        );
}

void Partida::reportarAbandonoPuma() {
    pthread_mutex_lock(&mutexTablero);

    if (!terminada) {
        abandonoPuma = true;
        estadoPendiente = true;

        pthread_cond_broadcast(
            &condEstado
        );
    }

    pthread_mutex_unlock(&mutexTablero);
}

void Partida::reportarAbandonoPerros() {
    pthread_mutex_lock(&mutexTablero);

    if (!terminada) {
        abandonoPerros = true;
        estadoPendiente = true;

        pthread_cond_broadcast(
            &condEstado
        );
    }

    pthread_mutex_unlock(&mutexTablero);
}

int Partida::obtenerGanador() {
    pthread_mutex_lock(&mutexTablero);

    int resultado = ganador;

    pthread_mutex_unlock(&mutexTablero);

    return resultado;
}

bool Partida::terminoPorAbandono() {
    pthread_mutex_lock(&mutexTablero);

    bool resultado =
        finalizoPorAbandono;

    pthread_mutex_unlock(&mutexTablero);

    return resultado;
}

bool Partida::moverPerroAleatorio(
    int indice,
    unsigned int& seed
) {
    if (
        indice < 0 ||
        indice >= static_cast<int>(
            posicionesPerros.size()
        )
    ) {
        return false;
    }

    int origen =
        posicionesPerros[indice];

    if (origen == -1) {
        return false;
    }

    if (
        juego.obtenerTablero()
                .obtenerFicha(origen) != 'D'
    ) {
        posicionesPerros[indice] = -1;
        return false;
    }

    vector<int> destinos;

    for (
        int destino = 0;
        destino < Tablero::TOTAL_CASILLAS;
        destino++
    ) {
        if (
            juego.obtenerTablero()
                    .estaLibre(destino) &&
            juego.obtenerTablero()
                    .sonAdyacentes(
                        origen,
                        destino
                    )
        ) {
            destinos.push_back(destino);
        }
    }

    if (destinos.empty()) {
        return false;
    }

    int indiceDestino =
        rand_r(&seed) % destinos.size();

    int destino =
        destinos[indiceDestino];

    if (!juego.moverPerro(origen, destino)) {
        return false;
    }

    posicionesPerros[indice] = destino;

    cout << "Perro "
         << indice + 1
         << ": "
         << origen
         << " -> "
         << destino
         << "\n";

    return true;
}

bool Partida::moverPumaAleatorio(
    unsigned int& seed
) {
    struct Captura {
        int perro;
        int destino;
    };

    vector<int> movimientos;
    vector<Captura> capturas;

    for (
        int destino = 0;
        destino < Tablero::TOTAL_CASILLAS;
        destino++
    ) {
        if (
            juego.obtenerTablero()
                    .estaLibre(destino) &&
            juego.obtenerTablero()
                    .sonAdyacentes(
                        posicionPuma,
                        destino
                    )
        ) {
            movimientos.push_back(destino);
        }
    }

    for (
        int perro = 0;
        perro < Tablero::TOTAL_CASILLAS;
        perro++
    ) {
        if (
            juego.obtenerTablero()
                    .obtenerFicha(perro) != 'D'
        ) {
            continue;
        }

        for (
            int destino = 0;
            destino < Tablero::TOTAL_CASILLAS;
            destino++
        ) {
            if (
                juego.puedeCapturarPerro(
                    posicionPuma,
                    perro,
                    destino
                )
            ) {
                capturas.push_back(
                    {perro, destino}
                );
            }
        }
    }

    int totalOpciones =
        movimientos.size() +
        capturas.size();

    if (totalOpciones == 0) {
        return false;
    }

    int opcion =
        rand_r(&seed) % totalOpciones;

    if (
        opcion <
        static_cast<int>(capturas.size())
    ) {
        Captura captura =
            capturas[opcion];

        int origen =
            posicionPuma;

        if (
            !juego.capturarPerro(
                origen,
                captura.perro,
                captura.destino
            )
        ) {
            return false;
        }

        posicionPuma =
            captura.destino;

        for (
            int i = 0;
            i < static_cast<int>(
                posicionesPerros.size()
            );
            i++
        ) {
            if (
                posicionesPerros[i] ==
                captura.perro
            ) {
                posicionesPerros[i] = -1;
                break;
            }
        }

        cout << "Puma: "
             << origen
             << " -> "
             << captura.destino
             << " captura Perro en "
             << captura.perro
             << "\n";

        return true;
    }

    int indiceMovimiento =
        opcion -
        static_cast<int>(
            capturas.size()
        );

    int destino =
        movimientos[indiceMovimiento];

    int origen =
        posicionPuma;

    if (
        !juego.moverPuma(
            origen,
            destino
        )
    ) {
        return false;
    }

    posicionPuma = destino;

    cout << "Puma: "
         << origen
         << " -> "
         << destino
         << "\n";

    return true;
}

void* Partida::ejecutarJuez(
    void* datos
) {
    Partida* partida =
        static_cast<Partida*>(datos);

    pthread_mutex_lock(
        &partida->mutexTablero
    );

    while (!partida->terminada) {
        while (
            !partida->estadoPendiente &&
            !partida->terminada
        ) {
            pthread_cond_wait(
                &partida->condEstado,
                &partida->mutexTablero
            );
        }

        if (partida->terminada) {
            break;
        }

        if (partida->abandonoPuma) {
            cout
                << "\nJuez: Los Perros ganan por abandono del Puma.\n";

            partida->ganador = 2;
            partida->finalizoPorAbandono = true;
            partida->terminada = true;
        }
        else if (partida->abandonoPerros) {
            cout
                << "\nJuez: El Puma gana por abandono de los Perros.\n";

            partida->ganador = 1;
            partida->finalizoPorAbandono = true;
            partida->terminada = true;
        }
        else if (
            partida->juego.victoriaPuma()
        ) {
            cout
                << "\nJuez: El Puma gana la partida.\n";

            partida->ganador = 1;
            partida->terminada = true;
        }
        else if (
            partida->juego.victoriaPerros()
        ) {
            cout
                << "\nJuez: Los Perros ganan la partida.\n";

            partida->ganador = 2;
            partida->terminada = true;
        }

        partida->estadoPendiente = false;

        pthread_cond_broadcast(
            &partida->condEstado
        );
    }

    pthread_cond_broadcast(
        &partida->condEstado
    );

    pthread_mutex_unlock(
        &partida->mutexTablero
    );

    return nullptr;
}

void* Partida::ejecutarPuma(
    void* datos
) {
    DatosHilo* info =
        static_cast<DatosHilo*>(datos);

    Partida* partida =
        info->partida;

    unsigned int seed =
        info->seed;

    while (true) {
        int retardo =
            partida->obtenerRetardo(seed);

        usleep(retardo * 1000);

        pthread_mutex_lock(
            &partida->mutexTablero
        );

        while (
            partida->estadoPendiente &&
            !partida->terminada
        ) {
            pthread_cond_wait(
                &partida->condEstado,
                &partida->mutexTablero
            );
        }

        if (partida->terminada) {
            pthread_mutex_unlock(
                &partida->mutexTablero
            );

            break;
        }

        bool movio =
            partida->moverPumaAleatorio(
                seed
            );

        if (movio) {
            partida->juego
                .obtenerTablero()
                .mostrar();

            partida->estadoPendiente =
                true;

            pthread_cond_broadcast(
                &partida->condEstado
            );
        }

        pthread_mutex_unlock(
            &partida->mutexTablero
        );
    }

    return nullptr;
}

void* Partida::ejecutarPerro(
    void* datos
) {
    DatosHilo* info =
        static_cast<DatosHilo*>(datos);

    Partida* partida =
        info->partida;

    int indice =
        info->indice;

    unsigned int seed =
        info->seed;

    while (true) {
        int retardo =
            partida->obtenerRetardo(seed);

        usleep(retardo * 1000);

        pthread_mutex_lock(
            &partida->mutexTablero
        );

        while (
            partida->estadoPendiente &&
            !partida->terminada
        ) {
            pthread_cond_wait(
                &partida->condEstado,
                &partida->mutexTablero
            );
        }

        if (partida->terminada) {
            pthread_mutex_unlock(
                &partida->mutexTablero
            );

            break;
        }

        if (
            partida->posicionesPerros[indice]
            == -1
        ) {
            pthread_mutex_unlock(
                &partida->mutexTablero
            );

            break;
        }

        bool movio =
            partida->moverPerroAleatorio(
                indice,
                seed
            );

        if (movio) {
            partida->juego
                .obtenerTablero()
                .mostrar();

            partida->estadoPendiente =
                true;

            pthread_cond_broadcast(
                &partida->condEstado
            );
        }

        pthread_mutex_unlock(
            &partida->mutexTablero
        );
    }

    return nullptr;
}

void Partida::iniciar() {
    pthread_t hiloPuma;
    pthread_t hiloJuez;

    vector<pthread_t> hilosPerros(
        posicionesPerros.size()
    );

    vector<bool> perrosCreados(
        posicionesPerros.size(),
        false
    );

    DatosHilo datosPuma;

    vector<DatosHilo> datosPerros(
        posicionesPerros.size()
    );

    datosPuma.partida = this;
    datosPuma.indice = -1;
    datosPuma.seed =
        semillaBase + 100;

    cout
        << "\nEstado inicial de la partida:\n";

    juego.obtenerTablero().mostrar();

    int resultadoJuez =
        pthread_create(
            &hiloJuez,
            nullptr,
            ejecutarJuez,
            this
        );

    if (resultadoJuez != 0) {
        cout
            << "Error: no se pudo crear el Juez.\n";

        terminada = true;

        return;
    }

    int resultadoPuma =
        pthread_create(
            &hiloPuma,
            nullptr,
            ejecutarPuma,
            &datosPuma
        );

    bool pumaCreado =
        resultadoPuma == 0;

    if (!pumaCreado) {
        cout
            << "Fallo interno del jugador Puma.\n";

        reportarAbandonoPuma();
    }

    if (pumaCreado) {
        for (
            int i = 0;
            i < static_cast<int>(
                posicionesPerros.size()
            );
            i++
        ) {
            datosPerros[i].partida = this;
            datosPerros[i].indice = i;

            datosPerros[i].seed =
                semillaBase + 200 + i;

            int resultado =
                pthread_create(
                    &hilosPerros[i],
                    nullptr,
                    ejecutarPerro,
                    &datosPerros[i]
                );

            if (resultado != 0) {
                cout
                    << "Fallo interno del jugador Perros.\n";

                reportarAbandonoPerros();

                break;
            }

            perrosCreados[i] = true;
        }
    }

    pthread_join(
        hiloJuez,
        nullptr
    );

    if (pumaCreado) {
        pthread_join(
            hiloPuma,
            nullptr
        );
    }

    for (
        int i = 0;
        i < static_cast<int>(
            hilosPerros.size()
        );
        i++
    ) {
        if (perrosCreados[i]) {
            pthread_join(
                hilosPerros[i],
                nullptr
            );
        }
    }

    cout << "\nPartida finalizada.\n";
}