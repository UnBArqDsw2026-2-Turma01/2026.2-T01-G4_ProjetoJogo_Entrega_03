#include "jogo/inimigo_no_mapa.h"

#include "jogo/comum.h"
#include "jogo/jogador.h"

#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace jogo;

static const double TEMPO_PARA_REAPARECER = 90.0;
static const double TEMPO_APOS_FUGA = 4.0;

void InimigoNoMapa::_ready() {
	// O primeiro inimigo do grupo representa o encontro no mapa (arte 3x, como o resto do mundo).
	const String pasta = String("res://assets/combate/inimigos/") + ids[0] + "/idle_l_frame_";
	Ref<SpriteFrames> frames;
	frames.instantiate();
	frames->set_animation_speed("default", 5.0);
	for (int i = 1; ResourceLoader::get_singleton()->exists(pasta + String::num_int64(i) + ".png"); i++) {
		frames->add_frame("default", textura(pasta + String::num_int64(i) + ".png"));
	}
	sprite = memnew(AnimatedSprite2D);
	sprite->set_sprite_frames(frames);
	sprite->set_scale(Vector2(3, 3));
	if (frames->get_frame_count("default") > 0) {
		sprite->set_offset(Vector2(0, -frames->get_frame_texture("default", 0)->get_height() / 2.0f));
	}
	sprite->play("default");
	add_child(sprite);

	CollisionShape2D *colisao = memnew(CollisionShape2D);
	Ref<CircleShape2D> forma;
	forma.instantiate();
	forma->set_radius(30.0);
	colisao->set_shape(forma);
	colisao->set_position(Vector2(0, -12));
	add_child(colisao);

	connect("body_entered", callable_mp(this, &InimigoNoMapa::aoEncostar));
}

void InimigoNoMapa::_process(double delta) {
	tempo += delta;
	sprite->set_position(Vector2(0, Math::sin(tempo * 2.0) * 2.0));
}

void InimigoNoMapa::aoEncostar(Node2D *corpo) {
	if (ativo && Object::cast_to<Jogador>(corpo)) {
		emit_signal("encontrado", indice);
	}
}

void InimigoNoMapa::derrotar() {
	ativo = false;
	hide();
	set_deferred("monitoring", false);
	get_tree()->create_timer(TEMPO_PARA_REAPARECER)->connect("timeout", callable_mp(this, &InimigoNoMapa::reaparecer));
}

void InimigoNoMapa::afastar() {
	ativo = false;
	set_modulate(Color(1, 1, 1, 0.5));
	get_tree()->create_timer(TEMPO_APOS_FUGA)->connect("timeout", callable_mp(this, &InimigoNoMapa::liberar));
}

void InimigoNoMapa::reaparecer() {
	show();
	set_monitoring(true);
	liberar();
}

void InimigoNoMapa::liberar() {
	ativo = true;
	set_modulate(Color(1, 1, 1, 1));
}

void InimigoNoMapa::_bind_methods() {
	ADD_SIGNAL(MethodInfo("encontrado", PropertyInfo(Variant::INT, "indice")));
}
