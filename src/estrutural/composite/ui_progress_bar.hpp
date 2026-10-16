// G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
// Padrão: Composite
// Autor: João Victor da Silva Batista de Farias (@beyondmagic)
// Arquivo: ui_progress_bar.hpp
//
// Papel no Padrão GoF: Leaf (Folha)
// Elemento atômico de medição (HP, MP, medidor de turnos ATB).

#ifndef G4_UI_PROGRESS_BAR_HPP
#define G4_UI_PROGRESS_BAR_HPP

#include "ui_component.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace G4UI {

class UIProgressBar : public UIComponent {
private:
    std::string m_barType;
    float m_currentValue;
    float m_maxValue;

public:
    UIProgressBar(std::string name, std::string barType, float cur = 100.0f, float max = 100.0f, Vector2 pos = Vector2())
        : UIComponent(std::move(name), pos, Vector2(160.0f, 20.0f)),
          m_barType(std::move(barType)),
          m_currentValue(cur),
          m_maxValue(max) {}

    void setValues(float cur, float max) {
        m_maxValue = max;
        m_currentValue = std::clamp(cur, 0.0f, max);
    }

    float getPercentage() const {
        return (m_maxValue > 0.0f) ? (m_currentValue / m_maxValue) * 100.0f : 0.0f;
    }

    std::string render(int indentLevel = 0) const override {
        if (!m_visible) return "";
        std::ostringstream ss;
        float pct = getPercentage();
        int totalBlocks = 10;
        int filledBlocks = static_cast<int>(std::round((pct / 100.0f) * totalBlocks));

        std::string barVisual = "";
        for (int i = 0; i < totalBlocks; ++i) {
            barVisual += (i < filledBlocks) ? "█" : "░";
        }

        ss << getIndent(indentLevel) << "|-- (Barra " << m_barType << ": " << m_name 
           << ") [" << barVisual << "] " << static_cast<int>(m_currentValue) << "/" 
           << static_cast<int>(m_maxValue) << " (" << static_cast<int>(pct) << "%)\n";
        return ss.str();
    }

    bool handleInput(const std::string& /*actionName*/) override {
        return false;
    }
};

} // namespace G4UI

#endif // G4_UI_PROGRESS_BAR_HPP

