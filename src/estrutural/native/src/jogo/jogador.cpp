#include "jogo/jogador.h"

#include "jogo/comum.h"

#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/rectangle_shape2d.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace jogo;

static const float VELOCIDADE = 150.0f;

void Jogador::_ready() {
	sprite = memnew(AnimatedSprite2D);
	sprite->set_sprite_frames(criarQuadros());
	sprite->set_offset(Vector2(0, -18)); // os pés ficam na origem do corpo
	add_child(sprite);

	CollisionShape2D *colisao = memnew(CollisionShape2D);
	Ref<RectangleShape2D> forma;
	forma.instantiate();
	forma->set_size(Vector2(18, 10));
	colisao->set_shape(forma);
	add_child(colisao);

	sprite->play("idle_r");
}

void Jogador::_physics_process(double delta) {
	Vector2 entrada;
	if (podeAndar) {
		entrada = Input::get_singleton()->get_vector("mover_esquerda", "mover_direita", "mover_cima", "mover_baixo");
	}
	set_velocity(entrada * VELOCIDADE);
	move_and_slide();

	if (entrada.x != 0.0f) {
		lado = entrada.x < 0.0f ? "l" : "r";
	}
	sprite->play(String(entrada != Vector2() ? "walk_" : "idle_") + lado);
}

Ref<SpriteFrames> Jogador::criarQuadros() {
	Ref<SpriteFrames> quadros;
	quadros.instantiate();
	quadros->remove_animation("default");
	for (const char *estado : { "idle", "walk" }) {
		for (const char *ladoQuadro : { "l", "r" }) {
			const String nome = String(estado) + "_" + ladoQuadro;
			quadros->add_animation(nome);
			quadros->set_animation_speed(nome, String(estado) == "idle" ? 8.0 : 12.0);
			for (int i = 1; i <= 8; i++) {
				quadros->add_frame(nome, textura(String("res://assets/mundo/jogador/") + nome + "_frame_" + String::num_int64(i) + ".png"));
			}
		}
	}
	return quadros;
}

void Jogador::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_pode_andar", "pode"), &Jogador::setPodeAndar);
	ClassDB::bind_method(D_METHOD("get_pode_andar"), &Jogador::getPodeAndar);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "pode_andar"), "set_pode_andar", "get_pode_andar");
}
