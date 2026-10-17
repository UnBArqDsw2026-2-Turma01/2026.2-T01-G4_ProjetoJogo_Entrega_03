# G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
# Padrão: Composite
# Autor: João Victor da Silva Batista de Farias (@beyondmagic)
# Arquivo: ui_composite.gd
#
# Papel no Padrão GoF: Composite (Nó Composto)
# Define o comportamento para nós que contêm filhos. Implementa as operações
# de gerenciamento de filhos e delega a execução de operações aos seus elementos.

class_name UIComposite
extends UIComponent

## Lista de componentes filhos gerenciados por este contêiner.
var children: Array[UIComponent] = []

func _init(p_name: String = "UIComposite", p_pos: Vector2 = Vector2.ZERO, p_size: Vector2 = Vector2.ZERO) -> void:
	super._init(p_name, p_pos, p_size)

## Retorna true para nós compostos.
func is_composite() -> bool:
	return true

## Adiciona um componente filho à hierarquia.
func add(component: UIComponent) -> void:
	if component == null:
		return
	if not children.has(component):
		children.append(component)
		component.parent = self

## Remove um componente filho da hierarquia.
func remove(component: UIComponent) -> void:
	if children.has(component):
		children.erase(component)
		component.parent = null

## Retorna um filho pelo índice.
func get_child(index: int) -> UIComponent:
	if index >= 0 and index < children.size():
		return children[index]
	return null

## Retorna a lista completa de filhos.
func get_children() -> Array[UIComponent]:
	return children

## Busca recursiva de um componente na subárvore pelo nome.
func find_by_name(target_name: String) -> UIComponent:
	if name == target_name:
		return self
	for child in children:
		if child is UIComposite:
			var found: UIComponent = child.find_by_name(target_name)
			if found != null:
				return found
		elif child.name == target_name:
			return child
	return null

## Renderização polimórfica recursiva:
## Imprime a moldura do contêiner e invoca render() de cada filho visível.
func render(indent_level: int = 0) -> String:
	if not visible:
		return ""
	
	var indent: String = get_indent_string(indent_level)
	var output: String = indent + "+-- [COMPOSTO: " + name + "] (pos: " + str(position) + ", size: " + str(size) + ")\n"
	
	for child in children:
		output += child.render(indent_level + 1)
	
	return output

## Delegação recursiva de entrada aos filhos.
func handle_input(action_name: String) -> bool:
	if not visible:
		return false
	
	# Percorre do último para o primeiro (ordem z-index típica de UI)
	for i in range(children.size() - 1, -1, -1):
		var child: UIComponent = children[i]
		if child.handle_input(action_name):
			return true
	
	return false

