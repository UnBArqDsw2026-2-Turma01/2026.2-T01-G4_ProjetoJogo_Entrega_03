#pragma once

// Grupo de inimigos parado no mapa (data/inimigos.json, "encontros"). Encostar
// nele começa o combate. Derrotado, some e volta depois de um tempo.

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>

using namespace godot;

class InimigoNoMapa : public Area2D {
	GDCLASS(InimigoNoMapa, Area2D)

public:
	void _ready() override;
	void _process(double delta) override;

	void derrotar();
	void afastar(); // depois de uma fuga: não puxa o jogador de novo na hora

	PackedStringArray ids;
	String area;
	int indice = 0;

protected:
	static void _bind_methods();

private:
	void aoEncostar(Node2D *corpo);
	void reaparecer();
	void liberar();

	AnimatedSprite2D *sprite = nullptr;
	double tempo = 0.0;
	bool ativo = true;
};
