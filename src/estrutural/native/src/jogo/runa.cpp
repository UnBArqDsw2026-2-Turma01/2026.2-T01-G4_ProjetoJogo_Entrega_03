#include "jogo/runa.h"

#include "jogo/comum.h"
#include "jogo/jogador.h"

#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace jogo;

void Runa::_ready() {
	Ref<SpriteFrames> frames;
	frames.instantiate();
	frames->set_animation_speed("default", 4.0);
	for (int i = 1; i <= 4; i++) {
		frames->add_frame("default", textura(String("res://assets/mundo/segredos/runa_secreta_frame_") + String::num_int64(i) + ".png"));
	}
	sprite = memnew(AnimatedSprite2D);
	sprite->set_sprite_frames(frames);
	sprite->set_scale(Vector2(3, 3));
	sprite->play("default");
	add_child(sprite);

	dica = memnew(Label);
	dica->set_text(u("E: examinar a runa"));
	dica->add_theme_color_override("font_outline_color", cor_texto());
	dica->add_theme_constant_override("outline_size", 6);
	dica->set_position(Vector2(-70, -60));
	dica->hide();
	add_child(dica);

	CollisionShape2D *colisao = memnew(CollisionShape2D);
	Ref<CircleShape2D> forma;
	forma.instantiate();
	forma->set_radius(40.0);
	colisao->set_shape(forma);
	add_child(colisao);
	connect("body_entered", callable_mp(this, &Runa::aoAproximar).bind(true));
	connect("body_exited", callable_mp(this, &Runa::aoAproximar).bind(false));

	hide();
}

void Runa::revelar() {
	if (revelada || coletada) {
		return;
	}
	revelada = true;
	show();
	set_modulate(Color(1, 1, 1, 0));
	create_tween()->tween_property(this, "modulate:a", 1.0, 1.0);
	dica->set_visible(jogadorPerto);
}

void Runa::aoAproximar(Node2D *corpo, bool perto) {
	if (!Object::cast_to<Jogador>(corpo)) {
		return;
	}
	jogadorPerto = perto;
	dica->set_visible(perto && isDisponivel());
}

void Runa::_unhandled_input(const Ref<InputEvent> &evento) {
	if (!jogadorPerto || !isDisponivel() || !evento->is_action_pressed("interagir")) {
		return;
	}
	get_viewport()->set_input_as_handled();
	coletada = true;
	Array obtidos;
	for (int i = 0; i < recompensa.size(); i++) {
		const Dictionary item = recompensa[i];
		if (fachada(this)->coletarIngrediente(item["id"], int(item["quantidade"]))) {
			obtidos.push_back(item);
		}
	}
	emit_signal("coletada", obtidos);
	Ref<Tween> t = create_tween();
	t->tween_property(this, "modulate:a", 0.0, 0.6);
	t->tween_callback(callable_mp((CanvasItem *)this, &CanvasItem::hide));
}

void Runa::_bind_methods() {
	ClassDB::bind_method(D_METHOD("revelar"), &Runa::revelar);
	ClassDB::bind_method(D_METHOD("is_disponivel"), &Runa::isDisponivel);
	ADD_SIGNAL(MethodInfo("coletada", PropertyInfo(Variant::ARRAY, "obtidos")));
}
