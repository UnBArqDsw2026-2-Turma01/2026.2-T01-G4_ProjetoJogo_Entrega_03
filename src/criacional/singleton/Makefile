CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CORE = src/partida.cpp src/clientes.cpp
HEADERS = $(wildcard src/*.hpp)

.PHONY: all run demo roteiro test garantia publicar clean
all: bin/singleton

bin/singleton: $(CORE) src/main.cpp $(HEADERS)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(CORE) src/main.cpp -o $@

bin/testes: $(CORE) tests/testes.cpp $(HEADERS)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -pthread -Isrc $(CORE) tests/testes.cpp -o $@

run: bin/singleton
	./bin/singleton
demo: bin/singleton
	./bin/singleton --demo
roteiro: bin/singleton
	./bin/singleton --roteiro
test: bin/testes garantia
	./bin/testes

# Passa somente se o compilador recusar tests/nao_compila.cpp.
garantia:
	@if $(CXX) $(CXXFLAGS) -fsyntax-only -Isrc tests/nao_compila.cpp 2>/dev/null; then \
		echo "ERRO: foi possível criar ou copiar uma Partida fora da classe"; exit 1; \
	else \
		echo "OK  Compilador recusa criar ou copiar uma Partida fora da classe"; \
	fi

clean:
	rm -rf bin

# Gera o roteiro usado pela visualização do relatório a partir da execução real.
DOCS = ../../../docs/Base/Relatórios/SubEquipe_01/singleton
publicar: bin/singleton
	{ echo '// Gerado por "make publicar" a partir de ./bin/singleton --roteiro. Não editar.'; \
	  echo 'window.ROTEIRO = ['; ./bin/singleton --roteiro | sed 's/$$/,/'; echo '];'; } > "$(DOCS)/roteiro.js"
	@echo "Roteiro publicado em $(DOCS)/roteiro.js"
