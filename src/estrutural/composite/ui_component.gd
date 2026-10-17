# G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
# Padrão: Composite
# Autor: João Victor da Silva Batista de Farias (@beyondmagic)
# Arquivo: ui_component.gd
#
# Papel no Padrão GoF: Component (Abstração Base)
# Define a interface comum para todos os elementos simples (folhas)
# e elementos compostos (contêineres/painéis) da interface gráfica (UI) do jogo.

class_name UIComponent
extends RefCounted

## Nome identificador do componente na árvore de UI.
var name: String = "UIComponent"

## Estado de visibilidade do componente.
var visible: bool = true

## Posição relativa (x, y) do componente na tela.
var position: Vector2 = Vector2.ZERO

## Dimensões estimadas (largura, altura) do componente.
var size: Vector2 = Vector2.ZERO

## Referência ao componente pai na árvore hierárquica (se houver).
var parent: UIComponent = null

func _init(p_name: String = "UIComponent", p_pos: Vector2 = Vector2.ZERO, p_size: Vector2 = Vector2.ZERO) -> void:
	name = p_name
	position = p_pos
	size = p_size

# -----------------------------------------------------------------------------
# Operações do Padrão Composite
# -----------------------------------------------------------------------------

## Renderiza o componente no formato textual hierárquico.
## Método polimórfico fundamental do padrão: tanto folhas quanto compostos
## respondem a render(), permitindo desenhar a árvore inteira uniformemente.
func render(indent_level: int = 0) -> String:
	push_error("Método render() deve ser implementado pelas subclasses.")
	return ""

## Processa um evento ou comando de entrada de forma recursiva.
## Retorna true caso o evento tenha sido consumido por este componente ou por algum de seus filhos.
func handle_input(action_name: String) -> bool:
	return false

## Altera a visibilidade do componente.
func set_visible(is_visible: bool) -> void:
	visible = is_visible

## Verifica se o componente é um nó composto (capaz de conter filhos).
func is_composite() -> bool:
	return false

# -----------------------------------------------------------------------------
# Gerenciamento de Filhos (Interface padrão GoF)
# Implementação padrão lança aviso/erro em Folhas que não aceitam filhos.
# -----------------------------------------------------------------------------

func add(component: UIComponent) -> void:
	push_warning("Operação add() não suportada em folhas (" + name + ").")

func remove(component: UIComponent) -> void:
	push_warning("Operação remove() não suportada em folhas (" + name + ").")

func get_child(index: int) -> UIComponent:
	return null

func get_children() -> Array[UIComponent]:
	var empty: Array[UIComponent] = []
	return empty

func get_indent_string(level: int) -> String:
	var result: String = ""
	for i in range(level):
		result += "    "
	return result

