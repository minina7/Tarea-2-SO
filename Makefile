CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -Iinclude

TARGET = torneo

SRC = src/main.cpp \
      src/tablero.cpp \
      src/juego.cpp \
      src/partida.cpp \
      src/servidor.cpp \
      src/configuracion.cpp

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

tests: test_juez test_colision test_estres
	./test_juez
	./test_colision
	./test_estres

clean:
	rm -f $(TARGET) test_juez test_colision test_estres test_estres_tsan

.PHONY: all tests test_juez test_colision test_estres test_estres_tsan clean