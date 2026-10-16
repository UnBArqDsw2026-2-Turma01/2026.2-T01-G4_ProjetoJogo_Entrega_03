// G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
// Padrão: Composite
// Autor: João Victor da Silva Batista de Farias (@beyondmagic)
// Arquivo: battle_hud.hpp
//
// Papel no Padrão GoF: Domain-Specific Composite
// Representa a interface completa de Batalha (BattleHUD) do G4_ProjetoJogo.

#ifndef G4_BATTLE_HUD_HPP
#define G4_BATTLE_HUD_HPP

#include "ui_panel.hpp"
#include "ui_label.hpp"
#include "ui_button.hpp"
#include "ui_progress_bar.hpp"

namespace G4UI {

class BattleHUD : public UIComposite {
private:
    std::shared_ptr<UIPanel> m_playerPanel;
    std::shared_ptr<UIPanel> m_actionMenu;
    std::shared_ptr<UIPanel> m_enemyPanel;
    std::shared_ptr<UIPanel> m_combatLog;

public:
    BattleHUD(const std::string& playerName = "Alquimista Edward", const std::string& enemyName = "Gólem de Enxofre")
        : UIComposite("BattleHUD_Root", Vector2(0.0f, 0.0f), Vector2(1920.0f, 1080.0f)) {
        buildHUD(playerName, enemyName);
    }

    void buildHUD(const std::string& playerName, const std::string& enemyName) {
        // 1. Painel do Jogador
        m_playerPanel = std::make_shared<UIPanel>("PlayerStatusPanel", "Status do Jogador", Vector2(40.0f, 750.0f), Vector2(400.0f, 250.0f));
        m_playerPanel->add(std::make_shared<UILabel>("LblPlayerName", "Herói: " + playerName + " (Nível 4)"));
        m_playerPanel->add(std::make_shared<UIProgressBar>("BarHP", "HP", 85.0f, 100.0f));
        m_playerPanel->add(std::make_shared<UIProgressBar>("BarMP", "MP", 42.0f, 50.0f));
        m_playerPanel->add(std::make_shared<UIProgressBar>("BarATB", "ATB", 100.0f, 100.0f));
        add(m_playerPanel);

        // 2. Painel de Comandos
        m_actionMenu = std::make_shared<UIPanel>("ActionMenuPanel", "Menu de Comandos (Turno Ativo)", Vector2(460.0f, 750.0f), Vector2(380.0f, 250.0f));
        m_actionMenu->add(std::make_shared<UILabel>("LblMenuPrompt", "Escolha a sua ação:"));
        m_actionMenu->add(std::make_shared<UIButton>("BtnAttack", "[1] Atacar", "cmd_attack"));
        m_actionMenu->add(std::make_shared<UIButton>("BtnMixChemistry", "[2] Misturar Elementos", "cmd_mix"));
        m_actionMenu->add(std::make_shared<UIButton>("BtnUseItem", "[3] Usar Poção/Item", "cmd_item"));
        m_actionMenu->add(std::make_shared<UIButton>("BtnFlee", "[4] Tentar Fuga", "cmd_flee"));
        add(m_actionMenu);

        // 3. Painel do Inimigo
        m_enemyPanel = std::make_shared<UIPanel>("EnemyStatusPanel", "Inimigo Encontrado", Vector2(1400.0f, 60.0f), Vector2(450.0f, 200.0f));
        m_enemyPanel->add(std::make_shared<UILabel>("LblEnemyName", "Alvo: " + enemyName + " [Elemento: Terra/Enxofre]"));
        m_enemyPanel->add(std::make_shared<UIProgressBar>("BarEnemyHP", "HP Inimigo", 140.0f, 200.0f));
        m_enemyPanel->add(std::make_shared<UIProgressBar>("BarEnemyATB", "ATB Inimigo", 65.0f, 100.0f));
        add(m_enemyPanel);

        // 4. Histórico de Batalha
        m_combatLog = std::make_shared<UIPanel>("CombatLogPanel", "Histórico de Batalha", Vector2(600.0f, 40.0f), Vector2(600.0f, 120.0f));
        m_combatLog->add(std::make_shared<UILabel>("LogLine1", "[Sistema] Encontro de batalha iniciado!"));
        m_combatLog->add(std::make_shared<UILabel>("LogLine2", "[Vez] A barra de ATB do jogador encheu primeiro."));
        add(m_combatLog);
    }

    void setTurnActive(bool isPlayerTurn) {
        if (m_actionMenu) {
            m_actionMenu->setVisible(isPlayerTurn);
        }
    }
};

} // namespace G4UI

#endif // G4_BATTLE_HUD_HPP

