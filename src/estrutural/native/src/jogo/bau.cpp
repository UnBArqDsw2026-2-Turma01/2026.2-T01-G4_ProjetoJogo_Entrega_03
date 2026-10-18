#include "jogo/bau.h"

#include "jogo/comum.h"
#include "jogo/jogador.h"

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/rectangle_shape2d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>
#include <godot_cpp/classes/style_box_flat.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace jogo;

static const double TEMPO_PARA_REABASTECER = 45.0;

void Bau::_ready() {
	sprite = memnew(Sprite2D);
	sprite->set_texture(icone("bau_fechado"));
	sprite->set_scale(Vector2(3, 3));
	sprite->set_offset(Vector2(0, -6));
	add_child(sprite);

	CollisionShape2D *colisao = memnew(CollisionShape2D);
	Ref<RectangleShape2D> forma;
	forma.instantiate();
	forma->set_size(Vector2(42, 16));
	colisao->set_shape(forma);
	add_child(colisao);

	Area2D *alcance = memnew(Area2D);
	CollisionShape2D *formaAlcance = memnew(CollisionShape2D);
	Ref<CircleShape2D> circulo;
	circulo.instantiate();
	circulo->set_radius(48.0);
	formaAlcance->set_shape(circulo);
	alcance->add_child(formaAlcance);
	alcance->connect("body_entered", callable_mp(this, &Bau::aoAproximar).bind(true));
	alcance->connect("body_exited", callable_mp(this, &Bau::aoAproximar).bind(false));
	add_child(alcance);

	painel = criarPainel();
	painel->hide();
	add_child(painel);
}

void Bau::_unhandled_input(const Ref<InputEvent> &evento) {
	if (jogadorPerto && !aberto && evento->is_action_pressed("interagir")) {
		get_viewport()->set_input_as_handled();
		abrir();
	}
}

void Bau::abrir() {
	Array obtidos;
	const Array itens = tipo["itens"];
	// Sorte do jogador (Amuleto Líquido, um decorador contado em saques) soma na chance.
	const int sorte = fachada(this)->bonusDeSorte();
	for (int i = 0; i < itens.size(); i++) {
		const Dictionary item = itens[i];
		if (UtilityFunctions::randf() * 100.0 < double(item["chance"]) + sorte) {
			const int quantidade = UtilityFunctions::randi_range(int(item["min"]), int(item["max"]));
			if (fachada(this)->coletarIngrediente(item["id"], quantidade)) {
				Dictionary obtido;
				obtido["nome"] = nomes[item["id"]];
				obtido["quantidade"] = quantidade;
				obtidos.push_back(obtido);
			}
		}
	}

	fachada(this)->registrarSaque();
	aberto = true;
	sprite->set_texture(icone("bau_aberto"));
	painel->hide();
	emit_signal("aberto", tipo["nome"], obtidos);
	get_tree()->create_timer(TEMPO_PARA_REABASTECER)->connect("timeout", callable_mp(this, &Bau::reabastecer));
}

void Bau::reabastecer() {
	aberto = false;
	sprite->set_texture(icone("bau_fechado"));
	painel->set_visible(jogadorPerto);
}

void Bau::aoAproximar(Node2D *corpo, bool perto) {
	if (!Object::cast_to<Jogador>(corpo)) {
		return;
	}
	jogadorPerto = perto;
	painel->set_visible(perto && !aberto);
	if (painel->is_visible()) {
		const Vector2 tamanho = painel->get_combined_minimum_size();
		painel->set_position(Vector2(-tamanho.x / 2.0f, -56.0f - tamanho.y));
	}
}

PanelContainer *Bau::criarPainel() {
	PanelContainer *p = memnew(PanelContainer);
	p->set_z_index(100);
	Ref<StyleBoxFlat> estilo;
	estilo.instantiate();
	estilo->set_bg_color(Color::html("#f3e6cf"));
	estilo->set_border_color(cor_texto());
	estilo->set_border_width_all(2);
	estilo->set_corner_radius_all(4);
	estilo->set_content_margin_all(8);
	p->add_theme_stylebox_override("panel", estilo);

	VBoxContainer *linhas = memnew(VBoxContainer);
	linhas->add_theme_constant_override("separation", 2);
	p->add_child(linhas);
	linhas->add_child(linha(tipo["nome"], 15, cor_texto()));
	const Array itens = tipo["itens"];
	for (int i = 0; i < itens.size(); i++) {
		const Dictionary item = itens[i];
		HBoxContainer *l = memnew(HBoxContainer);
		TextureRect *ic = memnew(TextureRect);
		ic->set_texture(icone(item["id"]));
		l->add_child(ic);
		const String texto = String(nomes[item["id"]]) + "  " + String::num_int64(int(item["chance"])) + "%  (" +
				String::num_int64(int(item["min"])) + " a " + String::num_int64(int(item["max"])) + ")";
		l->add_child(linha(texto, 13, cor_texto()));
		linhas->add_child(l);
	}
	linhas->add_child(linha(u("E: abrir o baú"), 13, cor_destaque()));
	return p;
}

Label *Bau::linha(const String &texto, int tamanho, const Color &cor) {
	Label *r = memnew(Label);
	r->set_text(texto);
	r->add_theme_font_size_override("font_size", tamanho);
	r->add_theme_color_override("font_color", cor);
	r->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	return r;
}

void Bau::_bind_methods() {
	ClassDB::bind_method(D_METHOD("is_aberto"), &Bau::isAberto);
	ADD_SIGNAL(MethodInfo("aberto", PropertyInfo(Variant::STRING, "nome_bau"), PropertyInfo(Variant::ARRAY, "obtidos")));
}
