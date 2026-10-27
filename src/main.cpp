#include <iostream>
#include <vector>

#include "partida.hpp"

using namespace std;


int main() {

    cout << "Torneo de Komikan Concurrente\n";


    int posicionPuma = 12;

    vector<int> posicionesPerros = {
        0, 1, 2, 3, 4,
        5, 6, 7, 8, 9,
        11, 13
    };


    Partida partida(
        posicionPuma,
        posicionesPerros,
        50,
        300
    );


    partida.iniciar();


    return 0;
}