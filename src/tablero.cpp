#include "tablero.hpp"
#include <algorithm>
#include <iostream>

using namespace std;


Tablero::Tablero()
    : casillas(TOTAL_CASILLAS, '.'),
      vecinos(TOTAL_CASILLAS) {

    construirConexiones();
}


void Tablero::conectar(int a, int b) {
    vecinos[a].push_back(b);
    vecinos[b].push_back(a);
}


void Tablero::construirConexiones() {

    for (int fila = 0; fila < 5; ++fila) {

        for (int columna = 0; columna < 5; ++columna) {

            int actual = fila * 5 + columna;

            if (columna < 4) {
                conectar(actual, actual + 1);
            }

            if (fila < 4) {
                conectar(actual, actual + 5);
            }
        }
    }


    const int TRONO_IZQ = 25;
    const int TRONO_SUP = 26;
    const int TRONO_DER = 27;

    conectar(TRONO_IZQ, 0);
    conectar(TRONO_IZQ, 1);

    conectar(TRONO_SUP, TRONO_IZQ);
    conectar(TRONO_SUP, 2);
    conectar(TRONO_SUP, TRONO_DER);

    conectar(TRONO_IZQ, TRONO_DER);

    conectar(TRONO_DER, 3);
    conectar(TRONO_DER, 4);
}


bool Tablero::esPosicionValida(int posicion) const {

    return posicion >= 0 &&
           posicion < TOTAL_CASILLAS;
}


bool Tablero::estaLibre(int posicion) const {

    if (!esPosicionValida(posicion)) {
        return false;
    }

    return casillas[posicion] == '.';
}


bool Tablero::sonAdyacentes(int origen, int destino) const {

    if (!esPosicionValida(origen) ||
        !esPosicionValida(destino)) {
        return false;
    }

    const auto& lista = vecinos[origen];

    return find(
        lista.begin(),
        lista.end(),
        destino
    ) != lista.end();
}


char Tablero::obtenerFicha(int posicion) const {

    if (!esPosicionValida(posicion)) {
        return '?';
    }

    return casillas[posicion];
}


bool Tablero::colocarFicha(int posicion, char ficha) {

    if (!esPosicionValida(posicion)) {
        return false;
    }

    if (!estaLibre(posicion)) {
        return false;
    }

    casillas[posicion] = ficha;

    return true;
}


bool Tablero::moverFicha(int origen, int destino) {

    if (!esPosicionValida(origen) ||
        !esPosicionValida(destino)) {
        return false;
    }

    if (casillas[origen] == '.') {
        return false;
    }

    if (!estaLibre(destino)) {
        return false;
    }

    if (!sonAdyacentes(origen, destino)) {
        return false;
    }

    casillas[destino] = casillas[origen];
    casillas[origen] = '.';

    return true;
}

bool Tablero::capturarFicha(int origen, int capturada, int destino) {

    if (!esPosicionValida(origen) ||
        !esPosicionValida(capturada) ||
        !esPosicionValida(destino)) {
        return false;
    }

    if (casillas[origen] != 'P') {
        return false;
    }

    if (casillas[capturada] != 'D') {
        return false;
    }

    if (!estaLibre(destino)) {
        return false;
    }

    casillas[origen] = '.';
    casillas[capturada] = '.';
    casillas[destino] = 'P';

    return true;
}


void Tablero::mostrar() const {

    cout << "\n";

    cout << "              [" << casillas[26] << "]\n";
    cout << "             / | \\\n";
    cout << "          [" << casillas[25] << "]---[" << casillas[27] << "]\n";
    cout << "          / |   |   | \\\n";

    for (int fila = 0; fila < 5; ++fila) {

        for (int columna = 0; columna < 5; ++columna) {

            int posicion = fila * 5 + columna;

            cout << "[" << casillas[posicion] << "]";

            if (columna < 4) {
                cout << "---";
            }
        }

        cout << "\n";

        if (fila < 4) {
            cout << " |     |     |     |     |\n";
        }
    }

    cout << "\n";
}


void Tablero::mostrarVecinos(int posicion) const {

    if (!esPosicionValida(posicion)) {

        cout << "Posicion invalida\n";
        return;
    }

    cout << "Vecinos de "
         << posicion
         << ": ";

    for (int vecino : vecinos[posicion]) {
        cout << vecino << " ";
    }

    cout << "\n";
}