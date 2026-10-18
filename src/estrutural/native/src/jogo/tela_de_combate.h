#pragma once

// Tela de combate por turnos. As regras ficam no núcleo do padrão Decorator
// (decorator/combate.hpp): esta tela só desenha os participantes, mostra os
// efeitos ativos de cada um (lidos da cadeia de decoradores) e anima a lista
// de eventos que o combate devolve. As poções passam pela fachada, que tira
// a poção da mochila e decora os alvos.

#include <combate.hpp>

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/grid_container.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/rich_text_label.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/texture_progress_bar.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

#include <deque>
#include <memory>
#include <vector>

using namespace godot;

class TelaDeCombate : public Control {
	GDCLASS(TelaDeCombate, Control)

public:
	// ids dos inimigos (data/inimigos.json) e a área ("mundo" ou "caverna").
	void iniciar(const PackedStringArray &idsInimigos, const String &area);

	void _unhandled_input(const Ref<InputEvent> &evento) override;

	// Ações do jogador (também usadas pelos scripts de captura).
	void acaoAtacar(int alvo);
	void acaoMagia(int alvo);
	void acaoPocao(const String &idPocao, int alvo);
	void acaoFugir();
	bool isAguardando() const;
	PackedStringArray efeitosDe(int participante) const;
	PackedInt32Array alvos() const;

protected:
	static void _bind_methods();

private:
	struct Lutador {
		Node2D *raiz = nullptr;
		AnimatedSprite2D *sprite = nullptr;
		AnimatedSprite2D *sobreposicao = nullptr;
		Vector2 base;           // pés, em pixels do palco (320x180)
		float altura = 16.0f;   // altura do desenho, para efeitos e textos
		String idInimigo;
		TextureProgressBar *barraHp = nullptr;
		TextureProgressBar *barraMp = nullptr;
		HBoxContainer *icones = nullptr;
		Label *rotuloHp = nullptr;
		Button *botaoAlvo = nullptr;
		AnimatedSprite2D *cursor = nullptr;
		String ultimaSobreposicao;
	};

	enum class Pendente { NADA, ATAQUE, MAGIA, POCAO };

	void montarTela(const String &area);
	void montarParticipantes();
	PanelContainer *montarPainelJogador();
	PanelContainer *montarPainelInimigos();
	Control *montarMenu();
	PanelContainer *montarListaDePocoes();

	// Fluxo dos eventos.
	void enfileirar(const std::vector<G4Efeitos::Evento> &eventos);
	void processarProximo();
	float animar(const G4Efeitos::Evento &evento);
	void finalizar();

	// Escolha de ação e alvo.
	void mostrarMenu(bool visivel);
	void escolherAcao(int acao);
	void abrirPocoes();
	void escolherPocao(const String &idPocao);
	void pedirAlvo(Pendente acao);
	void confirmarAlvo(int alvo);
	void cancelarEscolha();
	void executar(Pendente acao, int alvo);

	// Visual.
	void atualizarParticipantes();
	void atualizarIcones(Lutador &l, const PackedStringArray &efeitos);
	void tocarEfeito(const String &vfx, int alvo, const Color &cor, bool tingir);
	void textoFlutuante(int participante, const String &texto, const Color &cor);
	void avancarERecuar(int participante);
	void piscar(int participante, const Color &cor);
	Vector2 naTela(int participante, float alturaRelativa) const;
	void registrar(const String &texto);

	static Ref<SpriteFrames> quadros(const String &pasta, const PackedStringArray &animacoes, const String &sufixo, double fps);
	TextureProgressBar *barra(const String &preenchimento, const Vector2 &tamanho) const;

	std::unique_ptr<G4Efeitos::Combate> combate;
	std::vector<Lutador> lutadores; // 0 = jogador
	Dictionary dadosInimigos;
	std::deque<G4Efeitos::Evento> fila;
	bool processando = false;
	bool encerrado = false;

	Pendente pendente = Pendente::NADA;
	String pocaoEscolhida;
	String vfxAtual;
	Color corAtual;
	bool sopraTocado = false;

	Node2D *palco = nullptr;
	Control *sobreTela = nullptr;
	Control *menu = nullptr;
	PanelContainer *listaPocoes = nullptr;
	GridContainer *gradePocoes = nullptr;
	Label *instrucao = nullptr;
	Button *botaoMagia = nullptr;
	RichTextLabel *registro = nullptr;
	Label *faixa = nullptr;
};
