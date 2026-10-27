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


// Obtiene una coordenada para cada posicion.
// Esto sirve para comprobar que un salto siga
// una misma direccion.
bool Juego::obtenerCoordenadas(
    int posicion,
    int& x,
    int& y
) const {

    // Cuadricula principal.
    if (posicion >= 0 && posicion <= 24) {

        x = posicion % 5;
        y = posicion / 5;

        return true;
    }

    // Trono izquierdo.
    if (posicion == 25) {

        x = 1;
        y = -1;

        return true;
    }

    // Punta del trono.
    if (posicion == 26) {

        x = 2;
        y = -2;

        return true;
    }

    // Trono derecho.
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


    // El Perro debe estar al lado del Puma.
    if (!tablero.sonAdyacentes(origen, perro)) {
        return false;
    }


    // El destino debe estar al lado del Perro.
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


    // El salto debe continuar en la misma direccion.
    return dx1 == dx2 &&
           dy1 == dy2;
}


bool Juego::capturarPerro(
    int origen,
    int perro,
    int destino
) {

    // Tiene que haber un Puma en el origen.
    if (tablero.obtenerFicha(origen) != 'P') {
        return false;
    }


    // Tiene que existir un Perro que capturar.
    if (tablero.obtenerFicha(perro) != 'D') {
        return false;
    }


    // El lugar donde cae el Puma debe estar libre.
    if (!tablero.estaLibre(destino)) {
        return false;
    }


    // Comprobamos que realmente sea un salto valido.
    if (!esSaltoValido(
            origen,
            perro,
            destino)) {

        return false;
    }


    if (!tablero.capturarFicha(
            origen,
            perro,
            destino)) {

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


    // Buscar al Puma.
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


    // Primero buscamos movimientos normales.
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


    // Ahora buscamos posibles capturas.
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