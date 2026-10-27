#ifndef JUEGO_HPP
#define JUEGO_HPP

#include "tablero.hpp"

using namespace std;


class Juego {
private:

    Tablero tablero;

    int perrosCapturados;

    bool obtenerCoordenadas(
        int posicion,
        int& x,
        int& y
    ) const;

    bool esSaltoValido(
        int origen,
        int perro,
        int destino
    ) const;

public:

    Juego();

    Tablero& obtenerTablero();

    bool moverPuma(int origen, int destino);

    bool moverPerro(int origen, int destino);

    bool capturarPerro(
        int origen,
        int perro,
        int destino
    );

    int obtenerPerrosCapturados() const;

    bool victoriaPuma() const;

    bool pumaTieneMovimientos() const;

    bool victoriaPerros() const;
};

#endif