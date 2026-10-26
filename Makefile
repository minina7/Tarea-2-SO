CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -Iinclude

TARGET = torneo

SRC = src/main.cpp \
      src/tablero.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean