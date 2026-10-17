# G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
# Padrão: Composite
# Autor: João Victor da Silva Batista de Farias (@beyondmagic)
# Arquivo: ui_progress_bar.gd
#
# Papel no Padrão GoF: Leaf (Folha)
# Representa uma barra de preenchimento ou progresso (HP, MP, Barra de ATB).

class_name UIProgressBar
extends UIComponent

## Valor atual do medidor.
var current_value: float = 100.0

## Valor máximo do medidor.
var max_value: float = 100.0

## Rótulo identificador (ex: "HP", "MP", "ATB").
var bar_type: String = "HP"

func _init(p_name: String = "UIProgressBar", p_type: String = "HP", p_cur: float = 100.0, p_max: float = 100.0, p_pos: Vector2 = Vector2.ZERO) -> void:
	super._init(p_name, p_pos, Vector2(160, 20))
	bar_type = p_type
	current_value = p_cur
	max_value = p_max

func set_values(cur: float, mx: float) -> void:
	current_value = clamp(cur, 0.0, mx)
	max_value = mx

func get_percentage() -> float:
	if max_value <= 0.0:
		return 0.0
	return (current_value / max_value) * 100.0

func render(indent_level: int = 0) -> String:
	if not visible:
		return ""
	var indent: String = get_indent_string(indent_level)
	var pct: float = get_percentage()
	var total_blocks: int = 10
	var filled_blocks: int = int(round((pct / 100.0) * total_blocks))
	var bar_visual: String = ""
	for i in range(total_blocks):
		if i < filled_blocks:
			bar_visual += "█"
		else:
			bar_visual += "░"
	
	return indent + "|-- (Barra " + bar_type + ": " + name + ") [" + bar_visual + "] " + str(int(current_value)) + "/" + str(int(max_value)) + " (" + str(int(pct)) + "%)\n"

