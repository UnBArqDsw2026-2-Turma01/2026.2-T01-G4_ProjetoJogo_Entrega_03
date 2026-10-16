// G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
// Padrão: Composite
// Autor: João Victor da Silva Batista de Farias (@beyondmagic)
// Arquivo: bancada_panel.hpp
//
// Papel no Padrão GoF: Composto Reutilizável (Composite Subtree)
// Painel da Bancada de Alquimia que pode ser acoplado tanto na Mochila
// quanto no HUD de Batalha (BattleHUD) de forma transparente e modular.

#ifndef G4_BANCADA_PANEL_HPP
#define G4_BANCADA_PANEL_HPP

#include "ui_panel.hpp"
#include "ui_button.hpp"
#include "ui_label.hpp"
#include <functional>
#include <string>

namespace G4UI {

class BancadaPanel : public UIPanel {
private:
    std::string m_slotA;
    std::string m_slotB;
    std::shared_ptr<UILabel> m_lblSlotA;
    std::shared_ptr<UILabel> m_lblSlotB;
    std::shared_ptr<UIButton> m_btnMix;
    std::function<void(const std::string&, const std::string&)> m_onMixCallback;

public:
    BancadaPanel(Vector2 pos = Vector2(580.0f, 40.0f), Vector2 size = Vector2(600.0f, 320.0f))
        : UIPanel("BancadaPanel", "Bancada de Alquimia", pos, size),
          m_slotA(""), m_slotB("") {
        buildPanel();
    }

    void buildPanel() {
        m_lblSlotA = std::make_shared<UILabel>("LblSlotA", "Slot 1: [Vazio]");
        auto lblPlus = std::make_shared<UILabel>("LblPlus", "   + (Reagente Combinado)");
        m_lblSlotB = std::make_shared<UILabel>("LblSlotB", "Slot 2: [Vazio]");
        m_btnMix = std::make_shared<UIButton>("BtnMisturar", "[Misturar Ingredientes]", "cmd_mix");

        add(m_lblSlotA);
        add(lblPlus);
        add(m_lblSlotB);
        add(m_btnMix);
    }

    void colocarNaBancada(const std::string& item) {
        if (m_slotA.empty()) {
            m_slotA = item;
            m_lblSlotA->setText("Slot 1: [" + item + "]");
        } else if (m_slotB.empty()) {
            m_slotB = item;
            m_lblSlotB->setText("Slot 2: [" + item + "]");
        } else {
            // Substitui o segundo slot se já estiver cheio
            m_slotB = item;
            m_lblSlotB->setText("Slot 2: [" + item + "]");
        }
    }

    void tirarDaBancada(int index) {
        if (index == 0) {
            m_slotA = "";
            m_lblSlotA->setText("Slot 1: [Vazio]");
        } else if (index == 1) {
            m_slotB = "";
            m_lblSlotB->setText("Slot 2: [Vazio]");
        }
    }

    void limpar() {
        m_slotA = "";
        m_slotB = "";
        m_lblSlotA->setText("Slot 1: [Vazio]");
        m_lblSlotB->setText("Slot 2: [Vazio]");
    }

    const std::string& getSlotA() const { return m_slotA; }
    const std::string& getSlotB() const { return m_slotB; }

    void setOnMixCallback(std::function<void(const std::string&, const std::string&)> callback) {
        m_onMixCallback = std::move(callback);
    }

    bool handleInput(const std::string& inputCommand) override {
        if (!m_visible) return false;

        if (inputCommand == "cmd_mix") {
            if (m_onMixCallback) {
                m_onMixCallback(m_slotA, m_slotB);
            }
            return UIPanel::handleInput(inputCommand);
        }
        return UIPanel::handleInput(inputCommand);
    }
};

} // namespace G4UI

#endif // G4_BANCADA_PANEL_HPP
