extends SceneTree
## Abre a mochila, faz algumas misturas e salva prints em user://captura_*.png.

func _initialize() -> void:
	await process_frame
	var alquimia: Node = root.get_node_or_null("Alquimia")
	alquimia.esquecer_livro()
	var cena: Control = load("res://scenes/mochila.tscn").instantiate()
	root.add_child(cena)
	await _esperar()
	_salvar("1_inicio")
	cena.colocar_na_bancada("erva_lunar")
	cena.colocar_na_bancada("cogumelo_roxo")
	await _esperar()
	_salvar("2_bancada")
	cena.misturar()
	cena.colocar_na_bancada("cristal_de_quartzo")
	cena.colocar_na_bancada("cristal_de_quartzo")
	cena.misturar()
	cena.colocar_na_bancada("salvia_de_fogo")
	cena.colocar_na_bancada("escama_de_salamandra")
	cena.misturar()
	await _esperar()
	_salvar("3_misturas")
	cena.colocar_na_bancada("pocao_de_foco")
	cena.colocar_na_bancada("erva_lunar")
	cena.misturar()
	await _esperar()
	_salvar("4_pocao_recusada")
	alquimia.esquecer_livro()
	quit()

func _esperar() -> void:
	for i in 4:
		await process_frame

func _salvar(nome: String) -> void:
	var caminho := OS.get_user_data_dir().path_join("captura_%s.png" % nome)
	root.get_texture().get_image().save_png(caminho)
	print("salvo: ", caminho)
