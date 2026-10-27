#include "senales.hpp"

#include <csignal>

using namespace std;

static volatile sig_atomic_t detenerServidor = 0;

static void manejarSIGINT(int) {
    detenerServidor = 1;
}

void instalarManejadorSIGINT() {
    struct sigaction accion {};

    accion.sa_handler = manejarSIGINT;

    sigemptyset(&accion.sa_mask);

    accion.sa_flags = 0;

    sigaction(
        SIGINT,
        &accion,
        nullptr
    );
}

bool interrupcionSolicitada() {
    return detenerServidor != 0;
}