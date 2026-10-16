#include "jogo/mochila_ui.h"

#include "jogo/comum.h"

#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/margin_container.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/scroll_container.hpp>
#include <godot_cpp/classes/style_box_empty.hpp>
#include <godot_cpp/classes/text_server.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace jogo;

static const int TOTAL_RECEITAS = 36;

void MochilaUI::_ready() {
	set_theme(criarTema());
	montarTela();
	atualizar();
}

void MochilaUI::abrir() {
	limparBancada();
	resultado->set_text(u("Escolha dois itens da mochila e clique em Misturar."));
	atualizar();
}



void MochilaUI::misturar() {
	if (bancada[0].is_empty() || bancada[1].is_empty()) {
		mostrarAviso(u("Coloque dois itens na bancada."));
		return;
	}
	const Ref<ResultadoMistura> r = fachada(this)->combinar(bancada[0], bancada[1]);
	mostrarResultado(r);
	if (r->isSucesso()) {
		bancada[0] = "";
		bancada[1] = "";
	}
	atualizar();
}

void MochilaUI::refazer(const String &idReceita) {
	mostrarResultado(fachada(this)->refazerReceita(idReceita));
	atualizar();
}

// --- Bancada ---

void MochilaUI::colocarNaBancada(const String &idItem) {
	for (String &espaco : bancada) {
		if (espaco.is_empty()) {
			espaco = idItem;
			atualizarBancada();
			return;
		}
	}
	mostrarAviso(u("A bancada já tem dois itens. Clique em um slot da bancada para tirá-lo."));
}

void MochilaUI::tirarDaBancada(int indice) {
	bancada[indice] = "";
	atualizarBancada();
}

void MochilaUI::limparBancada() {
	bancada[0] = "";
	bancada[1] = "";
	atualizarBancada();
}

void MochilaUI::pedirFechar() {
	emit_signal("fechar_pedido");
}

void MochilaUI::mostrarDica(const Dictionary &item) {
	dica->set_text(descreverItem(item));
}

// --- Atualização da tela (só consultas à fachada) ---

void MochilaUI::atualizar() {
	atualizarInventario();
	atualizarBancada();
	atualizarLivro();
}

static void esvaziar(Node *no) {
	const TypedArray<Node> filhos = no->get_children();
	for (int i = 0; i < filhos.size(); i++) {
		Object::cast_to<Node>(filhos[i])->queue_free();
	}
}

void MochilaUI::atualizarInventario() {
	esvaziar(grade);
	const Array itens = fachada(this)->listarInventario();
	for (int i = 0; i < itens.size(); i++) {
		grade->add_child(criarSlotInventario(itens[i]));
	}
}

void MochilaUI::atualizarBancada() {
	if (!slotsBancada[0]) {
		return;
	}
	for (int i = 0; i < 2; i++) {
		slotsBancada[i]->set_button_icon(icone(bancada[i]));
		slotsBancada[i]->set_tooltip_text(bancada[i].is_empty() ? String() : u("Clique para tirar da bancada"));
	}
}

void MochilaUI::atualizarLivro() {
	esvaziar(listaLivro);
	AlquimiaFacade *alquimiaFacade = fachada(this);
	const PackedStringArray receitas = alquimiaFacade->listarReceitasDescobertas();
	contadorLivro->set_text(u("Livro do Aventureiro  (") + String::num_int64(receitas.size()) + "/" + String::num_int64(TOTAL_RECEITAS) + u(" receitas)"));
	if (receitas.is_empty()) {
		listaLivro->add_child(rotulo(u("Nenhuma receita descoberta ainda. Misture dois ingredientes!"), cor_suave()));
	}
	for (int i = 0; i < receitas.size(); i++) {
		listaLivro->add_child(criarLinhaReceita(alquimiaFacade->descreverReceita(receitas[i])));
	}
}

void MochilaUI::mostrarResultado(const Ref<ResultadoMistura> &r) {
	if (!r->isSucesso()) {
		slotResultado->set_texture(Ref<Texture2D>());
		resultado->set_text(String("[color=#") + cor_erro().to_html(false) + "]" + r->getMensagem() + "[/color]");
		return;
	}

	const Dictionary pocao = r->getPocao();
	slotResultado->set_texture(icone(pocao["id"]));
	String texto;
	if (r->isReceitaNova()) {
		texto += String("[color=#") + cor_destaque().to_html(false) + "]" + u("Receita nova descoberta!") + "[/color]\n";
	}
	const Color corPocao = pocao["cor"];
	texto += String("[color=#") + corPocao.darkened(0.35).to_html(false) + "]" + String(pocao["nome"]) + "[/color]: " + String(pocao["efeito"]) + "\n";
	const Array reveladas = r->getComposicoesReveladas();
	for (int i = 0; i < reveladas.size(); i++) {
		const Dictionary c = reveladas[i];
		texto += u("Composição de [color=#7a3b5a]") + String(c["nome"]) + "[/color]: " + formatarElementos(c["elementos"]) + "\n";
	}
	resultado->set_text(texto);
}

void MochilaUI::mostrarAviso(const String &texto) {
	resultado->set_text(String("[color=#") + cor_suave().to_html(false) + "]" + texto + "[/color]");
}

String MochilaUI::descreverItem(const Dictionary &item) const {
	const String nome = item["nome"];
	if (String(item["tipo"]) == "pocao") {
		return nome + String(": ") + String(item["efeito"]) + u(" (poções não entram na bancada)");
	}
	const Array elementos = fachada(this)->consultarComposicao(item["id"]);
	if (elementos.is_empty()) {
		return nome + u(": composição desconhecida. Use em uma mistura para descobrir.");
	}
	return nome + String(": ") + formatarElementos(elementos);
}

String MochilaUI::formatarElementos(const Array &elementos) {
	PackedStringArray partes;
	for (int i = 0; i < elementos.size(); i++) {
		const Dictionary e = elementos[i];
		partes.push_back(String(e["simbolo"]) + " (" + String(e["nome"]) + ")");
	}
	return String(", ").join(partes);
}

// --- Construção da tela ---

void MochilaUI::montarTela() {
	TextureRect *fundo = memnew(TextureRect);
	fundo->set_texture(textura("res://assets/ui/board.png"));
	fundo->set_stretch_mode(TextureRect::STRETCH_SCALE);
	fundo->set_anchors_preset(PRESET_FULL_RECT);
	add_child(fundo);

	MarginContainer *margem = memnew(MarginContainer);
	margem->set_anchors_preset(PRESET_FULL_RECT);
	for (const char *lado : { "margin_left", "margin_right", "margin_top", "margin_bottom" }) {
		margem->add_theme_constant_override(lado, 40);
	}
	add_child(margem);

	HBoxContainer *colunas = memnew(HBoxContainer);
	colunas->add_theme_constant_override("separation", 40);
	margem->add_child(colunas);

	colunas->add_child(montarMochila());
	colunas->add_child(montarBancadaELivro());
}

static ScrollContainer *rolagem_vertical(Control *conteudo) {
	ScrollContainer *rolagem = memnew(ScrollContainer);
	rolagem->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	rolagem->set_horizontal_scroll_mode(ScrollContainer::SCROLL_MODE_DISABLED);
	rolagem->add_child(conteudo);
	return rolagem;
}

Control *MochilaUI::montarMochila() {
	VBoxContainer *coluna = memnew(VBoxContainer);
	coluna->set_custom_minimum_size(Vector2(520, 0));
	coluna->add_theme_constant_override("separation", 12);
	coluna->add_child(titulo("Mochila"));

	grade = memnew(GridContainer);
	grade->set_columns(6);
	grade->add_theme_constant_override("h_separation", 8);
	grade->add_theme_constant_override("v_separation", 8);
	coluna->add_child(rolagem_vertical(grade));

	dica = rotulo(u("Passe o mouse sobre um item para ver os detalhes. Clique para levá-lo à bancada."), cor_suave());
	dica->set_custom_minimum_size(Vector2(0, 52));
	dica->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	coluna->add_child(dica);

	HBoxContainer *botoes = memnew(HBoxContainer);
	botoes->add_theme_constant_override("separation", 12);
	if (podeFechar) {
		botoes->add_child(botao(u("Fechar (I)"), callable_mp(this, &MochilaUI::pedirFechar)));
	}
	coluna->add_child(botoes);
	return coluna;
}

Control *MochilaUI::montarBancadaELivro() {
	VBoxContainer *coluna = memnew(VBoxContainer);
	coluna->set_h_size_flags(SIZE_EXPAND_FILL);
	coluna->add_theme_constant_override("separation", 12);
	coluna->add_child(titulo("Bancada de Alquimia"));

	HBoxContainer *linha = memnew(HBoxContainer);
	linha->set_alignment(BoxContainer::ALIGNMENT_CENTER);
	linha->add_theme_constant_override("separation", 12);
	for (int i = 0; i < 2; i++) {
		slotsBancada[i] = slot(textura("res://assets/ui/button_a.png"), Vector2(88, 88), true);
		slotsBancada[i]->connect("pressed", callable_mp(this, &MochilaUI::tirarDaBancada).bind(i));
		linha->add_child(slotsBancada[i]);
		linha->add_child(titulo(i == 0 ? "+" : "="));
	}
	slotResultado = memnew(TextureRect);
	slotResultado->set_custom_minimum_size(Vector2(88, 88));
	slotResultado->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
	slotResultado->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
	PanelContainer *moldura = memnew(PanelContainer);
	moldura->add_theme_stylebox_override("panel", estilo(textura("res://assets/ui/button_a.png")));
	moldura->add_child(slotResultado);
	linha->add_child(moldura);
	coluna->add_child(linha);

	HBoxContainer *acoes = memnew(HBoxContainer);
	acoes->set_alignment(BoxContainer::ALIGNMENT_CENTER);
	acoes->add_theme_constant_override("separation", 12);
	acoes->add_child(botao("Misturar", callable_mp(this, &MochilaUI::misturar)));
	acoes->add_child(botao("Limpar bancada", callable_mp(this, &MochilaUI::limparBancada)));
	coluna->add_child(acoes);

	resultado = memnew(RichTextLabel);
	resultado->set_use_bbcode(true);
	resultado->set_custom_minimum_size(Vector2(0, 150));
	resultado->add_theme_font_size_override("normal_font_size", 16);
	resultado->set_text(u("Escolha dois itens da mochila e clique em Misturar."));
	coluna->add_child(resultado);

	contadorLivro = titulo("Livro do Aventureiro");
	coluna->add_child(contadorLivro);
	listaLivro = memnew(VBoxContainer);
	listaLivro->set_h_size_flags(SIZE_EXPAND_FILL);
	listaLivro->add_theme_constant_override("separation", 6);
	coluna->add_child(rolagem_vertical(listaLivro));
	return coluna;
}

Button *MochilaUI::criarSlotInventario(const Dictionary &item) {
	Button *s = slot(textura("res://assets/ui/button_b.png"), Vector2(76, 76), false);
	s->set_button_icon(icone(item["id"]));
	s->connect("pressed", callable_mp(this, &MochilaUI::colocarNaBancada).bind(item["id"]));
	s->connect("mouse_entered", callable_mp(this, &MochilaUI::mostrarDica).bind(item));

	Label *quantidade = memnew(Label);
	quantidade->set_text(String("x") + String::num_int64(int64_t(item["quantidade"])));
	quantidade->add_theme_font_size_override("font_size", 14);
	quantidade->set_anchors_and_offsets_preset(PRESET_BOTTOM_RIGHT);
	quantidade->set_position(quantidade->get_position() - Vector2(8, 4));
	quantidade->set_h_grow_direction(GROW_DIRECTION_BEGIN);
	quantidade->set_v_grow_direction(GROW_DIRECTION_BEGIN);
	s->add_child(quantidade);
	return s;
}

Control *MochilaUI::criarLinhaReceita(const Dictionary &receita) {
	HBoxContainer *linha = memnew(HBoxContainer);
	linha->add_theme_constant_override("separation", 6);
	const Dictionary pocao = receita["pocao"];
	linha->add_child(miniIcone(receita["ingrediente_a"]));
	linha->add_child(rotulo("+", cor_texto()));
	linha->add_child(miniIcone(receita["ingrediente_b"]));
	linha->add_child(rotulo("=", cor_texto()));
	linha->add_child(miniIcone(pocao["id"]));

	Label *nome = rotulo(pocao["nome"], cor_texto());
	nome->set_h_size_flags(SIZE_EXPAND_FILL);
	nome->set_tooltip_text(String(receita["nome_a"]) + " + " + String(receita["nome_b"]) + ": " + String(pocao["efeito"]));
	nome->set_mouse_filter(MOUSE_FILTER_PASS);
	linha->add_child(nome);

	const bool podeRefazer = receita["pode_refazer"];
	Button *refazer = botao("Refazer", callable_mp(this, &MochilaUI::refazer).bind(receita["id"]));
	refazer->set_disabled(!podeRefazer);
	refazer->set_tooltip_text(podeRefazer ? String() : u("Faltam ingredientes na mochila."));
	linha->add_child(refazer);
	return linha;
}

// --- Fábricas de widgets ---

Ref<Theme> MochilaUI::criarTema() const {
	const Ref<Texture2D> claro = textura("res://assets/ui/button_b.png");
	Ref<Theme> tema;
	tema.instantiate();
	tema->set_default_font_size(18);
	tema->set_color("font_color", "Label", cor_texto());
	tema->set_color("default_color", "RichTextLabel", cor_texto());
	for (const char *estado : { "font_color", "font_hover_color", "font_pressed_color", "font_focus_color" }) {
		tema->set_color(estado, "Button", cor_texto());
	}
	tema->set_color("font_disabled_color", "Button", cor_suave());
	tema->set_stylebox("normal", "Button", estilo(claro));
	tema->set_stylebox("hover", "Button", estilo(claro, Color(0.92, 0.88, 0.82)));
	tema->set_stylebox("pressed", "Button", estilo(claro, Color(0.8, 0.75, 0.7)));
	Ref<StyleBoxEmpty> vazio;
	vazio.instantiate();
	tema->set_stylebox("focus", "Button", vazio);
	tema->set_stylebox("disabled", "Button", estilo(claro, Color(1, 1, 1, 0.5)));
	return tema;
}

Ref<StyleBoxTexture> MochilaUI::estilo(const Ref<Texture2D> &tex, const Color &cor) {
	Ref<StyleBoxTexture> e;
	e.instantiate();
	e->set_texture(tex);
	e->set_modulate(cor);
	e->set_texture_margin_all(10);
	e->set_content_margin_all(12);
	return e;
}

Button *MochilaUI::slot(const Ref<Texture2D> &tex, const Vector2 &tamanho, bool escuro) {
	Button *s = memnew(Button);
	s->set_custom_minimum_size(tamanho);
	s->set_expand_icon(true);
	s->set_icon_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	s->set_vertical_icon_alignment(VERTICAL_ALIGNMENT_CENTER);
	if (escuro) {
		s->add_theme_stylebox_override("normal", estilo(tex));
		s->add_theme_stylebox_override("hover", estilo(tex, Color(0.9, 0.9, 0.9)));
		s->add_theme_stylebox_override("pressed", estilo(tex, Color(0.8, 0.8, 0.8)));
	}
	return s;
}

Button *MochilaUI::botao(const String &texto, const Callable &acao) {
	Button *b = memnew(Button);
	b->set_text(texto);
	b->set_custom_minimum_size(Vector2(0, 44));
	b->connect("pressed", acao);
	return b;
}

Label *MochilaUI::titulo(const String &texto) {
	Label *r = rotulo(texto, cor_texto());
	r->add_theme_font_size_override("font_size", 26);
	return r;
}

Label *MochilaUI::rotulo(const String &texto, const Color &cor) {
	Label *r = memnew(Label);
	r->set_text(texto);
	r->add_theme_color_override("font_color", cor);
	r->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	return r;
}

TextureRect *MochilaUI::miniIcone(const String &id) {
	TextureRect *i = memnew(TextureRect);
	i->set_texture(icone(id));
	i->set_custom_minimum_size(Vector2(32, 32));
	i->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
	i->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
	return i;
}

void MochilaUI::_bind_methods() {
	ClassDB::bind_method(D_METHOD("abrir"), &MochilaUI::abrir);
	ClassDB::bind_method(D_METHOD("colocar_na_bancada", "id_item"), &MochilaUI::colocarNaBancada);
	ClassDB::bind_method(D_METHOD("misturar"), &MochilaUI::misturar);
	ClassDB::bind_method(D_METHOD("set_pode_fechar", "pode"), &MochilaUI::setPodeFechar);
	ClassDB::bind_method(D_METHOD("get_pode_fechar"), &MochilaUI::getPodeFechar);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "pode_fechar"), "set_pode_fechar", "get_pode_fechar");
	ADD_SIGNAL(MethodInfo("fechar_pedido"));
}
