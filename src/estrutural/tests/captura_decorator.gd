extends SceneTree
## Percorre os efeitos das poções (padrão Decorator) no jogo e salva prints em
## user://captura_decorator_*.png. Precisa de janela (não use --headless):
##   godot --path src/estrutural -s res://tests/captura_decorator.gd

var mundo: Node2D
var alquimia: Node


func _initialize() -> void:
	await process_frame
	mundo = load("res://scenes/mundo.tscn").instantiate()
	root.add_child(mundo)
	alquimia = root.get_node("Alquimia")
	alquimia.coletar_ingredientes(10)
	for dupla in [["escama_de_salamandra", "pena_de_corvo"], ["erva_lunar", "cogumelo_roxo"], ["cogumelo_roxo", "cogumelo_roxo"],
			["salvia_de_fogo", "escama_de_salamandra"], ["salvia_de_fogo", "cogumelo_brilhante"], ["erva_lunar", "cogumelo_brilhante"],
			["pena_de_corvo", "pena_de_corvo"], ["cogumelo_roxo", "pena_de_corvo"]]:
		alquimia.combinar(dupla[0], dupla[1])
	await _esperar(10)

	# 1. Elixir da Quimera pela mochila: dois decoradores, ícones na HUD.
	mundo.alternar_mochila()
	await _esperar(3)
	_mochila().usar_pocao("elixir_da_quimera")
	await _esperar(4)
	_salvar("1_mochila_quimera")
	mundo.alternar_mochila()
	await _esperar(6)
	_salvar("2_hud_efeitos")

	# 2. Combate contra o Gólem e o Cogumelo.
	mundo.iniciar_combate(2)
	var tela: Control = mundo.get_tela_de_combate()
	await _aguardar(tela)
	_salvar("3_combate")
	_tecla(KEY_3)
	await _esperar(4)
	_salvar("3b_lista_de_pocoes")
	_tecla(KEY_ESCAPE)
	await _esperar(2)
	tela.acao_pocao("elixir_do_sono", 1)
	await create_timer(0.9).timeout
	_salvar("4_sono_no_golem")
	await _aguardar(tela)
	tela.acao_pocao("veneno_roxo", 1)
	await _aguardar(tela)
	_salvar("5_golem_dormindo_e_envenenado")
	tela.acao_pocao("sopro_do_dragao", -1)
	await create_timer(0.85).timeout
	_salvar("6_sopro_do_dragao")
	var rodadas := 0
	while mundo.get_tela_de_combate() != null and rodadas < 30:
		await _aguardar(tela)
		if mundo.get_tela_de_combate() == null:
			break
		if tela.is_aguardando() and not tela.alvos().is_empty():
			tela.acao_atacar(tela.alvos()[0])
		rodadas += 1
		if rodadas == 2:
			await _esperar(20)
			_salvar("7_combate_andamento")
	await _esperar(30)
	_salvar("8_depois_do_combate")

	# 3. Caverna escura; Chama Fátua acende a luz; Clarividência revela runas.
	mundo.entrar_na_caverna()
	await _esperar(30)
	_salvar("9_caverna_escura")
	mundo.alternar_mochila()
	await _esperar(3)
	_mochila().usar_pocao("chama_fatua")
	_mochila().usar_pocao("pocao_de_clarividencia")
	mundo.alternar_mochila()
	await _esperar(90)
	_salvar("10_chama_fatua")
	mundo.get_jogador().position = Vector2(0, 2800) + Vector2(100, 175) * 3
	await _esperar(40)
	_salvar("11_runa_revelada")
	quit()


func _mochila() -> Node:
	return mundo.find_children("*", "MochilaUI", true, false)[0]


func _aguardar(tela: Control) -> void:
	for i in 600:
		if mundo.get_tela_de_combate() == null or tela.is_aguardando():
			return
		await process_frame


func _tecla(codigo: Key) -> void:
	var e := InputEventKey.new()
	e.keycode = codigo
	e.physical_keycode = codigo
	e.pressed = true
	Input.parse_input_event(e)
	if codigo == KEY_ESCAPE:
		var cancelar := InputEventAction.new()
		cancelar.action = "ui_cancel"
		cancelar.pressed = true
		Input.parse_input_event(cancelar)


func _esperar(n: int) -> void:
	for i in n:
		await process_frame


func _salvar(nome: String) -> void:
	root.get_texture().get_image().save_png(OS.get_user_data_dir().path_join("captura_decorator_%s.png" % nome))
