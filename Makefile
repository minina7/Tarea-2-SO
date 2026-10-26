CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread

TARGET = torneo
SRC = src/main.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
