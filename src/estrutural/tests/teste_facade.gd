extends SceneTree
## Teste de fumaça da AlquimiaFacade, executado sem janela:
##   godot --headless --path src/estrutural -s res://tests/teste_facade.gd

var _falhas := 0


func _initialize() -> void:
	await process_frame # a raiz só entra na árvore depois do _initialize
	var f := _nova_fachada()
	f.esquecer_livro()

	# 1. Primeira mistura: receita nova e composição revelada (A+A gasta dois).
	var r: ResultadoMistura = f.combinar("erva_lunar", "erva_lunar")
	_checar(r.is_sucesso(), "A+A mistura")
	_checar(r.is_receita_nova(), "A+A é receita nova")
	_checar(r.get_composicoes_reveladas().size() == 1, "A+A revela uma composição")
	_checar(_quantidade(f, "erva_lunar") == 3, "A+A consome dois ingredientes")
	_checar(_quantidade(f, r.get_pocao().id) == 1, "poção vai para a mochila")

	# 2. Repetir não descobre nada novo.
	r = f.combinar("erva_lunar", "erva_lunar")
	_checar(r.is_sucesso() and not r.is_receita_nova(), "repetir não é receita nova")
	_checar(r.get_composicoes_reveladas().is_empty(), "repetir não revela composição")

	# 3. A+B == B+A.
	var ab: ResultadoMistura = f.combinar("cogumelo_roxo", "pepita_de_cobre")
	var ba: ResultadoMistura = f.combinar("pepita_de_cobre", "cogumelo_roxo")
	_checar(ab.get_id_receita() == ba.get_id_receita(), "A+B e B+A dão a mesma receita")
	_checar(ab.get_composicoes_reveladas().size() == 2, "A+B revela as duas composições")

	# 4. Poção não entra na bancada.
	r = f.combinar(r.get_pocao().id, "erva_lunar")
	_checar(r.get_falha() == ResultadoMistura.ITEM_NAO_MISTURAVEL, "poção não mistura")

	# 5. Refazer só funciona para receitas descobertas.
	r = f.refazer_receita("cristal_de_quartzo+pena_de_corvo")
	_checar(r.get_falha() == ResultadoMistura.RECEITA_DESCONHECIDA, "refazer receita desconhecida falha")
	r = f.refazer_receita(ab.get_id_receita())
	_checar(r.is_sucesso(), "refazer receita conhecida funciona")
	_checar(_quantidade(f, "cogumelo_roxo") == 2, "refazer consome ingredientes")

	# 6. Sem ingredientes (sobrou 1 Erva Lunar, A+A precisa de 2).
	r = f.combinar("erva_lunar", "erva_lunar")
	_checar(r.get_falha() == ResultadoMistura.INGREDIENTE_INSUFICIENTE, "sem ingrediente falha")
	_checar(not f.pode_refazer("erva_lunar+erva_lunar"), "pode_refazer falso sem ingrediente")

	# 7. Toda dupla tem receita (36 no total).
	f.coletar_ingredientes(20)
	var ids := []
	for ingrediente: Dictionary in f.listar_ingredientes():
		ids.append(ingrediente.id)
	for i in ids.size():
		for j in range(i, ids.size()):
			_checar(f.combinar(ids[i], ids[j]).is_sucesso(), "receita %s+%s" % [ids[i], ids[j]])
	_checar(f.listar_receitas_descobertas().size() == 36, "36 receitas descobertas")

	# 8. O livro sobrevive a uma nova instância (persistência em user://).
	var outra := _nova_fachada()
	_checar(outra.listar_receitas_descobertas().size() == 36, "livro persistido e recarregado")
	_checar(not outra.consultar_composicao("erva_lunar").is_empty(), "composição persistida")

	f.esquecer_livro()
	print("RESULTADO: %s (%d falha(s))" % ["OK" if _falhas == 0 else "FALHOU", _falhas])
	quit(_falhas)


func _nova_fachada() -> AlquimiaFacade:
	var f := AlquimiaFacade.new()
	root.add_child(f) # dispara _ready: carrega dados e o livro salvo
	return f


func _quantidade(f: AlquimiaFacade, id: String) -> int:
	for item: Dictionary in f.listar_inventario():
		if item.id == id:
			return item.quantidade
	return 0


func _checar(condicao: bool, descricao: String) -> void:
	if not condicao:
		_falhas += 1
		print("FALHOU: ", descricao)
