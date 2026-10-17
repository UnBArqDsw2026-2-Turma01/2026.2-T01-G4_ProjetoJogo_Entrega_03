#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
Verificador / Espelho de Execução do Padrão Composite (UI)
Autor: João Victor da Silva Batista de Farias (@beyondmagic)

Permite demonstrar a execução exata da hierarquia Composite mesmo em
ambientes sem o binário do Godot 4 instalado, validando a lógica estrutural.
"""

from typing import List, Optional

class Vector2:
    def __init__(self, x: float = 0.0, y: float = 0.0):
        self.x = x
        self.y = y
    def __str__(self):
        return f"({self.x}, {self.y})"

class UIComponent:
    def __init__(self, name: str = "UIComponent", pos: Vector2 = None, size: Vector2 = None):
        self.name = name
        self.position = pos or Vector2()
        self.size = size or Vector2()
        self.visible = True
        self.parent: Optional['UIComponent'] = None

    def render(self, indent: int = 0) -> str:
        raise NotImplementedError

    def handle_input(self, action: str) -> bool:
        return False

    def set_visible(self, is_visible: bool):
        self.visible = is_visible

    def is_composite(self) -> bool:
        return False

    def add(self, comp: 'UIComponent'):
        pass

    def remove(self, comp: 'UIComponent'):
        pass

    def get_children(self) -> List['UIComponent']:
        return []

    def get_indent(self, level: int) -> str:
        return "    " * level

class UIComposite(UIComponent):
    def __init__(self, name: str = "UIComposite", pos: Vector2 = None, size: Vector2 = None):
        super().__init__(name, pos, size)
        self.children: List[UIComponent] = []

    def is_composite(self) -> bool:
        return True

    def add(self, comp: UIComponent):
        if comp and comp not in self.children:
            self.children.append(comp)
            comp.parent = self

    def remove(self, comp: UIComponent):
        if comp in self.children:
            self.children.remove(comp)
            comp.parent = None

    def get_children(self) -> List[UIComponent]:
        return self.children

    def render(self, indent: int = 0) -> str:
        if not self.visible:
            return ""
        ind = self.get_indent(indent)
        out = f"{ind}+-- [COMPOSTO: {self.name}] (pos: {self.position}, size: {self.size})\n"
        for child in self.children:
            out += child.render(indent + 1)
        return out

    def handle_input(self, action: str) -> bool:
        if not self.visible:
            return False
        for child in reversed(self.children):
            if child.handle_input(action):
                return True
        return False

class UILabel(UIComponent):
    def __init__(self, name: str, text: str, pos: Vector2 = None):
        super().__init__(name, pos, Vector2(len(text) * 8, 16))
        self.text = text

    def render(self, indent: int = 0) -> str:
        if not self.visible:
            return ""
        ind = self.get_indent(indent)
        return f'{ind}|-- (Rótulo: {self.name}) "{self.text}"\n'

class UIButton(UIComponent):
    def __init__(self, name: str, text: str, action_id: str = "", pos: Vector2 = None):
        super().__init__(name, pos, Vector2(120, 32))
        self.text = text
        self.action_id = action_id or name.lower()
        self.is_enabled = True
        self.click_count = 0

    def render(self, indent: int = 0) -> str:
        if not self.visible:
            return ""
        ind = self.get_indent(indent)
        state = "[ATIVO]" if self.is_enabled else "[DESATIVADO]"
        return f"{ind}|-- (Botão: {self.name}) [{self.text}] -> ação: '{self.action_id}' {state}\n"

    def handle_input(self, action: str) -> bool:
        if not self.visible or not self.is_enabled:
            return False
        if action in (self.action_id, self.name):
            self.click_count += 1
            print(f"   >> [EVENTO UI] Botão '{self.name}' acionado com sucesso! (Ação: {self.action_id}, Cliques: {self.click_count})")
            return True
        return False

class UIProgressBar(UIComponent):
    def __init__(self, name: str, bar_type: str, cur: float, mx: float, pos: Vector2 = None):
        super().__init__(name, pos, Vector2(160, 20))
        self.bar_type = bar_type
        self.current_value = cur
        self.max_value = mx

    def get_percentage(self) -> float:
        return (self.current_value / self.max_value) * 100.0 if self.max_value > 0 else 0.0

    def render(self, indent: int = 0) -> str:
        if not self.visible:
            return ""
        ind = self.get_indent(indent)
        pct = self.get_percentage()
        total_blocks = 10
        filled = int(round((pct / 100.0) * total_blocks))
        bar_visual = "█" * filled + "░" * (total_blocks - filled)
        return f"{ind}|-- (Barra {self.bar_type}: {self.name}) [{bar_visual}] {int(self.current_value)}/{int(self.max_value)} ({int(pct)}%)\n"

class UIPanel(UIComposite):
    def __init__(self, name: str, title: str, pos: Vector2 = None, size: Vector2 = None):
        super().__init__(name, pos, size or Vector2(300, 200))
        self.title = title

    def render(self, indent: int = 0) -> str:
        if not self.visible:
            return ""
        ind = self.get_indent(indent)
        out = f"{ind}+========================================================\n"
        out += f"{ind}| [PAINEL: {self.title}] (id: {self.name}, pos: {self.position})\n"
        out += f"{ind}+--------------------------------------------------------\n"
        for child in self.children:
            out += child.render(indent + 1)
        out += f"{ind}+========================================================\n"
        return out

class BattleHUD(UIComposite):
    def __init__(self, player_name: str, enemy_name: str):
        super().__init__("BattleHUD_Root", Vector2(0, 0), Vector2(1920, 1080))
        # 1. Painel Jogador
        self.player_panel = UIPanel("PlayerStatusPanel", "Status do Jogador", Vector2(40, 750), Vector2(400, 250))
        self.player_panel.add(UILabel("LblPlayerName", f"Herói: {player_name} (Nível 4)"))
        self.player_panel.add(UIProgressBar("BarHP", "HP", 85.0, 100.0))
        self.player_panel.add(UIProgressBar("BarMP", "MP", 42.0, 50.0))
        self.player_panel.add(UIProgressBar("BarATB", "ATB", 100.0, 100.0))
        self.add(self.player_panel)

        # 2. Painel Comandos
        self.action_menu = UIPanel("ActionMenuPanel", "Menu de Comandos (Turno Ativo)", Vector2(460, 750), Vector2(380, 250))
        self.action_menu.add(UILabel("LblMenuPrompt", "Escolha a sua ação:"))
        self.action_menu.add(UIButton("BtnAttack", "[1] Atacar", "cmd_attack"))
        self.action_menu.add(UIButton("BtnMixChemistry", "[2] Misturar Elementos", "cmd_mix"))
        self.action_menu.add(UIButton("BtnUseItem", "[3] Usar Poção/Item", "cmd_item"))
        self.action_menu.add(UIButton("BtnFlee", "[4] Tentar Fuga", "cmd_flee"))
        self.add(self.action_menu)

        # 3. Painel Inimigo
        self.enemy_panel = UIPanel("EnemyStatusPanel", "Inimigo Encontrado", Vector2(1400, 60), Vector2(450, 200))
        self.enemy_panel.add(UILabel("LblEnemyName", f"Alvo: {enemy_name} [Elemento: Terra/Enxofre]"))
        self.enemy_panel.add(UIProgressBar("BarEnemyHP", "HP Inimigo", 140.0, 200.0))
        self.enemy_panel.add(UIProgressBar("BarEnemyATB", "ATB Inimigo", 65.0, 100.0))
        self.add(self.enemy_panel)

        # 4. Histórico
        self.combat_log = UIPanel("CombatLogPanel", "Histórico de Batalha", Vector2(600, 40), Vector2(600, 120))
        self.combat_log.add(UILabel("LogLine1", "[Sistema] Encontro de batalha iniciado!"))
        self.combat_log.add(UILabel("LogLine2", "[Vez] A barra de ATB do jogador encheu primeiro."))
        self.add(self.combat_log)

    def set_turn_active(self, is_player_turn: bool):
        self.action_menu.set_visible(is_player_turn)

def main():
    print("\n" + "=" * 75)
    print(" G4_ProjetoJogo — DEMONSTRAÇÃO DO PADRÃO GOF COMPOSITE (UI)")
    print(" SubEquipe 02: João Victor da Silva Batista de Farias (@beyondmagic)")
    print("=" * 75 + "\n")

    print("[PASSO 1] Construindo a árvore de UI da Batalha (Composite)...")
    hud = BattleHUD("Alquimista Edward", "Gólem de Enxofre")
    print(">> Árvore instanciada com sucesso!\n")

    print("[PASSO 2] Renderizando a HUD completa chamando hud.render():")
    print("-" * 75)
    print(hud.render(), end="")
    print("-" * 75 + "\n")

    print("[PASSO 3] Enviando comandos de entrada para o Composite Raiz:")
    print(">> Testando comando 'cmd_mix' (Misturar Elementos):")
    handled_mix = hud.handle_input("cmd_mix")
    print(f"   Resultado: {'Consumido pela árvore' if handled_mix else 'Não tratado'}\n")

    print(">> Testando comando 'cmd_attack' (Atacar):")
    handled_attack = hud.handle_input("cmd_attack")
    print(f"   Resultado: {'Consumido pela árvore' if handled_attack else 'Não tratado'}\n")

    print(">> Testando comando desconhecido 'cmd_dançar':")
    handled_unknown = hud.handle_input("cmd_dançar")
    print(f"   Resultado: {'Consumido pela árvore' if handled_unknown else 'Não tratado (esperado)'}\n")

    print("[PASSO 4] Adicionando dinamicamente um novo Composite (Bancada de Mistura Rápida):")
    bench = UIPanel("AlchemyQuickBench", "Bancada Rápida de Campo", Vector2(700, 400), Vector2(400, 200))
    bench.add(UILabel("LblSlot1", "Slot A: [H] Hidrogênio x2"))
    bench.add(UILabel("LblSlot2", "Slot B: [O] Oxigênio x1"))
    bench.add(UIButton("BtnConfirmMix", "Sintetizar [H2O - Poção de Água Pura]", "cmd_synthesize"))
    hud.add(bench)
    print(">> Subárvore 'AlchemyQuickBench' acoplada como filho do Composite raiz!\n")

    print("[PASSO 5] Ocultando o Menu de Comandos (ação de fim de turno):")
    hud.set_turn_active(False)
    print(">> hud.set_turn_active(False) executado.")
    print(">> Renderizando novamente para evidenciar que o Composite 'ActionMenuPanel' e todos os seus filhos foram omitidos recursivamente:")
    print("-" * 75)
    print(hud.render(), end="")
    print("-" * 75 + "\n")

    print("[PASSO 6] Disparando comando na janela recém-adicionada ('cmd_synthesize'):")
    handled_synth = hud.handle_input("cmd_synthesize")
    print(f"   Resultado: {'Consumido com sucesso pela nova subárvore' if handled_synth else 'Não tratado'}\n")

    print("=" * 75)
    print(" DEMONSTRAÇÃO CONCLUÍDA COM SUCESSO! O padrão Composite opera conforme o GoF.")
    print("=" * 75 + "\n")

if __name__ == "__main__":
    main()

