# Makefile para o Padrão GoF Composite (C++)
# SubEquipe 02 - João Victor da Silva Batista de Farias (@beyondmagic)

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

TARGET = composite_demo
SRC = composite_demo.cpp

all: $(TARGET)

$(TARGET): $(SRC) ui_component.hpp ui_composite.hpp ui_label.hpp ui_button.hpp ui_progress_bar.hpp ui_panel.hpp battle_hud.hpp
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean

