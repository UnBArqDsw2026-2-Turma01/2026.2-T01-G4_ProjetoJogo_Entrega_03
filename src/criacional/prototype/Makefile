CXX ?= g++
EMXX ?= em++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CORE = src/inimigo.cpp src/catalogo.cpp src/demonstracao.cpp
HEADERS = $(wildcard src/*.hpp)

.PHONY: all run test web publish check clean
all: bin/prototype

bin/prototype: $(CORE) src/main.cpp $(HEADERS)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(CORE) src/main.cpp -o $@

bin/testes: $(CORE) tests/testes.cpp $(HEADERS)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -Isrc $(CORE) tests/testes.cpp -o $@

run: bin/prototype
	./bin/prototype
test: bin/testes
	./bin/testes

web: bin/motor.js
bin/motor.js: $(CORE) src/web.cpp $(HEADERS)
	mkdir -p bin
	$(EMXX) $(CXXFLAGS) $(CORE) src/web.cpp --no-entry -sMODULARIZE=1 -sEXPORT_NAME=CriarPrototype -sSINGLE_FILE=1 -sENVIRONMENT=web,node -sALLOW_MEMORY_GROWTH=1 -sDISABLE_EXCEPTION_CATCHING=0 -sEXPORTED_FUNCTIONS=_comando,_estado -sEXPORTED_RUNTIME_METHODS=cwrap -o $@

publish: web
	python3 publicar.py
check:
	python3 publicar.py --check
clean:
	rm -rf bin
