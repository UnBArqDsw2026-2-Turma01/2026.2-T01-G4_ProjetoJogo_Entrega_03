# G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
# Padrão: Composite
# Autor: João Victor da Silva Batista de Farias (@beyondmagic)
# Arquivo: ui_label.gd
#
# Papel no Padrão GoF: Leaf (Folha)
# Representa um elemento atômico de texto na interface gráfica, como
# títulos, informações de status, valores numéricos ou descrições.

class_name UILabel
extends UIComponent

## Texto exibido no rótulo.
var text: String = ""

func _init(p_name: String = "UILabel", p_text: String = "", p_pos: Vector2 = Vector2.ZERO) -> void:
	super._init(p_name, p_pos, Vector2(p_text.length() * 8, 16))
	text = p_text

func set_text(new_text: String) -> void:
	text = new_text
	size = Vector2(text.length() * 8, 16)

func render(indent_level: int = 0) -> String:
	if not visible:
		return ""
	var indent: String = get_indent_string(indent_level)
	return indent + "|-- (Rótulo: " + name + ") \"" + text + "\"\n"

