# G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
# Padrão: Composite
# Autor: João Victor da Silva Batista de Farias (@beyondmagic)
# Arquivo: ui_button.gd
#
# Papel no Padrão GoF: Leaf (Folha)
# Representa um botão de ação interativo do HUD ou menu, como comandos
# de combate ("Atacar", "Misturar Elementos", "Fugir") ou seleção de itens.

class_name UIButton
extends UIComponent

## Texto exibido no corpo do botão.
var label_text: String = "Botão"

## Identificador único da ação disparada quando pressionado.
var action_id: String = ""

## Se o botão está ativo para interação.
var is_enabled: bool = true

## Contador de cliques para verificação em testes/execução.
var click_count: int = 0

func _init(p_name: String = "UIButton", p_text: String = "Ação", p_action_id: String = "", p_pos: Vector2 = Vector2.ZERO) -> void:
	super._init(p_name, p_pos, Vector2(120, 32))
	label_text = p_text
	action_id = p_action_id if p_action_id != "" else p_name.to_lower()

func render(indent_level: int = 0) -> String:
	if not visible:
		return ""
	var indent: String = get_indent_string(indent_level)
	var state_str: String = "[ATIVO]" if is_enabled else "[DESATIVADO]"
	return indent + "|-- (Botão: " + name + ") [" + label_text + "] -> ação: '" + action_id + "' " + state_str + "\n"

func handle_input(action_name: String) -> bool:
	if not visible or not is_enabled:
		return false
	
	if action_name == action_id or action_name == name:
		click_count += 1
		print("   >> [EVENTO UI] Botão '" + name + "' acionado com sucesso! (Ação: " + action_id + ", Cliques: " + str(click_count) + ")")
		return true
	
	return false

