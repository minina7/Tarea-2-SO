#include <iostream>

#include "juego.hpp"

using namespace std;


int main() {

    cout << "Torneo de Komikan Concurrente\n";

    cout << "\n--- PRUEBA 1: Captura valida ---\n";

    Juego juego1;
    Tablero& tablero1 = juego1.obtenerTablero();

    tablero1.colocarFicha(12, 'P');
    tablero1.colocarFicha(13, 'D');

    cout << "\nAntes de la captura:\n";
    tablero1.mostrar();

    if (juego1.capturarPerro(12, 13, 14)) {

        cout << "Captura realizada correctamente.\n";

    } else {

        cout << "ERROR: la captura deberia ser valida.\n";
    }

    tablero1.mostrar();

    cout << "Perros capturados: "
         << juego1.obtenerPerrosCapturados()
         << "\n";

    cout << "\nPRUEBA 2: Captura invalida\n";

    Juego juego2;
    Tablero& tablero2 = juego2.obtenerTablero();

    tablero2.colocarFicha(12, 'P');
    tablero2.colocarFicha(13, 'D');

    if (juego2.capturarPerro(12, 13, 18)) {

        cout << "ERROR: se permitio una captura invalida.\n";

    } else {

        cout << "Captura invalida rechazada correctamente.\n";
    }

    cout << "\nPRUEBA 3: Movimiento del Perro\n";

    Juego juego3;
    Tablero& tablero3 = juego3.obtenerTablero();

    tablero3.colocarFicha(12, 'P');
    tablero3.colocarFicha(13, 'D');

    if (juego3.moverPerro(13, 12)) {

        cout << "ERROR: el Perro pudo ocupar la casilla del Puma.\n";

    } else {

        cout << "Movimiento rechazado correctamente.\n";
    }

    cout << "\nPRUEBA 4: Puma acorralado\n";

    Juego juego4;
    Tablero& tablero4 = juego4.obtenerTablero();

    tablero4.colocarFicha(20, 'P');

    tablero4.colocarFicha(15, 'D');
    tablero4.colocarFicha(21, 'D');

    tablero4.colocarFicha(10, 'D');
    tablero4.colocarFicha(22, 'D');

    tablero4.mostrar();

    if (juego4.victoriaPerros()) {

        cout << "Los Perros ganan: el Puma esta acorralado.\n";

    } else {

        cout << "ERROR: el Puma deberia estar acorralado.\n";
    }


    return 0;
}