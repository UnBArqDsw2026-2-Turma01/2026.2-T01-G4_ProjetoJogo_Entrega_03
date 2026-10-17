// G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
// Padrão: Composite
// Autor: João Victor da Silva Batista de Farias (@beyondmagic)
// Arquivo: ui_panel.hpp
//
// Papel no Padrão GoF: Concrete Composite
// Representa um contêiner ou janela estilizada da interface gráfica.

#ifndef G4_UI_PANEL_HPP
#define G4_UI_PANEL_HPP

#include "ui_composite.hpp"

namespace G4UI {

class UIPanel : public UIComposite {
private:
    std::string m_title;

public:
    UIPanel(std::string name, std::string title, Vector2 pos = Vector2(), Vector2 size = Vector2(300.0f, 200.0f))
        : UIComposite(std::move(name), pos, size), m_title(std::move(title)) {}

    const std::string& getTitle() const { return m_title; }

    std::string render(int indentLevel = 0) const override {
        if (!m_visible) return "";

        std::ostringstream ss;
        std::string indent = getIndent(indentLevel);

        ss << indent << "+========================================================\n";
        ss << indent << "| [PAINEL: " << m_title << "] (id: " << m_name << ", pos: " << m_position.toString() << ")\n";
        ss << indent << "+--------------------------------------------------------\n";

        for (const auto& child : m_children) {
            ss << child->render(indentLevel + 1);
        }

        ss << indent << "+========================================================\n";
        return ss.str();
    }
};

} // namespace G4UI

#endif // G4_UI_PANEL_HPP

