extends SceneTree
## Teste do padrão Decorator dentro do jogo, pela fachada, executado sem janela:
##   godot --headless --path src/estrutural -s res://tests/teste_decorator.gd
## (o núcleo em C++ tem seus próprios testes: make -C src/estrutural/decorator test)

var _falhas := 0


func _initialize() -> void:
	await process_frame
	var f := AlquimiaFacade.new()
	root.add_child(f)
	f.coletar_ingredientes(10)

	# Poções feitas na bancada, como o jogador faria.
	for dupla in [["escama_de_salamandra", "pena_de_corvo"], ["pena_de_corvo", "pepita_de_cobre"],
			["salvia_de_fogo", "cogumelo_brilhante"], ["pepita_de_cobre", "pepita_de_cobre"],
			["erva_lunar", "cogumelo_brilhante"], ["cogumelo_roxo", "cogumelo_roxo"], ["erva_lunar", "erva_lunar"]]:
		_checar(f.combinar(dupla[0], dupla[1]).is_sucesso(), "mistura %s + %s" % dupla)

	var jogador: Dictionary = f.descrever_jogador()
	_checar(jogador.atributos.forca == 9 and jogador.efeitos.is_empty(), "jogador começa sem efeitos")

	# 1. Elixir da Quimera: dois decoradores sobre o jogador.
	var r: Dictionary = f.usar_pocao("elixir_da_quimera")
	_checar(r.sucesso and r.mensagens.size() == 2, "Quimera aplica dois efeitos")
	jogador = f.descrever_jogador()
	_checar(jogador.atributos.forca == 11 and jogador.atributos.velocidade == 6, "Quimera soma força e velocidade")
	_checar(jogador.efeitos == PackedStringArray(["forca", "velocidade"]), "a cadeia tem os dois ícones, em ordem")
	_checar(_quantidade(f, "elixir_da_quimera") == 0, "a poção sai da mochila")

	# 2. Poções de combate não são usadas fora dele; poção que não tem não é usada.
	r = f.usar_pocao("veneno_roxo")
	_checar(not r.sucesso and _quantidade(f, "veneno_roxo") == 1, "Veneno Roxo só em combate (e não é gasto)")
	_checar(f.precisa_de_inimigo("veneno_roxo") and not f.precisa_de_inimigo("pocao_de_cura_menor"), "precisa_de_inimigo")
	_checar(not f.usar_pocao("elixir_da_quimera").sucesso, "sem a poção na mochila, nada acontece")
	_checar(not f.usar_pocao("erva_lunar").sucesso, "ingrediente não é poção")

	# 3. Amuleto Líquido: sorte contada em saques de baú.
	f.usar_pocao("amuleto_liquido")
	_checar(f.bonus_de_sorte() == 25, "Amuleto dá +25 de sorte")
	for i in 3:
		f.registrar_saque()
	_checar(f.bonus_de_sorte() == 0, "a sorte acaba depois de 3 saques")

	# 4. Chama Fátua: luz até sair da caverna.
	_checar(f.descrever_jogador().raio_de_luz == 40.0, "raio de luz base")
	f.usar_pocao("chama_fatua")
	_checar(f.descrever_jogador().raio_de_luz == 96.0, "Chama Fátua aumenta o raio de luz")
	f.apagar_luzes()
	_checar(f.descrever_jogador().raio_de_luz == 40.0, "a luz apaga ao sair da caverna")
	_checar(f.descrever_jogador().efeitos.has("forca"), "apagar a luz não tira os outros efeitos")

	# 5. Instantâneas: Ferro (permanente), Clarividência (mundo), Cura.
	f.usar_pocao("pocao_de_ferro")
	jogador = f.descrever_jogador()
	_checar(jogador.hp_maximo == 70 and jogador.hp == 70, "Poção de Ferro: +10 de HP máximo")
	_checar(f.usar_pocao("pocao_de_clarividencia").revelar_mapa, "Clarividência revela o mapa")
	_checar(f.usar_pocao("pocao_de_cura_menor").sucesso, "Cura Menor pode ser usada fora de combate")

	f.restaurar_jogador()
	_checar(f.descrever_jogador().efeitos.is_empty(), "restaurar tira todos os efeitos")

	f.esquecer_livro()
	print("RESULTADO: %s (%d falha(s))" % ["OK" if _falhas == 0 else "FALHOU", _falhas])
	quit(_falhas)


func _quantidade(f: AlquimiaFacade, id: String) -> int:
	for item: Dictionary in f.listar_inventario():
		if item.id == id:
			return item.quantidade
	return 0


func _checar(condicao: bool, descricao: String) -> void:
	if not condicao:
		_falhas += 1
		print("FALHOU: ", descricao)
