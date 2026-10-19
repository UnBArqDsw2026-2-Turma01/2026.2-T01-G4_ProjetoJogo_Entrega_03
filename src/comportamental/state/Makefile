CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic

FONTES = src/dominio.cpp src/combate.cpp src/turno_jogador.cpp src/turno_inimigo.cpp \
	src/vitoria.cpp src/derrota.cpp src/menu_terminal.cpp

.PHONY: all run demo test clean

all: bin/combate

bin/combate: $(FONTES) src/main.cpp src/*.h
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(FONTES) src/main.cpp -o $@

bin/testes: $(FONTES) tests/testes.cpp src/*.h
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(FONTES) tests/testes.cpp -o $@

run: bin/combate
	./bin/combate

demo: bin/combate
	./bin/combate --demo

test: bin/testes
	./bin/testes

clean:
	rm -rf bin
