#include <iostream>

#include "juego.hpp"

using namespace std;

bool probarConexionesTrono() {
    Tablero tablero;

    if (!tablero.sonAdyacentes(26, 25)) {
        return false;
    }

    if (!tablero.sonAdyacentes(26, 27)) {
        return false;
    }

    if (!tablero.sonAdyacentes(26, 2)) {
        return false;
    }

    if (!tablero.sonAdyacentes(25, 27)) {
        return false;
    }

    if (!tablero.sonAdyacentes(25, 0)) {
        return false;
    }

    if (!tablero.sonAdyacentes(25, 1)) {
        return false;
    }

    if (!tablero.sonAdyacentes(27, 3)) {
        return false;
    }

    if (!tablero.sonAdyacentes(27, 4)) {
        return false;
    }

    return true;
}

bool probarMovimientoTrono() {
    Juego juego;

    Tablero& tablero =
        juego.obtenerTablero();

    if (
        !tablero.colocarFicha(
            26,
            'P'
        )
    ) {
        return false;
    }

    if (
        !juego.moverPuma(
            26,
            2
        )
    ) {
        return false;
    }

    return
        tablero.obtenerFicha(26) == '.' &&
        tablero.obtenerFicha(2) == 'P';
}

bool probarCapturaVerticalCentral() {
    Juego juego;

    Tablero& tablero =
        juego.obtenerTablero();

    if (
        !tablero.colocarFicha(
            7,
            'P'
        )
    ) {
        return false;
    }

    if (
        !tablero.colocarFicha(
            2,
            'D'
        )
    ) {
        return false;
    }

    if (
        !juego.capturarPerro(
            7,
            2,
            26
        )
    ) {
        return false;
    }

    return
        tablero.obtenerFicha(7) == '.' &&
        tablero.obtenerFicha(2) == '.' &&
        tablero.obtenerFicha(26) == 'P' &&
        juego.obtenerPerrosCapturados() == 1;
}

bool probarCapturaDiagonalIzquierda() {
    Juego juego;

    Tablero& tablero =
        juego.obtenerTablero();

    if (
        !tablero.colocarFicha(
            0,
            'P'
        )
    ) {
        return false;
    }

    if (
        !tablero.colocarFicha(
            25,
            'D'
        )
    ) {
        return false;
    }

    return juego.capturarPerro(
        0,
        25,
        26
    );
}

bool probarCapturaDiagonalDerecha() {
    Juego juego;

    Tablero& tablero =
        juego.obtenerTablero();

    if (
        !tablero.colocarFicha(
            4,
            'P'
        )
    ) {
        return false;
    }

    if (
        !tablero.colocarFicha(
            27,
            'D'
        )
    ) {
        return false;
    }

    return juego.capturarPerro(
        4,
        27,
        26
    );
}

bool probarCapturaVerticalIzquierda() {
    Juego juego;

    Tablero& tablero =
        juego.obtenerTablero();

    if (
        !tablero.colocarFicha(
            6,
            'P'
        )
    ) {
        return false;
    }

    if (
        !tablero.colocarFicha(
            1,
            'D'
        )
    ) {
        return false;
    }

    return juego.capturarPerro(
        6,
        1,
        25
    );
}

bool probarSaltoConGiroInvalido() {
    Juego juego;

    Tablero& tablero =
        juego.obtenerTablero();

    if (
        !tablero.colocarFicha(
            0,
            'P'
        )
    ) {
        return false;
    }

    if (
        !tablero.colocarFicha(
            25,
            'D'
        )
    ) {
        return false;
    }

    return !juego.capturarPerro(
        0,
        25,
        27
    );
}

int main() {
    cout
        << "Prueba del trono\n";

    if (!probarConexionesTrono()) {
        cout
            << "ERROR: conexiones incorrectas en el trono.\n";

        return 1;
    }

    cout
        << "OK: conexiones del trono correctas.\n";

    if (!probarMovimientoTrono()) {
        cout
            << "ERROR: movimiento entre el trono y la cuadricula.\n";

        return 1;
    }

    cout
        << "OK: movimiento entre trono y cuadricula correcto.\n";

    if (!probarCapturaVerticalCentral()) {
        cout
            << "ERROR: captura vertical central del trono.\n";

        return 1;
    }

    cout
        << "OK: captura 7 -> 2 -> 26 correcta.\n";

    if (!probarCapturaDiagonalIzquierda()) {
        cout
            << "ERROR: captura diagonal izquierda del trono.\n";

        return 1;
    }

    cout
        << "OK: captura 0 -> 25 -> 26 correcta.\n";

    if (!probarCapturaDiagonalDerecha()) {
        cout
            << "ERROR: captura diagonal derecha del trono.\n";

        return 1;
    }

    cout
        << "OK: captura 4 -> 27 -> 26 correcta.\n";

    if (!probarCapturaVerticalIzquierda()) {
        cout
            << "ERROR: captura vertical lateral del trono.\n";

        return 1;
    }

    cout
        << "OK: captura 6 -> 1 -> 25 correcta.\n";

    if (!probarSaltoConGiroInvalido()) {
        cout
            << "ERROR: se permitio una captura con cambio de direccion.\n";

        return 1;
    }

    cout
        << "OK: captura con cambio de direccion rechazada.\n";

    cout
        << "test_trono finalizado correctamente.\n";

    return 0;
}