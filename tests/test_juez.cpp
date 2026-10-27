#include <iostream>
#include <vector>

#include "juego.hpp"

using namespace std;

bool probarVictoriaPerros() {
    Juego juego;

    Tablero& tablero =
        juego.obtenerTablero();

    if (!tablero.colocarFicha(20, 'P')) {
        return false;
    }

    vector<int> perros = {
        15, 21, 10, 22,
        0, 1, 2, 3,
        4, 5, 6, 7
    };

    for (int posicion : perros) {
        if (!tablero.colocarFicha(posicion, 'D')) {
            return false;
        }
    }

    return juego.victoriaPerros();
}

bool probarVictoriaPuma() {
    Juego juego;

    Tablero& tablero =
        juego.obtenerTablero();

    if (!tablero.colocarFicha(10, 'P')) {
        return false;
    }

    vector<int> perros = {
        11, 13, 9, 3, 7, 17
    };

    for (int posicion : perros) {
        if (!tablero.colocarFicha(posicion, 'D')) {
            return false;
        }
    }

    if (!juego.capturarPerro(10, 11, 12)) {
        return false;
    }

    if (!juego.capturarPerro(12, 13, 14)) {
        return false;
    }

    if (!juego.capturarPerro(14, 9, 4)) {
        return false;
    }

    if (!juego.capturarPerro(4, 3, 2)) {
        return false;
    }

    if (!juego.capturarPerro(2, 7, 12)) {
        return false;
    }

    if (!juego.capturarPerro(12, 17, 22)) {
        return false;
    }

    return juego.victoriaPuma();
}

int main() {
    cout << "Prueba del Juez\n";

    if (!probarVictoriaPerros()) {
        cout << "ERROR: no se detecto al Puma acorralado.\n";
        return 1;
    }

    cout << "OK: Puma acorralado detectado.\n";

    if (!probarVictoriaPuma()) {
        cout << "ERROR: no se detecto la victoria del Puma.\n";
        return 1;
    }

    cout << "OK: victoria del Puma con 6 capturas detectada.\n";

    cout << "test_juez finalizado correctamente.\n";

    return 0;
}