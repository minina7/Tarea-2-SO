#include <iostream>
#include <cstdlib>

#include "servidor.hpp"
#include "configuracion.hpp"
#include "senales.hpp"

using namespace std;

int main(
    int argc,
    char* argv[]
) {
    if (
        argc < 3 ||
        argc > 4
    ) {
        cout
            << "Uso: ./torneo N M [config.txt]\n";

        return 1;
    }

    int N =
        atoi(argv[1]);

    int M =
        atoi(argv[2]);

    if (N < 2) {
        cout
            << "Error: N debe ser al menos 2.\n";

        return 1;
    }

    if (M < 1) {
        cout
            << "Error: M debe ser mayor que 0.\n";

        return 1;
    }

    Configuracion config;

    if (argc == 4) {
        string error;

        if (
            !config.cargar(
                argv[3],
                error
            )
        ) {
            cout
                << "Error de configuracion: "
                << error
                << "\n";

            return 1;
        }

        cout
            << "Configuracion cargada desde "
            << argv[3]
            << "\n";
    }
    else {
        cout
            << "Usando configuracion por defecto.\n";
    }

    instalarManejadorSIGINT();

    cout
        << "Torneo de Komikan Concurrente\n";

    cout
        << "Retardo: "
        << config.retardoMin
        << "-"
        << config.retardoMax
        << " ms\n";

    cout
        << "Semilla: "
        << config.semilla
        << "\n";

    Servidor servidor(
        N,
        M,
        config
    );

    servidor.iniciar();

    if (
        interrupcionSolicitada()
    ) {
        cout
            << "Cierre ordenado completado.\n";
    }

    return 0;
}