# Makefile para o Padrão GoF Decorator (C++)
# SubEquipe 02 - Marcos Vinícius Gündel da Silva (@MarcosViniciusG)
#
# Mesmas flags do godot-cpp (C++17, sem exceções), para garantir que o núcleo
# compila igual aqui e dentro da GDExtension.

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -fno-exceptions

CABECALHOS = $(wildcard *.hpp)

all: decorator_demo testes_decorator

decorator_demo: decorator_demo.cpp $(CABECALHOS)
	$(CXX) $(CXXFLAGS) decorator_demo.cpp -o $@

testes_decorator: testes_decorator.cpp $(CABECALHOS)
	$(CXX) $(CXXFLAGS) testes_decorator.cpp -o $@

run: decorator_demo
	./decorator_demo

test: testes_decorator
	./testes_decorator ../data/alquimia.json

clean:
	rm -f decorator_demo testes_decorator

.PHONY: all run test clean
