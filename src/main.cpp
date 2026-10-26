#include <iostream>

#include "tablero.hpp"

int main() {

    std::cout << "=== Torneo de Komikan Concurrente ===\n";

    Tablero tablero;

    std::cout << "\nTablero inicial:\n";

    tablero.mostrar();

    tablero.colocarFicha(12, 'P');
    tablero.colocarFicha(13, 'D');

    std::cout << "Tablero con fichas de prueba:\n";

    tablero.mostrar();


    tablero.mostrarVecinos(12);


    std::cout << "\nIntentando mover Puma de 12 a 7...\n";

    if (tablero.moverFicha(12, 7)) {

        std::cout << "Movimiento realizado correctamente.\n";

    } else {

        std::cout << "Movimiento invalido.\n";
    }


    tablero.mostrar();

    return 0;
}