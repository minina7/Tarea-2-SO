#include "partida.hpp"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>

#include "senales.hpp"

using namespace std;

Partida::Partida(
    int posicionInicialPuma,
    const vector<int>& posicionesInicialesPerros,
    int retardoMinimo,
    int retardoMaximo,
    unsigned int semilla,
    int numero
) {
    posicionPuma =
        posicionInicialPuma;

    posicionesPerros =
        posicionesInicialesPerros;

    retardoMin =
        retardoMinimo;

    retardoMax =
        retardoMaximo;

    semillaBase =
        semilla;

    numeroPartida =
        numero;

    terminada = false;
    estadoPendiente = true;

    abandonoPuma = false;
    abandonoPerros = false;

    interrupcionServidor = false;

    ganador = 0;

    finalizoPorAbandono = false;

    pthread_mutex_init(
        &mutexTablero,
        nullptr
    );

    pthread_cond_init(
        &condEstado,
        nullptr
    );

    mkdir(
        "logs",
        0755
    );

    string ruta =
        "logs/partida_" +
        to_string(numeroPartida) +
        ".txt";

    archivoLog.open(
        ruta,
        ios::out
    );

    registrarLog(
        "Partida " +
        to_string(numeroPartida) +
        " iniciada"
    );

    registrarLog(
        "Semilla: " +
        to_string(semillaBase)
    );

    registrarLog(
        "Puma inicial: " +
        to_string(posicionPuma)
    );

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
    if (archivoLog.is_open()) {
        archivoLog.close();
    }

    pthread_cond_destroy(
        &condEstado
    );

    pthread_mutex_destroy(
        &mutexTablero
    );
}

void Partida::registrarLog(
    const string& mensaje
) {
    if (archivoLog.is_open()) {
        archivoLog
            << mensaje
            << endl;
    }
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
    pthread_mutex_lock(
        &mutexTablero
    );

    if (
        !terminada &&
        !abandonoPuma
    ) {
        abandonoPuma = true;
        estadoPendiente = true;

        registrarLog(
            "Abandono reportado: Puma"
        );

        pthread_cond_broadcast(
            &condEstado
        );
    }

    pthread_mutex_unlock(
        &mutexTablero
    );
}

void Partida::reportarAbandonoPerros() {
    pthread_mutex_lock(
        &mutexTablero
    );

    if (
        !terminada &&
        !abandonoPerros
    ) {
        abandonoPerros = true;
        estadoPendiente = true;

        registrarLog(
            "Abandono reportado: Perros"
        );

        pthread_cond_broadcast(
            &condEstado
        );
    }

    pthread_mutex_unlock(
        &mutexTablero
    );
}

void Partida::solicitarDetencion() {
    pthread_mutex_lock(
        &mutexTablero
    );

    if (
        !terminada &&
        !interrupcionServidor
    ) {
        interrupcionServidor = true;
        estadoPendiente = true;

        registrarLog(
            "Interrupcion SIGINT solicitada"
        );

        pthread_cond_broadcast(
            &condEstado
        );
    }

    pthread_mutex_unlock(
        &mutexTablero
    );
}

int Partida::obtenerGanador() {
    pthread_mutex_lock(
        &mutexTablero
    );

    int resultado =
        ganador;

    pthread_mutex_unlock(
        &mutexTablero
    );

    return resultado;
}

bool Partida::terminoPorAbandono() {
    pthread_mutex_lock(
        &mutexTablero
    );

    bool resultado =
        finalizoPorAbandono;

    pthread_mutex_unlock(
        &mutexTablero
    );

    return resultado;
}

bool Partida::fueInterrumpida() {
    pthread_mutex_lock(
        &mutexTablero
    );

    bool resultado =
        interrupcionServidor;

    pthread_mutex_unlock(
        &mutexTablero
    );

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
        posicionesPerros[indice] =
            -1;

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
            destinos.push_back(
                destino
            );
        }
    }

    if (destinos.empty()) {
        return false;
    }

    int indiceDestino =
        rand_r(&seed) %
        destinos.size();

    int destino =
        destinos[indiceDestino];

    if (
        !juego.moverPerro(
            origen,
            destino
        )
    ) {
        return false;
    }

    posicionesPerros[indice] =
        destino;

    cout
        << "Perro "
        << indice + 1
        << ": "
        << origen
        << " -> "
        << destino
        << "\n";

    registrarLog(
        "Perro " +
        to_string(indice + 1) +
        ": " +
        to_string(origen) +
        " -> " +
        to_string(destino)
    );

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
            movimientos.push_back(
                destino
            );
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
                    {
                        perro,
                        destino
                    }
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
        rand_r(&seed) %
        totalOpciones;

    if (
        opcion <
        static_cast<int>(
            capturas.size()
        )
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
                posicionesPerros[i] =
                    -1;

                break;
            }
        }

        cout
            << "Puma: "
            << origen
            << " -> "
            << captura.destino
            << " captura Perro en "
            << captura.perro
            << "\n";

        registrarLog(
            "Puma: " +
            to_string(origen) +
            " -> " +
            to_string(captura.destino) +
            " captura Perro en " +
            to_string(captura.perro)
        );

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

    posicionPuma =
        destino;

    cout
        << "Puma: "
        << origen
        << " -> "
        << destino
        << "\n";

    registrarLog(
        "Puma: " +
        to_string(origen) +
        " -> " +
        to_string(destino)
    );

    return true;
}

void* Partida::ejecutarJuez(
    void* datos
) {
    Partida* partida =
        static_cast<Partida*>(
            datos
        );

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

        if (
            partida->interrupcionServidor
        ) {
            cout
                << "\nJuez: partida detenida por SIGINT.\n";

            partida->registrarLog(
                "Juez: partida detenida por SIGINT"
            );

            partida->ganador = 0;
            partida->terminada = true;
        }
        else if (
            partida->abandonoPuma
        ) {
            cout
                << "\nJuez: Los Perros ganan por abandono del Puma.\n";

            partida->registrarLog(
                "Juez: Los Perros ganan por abandono del Puma"
            );

            partida->ganador = 2;

            partida->finalizoPorAbandono =
                true;

            partida->terminada = true;
        }
        else if (
            partida->abandonoPerros
        ) {
            cout
                << "\nJuez: El Puma gana por abandono de los Perros.\n";

            partida->registrarLog(
                "Juez: El Puma gana por abandono de los Perros"
            );

            partida->ganador = 1;

            partida->finalizoPorAbandono =
                true;

            partida->terminada = true;
        }
        else if (
            partida->juego.victoriaPuma()
        ) {
            cout
                << "\nJuez: El Puma gana la partida.\n";

            partida->registrarLog(
                "Juez: El Puma gana la partida"
            );

            partida->ganador = 1;
            partida->terminada = true;
        }
        else if (
            partida->juego.victoriaPerros()
        ) {
            cout
                << "\nJuez: Los Perros ganan la partida.\n";

            partida->registrarLog(
                "Juez: Los Perros ganan la partida"
            );

            partida->ganador = 2;
            partida->terminada = true;
        }

        partida->estadoPendiente =
            false;

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
        static_cast<DatosHilo*>(
            datos
        );

    Partida* partida =
        info->partida;

    unsigned int seed =
        info->seed;

    while (true) {
        if (
            interrupcionSolicitada()
        ) {
            partida->solicitarDetencion();

            break;
        }

        int retardo =
            partida->obtenerRetardo(
                seed
            );

        usleep(
            retardo * 1000
        );

        if (
            interrupcionSolicitada()
        ) {
            partida->solicitarDetencion();

            break;
        }

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
            interrupcionSolicitada()
        ) {
            partida->interrupcionServidor =
                true;

            partida->estadoPendiente =
                true;

            partida->registrarLog(
                "Interrupcion SIGINT detectada por Puma"
            );

            pthread_cond_broadcast(
                &partida->condEstado
            );

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
        static_cast<DatosHilo*>(
            datos
        );

    Partida* partida =
        info->partida;

    int indice =
        info->indice;

    unsigned int seed =
        info->seed;

    while (true) {
        if (
            interrupcionSolicitada()
        ) {
            partida->solicitarDetencion();

            break;
        }

        int retardo =
            partida->obtenerRetardo(
                seed
            );

        usleep(
            retardo * 1000
        );

        if (
            interrupcionSolicitada()
        ) {
            partida->solicitarDetencion();

            break;
        }

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
            interrupcionSolicitada()
        ) {
            partida->interrupcionServidor =
                true;

            partida->estadoPendiente =
                true;

            partida->registrarLog(
                "Interrupcion SIGINT detectada por Perro"
            );

            pthread_cond_broadcast(
                &partida->condEstado
            );

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

    cout
        << "Log: logs/partida_"
        << numeroPartida
        << ".txt\n";

    juego.obtenerTablero().mostrar();

    registrarLog(
        "Threads de la partida iniciados"
    );

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

        registrarLog(
            "Error: no se pudo crear el Juez"
        );

        terminada = true;

        registrarLog(
            "Partida finalizada"
        );

        archivoLog.close();

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

        registrarLog(
            "Fallo interno del jugador Puma"
        );

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
            if (
                interrupcionSolicitada()
            ) {
                solicitarDetencion();

                break;
            }

            datosPerros[i].partida =
                this;

            datosPerros[i].indice =
                i;

            datosPerros[i].seed =
                semillaBase +
                200 +
                i;

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

                registrarLog(
                    "Fallo interno del jugador Perros"
                );

                reportarAbandonoPerros();

                break;
            }

            perrosCreados[i] =
                true;
        }
    }

    if (
        interrupcionSolicitada()
    ) {
        solicitarDetencion();
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
        if (
            perrosCreados[i]
        ) {
            pthread_join(
                hilosPerros[i],
                nullptr
            );
        }
    }

    registrarLog(
        "Partida finalizada"
    );

    if (ganador == 1) {
        registrarLog(
            "Ganador final: Puma"
        );
    }
    else if (ganador == 2) {
        registrarLog(
            "Ganador final: Perros"
        );
    }
    else {
        registrarLog(
            "Partida sin ganador"
        );
    }

    if (archivoLog.is_open()) {
        archivoLog.close();
    }

    cout
        << "\nPartida finalizada.\n";
}