// G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
// Padrão: Composite
// Autor: João Victor da Silva Batista de Farias (@beyondmagic)
// Arquivo: ui_button.hpp
//
// Papel no Padrão GoF: Leaf (Folha)
// Elemento atômico interativo (botão de comando do combate ou menus).

#ifndef G4_UI_BUTTON_HPP
#define G4_UI_BUTTON_HPP

#include "ui_component.hpp"
#include <sstream>

namespace G4UI {

class UIButton : public UIComponent {
private:
    std::string m_labelText;
    std::string m_actionId;
    bool m_isEnabled;
    int m_clickCount;

public:
    UIButton(std::string name, std::string text, std::string actionId = "", Vector2 pos = Vector2())
        : UIComponent(std::move(name), pos, Vector2(120.0f, 32.0f)),
          m_labelText(std::move(text)),
          m_actionId(actionId.empty() ? m_name : std::move(actionId)),
          m_isEnabled(true),
          m_clickCount(0) {}

    const std::string& getActionId() const { return m_actionId; }
    int getClickCount() const { return m_clickCount; }
    void setEnabled(bool enabled) { m_isEnabled = enabled; }

    std::string render(int indentLevel = 0) const override {
        if (!m_visible) return "";
        std::ostringstream ss;
        std::string state = m_isEnabled ? "[ATIVO]" : "[DESATIVADO]";
        ss << getIndent(indentLevel) << "|-- (Botão: " << m_name << ") [" << m_labelText 
           << "] -> ação: '" << m_actionId << "' " << state << "\n";
        return ss.str();
    }

    bool handleInput(const std::string& actionName) override {
        if (!m_visible || !m_isEnabled) return false;

        if (actionName == m_actionId || actionName == m_name) {
            m_clickCount++;
            std::cout << "   >> [EVENTO UI] Botão '" << m_name << "' acionado com sucesso! (Ação: " 
                      << m_actionId << ", Cliques: " << m_clickCount << ")\n";
            return true;
        }
        return false;
    }
};

} // namespace G4UI

#endif // G4_UI_BUTTON_HPP

