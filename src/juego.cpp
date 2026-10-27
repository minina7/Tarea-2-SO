#include "juego.hpp"

using namespace std;


Juego::Juego()
    : perrosCapturados(0) {
}


Tablero& Juego::obtenerTablero() {
    return tablero;
}


bool Juego::moverPuma(int origen, int destino) {

    if (tablero.obtenerFicha(origen) != 'P') {
        return false;
    }

    return tablero.moverFicha(origen, destino);
}


bool Juego::moverPerro(int origen, int destino) {

    if (tablero.obtenerFicha(origen) != 'D') {
        return false;
    }

    return tablero.moverFicha(origen, destino);
}


bool Juego::obtenerCoordenadas(
    int posicion,
    int& x,
    int& y
) const {

    if (posicion >= 0 && posicion <= 24) {

        x = posicion % 5;
        y = posicion / 5;

        return true;
    }

    if (posicion == 25) {

        x = 1;
        y = -1;

        return true;
    }

    if (posicion == 26) {

        x = 2;
        y = -2;

        return true;
    }

    if (posicion == 27) {

        x = 3;
        y = -1;

        return true;
    }

    return false;
}


bool Juego::esSaltoValido(
    int origen,
    int perro,
    int destino
) const {

    if (!tablero.esPosicionValida(origen) ||
        !tablero.esPosicionValida(perro) ||
        !tablero.esPosicionValida(destino)) {

        return false;
    }

    if (!tablero.sonAdyacentes(origen, perro)) {
        return false;
    }

    if (!tablero.sonAdyacentes(perro, destino)) {
        return false;
    }

    int xOrigen;
    int yOrigen;

    int xPerro;
    int yPerro;

    int xDestino;
    int yDestino;

    obtenerCoordenadas(
        origen,
        xOrigen,
        yOrigen
    );

    obtenerCoordenadas(
        perro,
        xPerro,
        yPerro
    );

    obtenerCoordenadas(
        destino,
        xDestino,
        yDestino
    );

    int dx1 = xPerro - xOrigen;
    int dy1 = yPerro - yOrigen;

    int dx2 = xDestino - xPerro;
    int dy2 = yDestino - yPerro;

    return dx1 == dx2 &&
           dy1 == dy2;
}


bool Juego::puedeCapturarPerro(
    int origen,
    int perro,
    int destino
) const {

    if (tablero.obtenerFicha(origen) != 'P') {
        return false;
    }

    if (tablero.obtenerFicha(perro) != 'D') {
        return false;
    }

    if (!tablero.estaLibre(destino)) {
        return false;
    }

    return esSaltoValido(origen, perro, destino);
}


bool Juego::capturarPerro(
    int origen,
    int perro,
    int destino
) {

    if (!puedeCapturarPerro(origen, perro, destino)) {
        return false;
    }

    if (!tablero.capturarFicha(origen, perro, destino)) {
        return false;
    }

    perrosCapturados++;

    return true;
}


int Juego::obtenerPerrosCapturados() const {

    return perrosCapturados;
}


bool Juego::victoriaPuma() const {

    return perrosCapturados >= 6;
}


bool Juego::pumaTieneMovimientos() const {

    int posicionPuma = -1;

    for (int i = 0;
         i < Tablero::TOTAL_CASILLAS;
         ++i) {

        if (tablero.obtenerFicha(i) == 'P') {

            posicionPuma = i;
            break;
        }
    }

    if (posicionPuma == -1) {
        return false;
    }

    for (int destino = 0;
         destino < Tablero::TOTAL_CASILLAS;
         ++destino) {

        if (tablero.estaLibre(destino) &&
            tablero.sonAdyacentes(
                posicionPuma,
                destino)) {

            return true;
        }
    }

    for (int perro = 0;
         perro < Tablero::TOTAL_CASILLAS;
         ++perro) {

        if (tablero.obtenerFicha(perro) != 'D') {
            continue;
        }

        for (int destino = 0;
             destino < Tablero::TOTAL_CASILLAS;
             ++destino) {

            if (!tablero.estaLibre(destino)) {
                continue;
            }

            if (esSaltoValido(
                    posicionPuma,
                    perro,
                    destino)) {

                return true;
            }
        }
    }

    return false;
}


bool Juego::victoriaPerros() const {

    return !pumaTieneMovimientos();
}