# G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
# Padrão: Composite
# Autor: João Victor da Silva Batista de Farias (@beyondmagic)
# Arquivo: ui_panel.gd
#
# Papel no Padrão GoF: Concrete Composite
# Representa um painel ou janela da interface (ex: Painel de Comandos,
# Painel de Status, Janela de Alquimia) que agrupa múltiplos nós de UI.

class_name UIPanel
extends UIComposite

## Título decorativo do painel.
var title: String = ""

func _init(p_name: String = "UIPanel", p_title: String = "", p_pos: Vector2 = Vector2.ZERO, p_size: Vector2 = Vector2(300, 200)) -> void:
	super._init(p_name, p_pos, p_size)
	title = p_title if p_title != "" else p_name

func render(indent_level: int = 0) -> String:
	if not visible:
		return ""
	
	var indent: String = get_indent_string(indent_level)
	var output: String = indent + "+========================================================\n"
	output += indent + "| [PAINEL: " + title + "] (id: " + name + ", pos: " + str(position) + ")\n"
	output += indent + "+--------------------------------------------------------\n"
	
	for child in children:
		output += child.render(indent_level + 1)
		
	output += indent + "+========================================================\n"
	return output

