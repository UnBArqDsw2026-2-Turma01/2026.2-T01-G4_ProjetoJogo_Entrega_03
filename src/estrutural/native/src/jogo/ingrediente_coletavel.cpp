#include "jogo/ingrediente_coletavel.h"

#include "jogo/comum.h"
#include "jogo/jogador.h"

#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace jogo;

static const double TEMPO_PARA_RENASCER = 20.0;

void IngredienteColetavel::_ready() {
	tempo = UtilityFunctions::randf() * 6.283185307;

	sprite = memnew(Sprite2D);
	sprite->set_texture(icone(idIngrediente));
	sprite->set_scale(Vector2(2, 2));
	add_child(sprite);

	CollisionShape2D *colisao = memnew(CollisionShape2D);
	Ref<CircleShape2D> forma;
	forma.instantiate();
	forma->set_radius(12.0);
	colisao->set_shape(forma);
	add_child(colisao);

	connect("body_entered", callable_mp(this, &IngredienteColetavel::aoEncostar));
}

void IngredienteColetavel::_process(double delta) {
	tempo += delta;
	sprite->set_position(Vector2(0, Math::sin(tempo * 3.0) * 3.0 - 12.0));
}

void IngredienteColetavel::aoEncostar(Node2D *corpo) {
	if (Object::cast_to<Jogador>(corpo) && fachada(this)->coletarIngrediente(idIngrediente, 1)) {
		emit_signal("coletado", nome);
		hide();
		set_deferred("monitoring", false);
		get_tree()->create_timer(TEMPO_PARA_RENASCER)->connect("timeout", callable_mp(this, &IngredienteColetavel::renascer));
	}
}

void IngredienteColetavel::renascer() {
	show();
	set_monitoring(true);
}

void IngredienteColetavel::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_id_ingrediente"), &IngredienteColetavel::getIdIngrediente);
	ADD_SIGNAL(MethodInfo("coletado", PropertyInfo(Variant::STRING, "nome")));
}
