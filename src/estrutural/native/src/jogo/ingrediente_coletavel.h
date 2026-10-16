#pragma once

// Ingrediente no chão do mundo. Ao encostar, o jogador o coleta pela fachada
// (coletarIngrediente) e ele reaparece depois de um tempo.

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

using namespace godot;

class IngredienteColetavel : public Area2D {
	GDCLASS(IngredienteColetavel, Area2D)

public:
	void _ready() override;
	void _process(double delta) override;

	String idIngrediente;
	String nome;

	String getIdIngrediente() const { return idIngrediente; }

protected:
	static void _bind_methods();

private:
	void aoEncostar(Node2D *corpo);
	void renascer();

	Sprite2D *sprite = nullptr;
	double tempo = 0.0;
};
