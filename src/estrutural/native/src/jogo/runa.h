#pragma once

// Segredo do mapa (data/exploracao.json). Fica invisível até a Poção de
// Clarividência revelar os segredos; depois, E perto dela dá a recompensa.

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/label.hpp>

using namespace godot;

class Runa : public Area2D {
	GDCLASS(Runa, Area2D)

public:
	void _ready() override;
	void _unhandled_input(const Ref<InputEvent> &evento) override;

	void revelar();
	bool isDisponivel() const { return revelada && !coletada; }

	Array recompensa; // [{ id, quantidade }]
	String area;

protected:
	static void _bind_methods();

private:
	void aoAproximar(Node2D *corpo, bool perto);

	AnimatedSprite2D *sprite = nullptr;
	Label *dica = nullptr;
	bool revelada = false;
	bool coletada = false;
	bool jogadorPerto = false;
};
