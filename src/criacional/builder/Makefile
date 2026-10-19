CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -Iinclude

SOURCES = src/attack_action.cpp src/character.cpp src/character_builder.cpp src/terminal_menu.cpp

.PHONY: all run test clean

all: bin/builder

bin/builder: $(SOURCES) src/main.cpp src/*.h
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SOURCES) src/main.cpp -o $@

bin/tests: $(SOURCES) tests/tests.cpp src/*.h
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SOURCES) tests/tests.cpp -o $@

run: bin/builder
	./bin/builder

test: bin/tests
	./bin/tests

clean:
	rm -rf bin
