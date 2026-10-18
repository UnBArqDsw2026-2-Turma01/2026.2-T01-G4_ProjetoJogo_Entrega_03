#pragma once

// Mundo de teste: o jogador anda pelo mapa, coleta ingredientes do chão,
// abre baús e usa a mochila (I ou Tab) para misturar. Tudo o que mexe no
// inventário passa pela fachada (Autoload "Alquimia").
//
// O chão e as posições vêm de tools/gerar_mapa.py (assets/mundo/mapa.png e
// data/mapa.json). Os inimigos, a caverna e os segredos usados pelos efeitos
// das poções (padrão Decorator) vêm de data/inimigos.json e data/exploracao.json.

#include "jogo/bau.h"
#include "jogo/inimigo_no_mapa.h"
#include "jogo/jogador.h"
#include "jogo/mochila_ui.h"
#include "jogo/runa.h"
#include "jogo/tela_de_combate.h"

#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/canvas_modulate.hpp>
#include <godot_cpp/classes/point_light2d.hpp>
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

	void _unhandled_input(const Ref<InputEvent> &evento) override;

	void alternarMochila();
	Jogador *getJogador() const { return jogador; }
	Node2D *getEntidades() const { return entidades; }

	// Efeitos das poções no mundo (também usados pelos scripts de captura).
	void iniciarCombate(int indice);
	void entrarNaCaverna();
	void sairDaCaverna();
	void revelarSegredos();
	TelaDeCombate *getTelaDeCombate() const { return telaCombate; }

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
	void colocarCenario();
	void soltarGalinhas();
	void criarHud();
	void criarMinimapa(CanvasLayer *hud);
	void desenharMarcas();
	Vector2 tamanhoMapa() const;

	void criarCaverna();
	void criarEncontros();
	void criarSegredos();
	void criarStatus(CanvasLayer *hud);
	void atualizarStatus();
	Vector2 naCaverna(const Vector2 &local) const;
	void aoAproximarDaEntrada(Node2D *corpo, bool perto);
	void aoPisarNaSaida(Node2D *corpo);
	void aoTerminarCombate(const String &resultado, const Array &recompensas);
	void aoUsarPocao(const Dictionary &resultado);
	void aoColetarRuna(const Array &obtidos);

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
	Control *molduraMinimapa = nullptr;

	Dictionary exploracao;
	Camera2D *camera = nullptr;
	CanvasModulate *escuridao = nullptr;
	PointLight2D *luz = nullptr;
	Label *dicaEntrada = nullptr;
	bool pertoDaEntrada = false;
	bool dentroDaCaverna = false;
	Vector<InimigoNoMapa *> encontros;
	Vector<Runa *> runas;
	CanvasLayer *camadaCombate = nullptr;
	TelaDeCombate *telaCombate = nullptr;
	int encontroAtual = -1;
	Label *status = nullptr;
	HBoxContainer *iconesStatus = nullptr;
	String efeitosMostrados;
	double tempoStatus = 0.0;
	double tempoDescanso = 0.0;
};
