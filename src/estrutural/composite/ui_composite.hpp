// G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
// Padrão: Composite
// Autor: João Victor da Silva Batista de Farias (@beyondmagic)
// Arquivo: ui_composite.hpp
//
// Papel no Padrão GoF: Composite (Nó Composto)
// Define o comportamento para nós que contêm filhos em C++.

#ifndef G4_UI_COMPOSITE_HPP
#define G4_UI_COMPOSITE_HPP

#include "ui_component.hpp"
#include <algorithm>
#include <sstream>

namespace G4UI {

class UIComposite : public UIComponent {
protected:
    std::vector<std::shared_ptr<UIComponent>> m_children;

public:
    UIComposite(std::string name = "UIComposite", Vector2 pos = Vector2(), Vector2 size = Vector2())
        : UIComponent(std::move(name), pos, size) {}

    bool isComposite() const override {
        return true;
    }

    void add(std::shared_ptr<UIComponent> component) override {
        if (!component) return;
        auto it = std::find(m_children.begin(), m_children.end(), component);
        if (it == m_children.end()) {
            m_children.push_back(component);
            component->setParent(this);
        }
    }

    void remove(std::shared_ptr<UIComponent> component) override {
        auto it = std::find(m_children.begin(), m_children.end(), component);
        if (it != m_children.end()) {
            (*it)->setParent(nullptr);
            m_children.erase(it);
        }
    }

    std::vector<std::shared_ptr<UIComponent>> getChildren() const override {
        return m_children;
    }

    std::shared_ptr<UIComponent> findByName(const std::string& targetName) {
        for (const auto& child : m_children) {
            if (child->getName() == targetName) {
                return child;
            }
            if (child->isComposite()) {
                auto comp = std::dynamic_pointer_cast<UIComposite>(child);
                if (comp) {
                    auto found = comp->findByName(targetName);
                    if (found) return found;
                }
            }
        }
        return nullptr;
    }

    std::string render(int indentLevel = 0) const override {
        if (!m_visible) return "";

        std::ostringstream ss;
        std::string indent = getIndent(indentLevel);
        ss << indent << "+-- [COMPOSTO: " << m_name << "] (pos: " << m_position.toString() 
           << ", size: " << m_size.toString() << ")\n";

        for (const auto& child : m_children) {
            ss << child->render(indentLevel + 1);
        }
        return ss.str();
    }

    bool handleInput(const std::string& actionName) override {
        if (!m_visible) return false;

        // Propagação reversa (ordem Z típica de interfaces gráficas)
        for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
            if ((*it)->handleInput(actionName)) {
                return true;
            }
        }
        return false;
    }
};

} // namespace G4UI

#endif // G4_UI_COMPOSITE_HPP

