#pragma once

// Mundo de teste: o jogador anda pelo mapa, coleta ingredientes do chão,
// abre baús e usa a mochila (I ou Tab) para misturar. Tudo o que mexe no
// inventário passa pela fachada (Autoload "Alquimia").
//
// O chão e as posições vêm de tools/gerar_mapa.py (assets/mundo/mapa.png e
// data/mapa.json).

#include "jogo/bau.h"
#include "jogo/jogador.h"
#include "jogo/mochila_ui.h"

#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

class Mundo : public Node2D {
	GDCLASS(Mundo, Node2D)

public:
	void _ready() override;
	void _process(double delta) override;
	void _input(const Ref<InputEvent> &evento) override;

	void alternarMochila();
	Jogador *getJogador() const { return jogador; }
	Node2D *getEntidades() const { return entidades; }

protected:
	static void _bind_methods();

private:
	void aoColetar(const String &nome);
	void aoAbrirBau(const String &nomeBau, const Array &obtidos);
	void avisar(const String &texto);

	static void criarAcoes();
	void criarChao();
	void criarColisoes();
	void espalharIngredientes();
	void colocarBaus();
	void soltarGalinhas();
	void criarHud();
	void criarMinimapa(CanvasLayer *hud);
	void desenharMarcas();
	Vector2 tamanhoMapa() const;

	Dictionary mapa;
	Dictionary nomes; // id do ingrediente -> nome
	Node2D *entidades = nullptr;
	Jogador *jogador = nullptr;
	MochilaUI *mochila = nullptr;
	Label *aviso = nullptr;
	Ref<Tween> animacaoAviso;
	Vector<Bau *> baus;
	TextureRect *minimapa = nullptr;
	Control *marcas = nullptr;
};
