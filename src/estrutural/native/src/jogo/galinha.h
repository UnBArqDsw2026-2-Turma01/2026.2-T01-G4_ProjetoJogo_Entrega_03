#pragma once

// Galinha que passeia à toa pelo mapa. Só decoração.

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

class Galinha : public Node2D {
	GDCLASS(Galinha, Node2D)

public:
	void _ready() override;
	void _process(double delta) override;

	Rect2 area;

protected:
	static void _bind_methods() {}

private:
	AnimatedSprite2D *sprite = nullptr;
	Vector2 destino;
	double pausa = 0.0;
};
