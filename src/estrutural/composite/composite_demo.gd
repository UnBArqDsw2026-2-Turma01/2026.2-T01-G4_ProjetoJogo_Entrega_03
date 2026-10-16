# G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
# Padrão: Composite
# Autor: João Victor da Silva Batista de Farias (@beyondmagic)
# Arquivo: composite_demo.gd
#
# Demonstração em Execução do Padrão GoF Composite.
# Pode ser executado pelo Godot Engine no modo headless:
#   godot --headless -s src/estrutural/composite/composite_demo.gd
# ou ser acionado dentro de uma cena do Godot 4.

class_name CompositeDemo
extends SceneTree

func _init() -> void:
	run_demonstration()
	quit()

static func run_demonstration() -> void:
	print("\n" + "=".repeat(75))
	print(" G4_ProjetoJogo — DEMONSTRAÇÃO DO PADRÃO GOF COMPOSITE (UI)")
	print(" SubEquipe 02: João Victor da Silva Batista de Farias (@beyondmagic)")
	print("=".repeat(75) + "\n")

	# -------------------------------------------------------------------------
	# 1. Construção da Árvore de UI usando BattleHUD (Composite Raiz)
	# -------------------------------------------------------------------------
	print("[PASSO 1] Construindo a árvore de UI da Batalha (Composite)...")
	var hud: BattleHUD = BattleHUD.new("Alquimista Edward", "Gólem de Enxofre")
	print(">> Árvore instanciada com sucesso!\n")

	# -------------------------------------------------------------------------
	# 2. Renderização Polimórfica Uniforme da Árvore Completa
	# -------------------------------------------------------------------------
	print("[PASSO 2] Renderizando a HUD completa chamando hud.render():")
	print("-".repeat(75))
	print(hud.render())
	print("-".repeat(75) + "\n")

	# -------------------------------------------------------------------------
	# 3. Propagação Recursiva de Eventos de Entrada (handle_input)
	# -------------------------------------------------------------------------
	print("[PASSO 3] Enviando comandos de entrada para o Composite Raiz:")
	print(">> Testando comando 'cmd_mix' (Misturar Elementos):")
	var handled_mix: bool = hud.handle_input("cmd_mix")
	print("   Resultado: " + ("Consumido pela árvore" if handled_mix else "Não tratado") + "\n")

	print(">> Testando comando 'cmd_attack' (Atacar):")
	var handled_attack: bool = hud.handle_input("cmd_attack")
	print("   Resultado: " + ("Consumido pela árvore" if handled_attack else "Não tratado") + "\n")

	print(">> Testando comando desconhecido 'cmd_dançar':")
	var handled_unknown: bool = hud.handle_input("cmd_dançar")
	print("   Resultado: " + ("Consumido pela árvore" if handled_unknown else "Não tratado (esperado)") + "\n")

	# -------------------------------------------------------------------------
	# 4. Modificação Dinâmica da Estrutura: Adicionando Subárvore de Alquimia
	# -------------------------------------------------------------------------
	print("[PASSO 4] Adicionando dinamicamente um novo Composite (Bancada de Mistura Rápida):")
	var alchemy_popup: UIPanel = UIPanel.new("AlchemyQuickBench", "Bancada Rápida de Campo", Vector2(700, 400), Vector2(400, 200))
	alchemy_popup.add(UILabel.new("LblSlot1", "Slot A: [H] Hidrogênio x2"))
	alchemy_popup.add(UILabel.new("LblSlot2", "Slot B: [O] Oxigênio x1"))
	alchemy_popup.add(UIButton.new("BtnConfirmMix", "Sintetizar [H2O - Poção de Água Pura]", "cmd_synthesize"))
	
	hud.add(alchemy_popup)
	print(">> Subárvore 'AlchemyQuickBench' acoplada como filho do Composite raiz!\n")

	# -------------------------------------------------------------------------
	# 5. Ocultando um Composto e Renderizando Novamente
	# -------------------------------------------------------------------------
	print("[PASSO 5] Ocultando o Menu de Comandos (ação de fim de turno):")
	hud.set_turn_active(false)
	print(">> hud.set_turn_active(false) executado.")
	print(">> Renderizando novamente para evidenciar que o Composite 'ActionMenuPanel' e todos os seus filhos foram omitidos recursivamente:")
	print("-".repeat(75))
	print(hud.render())
	print("-".repeat(75) + "\n")

	# -------------------------------------------------------------------------
	# 6. Testando acionamento da nova janela de alquimia
	# -------------------------------------------------------------------------
	print("[PASSO 6] Disparando comando na janela recém-adicionada ('cmd_synthesize'):")
	var handled_synth: bool = hud.handle_input("cmd_synthesize")
	print("   Resultado: " + ("Consumido com sucesso pela nova subárvore" if handled_synth else "Não tratado") + "\n")

	print("=".repeat(75))
	print(" DEMONSTRAÇÃO CONCLUÍDA COM SUCESSO! O padrão Composite opera conforme o GoF.")
	print("=".repeat(75) + "\n")

