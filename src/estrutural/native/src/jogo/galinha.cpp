#include "jogo/galinha.h"

#include "jogo/comum.h"

#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace jogo;

static const float VELOCIDADE = 30.0f;

void Galinha::_ready() {
	Ref<SpriteFrames> quadros;
	quadros.instantiate();
	quadros->remove_animation("default");
	for (const char *lado : { "l", "r" }) {
		quadros->add_animation(lado);
		quadros->set_animation_speed(lado, 6.0);
		for (int i = 1; i <= 3; i++) {
			quadros->add_frame(lado, textura(String("res://assets/mundo/galinha/chicken_") + lado + "_frame_" + String::num_int64(i) + ".png"));
		}
	}
	sprite = memnew(AnimatedSprite2D);
	sprite->set_sprite_frames(quadros);
	sprite->set_offset(Vector2(0, -14));
	add_child(sprite);
	destino = get_position();
}

void Galinha::_process(double delta) {
	if (pausa > 0.0) {
		pausa -= delta;
		sprite->stop();
		return;
	}
	const Vector2 posicao = get_position();
	if (posicao.distance_to(destino) < 2.0f) {
		pausa = UtilityFunctions::randf_range(1.0, 3.0);
		const Vector2 passo(UtilityFunctions::randf_range(-80, 80), UtilityFunctions::randf_range(-80, 80));
		destino = (posicao + passo).clamp(area.position, area.get_end());
		return;
	}
	const Vector2 direcao = posicao.direction_to(destino);
	set_position(posicao + direcao * VELOCIDADE * delta);
	sprite->play(direcao.x < 0.0f ? "l" : "r");
}
