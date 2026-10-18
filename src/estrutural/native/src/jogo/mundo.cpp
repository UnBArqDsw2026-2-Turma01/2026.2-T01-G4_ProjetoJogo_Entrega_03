#include "jogo/mundo.h"

#include "jogo/comum.h"
#include "jogo/galinha.h"
#include "jogo/ingrediente_coletavel.h"

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/property_tweener.hpp>
#include <godot_cpp/classes/rectangle_shape2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/static_body2d.hpp>
#include <godot_cpp/classes/style_box_flat.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace jogo;

static const int GALINHAS = 6;
static const float ZOOM_CAMERA = 1.0f; // menor = câmera mais longe

static const Vector2 TAMANHO_CAVERNA(1920, 1152); // mapa_caverna.png (640x384) ampliado 3x

static Vector2 vetor(const Array &par) {
	return Vector2(par[0], par[1]);
}

void Mundo::_ready() {
	mapa = ler_json("res://data/mapa.json");
	const Array ingredientes = fachada(this)->listarIngredientes();
	for (int i = 0; i < ingredientes.size(); i++) {
		const Dictionary ingrediente = ingredientes[i];
		nomes[ingrediente["id"]] = ingrediente["nome"];
	}

	criarAcoes();
	criarChao();
	criarColisoes();
	exploracao = ler_json("res://data/exploracao.json");
	criarCaverna();

	entidades = memnew(Node2D);
	entidades->set_y_sort_enabled(true);
	add_child(entidades);

	jogador = memnew(Jogador);
	jogador->set_position(vetor(mapa["inicio"]));
	entidades->add_child(jogador);

	camera = memnew(Camera2D);
	camera->set_zoom(Vector2(ZOOM_CAMERA, ZOOM_CAMERA));
	camera->set_limit(SIDE_LEFT, 0);
	camera->set_limit(SIDE_TOP, 0);
	camera->set_limit(SIDE_RIGHT, int(tamanhoMapa().x));
	camera->set_limit(SIDE_BOTTOM, int(tamanhoMapa().y));
	camera->set_position_smoothing_enabled(true);
	jogador->add_child(camera);

	// Luz do jogador: só acende na caverna; o raio vem da cadeia de
	// decoradores (Chama Fátua e Elixir da Noite aumentam).
	luz = memnew(PointLight2D);
	luz->set_texture(textura("res://assets/mundo/luz/luz_radial.png"));
	luz->set_energy(1.3);
	luz->set_color(Color::html("#ffe8c0"));
	luz->set_position(Vector2(0, -18));
	luz->set_enabled(false);
	jogador->add_child(luz);

	espalharIngredientes();
	colocarBaus();
	colocarCenario();
	soltarGalinhas();
	criarEncontros();
	criarSegredos();
	criarHud();
}

void Mundo::_process(double delta) {
	if (marcas) {
		marcas->queue_redraw();
	}
	// O raio vem em pixels de arte; o mundo é desenhado em 3x.
	const float raio = fachada(this)->getJogador().personagem().raioDeLuz() * 3.0f;
	luz->set_texture_scale(Math::lerp(float(luz->get_texture_scale()), raio / 64.0f, float(MIN(1.0, delta * 4.0))));
	tempoDescanso += delta;
	if (tempoDescanso >= 1.5) {
		tempoDescanso = 0.0;
		if (!telaCombate) {
			fachada(this)->descansar();
		}
	}
	tempoStatus -= delta;
	if (tempoStatus <= 0.0) {
		tempoStatus = 0.2;
		atualizarStatus();
	}
}

void Mundo::_input(const Ref<InputEvent> &evento) {
	if (telaCombate) {
		return;
	}
	const bool fechar = evento->is_action_pressed("ui_cancel") && mochila->is_visible();
	if (evento->is_action_pressed("abrir_mochila") || fechar) {
		alternarMochila();
		get_viewport()->set_input_as_handled();
	}
}

void Mundo::alternarMochila() {
	mochila->set_visible(!mochila->is_visible());
	jogador->setPodeAndar(!mochila->is_visible());
	if (mochila->is_visible()) {
		mochila->abrir();
	}
}

void Mundo::aoColetar(const String &nome) {
	avisar(String("+1 ") + nome);
}

void Mundo::_unhandled_input(const Ref<InputEvent> &evento) {
	if (pertoDaEntrada && !telaCombate && evento->is_action_pressed("interagir")) {
		get_viewport()->set_input_as_handled();
		entrarNaCaverna();
	}
}

void Mundo::aoAbrirBau(const String &nomeBau, const Array &obtidos) {
	if (obtidos.is_empty()) {
		avisar(nomeBau + String(": estava vazio desta vez."));
		return;
	}
	PackedStringArray partes;
	for (int i = 0; i < obtidos.size(); i++) {
		const Dictionary item = obtidos[i];
		partes.push_back(String("+") + String::num_int64(int(item["quantidade"])) + " " + String(item["nome"]));
	}
	avisar(nomeBau + String(": ") + String(", ").join(partes));
}

void Mundo::avisar(const String &texto) {
	aviso->set_text(texto);
	aviso->set_modulate(Color(1, 1, 1, 1));
	if (animacaoAviso.is_valid()) {
		animacaoAviso->kill();
	}
	animacaoAviso = create_tween();
	animacaoAviso->tween_interval(2.0);
	animacaoAviso->tween_property(aviso, "modulate:a", 0.0, 0.6);
}

// --- Montagem do mundo ---

void Mundo::criarAcoes() {
	struct Acao {
		const char *nome;
		Key teclas[2];
	};
	const Acao acoes[] = {
		{ "mover_esquerda", { KEY_A, KEY_LEFT } },
		{ "mover_direita", { KEY_D, KEY_RIGHT } },
		{ "mover_cima", { KEY_W, KEY_UP } },
		{ "mover_baixo", { KEY_S, KEY_DOWN } },
		{ "abrir_mochila", { KEY_I, KEY_TAB } },
		{ "interagir", { KEY_E, KEY_SPACE } },
	};
	InputMap *mapaEntrada = InputMap::get_singleton();
	for (const Acao &acao : acoes) {
		if (mapaEntrada->has_action(acao.nome)) {
			continue;
		}
		mapaEntrada->add_action(acao.nome);
		for (Key tecla : acao.teclas) {
			Ref<InputEventKey> evento;
			evento.instantiate();
			evento->set_physical_keycode(tecla);
			mapaEntrada->action_add_event(acao.nome, evento);
		}
	}
}

void Mundo::criarChao() {
	Sprite2D *chao = memnew(Sprite2D);
	chao->set_texture(textura("res://assets/mundo/mapa.png"));
	chao->set_centered(false);
	add_child(chao);
}

void Mundo::criarColisoes() {
	StaticBody2D *solidos = memnew(StaticBody2D);
	const Vector2 tamanho = tamanhoMapa();
	Array retangulos = Array(mapa["colisoes"]).duplicate();
	// bordas do mapa
	retangulos.push_back(Array::make(-32, -32, tamanho.x + 64, 32));
	retangulos.push_back(Array::make(-32, tamanho.y, tamanho.x + 64, 32));
	retangulos.push_back(Array::make(-32, 0, 32, tamanho.y));
	retangulos.push_back(Array::make(tamanho.x, 0, 32, tamanho.y));
	for (int i = 0; i < retangulos.size(); i++) {
		const Array r = retangulos[i];
		CollisionShape2D *colisao = memnew(CollisionShape2D);
		Ref<RectangleShape2D> forma;
		forma.instantiate();
		forma->set_size(Vector2(r[2], r[3]));
		colisao->set_shape(forma);
		colisao->set_position(Vector2(r[0], r[1]) + forma->get_size() / 2.0f);
		solidos->add_child(colisao);
	}
	add_child(solidos);
}

void Mundo::espalharIngredientes() {
	const Array regioes = mapa["regioes"];
	for (int i = 0; i < regioes.size(); i++) {
		const Dictionary regiao = regioes[i];
		Array pontos = Array(regiao["pontos"]).duplicate();
		pontos.shuffle();
		const Array tipos = regiao["ingredientes"];
		const int quantidade = MIN(int(regiao["quantidade"]), pontos.size());
		for (int j = 0; j < quantidade; j++) {
			const String id = tipos.pick_random();
			IngredienteColetavel *coletavel = memnew(IngredienteColetavel);
			coletavel->idIngrediente = id;
			coletavel->nome = nomes[id];
			coletavel->set_position(vetor(pontos[j]));
			coletavel->connect("coletado", callable_mp(this, &Mundo::aoColetar));
			entidades->add_child(coletavel);
		}
	}
}

void Mundo::colocarBaus() {
	const Dictionary tipos = ler_json("res://data/baus.json");
	const Array lista = mapa["baus"];
	for (int i = 0; i < lista.size(); i++) {
		const Dictionary dados = lista[i];
		Bau *bau = memnew(Bau);
		bau->tipo = tipos[dados["tipo"]];
		bau->nomes = nomes;
		bau->set_position(vetor(dados["posicao"]));
		bau->connect("aberto", callable_mp(this, &Mundo::aoAbrirBau));
		entidades->add_child(bau);
		baus.push_back(bau);
	}
}

// Árvores, arbustos, matos e rochas (assets de tools/gerar_cenario.py). A posição
// é a base da peça; só a base colide, então o jogador passa por trás das copas.
void Mundo::colocarCenario() {
	struct Base {
		const char *tipo;
		Vector2 colisao; // tamanho da área sólida na base (0 = não bloqueia)
	};
	const Base bases[] = {
		{ "arvore_grande", Vector2(30, 14) },
		{ "arvore_media", Vector2(24, 12) },
		{ "arvore_alquimia", Vector2(24, 12) },
		{ "pinheiro", Vector2(22, 12) },
		{ "arbusto_verde", Vector2(36, 14) },
		{ "arbusto_com_frutas", Vector2(36, 14) },
		{ "rocha_musgo", Vector2(36, 16) },
		{ "toco_de_arvore", Vector2(30, 14) },
		{ "mato_alto", Vector2() },
		{ "mato_com_flores", Vector2() },
	};
	const Array lista = mapa.get("cenario", Array());
	for (int i = 0; i < lista.size(); i++) {
		const Dictionary peca = lista[i];
		const String tipo = peca["tipo"];
		Vector2 colisao;
		for (const Base &base : bases) {
			if (tipo == base.tipo) {
				colisao = base.colisao;
			}
		}

		StaticBody2D *corpo = memnew(StaticBody2D);
		corpo->set_position(vetor(peca["posicao"]));
		const Ref<Texture2D> tex = textura(String("res://assets/mundo/cenario/") + tipo + ".png");
		Sprite2D *sprite = memnew(Sprite2D);
		sprite->set_texture(tex);
		sprite->set_scale(Vector2(3, 3));
		sprite->set_offset(Vector2(0, -tex->get_height() / 2.0f + 1.0f)); // base do desenho na origem
		corpo->add_child(sprite);
		if (colisao != Vector2()) {
			CollisionShape2D *forma = memnew(CollisionShape2D);
			Ref<RectangleShape2D> retangulo;
			retangulo.instantiate();
			retangulo->set_size(colisao);
			forma->set_shape(retangulo);
			forma->set_position(Vector2(0, -colisao.y / 2.0f));
			corpo->add_child(forma);
		}
		entidades->add_child(corpo);
	}
}

void Mundo::soltarGalinhas() {
	const Array a = mapa["area_galinhas"];
	const Rect2 area(a[0], a[1], a[2], a[3]);
	for (int i = 0; i < GALINHAS; i++) {
		Galinha *galinha = memnew(Galinha);
		galinha->area = area;
		galinha->set_position(area.position + Vector2(UtilityFunctions::randf() * area.size.x, UtilityFunctions::randf() * area.size.y));
		entidades->add_child(galinha);
	}
}

static Label *texto_com_contorno(const String &texto, int contorno) {
	Label *r = memnew(Label);
	r->set_text(texto);
	r->add_theme_color_override("font_outline_color", cor_texto());
	r->add_theme_constant_override("outline_size", contorno);
	return r;
}

void Mundo::criarHud() {
	CanvasLayer *hud = memnew(CanvasLayer);
	add_child(hud);

	Label *ajuda = texto_com_contorno(u("WASD ou setas: andar    E: interagir    I ou Tab: mochila"), 6);
	ajuda->set_position(Vector2(16, 12));
	hud->add_child(ajuda);
	criarStatus(hud);

	aviso = texto_com_contorno("", 8);
	aviso->set_anchors_and_offsets_preset(Control::PRESET_CENTER_BOTTOM);
	aviso->set_position(aviso->get_position() - Vector2(0, 64));
	aviso->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	aviso->set_h_grow_direction(Control::GROW_DIRECTION_BOTH);
	aviso->add_theme_font_size_override("font_size", 22);
	aviso->set_modulate(Color(1, 1, 1, 0));
	hud->add_child(aviso);

	criarMinimapa(hud);

	CanvasLayer *camadaMochila = memnew(CanvasLayer);
	camadaMochila->set_layer(10);
	add_child(camadaMochila);
	mochila = memnew(MochilaUI);
	mochila->setPodeFechar(true);
	mochila->set_anchors_preset(Control::PRESET_FULL_RECT);
	mochila->connect("fechar_pedido", callable_mp(this, &Mundo::alternarMochila));
	mochila->connect("pocao_usada", callable_mp(this, &Mundo::aoUsarPocao));
	mochila->hide();
	camadaMochila->add_child(mochila);

	camadaCombate = memnew(CanvasLayer);
	camadaCombate->set_layer(20);
	add_child(camadaCombate);
}

// Minimapa no canto: baús em amarelo (cinza quando abertos) e o jogador em vermelho.
void Mundo::criarMinimapa(CanvasLayer *hud) {
	PanelContainer *moldura = memnew(PanelContainer);
	Ref<StyleBoxFlat> estilo;
	estilo.instantiate();
	estilo->set_bg_color(cor_texto());
	estilo->set_content_margin_all(3);
	moldura->add_theme_stylebox_override("panel", estilo);
	moldura->set_position(Vector2(1280 - 256 - 6 - 16, 12));
	hud->add_child(moldura);
	molduraMinimapa = moldura;

	minimapa = memnew(TextureRect);
	minimapa->set_texture(textura("res://assets/mundo/mapa.png"));
	minimapa->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
	minimapa->set_custom_minimum_size(Vector2(256, 160));
	minimapa->set_texture_filter(CanvasItem::TEXTURE_FILTER_LINEAR);
	moldura->add_child(minimapa);

	marcas = memnew(Control);
	marcas->set_anchors_preset(Control::PRESET_FULL_RECT);
	marcas->connect("draw", callable_mp(this, &Mundo::desenharMarcas));
	minimapa->add_child(marcas);
}

void Mundo::desenharMarcas() {
	const Vector2 escala = minimapa->get_size() / tamanhoMapa();
	for (Bau *bau : baus) {
		const Rect2 marca(bau->get_position() * escala - Vector2(3, 3), Vector2(6, 6));
		marcas->draw_rect(marca, bau->isAberto() ? Color::html("#7a6a5a") : Color::html("#ffd23f"));
		marcas->draw_rect(marca, cor_texto(), false, 1.0);
	}
	// Segredos revelados pela Poção de Clarividência.
	const Ref<Texture2D> marcaSegredo = textura("res://assets/mundo/segredos/marca_minimapa.png");
	for (Runa *runa : runas) {
		if (runa->area == "mundo" && runa->isDisponivel()) {
			marcas->draw_texture_rect(marcaSegredo, Rect2(runa->get_position() * escala - Vector2(6, 6), Vector2(12, 12)), false);
		}
	}
	marcas->draw_circle(jogador->get_position() * escala, 4.0, Color::html("#e03030"));
}

Vector2 Mundo::tamanhoMapa() const {
	return vetor(mapa["tamanho"]);
}

// --- Efeitos das poções no mundo (padrão Decorator) ---

Vector2 Mundo::naCaverna(const Vector2 &local) const {
	const Dictionary caverna = exploracao["caverna"];
	return vetor(caverna["origem"]) + local * float(caverna["escala"]);
}

// A caverna fica fora dos limites do mapa, na mesma cena: entrar é
// teletransportar o jogador e trocar os limites da câmera e a iluminação.
void Mundo::criarCaverna() {
	const Dictionary caverna = exploracao["caverna"];
	const float escala = caverna["escala"];

	Sprite2D *chao = memnew(Sprite2D);
	chao->set_texture(textura("res://assets/mundo/caverna/mapa_caverna.png"));
	chao->set_centered(false);
	chao->set_scale(Vector2(escala, escala));
	chao->set_position(naCaverna(Vector2()));
	add_child(chao);

	StaticBody2D *paredes = memnew(StaticBody2D);
	const Array colisoes = caverna["colisoes"];
	for (int i = 0; i < colisoes.size(); i++) {
		const Array r = colisoes[i];
		CollisionShape2D *colisao = memnew(CollisionShape2D);
		Ref<RectangleShape2D> forma;
		forma.instantiate();
		forma->set_size(Vector2(r[2], r[3]) * escala);
		colisao->set_shape(forma);
		colisao->set_position(naCaverna(Vector2(r[0], r[1])) + forma->get_size() / 2.0f);
		paredes->add_child(colisao);
	}
	add_child(paredes);

	const Array s = caverna["saida"];
	Area2D *saida = memnew(Area2D);
	CollisionShape2D *formaSaida = memnew(CollisionShape2D);
	Ref<RectangleShape2D> retangulo;
	retangulo.instantiate();
	retangulo->set_size(Vector2(s[2], s[3]) * escala);
	formaSaida->set_shape(retangulo);
	formaSaida->set_position(naCaverna(Vector2(s[0], s[1])) + retangulo->get_size() / 2.0f);
	saida->add_child(formaSaida);
	saida->connect("body_entered", callable_mp(this, &Mundo::aoPisarNaSaida));
	add_child(saida);

	// Entrada no mapa: a arte da entrada e a área onde o E funciona.
	const Vector2 entrada = vetor(caverna["entrada"]);
	const Ref<Texture2D> tex = textura("res://assets/mundo/cenario/entrada_caverna.png");
	Sprite2D *arte = memnew(Sprite2D);
	arte->set_texture(tex);
	arte->set_scale(Vector2(3, 3));
	arte->set_position(entrada);
	arte->set_offset(Vector2(0, -tex->get_height() / 2.0f + 1.0f));
	add_child(arte);
	Area2D *alcance = memnew(Area2D);
	CollisionShape2D *formaAlcance = memnew(CollisionShape2D);
	Ref<CircleShape2D> circulo;
	circulo.instantiate();
	circulo->set_radius(56.0);
	formaAlcance->set_shape(circulo);
	alcance->add_child(formaAlcance);
	alcance->set_position(entrada + Vector2(0, -12));
	alcance->connect("body_entered", callable_mp(this, &Mundo::aoAproximarDaEntrada).bind(true));
	alcance->connect("body_exited", callable_mp(this, &Mundo::aoAproximarDaEntrada).bind(false));
	add_child(alcance);
	dicaEntrada = texto_com_contorno(u("E: entrar na caverna"), 6);
	dicaEntrada->set_position(entrada + Vector2(-80, -130));
	dicaEntrada->hide();
	add_child(dicaEntrada);

	escuridao = memnew(CanvasModulate);
	escuridao->set_color(Color(0.05, 0.05, 0.09));
	escuridao->hide();
	add_child(escuridao);
}

void Mundo::aoAproximarDaEntrada(Node2D *corpo, bool perto) {
	if (!Object::cast_to<Jogador>(corpo)) {
		return;
	}
	pertoDaEntrada = perto && !dentroDaCaverna;
	dicaEntrada->set_visible(pertoDaEntrada);
}

void Mundo::aoPisarNaSaida(Node2D *corpo) {
	if (dentroDaCaverna && Object::cast_to<Jogador>(corpo)) {
		call_deferred("sair_da_caverna"); // pisou na escada de saída
	}
}

void Mundo::entrarNaCaverna() {
	if (dentroDaCaverna) {
		return;
	}
	const Dictionary caverna = exploracao["caverna"];
	dentroDaCaverna = true;
	pertoDaEntrada = false;
	dicaEntrada->hide();
	jogador->set_position(naCaverna(vetor(caverna["chegada"])));
	const Vector2 inicio = naCaverna(Vector2());
	camera->set_limit(SIDE_LEFT, int(inicio.x));
	camera->set_limit(SIDE_TOP, int(inicio.y));
	camera->set_limit(SIDE_RIGHT, int(inicio.x + TAMANHO_CAVERNA.x));
	camera->set_limit(SIDE_BOTTOM, int(inicio.y + TAMANHO_CAVERNA.y));
	camera->reset_smoothing();
	escuridao->show();
	luz->set_enabled(true);
	molduraMinimapa->hide();
	avisar(u("Está escuro aqui... Chama Fátua ou Elixir da Noite ajudariam."));
}

void Mundo::sairDaCaverna() {
	if (!dentroDaCaverna) {
		return;
	}
	const Dictionary caverna = exploracao["caverna"];
	dentroDaCaverna = false;
	jogador->set_position(vetor(caverna["entrada"]) + Vector2(0, 64));
	camera->set_limit(SIDE_LEFT, 0);
	camera->set_limit(SIDE_TOP, 0);
	camera->set_limit(SIDE_RIGHT, int(tamanhoMapa().x));
	camera->set_limit(SIDE_BOTTOM, int(tamanhoMapa().y));
	camera->reset_smoothing();
	escuridao->hide();
	luz->set_enabled(false);
	molduraMinimapa->show();
	fachada(this)->apagarLuzes(); // a luz das poções dura só enquanto está na caverna
}

void Mundo::criarEncontros() {
	const Array lista = ler_json("res://data/inimigos.json").get("encontros", Array());
	for (int i = 0; i < lista.size(); i++) {
		const Dictionary dados = lista[i];
		InimigoNoMapa *inimigo = memnew(InimigoNoMapa);
		inimigo->indice = i;
		inimigo->area = dados["area"];
		inimigo->ids = PackedStringArray(Array(dados["inimigos"]));
		const Vector2 posicao = vetor(dados["posicao"]);
		inimigo->set_position(inimigo->area == "caverna" ? naCaverna(posicao) : posicao);
		inimigo->connect("encontrado", callable_mp(this, &Mundo::iniciarCombate));
		entidades->add_child(inimigo);
		encontros.push_back(inimigo);
	}
}

void Mundo::criarSegredos() {
	const Array lista = exploracao.get("segredos", Array());
	for (int i = 0; i < lista.size(); i++) {
		const Dictionary dados = lista[i];
		Runa *runa = memnew(Runa);
		runa->area = dados["area"];
		runa->recompensa = dados["recompensa"];
		const Vector2 posicao = vetor(dados["posicao"]);
		runa->set_position(runa->area == "caverna" ? naCaverna(posicao) : posicao);
		runa->connect("coletada", callable_mp(this, &Mundo::aoColetarRuna));
		entidades->add_child(runa);
		runas.push_back(runa);
	}
}

void Mundo::revelarSegredos() {
	for (Runa *runa : runas) {
		runa->revelar();
	}
	avisar(u("Os segredos do mapa foram revelados! Procure as runas."));
}

void Mundo::aoColetarRuna(const Array &obtidos) {
	PackedStringArray partes;
	for (int i = 0; i < obtidos.size(); i++) {
		const Dictionary item = obtidos[i];
		partes.push_back(String("+") + String::num_int64(int(item["quantidade"])) + " " + String(nomes[item["id"]]));
	}
	avisar(u("Runa: ") + String(", ").join(partes));
}

void Mundo::aoUsarPocao(const Dictionary &resultado) {
	if (bool(resultado["revelar_mapa"])) {
		revelarSegredos();
	}
	atualizarStatus();
}

void Mundo::iniciarCombate(int indice) {
	if (telaCombate || indice < 0 || indice >= encontros.size()) {
		return;
	}
	if (mochila->is_visible()) {
		alternarMochila();
	}
	encontroAtual = indice;
	jogador->setPodeAndar(false);
	telaCombate = memnew(TelaDeCombate);
	camadaCombate->add_child(telaCombate);
	telaCombate->connect("terminou", callable_mp(this, &Mundo::aoTerminarCombate));
	telaCombate->connect("mapa_revelado", callable_mp(this, &Mundo::revelarSegredos));
	telaCombate->iniciar(encontros[indice]->ids, encontros[indice]->area);
}

void Mundo::aoTerminarCombate(const String &resultado, const Array &recompensas) {
	telaCombate->queue_free();
	telaCombate = nullptr;
	jogador->setPodeAndar(true);
	InimigoNoMapa *encontro = encontros[encontroAtual];
	if (resultado == "vitoria") {
		encontro->derrotar();
		PackedStringArray partes;
		for (int i = 0; i < recompensas.size(); i++) {
			const Dictionary item = recompensas[i];
			partes.push_back(String("+") + String::num_int64(int(item["quantidade"])) + " " + String(item["nome"]));
		}
		avisar(u("Vitória! ") + String(", ").join(partes));
	} else if (resultado == "fuga") {
		encontro->afastar();
		avisar(u("Você fugiu do combate."));
	} else {
		fachada(this)->restaurarJogador();
		sairDaCaverna();
		jogador->set_position(vetor(mapa["inicio"]));
		camera->reset_smoothing();
		avisar(u("Você desmaiou e acordou no centro da vila."));
	}
	atualizarStatus();
}

void Mundo::criarStatus(CanvasLayer *hud) {
	HBoxContainer *linha = memnew(HBoxContainer);
	linha->set_position(Vector2(16, 40));
	linha->add_theme_constant_override("separation", 10);
	hud->add_child(linha);
	status = texto_com_contorno("", 6);
	linha->add_child(status);
	iconesStatus = memnew(HBoxContainer);
	iconesStatus->add_theme_constant_override("separation", 4);
	linha->add_child(iconesStatus);
}

// HP, MP e os ícones dos efeitos ativos, lidos da cadeia de decoradores do jogador.
void Mundo::atualizarStatus() {
	if (!status) {
		return;
	}
	const Dictionary jogadorDados = fachada(this)->descreverJogador();
	status->set_text(String("HP ") + String::num_int64(int(jogadorDados["hp"])) + "/" + String::num_int64(int(jogadorDados["hp_maximo"])) +
			"   MP " + String::num_int64(int(jogadorDados["mp"])) + "/" + String::num_int64(int(jogadorDados["mp_maximo"])));
	const PackedStringArray efeitos = jogadorDados["efeitos"];
	const String chave = String(",").join(efeitos);
	if (chave == efeitosMostrados) {
		return;
	}
	efeitosMostrados = chave;
	const TypedArray<Node> antigos = iconesStatus->get_children();
	for (int i = 0; i < antigos.size(); i++) {
		Object::cast_to<Node>(antigos[i])->queue_free();
	}
	for (int i = 0; i < efeitos.size(); i++) {
		TextureRect *ic = memnew(TextureRect);
		ic->set_texture(textura(String("res://assets/icones/status/") + efeitos[i] + ".png"));
		ic->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
		ic->set_custom_minimum_size(Vector2(28, 28));
		iconesStatus->add_child(ic);
	}
}

void Mundo::_bind_methods() {
	ClassDB::bind_method(D_METHOD("iniciar_combate", "indice"), &Mundo::iniciarCombate);
	ClassDB::bind_method(D_METHOD("entrar_na_caverna"), &Mundo::entrarNaCaverna);
	ClassDB::bind_method(D_METHOD("sair_da_caverna"), &Mundo::sairDaCaverna);
	ClassDB::bind_method(D_METHOD("revelar_segredos"), &Mundo::revelarSegredos);
	ClassDB::bind_method(D_METHOD("get_tela_de_combate"), &Mundo::getTelaDeCombate);
	ClassDB::bind_method(D_METHOD("alternar_mochila"), &Mundo::alternarMochila);
	ClassDB::bind_method(D_METHOD("get_jogador"), &Mundo::getJogador);
	ClassDB::bind_method(D_METHOD("get_entidades"), &Mundo::getEntidades);
}
