#include "servidor.hpp"

#include <iostream>
#include <cerrno>

#include "senales.hpp"

using namespace std;

Servidor::Servidor(
    int jugadores,
    int mesas,
    const Configuracion& config
)
    : cantidadJugadores(jugadores),
      maxMesas(mesas),
      configuracion(config),
      mesasActivas(0),
      siguientePartida(1) {

    int cantidadPartidas =
        cantidadJugadores / 2;

    partidasActivas.resize(
        cantidadPartidas + 1,
        nullptr
    );

    hilosPartidas.resize(
        cantidadPartidas
    );

    partidaCreada.resize(
        cantidadPartidas,
        0
    );

    datosPartidas.resize(
        cantidadPartidas
    );

    sem_init(
        &semaforoMesas,
        0,
        maxMesas
    );

    pthread_mutex_init(
        &mutexEstado,
        nullptr
    );

    pthread_mutex_init(
        &mutexEmparejamiento,
        nullptr
    );
}

Servidor::~Servidor() {
    pthread_mutex_destroy(
        &mutexEmparejamiento
    );

    pthread_mutex_destroy(
        &mutexEstado
    );

    sem_destroy(
        &semaforoMesas
    );
}

void* Servidor::ejecutarCliente(
    void* datos
) {
    DatosCliente* cliente =
        static_cast<DatosCliente*>(
            datos
        );

    if (
        interrupcionSolicitada()
    ) {
        return nullptr;
    }

    cliente->servidor->registrarCliente(
        cliente->jugador
    );

    return nullptr;
}

void Servidor::registrarCliente(
    int jugador
) {
    int jugadorPuma = -1;
    int jugadorPerros = -1;
    int numeroPartida = -1;

    pthread_mutex_lock(
        &mutexEmparejamiento
    );

    if (
        interrupcionSolicitada()
    ) {
        pthread_mutex_unlock(
            &mutexEmparejamiento
        );

        return;
    }

    cout
        << "Jugador "
        << jugador
        << " llego al servidor.\n";

    jugadoresEsperando.push_back(
        jugador
    );

    if (
        jugadoresEsperando.size() >= 2
    ) {
        jugadorPuma =
            jugadoresEsperando[0];

        jugadorPerros =
            jugadoresEsperando[1];

        jugadoresEsperando.erase(
            jugadoresEsperando.begin(),
            jugadoresEsperando.begin() + 2
        );

        numeroPartida =
            siguientePartida;

        siguientePartida++;

        cout
            << "Servidor: jugadores "
            << jugadorPuma
            << " y "
            << jugadorPerros
            << " emparejados en la partida "
            << numeroPartida
            << ".\n";
    }

    pthread_mutex_unlock(
        &mutexEmparejamiento
    );

    if (
        numeroPartida == -1
    ) {
        return;
    }

    int indice =
        numeroPartida - 1;

    datosPartidas[indice].servidor =
        this;

    datosPartidas[indice].jugadorPuma =
        jugadorPuma;

    datosPartidas[indice].jugadorPerros =
        jugadorPerros;

    datosPartidas[indice].numeroPartida =
        numeroPartida;

    int resultado =
        pthread_create(
            &hilosPartidas[indice],
            nullptr,
            ejecutarPartida,
            &datosPartidas[indice]
        );

    if (
        resultado != 0
    ) {
        cerr
            << "Error al crear la partida "
            << numeroPartida
            << ".\n";

        return;
    }

    partidaCreada[indice] = 1;
}

void* Servidor::ejecutarPartida(
    void* datos
) {
    DatosPartida* datosPartida =
        static_cast<DatosPartida*>(
            datos
        );

    Servidor* servidor =
        datosPartida->servidor;

    while (
        sem_wait(
            &servidor->semaforoMesas
        ) == -1
    ) {
        if (
            errno == EINTR
        ) {
            if (
                interrupcionSolicitada()
            ) {
                return nullptr;
            }

            continue;
        }

        return nullptr;
    }

    if (
        interrupcionSolicitada()
    ) {
        sem_post(
            &servidor->semaforoMesas
        );

        return nullptr;
    }

    unsigned int semillaPartida =
        servidor->configuracion.semilla +
        datosPartida->numeroPartida * 1000;

    Partida partida(
        servidor->configuracion.posicionPuma,
        servidor->configuracion.posicionesPerros,
        servidor->configuracion.retardoMin,
        servidor->configuracion.retardoMax,
        semillaPartida,
        datosPartida->numeroPartida
    );

    pthread_mutex_lock(
        &servidor->mutexEstado
    );

    servidor->mesasActivas++;

    servidor->partidasActivas[
        datosPartida->numeroPartida
    ] = &partida;

    cout
        << "\nPartida "
        << datosPartida->numeroPartida
        << " comienza | Puma: jugador "
        << datosPartida->jugadorPuma
        << " | Perros: jugador "
        << datosPartida->jugadorPerros
        << " | Mesas activas: "
        << servidor->mesasActivas
        << "/"
        << servidor->maxMesas
        << "\n";

    pthread_mutex_unlock(
        &servidor->mutexEstado
    );

    if (
        interrupcionSolicitada()
    ) {
        partida.solicitarDetencion();
    }

    partida.iniciar();

    bool interrumpida =
        partida.fueInterrumpida();

    pthread_mutex_lock(
        &servidor->mutexEstado
    );

    servidor->partidasActivas[
        datosPartida->numeroPartida
    ] = nullptr;

    servidor->mesasActivas--;

    if (interrumpida) {
        cout
            << "\nPartida "
            << datosPartida->numeroPartida
            << " detenida por SIGINT"
            << " | Mesas activas: "
            << servidor->mesasActivas
            << "/"
            << servidor->maxMesas
            << "\n";
    }
    else {
        cout
            << "\nPartida "
            << datosPartida->numeroPartida
            << " termino"
            << " | Mesas activas: "
            << servidor->mesasActivas
            << "/"
            << servidor->maxMesas
            << "\n";
    }

    pthread_mutex_unlock(
        &servidor->mutexEstado
    );

    sem_post(
        &servidor->semaforoMesas
    );

    return nullptr;
}

void Servidor::iniciar() {
    cout
        << "Servidor iniciado con "
        << cantidadJugadores
        << " jugadores y "
        << maxMesas
        << " mesas maximas.\n";

    vector<pthread_t> hilosClientes(
        cantidadJugadores
    );

    vector<DatosCliente> datosClientes(
        cantidadJugadores
    );

    vector<int> clienteCreado(
        cantidadJugadores,
        0
    );

    for (
        int i = 0;
        i < cantidadJugadores;
        i++
    ) {
        if (
            interrupcionSolicitada()
        ) {
            break;
        }

        datosClientes[i].servidor =
            this;

        datosClientes[i].jugador =
            i + 1;

        int resultado =
            pthread_create(
                &hilosClientes[i],
                nullptr,
                ejecutarCliente,
                &datosClientes[i]
            );

        if (
            resultado == 0
        ) {
            clienteCreado[i] = 1;
        }
        else {
            cerr
                << "Error al crear el cliente "
                << i + 1
                << ".\n";

            registrarCliente(
                i + 1
            );
        }
    }

    for (
        int i = 0;
        i < cantidadJugadores;
        i++
    ) {
        if (
            clienteCreado[i]
        ) {
            pthread_join(
                hilosClientes[i],
                nullptr
            );
        }
    }

    if (
        cantidadJugadores % 2 != 0 &&
        !interrupcionSolicitada()
    ) {
        pthread_mutex_lock(
            &mutexEmparejamiento
        );

        if (
            !jugadoresEsperando.empty()
        ) {
            cout
                << "Jugador "
                << jugadoresEsperando.front()
                << " quedo sin pareja.\n";
        }

        pthread_mutex_unlock(
            &mutexEmparejamiento
        );
    }

    for (
        int i = 0;
        i < static_cast<int>(
            hilosPartidas.size()
        );
        i++
    ) {
        if (
            partidaCreada[i]
        ) {
            pthread_join(
                hilosPartidas[i],
                nullptr
            );
        }
    }

    if (
        interrupcionSolicitada()
    ) {
        cout
            << "\nServidor detenido por SIGINT.\n";
    }
    else {
        cout
            << "\nTodas las partidas terminaron.\n";
    }
}

bool Servidor::reportarFalloPuma(
    int numeroPartida
) {
    pthread_mutex_lock(
        &mutexEstado
    );

    if (
        numeroPartida <= 0 ||
        numeroPartida >=
            static_cast<int>(
                partidasActivas.size()
            ) ||
        partidasActivas[
            numeroPartida
        ] == nullptr
    ) {
        pthread_mutex_unlock(
            &mutexEstado
        );

        return false;
    }

    cout
        << "\nServidor: fallo detectado en el Puma de la partida "
        << numeroPartida
        << "\n";

    partidasActivas[
        numeroPartida
    ]->reportarAbandonoPuma();

    pthread_mutex_unlock(
        &mutexEstado
    );

    return true;
}

bool Servidor::reportarFalloPerros(
    int numeroPartida
) {
    pthread_mutex_lock(
        &mutexEstado
    );

    if (
        numeroPartida <= 0 ||
        numeroPartida >=
            static_cast<int>(
                partidasActivas.size()
            ) ||
        partidasActivas[
            numeroPartida
        ] == nullptr
    ) {
        pthread_mutex_unlock(
            &mutexEstado
        );

        return false;
    }

    cout
        << "\nServidor: fallo detectado en los Perros de la partida "
        << numeroPartida
        << "\n";

    partidasActivas[
        numeroPartida
    ]->reportarAbandonoPerros();

    pthread_mutex_unlock(
        &mutexEstado
    );

    return true;
}

int Servidor::obtenerPartidaActiva() {
    pthread_mutex_lock(
        &mutexEstado
    );

    int numeroPartida = 0;

    for (
        int i = 1;
        i < static_cast<int>(
            partidasActivas.size()
        );
        i++
    ) {
        if (
            partidasActivas[i] != nullptr
        ) {
            numeroPartida = i;

            break;
        }
    }

    pthread_mutex_unlock(
        &mutexEstado
    );

    return numeroPartida;
}

int Servidor::obtenerMesasActivas() {
    pthread_mutex_lock(
        &mutexEstado
    );

    int cantidad =
        mesasActivas;

    pthread_mutex_unlock(
        &mutexEstado
    );

    return cantidad;
}