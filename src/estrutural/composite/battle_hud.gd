# G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
# Padrão: Composite
# Autor: João Victor da Silva Batista de Farias (@beyondmagic)
# Arquivo: battle_hud.gd
#
# Papel no Padrão GoF: Concrete Composite de Domínio
# Representa a HUD completa de Batalha do G4_ProjetoJogo.
# Monta uma hierarquia profunda de nós de interface para demonstrar a elegância
# do padrão Composite ao tratar telas inteiras, painéis e elementos individuais uniformemente.

class_name BattleHUD
extends UIComposite

var player_panel: UIPanel
var action_menu: UIPanel
var enemy_panel: UIPanel
var combat_log: UIPanel

func _init(p_player_name: String = "Alquimista", p_enemy_name: String = "Gólem de Enxofre") -> void:
	super._init("BattleHUD_Root", Vector2.ZERO, Vector2(1920, 1080))
	_build_hud(p_player_name, p_enemy_name)

func _build_hud(p_name: String, e_name: String) -> void:
	# 1. Painel do Jogador (Canto inferior esquerdo)
	player_panel = UIPanel.new("PlayerStatusPanel", "Status do Jogador", Vector2(40, 750), Vector2(400, 250))
	player_panel.add(UILabel.new("LblPlayerName", "Herói: " + p_name + " (Nível 4)"))
	player_panel.add(UIProgressBar.new("BarHP", "HP", 85.0, 100.0))
	player_panel.add(UIProgressBar.new("BarMP", "MP", 42.0, 50.0))
	player_panel.add(UIProgressBar.new("BarATB", "ATB", 100.0, 100.0))
	add(player_panel)

	# 2. Painel de Comandos / Ações de Combate (Centro inferior)
	action_menu = UIPanel.new("ActionMenuPanel", "Menu de Comandos (Turno Ativo)", Vector2(460, 750), Vector2(380, 250))
	action_menu.add(UILabel.new("LblMenuPrompt", "Escolha a sua ação:"))
	action_menu.add(UIButton.new("BtnAttack", "[1] Atacar", "cmd_attack"))
	action_menu.add(UIButton.new("BtnMixChemistry", "[2] Misturar Elementos", "cmd_mix"))
	action_menu.add(UIButton.new("BtnUseItem", "[3] Usar Poção/Item", "cmd_item"))
	action_menu.add(UIButton.new("BtnFlee", "[4] Tentar Fuga", "cmd_flee"))
	add(action_menu)

	# 3. Painel do Inimigo (Canto superior direito)
	enemy_panel = UIPanel.new("EnemyStatusPanel", "Inimigo Encontrado", Vector2(1400, 60), Vector2(450, 200))
	enemy_panel.add(UILabel.new("LblEnemyName", "Alvo: " + e_name + " [Elemento: Terra/Enxofre]"))
	enemy_panel.add(UIProgressBar.new("BarEnemyHP", "HP Inimigo", 140.0, 200.0))
	enemy_panel.add(UIProgressBar.new("BarEnemyATB", "ATB Inimigo", 65.0, 100.0))
	add(enemy_panel)

	# 4. Painel de Log de Batalha (Centro superior)
	combat_log = UIPanel.new("CombatLogPanel", "Histórico de Batalha", Vector2(600, 40), Vector2(600, 120))
	combat_log.add(UILabel.new("LogLine1", "[Sistema] Encontro de batalha iniciado!"))
	combat_log.add(UILabel.new("LogLine2", "[Vez] A barra de ATB do jogador encheu primeiro."))
	add(combat_log)

## Método de conveniência que atualiza o status do turno (ex: ocultando o menu de ações)
func set_turn_active(is_player_turn: bool) -> void:
	if action_menu != null:
		action_menu.set_visible(is_player_turn)

