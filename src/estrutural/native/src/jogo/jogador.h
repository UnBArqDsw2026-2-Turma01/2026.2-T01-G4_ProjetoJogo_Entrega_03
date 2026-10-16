#pragma once

// Personagem controlado pelo jogador (WASD ou setas).

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>

using namespace godot;

class Jogador : public CharacterBody2D {
	GDCLASS(Jogador, CharacterBody2D)

public:
	void _ready() override;
	void _physics_process(double delta) override;

	void setPodeAndar(bool p_pode) { podeAndar = p_pode; }
	bool getPodeAndar() const { return podeAndar; }

protected:
	static void _bind_methods();

private:
	static Ref<SpriteFrames> criarQuadros();

	bool podeAndar = true;
	String lado = "r";
	AnimatedSprite2D *sprite = nullptr;
};
