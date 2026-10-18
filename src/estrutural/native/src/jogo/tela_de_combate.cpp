#include "jogo/tela_de_combate.h"

#include "jogo/comum.h"
#include "ponte_efeitos.h"

#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/property_tweener.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/classes/style_box_flat.hpp>
#include <godot_cpp/classes/style_box_texture.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/theme.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>

using namespace jogo;
using G4Efeitos::Evento;

static const float ESCALA_PALCO = 4.0f; // o palco tem 320x180 pixels de arte
static const Vector2 POSICAO_JOGADOR(84, 132);
static const Color COR_CLARA = Color::hex(0xf3e6cfff); // hex: sem String (estático, antes do Godot iniciar)
static const Color COR_DANO = Color::hex(0xff6a5aff);
static const Color COR_CURA = Color::hex(0x8af08aff);

// Efeitos em tons de cinza: o jogo pinta com a cor da poção.
static bool efeito_neutro(const String &vfx) {
	return vfx == "nuvem" || vfx == "brilho_cura" || vfx == "aura_buff" || vfx == "aura_debuff" ||
			vfx == "escudo" || vfx == "penas" || vfx == "dissipar";
}

static String sobreposicao_de(const PackedStringArray &efeitos) {
	const char *pares[][2] = { { "sono", "zzz" }, { "confusao", "estrelas" }, { "veneno", "bolhas_veneno" }, { "cegueira", "nuvem_cega" } };
	for (const auto &par : pares) {
		if (efeitos.has(par[0])) {
			return par[1];
		}
	}
	return String();
}

static void adicionar_quadros(const Ref<SpriteFrames> &frames, const String &nome, const String &prefixo, double fps, bool repetir) {
	frames->add_animation(nome);
	frames->set_animation_speed(nome, fps);
	frames->set_animation_loop(nome, repetir);
	for (int i = 1; ResourceLoader::get_singleton()->exists(prefixo + String::num_int64(i) + ".png"); i++) {
		frames->add_frame(nome, textura(prefixo + String::num_int64(i) + ".png"));
	}
}

static Label *rotulo(const String &texto, int tamanho, const Color &cor = COR_CLARA) {
	Label *r = memnew(Label);
	r->set_text(texto);
	r->add_theme_font_size_override("font_size", tamanho);
	r->add_theme_color_override("font_color", cor);
	r->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	return r;
}

static Ref<StyleBoxFlat> estilo_painel() {
	Ref<StyleBoxFlat> e;
	e.instantiate();
	e->set_bg_color(Color(0.11, 0.08, 0.09, 0.88));
	e->set_border_color(COR_CLARA.darkened(0.3));
	e->set_border_width_all(2);
	e->set_corner_radius_all(4);
	e->set_content_margin_all(10);
	return e;
}

static PanelContainer *painel() {
	PanelContainer *p = memnew(PanelContainer);
	p->add_theme_stylebox_override("panel", estilo_painel());
	return p;
}

static Ref<StyleBoxTexture> estilo_botao(const Color &cor) {
	Ref<StyleBoxTexture> e;
	e.instantiate();
	e->set_texture(textura("res://assets/ui/button_b.png"));
	e->set_modulate(cor);
	e->set_texture_margin_all(10);
	e->set_content_margin_all(8);
	return e;
}

// --- Montagem ---

void TelaDeCombate::iniciar(const PackedStringArray &idsInimigos, const String &area) {
	dadosInimigos = Dictionary(ler_json("res://data/inimigos.json").get("inimigos", Dictionary()));

	std::vector<G4Efeitos::Inimigo> inimigos;
	for (int i = 0; i < idsInimigos.size(); i++) {
		const Dictionary dados = dadosInimigos.get(idsInimigos[i], Dictionary());
		G4Efeitos::Inimigo inimigo;
		inimigo.id = ponte::padrao(idsInimigos[i]);
		Dictionary comNome = dados.duplicate();
		if (idsInimigos.count(idsInimigos[i]) > 1) {
			comNome["nome"] = String(dados["nome"]) + " " + String::chr('A' + i); // dois iguais: A e B
		}
		inimigo.combatente = ponte::combatente(comNome);
		const Dictionary especial = dados.get("especial", Dictionary());
		if (!especial.is_empty()) {
			inimigo.chanceEspecial = int(especial["chance"]);
			inimigo.nomeEspecial = ponte::padrao(especial["nome"]);
			if (especial.has("ataque")) {
				const Dictionary ataque = especial["ataque"];
				bool ok = true;
				inimigo.especialEhAtaque = true;
				inimigo.ataqueEspecial = G4Efeitos::Ataque{ int(ataque["valor"]), G4Efeitos::elementoDeTexto(ponte::padrao(ataque.get("elemento", "")), ok), true };
			} else {
				String erro;
				inimigo.efeitoEspecial = ponte::especificacao(especial["efeito"], erro);
			}
		}
		inimigos.push_back(std::move(inimigo));
	}
	combate = std::make_unique<G4Efeitos::Combate>(fachada(this)->getJogador(), std::move(inimigos), UtilityFunctions::randi());

	lutadores.resize(combate->quantidade());
	for (int i = 1; i < combate->quantidade(); i++) {
		lutadores[i].idInimigo = idsInimigos[i - 1];
	}
	montarTela(area);
	atualizarParticipantes();
	registrar(u("Inimigos à frente!"));
	get_tree()->create_timer(0.6)->connect("timeout", callable_mp(this, &TelaDeCombate::processarProximo));
}

void TelaDeCombate::montarTela(const String &area) {
	set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	set_mouse_filter(MOUSE_FILTER_STOP);

	Ref<Theme> tema;
	tema.instantiate();
	tema->set_default_font_size(18);
	tema->set_color("font_color", "Label", COR_CLARA);
	for (const char *estado : { "font_color", "font_hover_color", "font_pressed_color", "font_focus_color" }) {
		tema->set_color(estado, "Button", cor_texto());
	}
	tema->set_color("font_disabled_color", "Button", cor_suave());
	tema->set_stylebox("normal", "Button", estilo_botao(Color(1, 1, 1)));
	tema->set_stylebox("hover", "Button", estilo_botao(Color(1.0, 0.95, 0.8)));
	tema->set_stylebox("pressed", "Button", estilo_botao(Color(0.8, 0.75, 0.7)));
	tema->set_stylebox("focus", "Button", estilo_botao(Color(1, 1, 1)));
	tema->set_stylebox("disabled", "Button", estilo_botao(Color(1, 1, 1, 0.45)));
	set_theme(tema);

	TextureRect *fundo = memnew(TextureRect);
	fundo->set_texture(textura(String("res://assets/combate/fundos/") + (area == "caverna" ? "fundo_caverna" : "fundo_floresta") + ".png"));
	fundo->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
	fundo->set_stretch_mode(TextureRect::STRETCH_SCALE);
	fundo->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	fundo->set_mouse_filter(MOUSE_FILTER_IGNORE);
	add_child(fundo);

	palco = memnew(Node2D);
	palco->set_scale(Vector2(ESCALA_PALCO, ESCALA_PALCO));
	add_child(palco);
	montarParticipantes();

	sobreTela = memnew(Control);
	sobreTela->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	sobreTela->set_mouse_filter(MOUSE_FILTER_IGNORE);
	add_child(sobreTela);

	// Botões invisíveis sobre os inimigos, para escolher o alvo com o mouse.
	for (int i = 1; i < combate->quantidade(); i++) {
		Lutador &l = lutadores[i];
		Button *b = memnew(Button);
		b->set_flat(true);
		Ref<StyleBoxFlat> transparente;
		transparente.instantiate();
		transparente->set_bg_color(Color(1, 1, 1, 0));
		for (const char *estado : { "normal", "hover", "pressed", "focus", "disabled" }) {
			b->add_theme_stylebox_override(estado, transparente);
		}
		const Vector2 tamanho = Vector2(l.altura + 8, l.altura + 8) * ESCALA_PALCO;
		b->set_position(naTela(i, 0.5f) - tamanho / 2.0f);
		b->set_size(tamanho);
		b->connect("pressed", callable_mp(this, &TelaDeCombate::confirmarAlvo).bind(i));
		b->hide();
		sobreTela->add_child(b);
		l.botaoAlvo = b;
	}

	PanelContainer *jogador = montarPainelJogador();
	jogador->set_position(Vector2(16, 556));
	jogador->set_custom_minimum_size(Vector2(400, 148));
	add_child(jogador);

	PanelContainer *inimigos = montarPainelInimigos();
	inimigos->set_position(Vector2(1280 - 16 - 340, 16));
	inimigos->set_custom_minimum_size(Vector2(340, 0));
	add_child(inimigos);

	menu = montarMenu();
	menu->set_position(Vector2(432, 556));
	add_child(menu);

	PanelContainer *caixaRegistro = painel();
	caixaRegistro->set_position(Vector2(764, 556));
	caixaRegistro->set_custom_minimum_size(Vector2(500, 148));
	registro = memnew(RichTextLabel);
	registro->set_use_bbcode(true);
	registro->set_scroll_follow(true);
	registro->set_custom_minimum_size(Vector2(480, 128));
	registro->add_theme_font_size_override("normal_font_size", 15);
	registro->add_theme_color_override("default_color", COR_CLARA);
	caixaRegistro->add_child(registro);
	add_child(caixaRegistro);

	listaPocoes = montarListaDePocoes();
	listaPocoes->hide();
	add_child(listaPocoes);

	faixa = rotulo("", 56, Color::html("#ffd23f"));
	faixa->add_theme_color_override("font_outline_color", cor_texto());
	faixa->add_theme_constant_override("outline_size", 12);
	faixa->set_anchors_and_offsets_preset(PRESET_CENTER);
	faixa->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	faixa->set_h_grow_direction(GROW_DIRECTION_BOTH);
	faixa->set_v_grow_direction(GROW_DIRECTION_BOTH);
	faixa->hide();
	add_child(faixa);

	mostrarMenu(false);
}

void TelaDeCombate::montarParticipantes() {
	const int n = combate->quantidade() - 1;
	static const Vector2 posicoes[3][3] = {
		{ Vector2(236, 126) },
		{ Vector2(220, 114), Vector2(264, 136) },
		{ Vector2(214, 104), Vector2(264, 122), Vector2(222, 148) },
	};
	Ref<SpriteFrames> status;
	status.instantiate();
	status->remove_animation("default");
	for (const char *nome : { "zzz", "estrelas", "bolhas_veneno", "nuvem_cega" }) {
		adicionar_quadros(status, nome, String("res://assets/combate/status/") + nome + "/frame_", 5.0, true);
	}
	Ref<SpriteFrames> cursor;
	cursor.instantiate();
	adicionar_quadros(cursor, "pulo", "res://assets/combate/ui/cursor_alvo_frame_", 3.0, true);

	for (int i = 0; i <= n; i++) {
		Lutador &l = lutadores[i];
		l.base = i == 0 ? POSICAO_JOGADOR : posicoes[MIN(n, 3) - 1][MIN(i - 1, 2)];
		l.raiz = memnew(Node2D);
		l.raiz->set_position(l.base);
		palco->add_child(l.raiz);

		Ref<SpriteFrames> frames;
		frames.instantiate();
		frames->remove_animation("default");
		l.sprite = memnew(AnimatedSprite2D);
		if (i == 0) {
			// O mesmo personagem da exploração (TinyPixel ou placeholder), 16 px de arte.
			adicionar_quadros(frames, "idle", "res://assets/mundo/jogador/idle_r_frame_", 8.0, true);
			l.sprite->set_scale(Vector2(1, 1) / 3.0f);
			l.sprite->set_offset(Vector2(0, -18));
			l.altura = 16.0f;
		} else {
			const String pasta = String("res://assets/combate/inimigos/") + l.idInimigo + "/";
			adicionar_quadros(frames, "idle", pasta + String("idle_l_frame_"), 5.0, true);
			adicionar_quadros(frames, "ataque", pasta + String("ataque_l_frame_"), 10.0, false);
			adicionar_quadros(frames, "dano", pasta + String("dano_l_frame_"), 8.0, false);
			adicionar_quadros(frames, "derrota", pasta + String("derrota_l_frame_"), 6.0, false);
			if (frames->get_frame_count("idle") > 0) {
				l.altura = frames->get_frame_texture("idle", 0)->get_height();
			}
			l.sprite->set_offset(Vector2(0, -l.altura / 2.0f));
		}
		l.sprite->set_sprite_frames(frames);
		l.sprite->play("idle");

		Sprite2D *sombra = memnew(Sprite2D);
		sombra->set_texture(textura("res://assets/combate/ui/sombra.png"));
		sombra->set_scale(Vector2(1, 1) * (l.altura / 32.0f + 0.25f));
		l.raiz->add_child(sombra);
		l.raiz->add_child(l.sprite);

		l.sobreposicao = memnew(AnimatedSprite2D);
		l.sobreposicao->set_sprite_frames(status);
		l.sobreposicao->set_position(Vector2(0, -l.altura - 6));
		l.sobreposicao->hide();
		l.raiz->add_child(l.sobreposicao);

		if (i > 0) {
			l.cursor = memnew(AnimatedSprite2D);
			l.cursor->set_sprite_frames(cursor);
			l.cursor->set_position(Vector2(0, -l.altura - 14));
			l.cursor->play("pulo");
			l.cursor->hide();
			l.raiz->add_child(l.cursor);
		}
	}
}

TextureProgressBar *TelaDeCombate::barra(const String &preenchimento, const Vector2 &tamanho) const {
	TextureProgressBar *b = memnew(TextureProgressBar);
	b->set_under_texture(textura("res://assets/combate/ui/barra_fundo.png"));
	b->set_progress_texture(textura(String("res://assets/combate/ui/") + preenchimento + ".png"));
	b->set_over_texture(textura("res://assets/combate/ui/barra_moldura.png"));
	b->set_nine_patch_stretch(true);
	for (Side lado : { SIDE_LEFT, SIDE_RIGHT, SIDE_TOP, SIDE_BOTTOM }) {
		b->set_stretch_margin(lado, 1);
	}
	b->set_custom_minimum_size(tamanho);
	b->set_v_size_flags(SIZE_SHRINK_CENTER);
	b->set_mouse_filter(MOUSE_FILTER_IGNORE);
	return b;
}

PanelContainer *TelaDeCombate::montarPainelJogador() {
	Lutador &l = lutadores[0];
	PanelContainer *p = painel();
	VBoxContainer *linhas = memnew(VBoxContainer);
	linhas->add_theme_constant_override("separation", 6);
	p->add_child(linhas);
	linhas->add_child(rotulo(ponte::texto(combate->participante(0).personagem().nome()), 20));

	for (int i = 0; i < 2; i++) {
		HBoxContainer *linha = memnew(HBoxContainer);
		linha->add_theme_constant_override("separation", 8);
		linha->add_child(rotulo(i == 0 ? "HP" : "MP", 16));
		TextureProgressBar *b = barra(i == 0 ? "barra_hp" : "barra_mp", Vector2(220, 16));
		linha->add_child(b);
		Label *valor = rotulo("", 16);
		linha->add_child(valor);
		linhas->add_child(linha);
		if (i == 0) {
			l.barraHp = b;
			l.rotuloHp = valor;
		} else {
			l.barraMp = b;
			valor->set_name("RotuloMp");
		}
	}
	l.icones = memnew(HBoxContainer);
	l.icones->set_custom_minimum_size(Vector2(0, 28));
	linhas->add_child(l.icones);
	return p;
}

PanelContainer *TelaDeCombate::montarPainelInimigos() {
	PanelContainer *p = painel();
	VBoxContainer *linhas = memnew(VBoxContainer);
	linhas->add_theme_constant_override("separation", 8);
	p->add_child(linhas);
	for (int i = 1; i < combate->quantidade(); i++) {
		Lutador &l = lutadores[i];
		VBoxContainer *bloco = memnew(VBoxContainer);
		bloco->add_theme_constant_override("separation", 2);
		HBoxContainer *cabecalho = memnew(HBoxContainer);
		cabecalho->add_theme_constant_override("separation", 8);
		Label *nome = rotulo(String::num_int64(i) + ". " + ponte::texto(combate->participante(i).personagem().nome()), 16);
		nome->set_h_size_flags(SIZE_EXPAND_FILL);
		cabecalho->add_child(nome);
		l.rotuloHp = rotulo("", 14);
		cabecalho->add_child(l.rotuloHp);
		bloco->add_child(cabecalho);
		l.barraHp = barra("barra_hp", Vector2(318, 12));
		bloco->add_child(l.barraHp);
		l.icones = memnew(HBoxContainer);
		l.icones->set_custom_minimum_size(Vector2(0, 24));
		bloco->add_child(l.icones);
		linhas->add_child(bloco);
	}
	return p;
}

Control *TelaDeCombate::montarMenu() {
	PanelContainer *p = painel();
	p->set_custom_minimum_size(Vector2(316, 148));
	VBoxContainer *coluna = memnew(VBoxContainer);
	coluna->add_theme_constant_override("separation", 8);
	p->add_child(coluna);
	instrucao = rotulo("", 15, Color::html("#ffd23f"));
	coluna->add_child(instrucao);
	GridContainer *grade = memnew(GridContainer);
	grade->set_columns(2);
	grade->add_theme_constant_override("h_separation", 8);
	grade->add_theme_constant_override("v_separation", 8);
	const String nomes[] = { "1. Atacar", u("2. Magia (") + String::num_int64(G4Efeitos::Combate::CUSTO_MAGIA) + " MP)", u("3. Poções"), "4. Fugir" };
	for (int i = 0; i < 4; i++) {
		Button *b = memnew(Button);
		b->set_text(nomes[i]);
		b->set_custom_minimum_size(Vector2(144, 40));
		b->connect("pressed", callable_mp(this, &TelaDeCombate::escolherAcao).bind(i));
		grade->add_child(b);
		if (i == 1) {
			botaoMagia = b;
		}
	}
	coluna->add_child(grade);
	return p;
}

PanelContainer *TelaDeCombate::montarListaDePocoes() {
	PanelContainer *p = painel();
	p->set_custom_minimum_size(Vector2(560, 0));
	VBoxContainer *coluna = memnew(VBoxContainer);
	coluna->add_theme_constant_override("separation", 10);
	p->add_child(coluna);
	coluna->add_child(rotulo(u("Poções da mochila"), 22));
	gradePocoes = memnew(GridContainer);
	gradePocoes->set_columns(6);
	gradePocoes->add_theme_constant_override("h_separation", 8);
	gradePocoes->add_theme_constant_override("v_separation", 8);
	coluna->add_child(gradePocoes);
	Button *voltar = memnew(Button);
	voltar->set_text("Voltar (Esc)");
	voltar->connect("pressed", callable_mp(this, &TelaDeCombate::cancelarEscolha));
	coluna->add_child(voltar);
	return p;
}

// --- Fluxo dos eventos ---

void TelaDeCombate::enfileirar(const std::vector<Evento> &eventos) {
	fila.insert(fila.end(), eventos.begin(), eventos.end());
	if (!processando) {
		processarProximo();
	}
}

void TelaDeCombate::processarProximo() {
	if (encerrado) {
		return;
	}
	if (fila.empty()) {
		processando = false;
		if (combate->terminou()) {
			finalizar();
			return;
		}
		if (combate->aguardandoJogador()) {
			mostrarMenu(true);
			return;
		}
		const std::vector<Evento> eventos = combate->avancar();
		if (eventos.empty()) {
			mostrarMenu(true);
			return;
		}
		fila.insert(fila.end(), eventos.begin(), eventos.end());
	}
	processando = true;
	const Evento evento = fila.front();
	fila.pop_front();
	const float duracao = animar(evento);
	atualizarParticipantes();
	get_tree()->create_timer(duracao)->connect("timeout", callable_mp(this, &TelaDeCombate::processarProximo));
}

float TelaDeCombate::animar(const Evento &e) {
	if (e.tipo != Evento::Tipo::VEZ) {
		registrar(ponte::texto(e.texto));
	}
	switch (e.tipo) {
		case Evento::Tipo::VEZ:
			return 0.05f;
		case Evento::Tipo::POR_TURNO:
			textoFlutuante(e.alvo, (e.valor > 0 ? "+" : "") + String::num_int64(e.valor), e.valor > 0 ? COR_CURA : Color::html("#c07af0"));
			piscar(e.alvo, e.valor > 0 ? COR_CURA : Color::html("#c07af0"));
			return 0.7f;
		case Evento::Tipo::NAO_AGE:
			textoFlutuante(e.origem, "Zzz...", COR_CLARA);
			return 0.7f;
		case Evento::Tipo::ATAQUE:
			avancarERecuar(e.origem);
			return 0.45f;
		case Evento::Tipo::MAGIA:
			avancarERecuar(e.origem);
			tocarEfeito("raio", e.alvo, Color(1, 1, 1), false);
			return 0.6f;
		case Evento::Tipo::ESPECIAL: {
			avancarERecuar(e.origem);
			const G4Efeitos::Inimigo &inimigo = combate->inimigo(e.origem);
			textoFlutuante(e.origem, ponte::texto(inimigo.nomeEspecial) + "!", Color::html("#ffd23f"));
			if (inimigo.especialEhAtaque) {
				tocarEfeito("fogo", 0, Color(1, 1, 1), false);
			}
			return 0.7f;
		}
		case Evento::Tipo::DANO:
			piscar(e.alvo, COR_DANO);
			textoFlutuante(e.alvo, "-" + String::num_int64(e.valor), COR_DANO);
			if (e.elemento == G4Efeitos::Elemento::FOGO && e.alvo != 0) {
				tocarEfeito("fogo", e.alvo, Color(1, 1, 1), false); // golpe com o Óleo de Salamandra
			}
			return 0.6f;
		case Evento::Tipo::ERROU:
			textoFlutuante(e.alvo, "Errou!", COR_CLARA);
			return 0.5f;
		case Evento::Tipo::ESQUIVOU:
			textoFlutuante(e.alvo, "Esquivou!", Color::html("#9fe7ff"));
			tocarEfeito("penas", e.alvo, Color(1, 1, 1), false);
			return 0.6f;
		case Evento::Tipo::REFLETIU:
			textoFlutuante(e.alvo, "Refletiu!", Color::html("#9fe7ff"));
			tocarEfeito("prisma", e.alvo, Color(1, 1, 1), false);
			return 0.7f;
		case Evento::Tipo::SEM_ALVO:
			textoFlutuante(e.origem, "Sem alvo...", COR_CLARA);
			return 0.6f;
		case Evento::Tipo::AVISO:
			if (e.alvo >= 0 && e.origem >= 0) {
				textoFlutuante(e.origem, "Confuso!", Color::html("#ffd23f"));
				return 0.5f;
			}
			return 0.2f;
		case Evento::Tipo::POCAO: {
			// O frasco da poção sobe sobre o jogador.
			Sprite2D *frasco = memnew(Sprite2D);
			frasco->set_texture(icone(pocaoEscolhida));
			frasco->set_position(lutadores[0].base + Vector2(10, -lutadores[0].altura - 4));
			palco->add_child(frasco);
			Ref<Tween> t = create_tween();
			t->tween_property(frasco, "position:y", frasco->get_position().y - 10, 0.45);
			t->parallel()->tween_property(frasco, "modulate:a", 0.0, 0.45);
			t->tween_callback(callable_mp((Node *)frasco, &Node::queue_free));
			return 0.5f;
		}
		case Evento::Tipo::EFEITO: {
			if (e.origem > 0) {
				tocarEfeito("nuvem", e.alvo, Color::html("#7ac74f"), true); // esporos do inimigo
				return 0.7f;
			}
			if (vfxAtual == "sopro_dragao") {
				if (!sopraTocado) {
					tocarEfeito(vfxAtual, -1, Color(1, 1, 1), false);
					sopraTocado = true;
				}
			} else if (e.alvo >= 0) {
				tocarEfeito(vfxAtual, e.alvo, corAtual, efeito_neutro(vfxAtual));
			}
			if (e.valor > 0 && e.alvo > 0 && (vfxAtual == "fogo" || vfxAtual == "raio" || vfxAtual == "sopro_dragao")) {
				textoFlutuante(e.alvo, "-" + String::num_int64(e.valor), COR_DANO);
				piscar(e.alvo, COR_DANO);
			} else if (e.valor > 0 && e.alvo == 0 && vfxAtual == "brilho_cura") {
				textoFlutuante(0, "+" + String::num_int64(e.valor), COR_CURA);
			}
			return 0.75f;
		}
		case Evento::Tipo::DERROTADO: {
			Lutador &l = lutadores[e.alvo];
			if (e.alvo > 0 && l.sprite->get_sprite_frames()->get_frame_count("derrota") > 0) {
				l.sprite->play("derrota");
			}
			Ref<Tween> t = create_tween();
			t->tween_interval(0.6);
			t->tween_property(l.raiz, "modulate:a", 0.0, 0.4);
			return 1.0f;
		}
		case Evento::Tipo::VITORIA:
		case Evento::Tipo::DERROTA:
		case Evento::Tipo::FUGIU:
			faixa->set_text(e.tipo == Evento::Tipo::VITORIA ? u("Vitória!") : e.tipo == Evento::Tipo::DERROTA ? u("Derrota...") : u("Você fugiu!"));
			faixa->add_theme_color_override("font_color", e.tipo == Evento::Tipo::DERROTA ? COR_DANO : Color::html("#ffd23f"));
			faixa->show();
			return 1.6f;
		case Evento::Tipo::FUGA_FALHOU:
			textoFlutuante(0, u("Não deu!"), COR_CLARA);
			return 0.6f;
	}
	return 0.3f;
}

void TelaDeCombate::finalizar() {
	if (encerrado) {
		return;
	}
	encerrado = true;
	mostrarMenu(false);
	String resultado = "fuga";
	Array recompensas;
	if (combate->resultado() == G4Efeitos::Combate::Resultado::VITORIA) {
		resultado = "vitoria";
		Dictionary nomes;
		const Array ingredientes = fachada(this)->listarIngredientes();
		for (int i = 0; i < ingredientes.size(); i++) {
			const Dictionary d = ingredientes[i];
			nomes[d["id"]] = d["nome"];
		}
		Dictionary total;
		for (int i = 1; i < combate->quantidade(); i++) {
			const Array lista = Dictionary(dadosInimigos.get(lutadores[i].idInimigo, Dictionary())).get("recompensas", Array());
			for (int j = 0; j < lista.size(); j++) {
				const Dictionary r = lista[j];
				const int quantidade = UtilityFunctions::randi_range(int(r["min"]), int(r["max"]));
				if (quantidade > 0 && fachada(this)->coletarIngrediente(r["id"], quantidade)) {
					total[r["id"]] = int(total.get(r["id"], 0)) + quantidade;
				}
			}
		}
		const Array ids = total.keys();
		for (int i = 0; i < ids.size(); i++) {
			Dictionary item;
			item["nome"] = nomes[ids[i]];
			item["quantidade"] = total[ids[i]];
			recompensas.push_back(item);
		}
	} else if (combate->resultado() == G4Efeitos::Combate::Resultado::DERROTA) {
		resultado = "derrota";
	}
	emit_signal("terminou", resultado, recompensas);
}

// --- Escolha de ação e alvo ---

void TelaDeCombate::mostrarMenu(bool visivel) {
	if (!menu) {
		return;
	}
	const bool ativo = visivel && !encerrado;
	for (int i = 0; i < menu->get_child(0)->get_child(1)->get_child_count(); i++) {
		Object::cast_to<Button>(menu->get_child(0)->get_child(1)->get_child(i))->set_disabled(!ativo);
	}
	if (ativo) {
		const G4Efeitos::Personagem &p = combate->participante(0).personagem();
		botaoMagia->set_disabled(p.mp() < G4Efeitos::Combate::CUSTO_MAGIA);
		instrucao->set_text(u("Sua vez! Escolha uma ação (1 a 4)."));
	} else {
		instrucao->set_text(encerrado ? String() : u("Aguarde..."));
	}
}

void TelaDeCombate::escolherAcao(int acao) {
	if (!isAguardando() || pendente != Pendente::NADA) {
		return;
	}
	switch (acao) {
		case 0: pedirAlvo(Pendente::ATAQUE); break;
		case 1: pedirAlvo(Pendente::MAGIA); break;
		case 2: abrirPocoes(); break;
		case 3: acaoFugir(); break;
	}
}

void TelaDeCombate::abrirPocoes() {
	const TypedArray<Node> filhos = gradePocoes->get_children();
	for (int i = 0; i < filhos.size(); i++) {
		Object::cast_to<Node>(filhos[i])->queue_free();
	}
	const Array itens = fachada(this)->listarInventario();
	int quantas = 0;
	for (int i = 0; i < itens.size(); i++) {
		const Dictionary item = itens[i];
		if (String(item["tipo"]) != "pocao") {
			continue;
		}
		Button *b = memnew(Button);
		b->set_button_icon(icone(item["id"]));
		b->set_expand_icon(true);
		b->set_custom_minimum_size(Vector2(76, 76));
		b->set_tooltip_text(String(item["nome"]) + ": " + String(item["efeito"]));
		b->connect("pressed", callable_mp(this, &TelaDeCombate::escolherPocao).bind(item["id"]));
		Label *quantidade = rotulo(String("x") + String::num_int64(int64_t(item["quantidade"])), 14, cor_texto());
		quantidade->set_anchors_and_offsets_preset(PRESET_BOTTOM_RIGHT);
		quantidade->set_position(quantidade->get_position() - Vector2(8, 4));
		quantidade->set_h_grow_direction(GROW_DIRECTION_BEGIN);
		quantidade->set_v_grow_direction(GROW_DIRECTION_BEGIN);
		b->add_child(quantidade);
		gradePocoes->add_child(b);
		quantas++;
	}
	if (quantas == 0) {
		gradePocoes->add_child(rotulo(u("Nenhuma poção na mochila."), 16));
	}
	listaPocoes->reset_size();
	listaPocoes->set_position((Vector2(1280, 720) - listaPocoes->get_combined_minimum_size()) / 2.0f - Vector2(0, 60));
	listaPocoes->show();
	instrucao->set_text(u("Escolha uma poção (Esc volta)."));
}

void TelaDeCombate::escolherPocao(const String &idPocao) {
	listaPocoes->hide();
	pocaoEscolhida = idPocao;
	if (fachada(this)->precisaEscolherAlvo(idPocao)) {
		pedirAlvo(Pendente::POCAO);
	} else {
		executar(Pendente::POCAO, -1);
	}
}

void TelaDeCombate::pedirAlvo(Pendente acao) {
	const std::vector<int> alvos = combate->alvosDoJogador();
	if (alvos.empty()) {
		registrar(u("Nenhum inimigo visível para mirar."));
		return;
	}
	if (alvos.size() == 1) {
		executar(acao, alvos[0]);
		return;
	}
	pendente = acao;
	for (int i : alvos) {
		lutadores[i].cursor->show();
		lutadores[i].botaoAlvo->show();
	}
	instrucao->set_text(u("Escolha o alvo: clique nele ou tecle o número (Esc volta)."));
}

void TelaDeCombate::confirmarAlvo(int alvo) {
	if (pendente == Pendente::NADA) {
		return;
	}
	const std::vector<int> alvos = combate->alvosDoJogador();
	if (std::find(alvos.begin(), alvos.end(), alvo) == alvos.end()) {
		return;
	}
	const Pendente acao = pendente;
	cancelarEscolha();
	executar(acao, alvo);
}

void TelaDeCombate::cancelarEscolha() {
	pendente = Pendente::NADA;
	listaPocoes->hide();
	for (size_t i = 1; i < lutadores.size(); i++) {
		lutadores[i].cursor->hide();
		lutadores[i].botaoAlvo->hide();
	}
	mostrarMenu(isAguardando());
}

void TelaDeCombate::executar(Pendente acao, int alvo) {
	pendente = Pendente::NADA;
	mostrarMenu(false);
	switch (acao) {
		case Pendente::ATAQUE:
			enfileirar(combate->jogadorAtaca(alvo));
			break;
		case Pendente::MAGIA:
			enfileirar(combate->jogadorUsaMagia(alvo));
			break;
		case Pendente::POCAO: {
			G4Efeitos::Combatente *escolhido = alvo > 0 ? &combate->participante(alvo) : nullptr;
			const UsoDePocao uso = fachada(this)->usarPocao(pocaoEscolhida, escolhido, combate->inimigosVivos());
			if (!uso.sucesso) {
				registrar(uso.mensagem);
				mostrarMenu(true);
				return;
			}
			vfxAtual = uso.vfx;
			corAtual = uso.cor;
			sopraTocado = false;
			if (uso.revelarMapa) {
				emit_signal("mapa_revelado");
			}
			enfileirar(combate->jogadorUsouPocao(ponte::padrao(uso.nome), uso.aplicacoes));
			break;
		}
		case Pendente::NADA:
			break;
	}
}

// --- Ações públicas ---

bool TelaDeCombate::isAguardando() const {
	return combate && combate->aguardandoJogador() && !processando && !encerrado;
}

void TelaDeCombate::acaoAtacar(int alvo) {
	if (isAguardando()) executar(Pendente::ATAQUE, alvo);
}

void TelaDeCombate::acaoMagia(int alvo) {
	if (isAguardando()) executar(Pendente::MAGIA, alvo);
}

void TelaDeCombate::acaoPocao(const String &idPocao, int alvo) {
	if (isAguardando()) {
		pocaoEscolhida = idPocao;
		executar(Pendente::POCAO, alvo);
	}
}

void TelaDeCombate::acaoFugir() {
	if (isAguardando()) {
		mostrarMenu(false);
		enfileirar(combate->jogadorFoge());
	}
}

PackedStringArray TelaDeCombate::efeitosDe(int participante) const {
	if (!combate || participante < 0 || participante >= combate->quantidade()) {
		return PackedStringArray();
	}
	return ponte::efeitosAtivos(combate->participante(participante));
}

PackedInt32Array TelaDeCombate::alvos() const {
	PackedInt32Array lista;
	if (combate) {
		for (int i : combate->alvosDoJogador()) {
			lista.push_back(i);
		}
	}
	return lista;
}

void TelaDeCombate::_unhandled_input(const Ref<InputEvent> &evento) {
	if (encerrado || !combate) {
		return;
	}
	if (evento->is_action_pressed("ui_cancel") && (pendente != Pendente::NADA || listaPocoes->is_visible())) {
		cancelarEscolha();
		get_viewport()->set_input_as_handled();
		return;
	}
	const Ref<InputEventKey> tecla = evento;
	if (tecla.is_null() || !tecla->is_pressed() || tecla->is_echo()) {
		return;
	}
	const int numero = int(tecla->get_keycode()) - int(KEY_1);
	if (numero < 0 || numero > 3) {
		return;
	}
	get_viewport()->set_input_as_handled();
	if (pendente != Pendente::NADA) {
		const std::vector<int> alvos = combate->alvosDoJogador();
		if (numero < int(alvos.size())) {
			confirmarAlvo(alvos[numero]);
		}
	} else if (!listaPocoes->is_visible()) {
		escolherAcao(numero);
	}
}

// --- Visual ---

void TelaDeCombate::atualizarParticipantes() {
	for (int i = 0; i < combate->quantidade(); i++) {
		Lutador &l = lutadores[i];
		const G4Efeitos::Combatente &c = combate->participante(i);
		const G4Efeitos::Personagem &p = c.personagem();
		l.barraHp->set_max(c.hpMaximo());
		l.barraHp->set_value(p.hp());
		l.rotuloHp->set_text(String::num_int64(p.hp()) + "/" + String::num_int64(c.hpMaximo()));
		if (l.barraMp) {
			l.barraMp->set_max(c.mpMaximo());
			l.barraMp->set_value(p.mp());
			Object::cast_to<Label>(l.barraMp->get_parent()->get_node_or_null(NodePath("RotuloMp")))->set_text(String::num_int64(p.mp()) + "/" + String::num_int64(c.mpMaximo()));
		}
		const PackedStringArray efeitos = ponte::efeitosAtivos(c);
		atualizarIcones(l, efeitos);

		const String sobre = c.derrotado() ? String() : sobreposicao_de(efeitos);
		if (sobre != l.ultimaSobreposicao) {
			l.ultimaSobreposicao = sobre;
			l.sobreposicao->set_visible(!sobre.is_empty());
			if (!sobre.is_empty()) {
				l.sobreposicao->play(sobre);
			}
		}
		if (!c.derrotado()) {
			l.sprite->set_modulate(Color(1, 1, 1, efeitos.has("invisivel") ? 0.4 : 1.0));
		}
	}
}

void TelaDeCombate::atualizarIcones(Lutador &l, const PackedStringArray &efeitos) {
	const TypedArray<Node> antigos = l.icones->get_children();
	for (int i = 0; i < antigos.size(); i++) {
		Object::cast_to<Node>(antigos[i])->queue_free();
	}
	for (int i = 0; i < efeitos.size(); i++) {
		TextureRect *ic = memnew(TextureRect);
		ic->set_texture(textura(String("res://assets/icones/status/") + efeitos[i] + ".png"));
		ic->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
		ic->set_custom_minimum_size(Vector2(24, 24));
		ic->set_tooltip_text(efeitos[i]);
		l.icones->add_child(ic);
	}
}

void TelaDeCombate::tocarEfeito(const String &vfx, int alvo, const Color &cor, bool tingir) {
	Ref<SpriteFrames> frames;
	frames.instantiate();
	frames->remove_animation("default");
	adicionar_quadros(frames, "efeito", String("res://assets/combate/vfx/") + vfx + "/frame_", 12.0, false);
	if (frames->get_frame_count("efeito") == 0) {
		return;
	}
	AnimatedSprite2D *efeito = memnew(AnimatedSprite2D);
	efeito->set_sprite_frames(frames);
	if (alvo < 0) {
		// Sopro do Dragão: a chama sai da esquerda e é ampliada sobre os inimigos.
		float y = 0.0f;
		for (size_t i = 1; i < lutadores.size(); i++) {
			y += lutadores[i].base.y - lutadores[i].altura / 2.0f;
		}
		y /= float(lutadores.size() - 1);
		efeito->set_centered(false);
		efeito->set_scale(Vector2(2, 2));
		efeito->set_position(Vector2(176, y - 48));
	} else {
		const Lutador &l = lutadores[alvo];
		const Vector2 tamanho = frames->get_frame_texture("efeito", 0)->get_size();
		efeito->set_position(l.base + Vector2(0, tamanho.y > tamanho.x ? -tamanho.y / 2.0f + 2.0f : -l.altura / 2.0f));
	}
	if (tingir) {
		efeito->set_modulate(cor.lightened(0.15));
	}
	palco->add_child(efeito);
	efeito->connect("animation_finished", callable_mp((Node *)efeito, &Node::queue_free));
	efeito->play("efeito");
}

Vector2 TelaDeCombate::naTela(int participante, float alturaRelativa) const {
	const Lutador &l = lutadores[participante];
	return (l.base + Vector2(0, -l.altura * alturaRelativa)) * ESCALA_PALCO;
}

void TelaDeCombate::textoFlutuante(int participante, const String &texto, const Color &cor) {
	if (participante < 0 || participante >= int(lutadores.size())) {
		return;
	}
	Label *r = rotulo(texto, 24, cor);
	r->add_theme_color_override("font_outline_color", cor_texto());
	r->add_theme_constant_override("outline_size", 8);
	r->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	r->set_custom_minimum_size(Vector2(200, 0));
	r->set_position(naTela(participante, 1.15f) - Vector2(100, 24));
	sobreTela->add_child(r);
	Ref<Tween> t = create_tween();
	t->tween_property(r, "position:y", r->get_position().y - 40, 0.9);
	t->parallel()->tween_property(r, "modulate:a", 0.0, 0.5)->set_delay(0.4);
	t->tween_callback(callable_mp((Node *)r, &Node::queue_free));
}

void TelaDeCombate::avancarERecuar(int participante) {
	if (participante < 0) {
		return;
	}
	Lutador &l = lutadores[participante];
	if (participante > 0 && l.sprite->get_sprite_frames()->get_frame_count("ataque") > 0) {
		l.sprite->play("ataque");
		get_tree()->create_timer(0.45)->connect("timeout", callable_mp(l.sprite, &AnimatedSprite2D::play).bind("idle", 1.0, false));
	}
	const Vector2 frente = l.base + Vector2(participante == 0 ? 12 : -12, 0);
	Ref<Tween> t = create_tween();
	t->tween_property(l.raiz, "position", frente, 0.12);
	t->tween_property(l.raiz, "position", l.base, 0.18);
}

void TelaDeCombate::piscar(int participante, const Color &cor) {
	if (participante < 0) {
		return;
	}
	Lutador &l = lutadores[participante];
	if (participante > 0 && l.sprite->get_sprite_frames()->get_frame_count("dano") > 0 && !combate->participante(participante).derrotado()) {
		l.sprite->play("dano");
		get_tree()->create_timer(0.35)->connect("timeout", callable_mp(l.sprite, &AnimatedSprite2D::play).bind("idle", 1.0, false));
	}
	const Color normal = l.sprite->get_modulate();
	Ref<Tween> t = create_tween();
	t->tween_property(l.sprite, "modulate", cor, 0.08);
	t->tween_property(l.sprite, "modulate", normal, 0.25);
	Ref<Tween> tremor = create_tween();
	tremor->tween_property(l.raiz, "position", l.base + Vector2(2, 0), 0.05);
	tremor->tween_property(l.raiz, "position", l.base + Vector2(-2, 0), 0.05);
	tremor->tween_property(l.raiz, "position", l.base, 0.05);
}

void TelaDeCombate::registrar(const String &texto) {
	if (registro) {
		registro->append_text(texto + String("\n"));
	}
}

void TelaDeCombate::_bind_methods() {
	ClassDB::bind_method(D_METHOD("iniciar", "ids_inimigos", "area"), &TelaDeCombate::iniciar);
	ClassDB::bind_method(D_METHOD("acao_atacar", "alvo"), &TelaDeCombate::acaoAtacar);
	ClassDB::bind_method(D_METHOD("acao_magia", "alvo"), &TelaDeCombate::acaoMagia);
	ClassDB::bind_method(D_METHOD("acao_pocao", "id_pocao", "alvo"), &TelaDeCombate::acaoPocao);
	ClassDB::bind_method(D_METHOD("acao_fugir"), &TelaDeCombate::acaoFugir);
	ClassDB::bind_method(D_METHOD("is_aguardando"), &TelaDeCombate::isAguardando);
	ClassDB::bind_method(D_METHOD("efeitos_de", "participante"), &TelaDeCombate::efeitosDe);
	ClassDB::bind_method(D_METHOD("alvos"), &TelaDeCombate::alvos);
	ADD_SIGNAL(MethodInfo("terminou", PropertyInfo(Variant::STRING, "resultado"), PropertyInfo(Variant::ARRAY, "recompensas")));
	ADD_SIGNAL(MethodInfo("mapa_revelado"));
}
