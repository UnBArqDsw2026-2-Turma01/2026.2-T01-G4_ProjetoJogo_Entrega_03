// G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
// Padrão: Composite
// Autor: João Victor da Silva Batista de Farias (@beyondmagic)
// Arquivo: ui_component.hpp
//
// Papel no Padrão GoF: Component (Abstração Base)
// Define a interface comum para todos os elementos simples (folhas)
// e elementos compostos (contêineres/painéis) da interface gráfica em C++.

#ifndef G4_UI_COMPONENT_HPP
#define G4_UI_COMPONENT_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace G4UI {

struct Vector2 {
    float x;
    float y;

    Vector2(float p_x = 0.0f, float p_y = 0.0f) : x(p_x), y(p_y) {}

    std::string toString() const {
        return "(" + std::to_string(static_cast<int>(x)) + ", " + std::to_string(static_cast<int>(y)) + ")";
    }
};

class UIComponent {
protected:
    std::string m_name;
    bool m_visible;
    Vector2 m_position;
    Vector2 m_size;
    UIComponent* m_parent;

public:
    UIComponent(std::string name = "UIComponent", Vector2 pos = Vector2(), Vector2 size = Vector2())
        : m_name(std::move(name)), m_visible(true), m_position(pos), m_size(size), m_parent(nullptr) {}

    virtual ~UIComponent() = default;

    const std::string& getName() const { return m_name; }
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }
    Vector2 getPosition() const { return m_position; }
    Vector2 getSize() const { return m_size; }

    void setParent(UIComponent* parent) {
        m_parent = parent;
    }

    UIComponent* getParent() const {
        return m_parent;
    }

    // Operações polimórficas essenciais do GoF Composite
    virtual std::string render(int indentLevel = 0) const = 0;
    virtual bool handleInput(const std::string& actionName) = 0;
    virtual bool isComposite() const { return false; }

    // Gerenciamento de filhos (padrão transparente do GoF)
    virtual void add(std::shared_ptr<UIComponent> /*component*/) {
        std::cerr << "[AVISO] Operação add() não suportada no nó folha: " << m_name << "\n";
    }

    virtual void remove(std::shared_ptr<UIComponent> /*component*/) {
        std::cerr << "[AVISO] Operação remove() não suportada no nó folha: " << m_name << "\n";
    }

    virtual std::vector<std::shared_ptr<UIComponent>> getChildren() const {
        return {};
    }

    static std::string getIndent(int level) {
        return std::string(level * 4, ' ');
    }
};

} // namespace G4UI

#endif // G4_UI_COMPONENT_HPP

