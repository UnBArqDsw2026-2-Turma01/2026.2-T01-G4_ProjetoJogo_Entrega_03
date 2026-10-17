// G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
// Padrão: Composite
// Autor: João Victor da Silva Batista de Farias (@beyondmagic)
// Arquivo: mochila_composite.hpp
//
// Papel no Padrão GoF: Composite Root (Interface da Mochila e Alquimia)
// Monta a tela completa da Mochila como uma árvore uniforme de UIComponent,
// delegando as operações de regras de negócio para a AlquimiaFacade.

#ifndef G4_MOCHILA_COMPOSITE_HPP
#define G4_MOCHILA_COMPOSITE_HPP

#include "ui_composite.hpp"
#include "ui_panel.hpp"
#include "ui_button.hpp"
#include "ui_label.hpp"
#include "bancada_panel.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include <string>

namespace G4UI {

// Interface abstrata da Fachada para desacoplamento completo da camada de UI
class IAlquimiaFacade {
public:
    virtual ~IAlquimiaFacade() = default;
    virtual std::string combinar(const std::string& itemA, const std::string& itemB) = 0;
    virtual std::vector<std::string> listarInventario() const = 0;
    virtual std::vector<std::string> listarReceitasDescobertas() const = 0;
};

// Implementação autônoma / simulada para execução de testes em C++ sem motor gráfico
class MockAlquimiaFacade : public IAlquimiaFacade {
private:
    std::vector<std::string> m_inventario;
    std::vector<std::string> m_receitasDescobertas;

public:
    MockAlquimiaFacade() {
        m_inventario = {"salvia_de_fogo", "solucao_condutora", "veneno_roxo", "vidro_fundido"};
    }

    std::string combinar(const std::string& a, const std::string& b) override {
        if (a.empty() || b.empty()) {
            return "ERRO: Coloque dois itens na bancada.";
        }
        std::string chave = a < b ? a + "+" + b : b + "+" + a;
        if (chave == "salvia_de_fogo+solucao_condutora") {
            m_receitasDescobertas.push_back("pocao_flamejante");
            return "SUCESSO: Poção Flamejante criada com sucesso!";
        } else if (chave == "salvia_de_fogo+veneno_roxo") {
            m_receitasDescobertas.push_back("pocao_sombria");
            return "SUCESSO: Poção Sombria criada com sucesso!";
        }
        return "SUCESSO: Poção Básica criada (" + chave + ")";
    }

    std::vector<std::string> listarInventario() const override {
        return m_inventario;
    }

    std::vector<std::string> listarReceitasDescobertas() const override {
        return m_receitasDescobertas;
    }
};

class MochilaComposite : public UIComposite {
private:
    std::shared_ptr<IAlquimiaFacade> m_facade;
    std::shared_ptr<UIPanel> m_inventarioPanel;
    std::shared_ptr<BancadaPanel> m_bancadaPanel;
    std::shared_ptr<UIPanel> m_livroPanel;
    std::shared_ptr<UILabel> m_lblResultado;
    std::shared_ptr<UILabel> m_lblReceitas;

public:
    MochilaComposite(std::shared_ptr<IAlquimiaFacade> facade = nullptr)
        : UIComposite("MochilaComposite_Root", Vector2(0.0f, 0.0f), Vector2(1920.0f, 1080.0f)),
          m_facade(facade) {
        if (!m_facade) {
            m_facade = std::make_shared<MockAlquimiaFacade>();
        }
        buildMochila();
    }

    void buildMochila() {
        // 1. Painel da Mochila (Inventário)
        m_inventarioPanel = std::make_shared<UIPanel>("InventarioPanel", "Mochila (Itens Coletados)", Vector2(40.0f, 40.0f), Vector2(500.0f, 750.0f));
        m_inventarioPanel->add(std::make_shared<UILabel>("LblInstrucao", "Clique em um item para enviar à bancada:"));

        auto itens = m_facade->listarInventario();
        for (size_t i = 0; i < itens.size(); ++i) {
            m_inventarioPanel->add(std::make_shared<UIButton>("SlotItem_" + std::to_string(i), "[" + itens[i] + "]", "cmd_slot_" + itens[i]));
        }
        m_inventarioPanel->add(std::make_shared<UIButton>("BtnFecharMochila", "[Fechar Mochila (Esc/I)]", "cmd_close"));
        add(m_inventarioPanel);

        // 2. Painel da Bancada de Alquimia (Reutilizável)
        m_bancadaPanel = std::make_shared<BancadaPanel>(Vector2(580.0f, 40.0f), Vector2(600.0f, 320.0f));
        m_bancadaPanel->setOnMixCallback([this](const std::string& a, const std::string& b) {
            executarMistura(a, b);
        });
        add(m_bancadaPanel);

        // 3. Painel do Livro do Aventureiro e Resultado
        m_livroPanel = std::make_shared<UIPanel>("LivroPanel", "Livro do Aventureiro (Receitas)", Vector2(580.0f, 380.0f), Vector2(600.0f, 410.0f));
        m_lblResultado = std::make_shared<UILabel>("LblResultado", "Status: Bancada pronta para misturas.");
        m_lblReceitas = std::make_shared<UILabel>("LblReceitas", "Receitas Descobertas: 0 / 36");
        m_livroPanel->add(m_lblResultado);
        m_livroPanel->add(m_lblReceitas);
        add(m_livroPanel);
    }

    void executarMistura(const std::string& a, const std::string& b) {
        if (!m_facade) return;
        std::string resultado = m_facade->combinar(a, b);
        m_lblResultado->setText("Status: " + resultado);
        auto descobertas = m_facade->listarReceitasDescobertas();
        m_lblReceitas->setText("Receitas Descobertas: " + std::to_string(descobertas.size()) + " / 36");
        std::cout << "   >> [MochilaComposite -> Facade] Mistura executada! " << resultado << "\n";
        m_bancadaPanel->limpar();
    }

    std::shared_ptr<BancadaPanel> getBancada() const {
        return m_bancadaPanel;
    }

    std::shared_ptr<IAlquimiaFacade> getFacade() const {
        return m_facade;
    }

    bool handleInput(const std::string& inputCommand) override {
        if (!m_visible) return false;

        // Trata seleção de slots: prefixo "cmd_slot_"
        if (inputCommand.rfind("cmd_slot_", 0) == 0) {
            std::string itemId = inputCommand.substr(9);
            m_bancadaPanel->colocarNaBancada(itemId);
            std::cout << "   >> [MochilaComposite] Item '" << itemId << "' colocado na bancada via Composite!\n";
            return true;
        }

        if (inputCommand == "cmd_close") {
            setVisible(false);
            std::cout << "   >> [MochilaComposite] Mochila fechada via comando Composite!\n";
            return true;
        }

        return UIComposite::handleInput(inputCommand);
    }
};

} // namespace G4UI

#endif // G4_MOCHILA_COMPOSITE_HPP
