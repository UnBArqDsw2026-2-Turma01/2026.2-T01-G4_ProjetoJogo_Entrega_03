#pragma once

// Client do padrão Facade.
//
// A mochila só conversa com o Autoload "Alquimia" (AlquimiaFacade). Ela não
// conhece inventário, catálogo, tabela de receitas, livro, criador de poção
// nem persistência: tudo isso fica escondido atrás da fachada.

#include "resultado_mistura.h"

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/grid_container.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/rich_text_label.hpp>
#include <godot_cpp/classes/style_box_texture.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/theme.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

using namespace godot;

class MochilaUI : public Control {
	GDCLASS(MochilaUI, Control)

public:
	void _ready() override;

	// Chamado pelo mundo ao abrir a mochila: o inventário pode ter mudado.
	void abrir();

	// Ações do jogador: cada uma é UMA chamada à fachada.
	void colocarNaBancada(const String &idItem);
	void misturar();

	void setPodeFechar(bool p_pode) { podeFechar = p_pode; }
	bool getPodeFechar() const { return podeFechar; }

protected:
	static void _bind_methods();

private:
	void refazer(const String &idReceita);
	void tirarDaBancada(int indice);
	void limparBancada();
	void pedirFechar();
	void mostrarDica(const Dictionary &item);

	void atualizar();
	void atualizarInventario();
	void atualizarBancada();
	void atualizarLivro();
	void mostrarResultado(const Ref<ResultadoMistura> &resultado);
	void mostrarAviso(const String &texto);
	String descreverItem(const Dictionary &item) const;
	static String formatarElementos(const Array &elementos);

	void montarTela();
	Control *montarMochila();
	Control *montarBancadaELivro();
	Button *criarSlotInventario(const Dictionary &item);
	Control *criarLinhaReceita(const Dictionary &receita);

	Ref<Theme> criarTema() const;
	static Ref<StyleBoxTexture> estilo(const Ref<Texture2D> &textura, const Color &cor = Color(1, 1, 1));
	Button *slot(const Ref<Texture2D> &textura, const Vector2 &tamanho, bool escuro);
	Button *botao(const String &texto, const Callable &acao);
	static Label *titulo(const String &texto);
	static Label *rotulo(const String &texto, const Color &cor);
	static TextureRect *miniIcone(const String &id);

	bool podeFechar = false;
	String bancada[2];

	GridContainer *grade = nullptr;
	Label *dica = nullptr;
	Button *slotsBancada[2] = { nullptr, nullptr };
	TextureRect *slotResultado = nullptr;
	RichTextLabel *resultado = nullptr;
	Label *contadorLivro = nullptr;
	VBoxContainer *listaLivro = nullptr;
};
