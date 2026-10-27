#include <iostream>
#include <cstdlib>

#include "servidor.hpp"

using namespace std;


int main(int argc, char* argv[]) {

    if (argc < 3) {

        cout << "Uso: ./torneo N M [config.txt]\n";

        return 1;
    }


    int N = atoi(argv[1]);
    int M = atoi(argv[2]);


    if (N < 2) {

        cout << "Error: N debe ser al menos 2.\n";

        return 1;
    }


    if (M < 1) {

        cout << "Error: M debe ser mayor que 0.\n";

        return 1;
    }


    cout << "Torneo de Komikan Concurrente\n";


    Servidor servidor(N, M);

    servidor.iniciar();


    return 0;
}