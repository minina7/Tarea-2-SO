CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -Iinclude

TARGET = torneo

SRC = src/main.cpp \
      src/tablero.cpp \
      src/juego.cpp \
      src/partida.cpp \
      src/servidor.cpp \
      src/configuracion.cpp \
      src/senales.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

test_juez:
	$(CXX) $(CXXFLAGS) tests/test_juez.cpp src/tablero.cpp src/juego.cpp -o test_juez

test_colision:
	$(CXX) $(CXXFLAGS) tests/test_colision.cpp src/tablero.cpp src/juego.cpp -o test_colision

test_estres:
	$(CXX) $(CXXFLAGS) tests/test_estres.cpp src/tablero.cpp src/juego.cpp -o test_estres

test_estres_tsan:
	$(CXX) $(CXXFLAGS) -fsanitize=thread -g tests/test_estres.cpp src/tablero.cpp src/juego.cpp -o test_estres_tsan

test_abandono:
	$(CXX) $(CXXFLAGS) tests/test_abandono.cpp src/tablero.cpp src/juego.cpp src/partida.cpp src/senales.cpp -o test_abandono

test_logs:
	$(CXX) $(CXXFLAGS) tests/test_logs.cpp src/tablero.cpp src/juego.cpp src/partida.cpp src/senales.cpp -o test_logs

test_fallo_servidor:
	$(CXX) $(CXXFLAGS) tests/test_fallo_servidor.cpp src/tablero.cpp src/juego.cpp src/partida.cpp src/servidor.cpp src/configuracion.cpp src/senales.cpp -o test_fallo_servidor

tests: test_juez test_colision test_estres test_abandono test_logs test_fallo_servidor
	./test_juez
	./test_colision
	./test_estres
	./test_abandono
	./test_logs
	./test_fallo_servidor

clean:
	rm -f $(TARGET) test_juez test_colision test_estres test_estres_tsan test_abandono test_logs test_fallo_servidor

.PHONY: all tests test_juez test_colision test_estres test_estres_tsan test_abandono test_logs test_fallo_servidor clean