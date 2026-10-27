#include "configuracion.hpp"

#include <fstream>
#include <sstream>
#include <set>

using namespace std;

static string limpiar(const string& texto) {
    size_t inicio = texto.find_first_not_of(" \t\r\n");

    if (inicio == string::npos) {
        return "";
    }

    size_t fin = texto.find_last_not_of(" \t\r\n");

    return texto.substr(inicio, fin - inicio + 1);
}

static bool convertirEntero(const string& texto, int& valor) {
    try {
        size_t usados = 0;

        int numero = stoi(texto, &usados);

        if (usados != texto.size()) {
            return false;
        }

        valor = numero;

        return true;
    }
    catch (...) {
        return false;
    }
}

static bool convertirSemilla(
    const string& texto,
    unsigned int& valor
) {
    try {
        size_t usados = 0;

        unsigned long numero =
            stoul(texto, &usados);

        if (usados != texto.size()) {
            return false;
        }

        valor = static_cast<unsigned int>(numero);

        return true;
    }
    catch (...) {
        return false;
    }
}

Configuracion::Configuracion() {
    retardoMin = 50;
    retardoMax = 300;

    semilla = 12345;

    posicionPuma = 12;

    posicionesPerros = {
        0, 1, 2, 3, 4,
        5, 6, 7, 8, 9,
        11, 13
    };
}

bool Configuracion::cargar(
    const string& ruta,
    string& error
) {
    ifstream archivo(ruta);

    if (!archivo.is_open()) {
        error = "No se pudo abrir el archivo de configuracion.";
        return false;
    }

    string linea;
    int numeroLinea = 0;

    while (getline(archivo, linea)) {
        numeroLinea++;

        linea = limpiar(linea);

        if (linea.empty()) {
            continue;
        }

        if (linea[0] == '#') {
            continue;
        }

        size_t igual = linea.find('=');

        if (igual == string::npos) {
            error =
                "Linea invalida en config.txt: " +
                to_string(numeroLinea);

            return false;
        }

        string clave =
            limpiar(linea.substr(0, igual));

        string valor =
            limpiar(linea.substr(igual + 1));

        if (clave == "retardo_min") {
            if (!convertirEntero(valor, retardoMin)) {
                error = "Valor invalido para retardo_min.";
                return false;
            }
        }
        else if (clave == "retardo_max") {
            if (!convertirEntero(valor, retardoMax)) {
                error = "Valor invalido para retardo_max.";
                return false;
            }
        }
        else if (clave == "semilla") {
            if (!convertirSemilla(valor, semilla)) {
                error = "Valor invalido para semilla.";
                return false;
            }
        }
        else if (clave == "puma") {
            if (!convertirEntero(valor, posicionPuma)) {
                error = "Valor invalido para puma.";
                return false;
            }
        }
        else if (clave == "perros") {
            posicionesPerros.clear();

            stringstream ss(valor);
            string numero;

            while (getline(ss, numero, ',')) {
                numero = limpiar(numero);

                int posicion;

                if (!convertirEntero(numero, posicion)) {
                    error =
                        "Posicion invalida en la lista de perros.";

                    return false;
                }

                posicionesPerros.push_back(posicion);
            }
        }
        else {
            error =
                "Opcion desconocida en config.txt: " +
                clave;

            return false;
        }
    }

    return validar(error);
}

bool Configuracion::validar(string& error) const {
    if (retardoMin < 0) {
        error = "retardo_min no puede ser negativo.";
        return false;
    }

    if (retardoMax < retardoMin) {
        error =
            "retardo_max debe ser mayor o igual que retardo_min.";

        return false;
    }

    if (posicionPuma < 0 || posicionPuma >= 28) {
        error = "La posicion del Puma no es valida.";
        return false;
    }

    if (posicionesPerros.size() != 12) {
        error = "Deben existir exactamente 12 Perros.";
        return false;
    }

    set<int> posiciones;

    posiciones.insert(posicionPuma);

    for (int posicion : posicionesPerros) {
        if (posicion < 0 || posicion >= 28) {
            error = "Existe una posicion de Perro invalida.";
            return false;
        }

        if (posiciones.count(posicion) > 0) {
            error =
                "Hay dos fichas utilizando la misma posicion.";

            return false;
        }

        posiciones.insert(posicion);
    }

    return true;
}