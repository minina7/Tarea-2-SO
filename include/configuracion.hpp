#ifndef CONFIGURACION_HPP
#define CONFIGURACION_HPP

#include <string>
#include <vector>

using namespace std;

class Configuracion {
public:
    int retardoMin;
    int retardoMax;
    unsigned int semilla;

    int posicionPuma;
    vector<int> posicionesPerros;

    Configuracion();

    bool cargar(const string& ruta, string& error);
    bool validar(string& error) const;
};

#endif