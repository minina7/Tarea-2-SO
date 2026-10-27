#ifndef TABLERO_HPP
#define TABLERO_HPP

#include <vector>

using namespace std;


class Tablero {
private:

    vector<char> casillas;

    vector<vector<int>> vecinos;

    void conectar(int a, int b);
    void construirConexiones();

public:

    static constexpr int TOTAL_CASILLAS = 28;

    Tablero();

    bool esPosicionValida(int posicion) const;
    bool estaLibre(int posicion) const;
    bool sonAdyacentes(int origen, int destino) const;

    char obtenerFicha(int posicion) const;

    bool colocarFicha(int posicion, char ficha);
    bool moverFicha(int origen, int destino);
    bool capturarFicha(int origen, int capturada, int destino);

    void mostrar() const;
    void mostrarVecinos(int posicion) const;
};

#endif