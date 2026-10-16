#pragma once

// Baú do mundo. Perto dele aparece a tabela de chances; ao abrir (E), cada
// item é sorteado pela sua porcentagem e o que sair vai para a mochila pela
// fachada (coletarIngrediente). Depois de um tempo ele reabastece.

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/static_body2d.hpp>

using namespace godot;

class Bau : public StaticBody2D {
	GDCLASS(Bau, StaticBody2D)

public:
	void _ready() override;
	void _unhandled_input(const Ref<InputEvent> &evento) override;

	// Entrada de data/baus.json: { nome, itens: [{ id, chance, min, max }] }.
	Dictionary tipo;
	// id do ingrediente -> nome, para escrever a tabela.
	Dictionary nomes;

	bool isAberto() const { return aberto; }

protected:
	static void _bind_methods();

private:
	void abrir();
	void reabastecer();
	void aoAproximar(Node2D *corpo, bool perto);
	PanelContainer *criarPainel();
	static Label *linha(const String &texto, int tamanho, const Color &cor);

	bool aberto = false;
	bool jogadorPerto = false;
	Sprite2D *sprite = nullptr;
	PanelContainer *painel = nullptr;
};
