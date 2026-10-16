extends SceneTree
## Abre o mundo, coleta um ingrediente, abre um baú e a mochila, salvando prints em user://.

func _initialize() -> void:
	await process_frame
	var mundo: Node2D = load("res://scenes/mundo.tscn").instantiate()
	root.add_child(mundo)
	await _esperar(10)
	_salvar("mundo_1_inicio")

	var coletavel = _primeiro(mundo, "IngredienteColetavel")
	var antes := _quantidade(coletavel.get_id_ingrediente())
	mundo.get_jogador().position = coletavel.position + Vector2(40, 0)
	Input.action_press("mover_esquerda")
	await _esperar(25)
	Input.action_release("mover_esquerda")
	print("coleta no chão: +", _quantidade(coletavel.get_id_ingrediente()) - antes, " ", coletavel.get_id_ingrediente())

	var bau = _primeiro(mundo, "Bau")
	mundo.get_jogador().position = bau.position + Vector2(0, 40)
	await _esperar(20)
	_salvar("mundo_2_bau_perto")
	var total_antes := _total()
	var tecla := InputEventAction.new()
	tecla.action = "interagir"
	tecla.pressed = true
	Input.parse_input_event(tecla)
	await _esperar(6)
	print("baú deu ", _total() - total_antes, " itens")
	_salvar("mundo_3_bau_aberto")

	mundo.alternar_mochila()
	await _esperar(4)
	_salvar("mundo_4_mochila")
	quit()

func _primeiro(mundo: Node, classe: String) -> Node:
	for filho in mundo.get_entidades().get_children():
		if filho.get_class() == classe:
			return filho
	return null

func _total() -> int:
	var t := 0
	for item: Dictionary in root.get_node("Alquimia").listar_inventario():
		t += item.quantidade
	return t

func _quantidade(id: String) -> int:
	for item: Dictionary in root.get_node("Alquimia").listar_inventario():
		if item.id == id:
			return item.quantidade
	return 0

func _esperar(n: int) -> void:
	for i in n:
		await process_frame

func _salvar(nome: String) -> void:
	root.get_texture().get_image().save_png(OS.get_user_data_dir().path_join("captura_%s.png" % nome))
