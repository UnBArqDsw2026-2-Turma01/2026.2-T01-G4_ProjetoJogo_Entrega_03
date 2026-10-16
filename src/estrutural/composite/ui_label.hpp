// G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
// Padrão: Composite
// Autor: João Victor da Silva Batista de Farias (@beyondmagic)
// Arquivo: ui_label.hpp
//
// Papel no Padrão GoF: Leaf (Folha)
// Elemento atômico de texto na interface gráfica.

#ifndef G4_UI_LABEL_HPP
#define G4_UI_LABEL_HPP

#include "ui_component.hpp"
#include <sstream>

namespace G4UI {

class UILabel : public UIComponent {
private:
    std::string m_text;

public:
    UILabel(std::string name, std::string text, Vector2 pos = Vector2())
        : UIComponent(std::move(name), pos, Vector2(static_cast<float>(text.length() * 8), 16.0f)),
          m_text(std::move(text)) {}

    void setText(const std::string& newText) {
        m_text = newText;
        m_size = Vector2(static_cast<float>(m_text.length() * 8), 16.0f);
    }

    const std::string& getText() const { return m_text; }

    std::string render(int indentLevel = 0) const override {
        if (!m_visible) return "";
        std::ostringstream ss;
        ss << getIndent(indentLevel) << "|-- (Rótulo: " << m_name << ") \"" << m_text << "\"\n";
        return ss.str();
    }

    bool handleInput(const std::string& /*actionName*/) override {
        return false; // Rótulos não consomem cliques/ações
    }
};

} // namespace G4UI

#endif // G4_UI_LABEL_HPP

