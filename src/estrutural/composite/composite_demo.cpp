// G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
// Padrão: Composite
// Autor: João Victor da Silva Batista de Farias (@beyondmagic)
// Arquivo: composite_demo.cpp
//
// Demonstração executável do padrão Composite em C++ para o G4_ProjetoJogo,
// integrando o HUD de Batalha (BattleHUD) e a Mochila/Bancada de Alquimia (MochilaComposite)
// com despacho polimórfico de comandos para a AlquimiaFacade.

#include "ui_component.hpp"
#include "battle_hud.hpp"
#include "bancada_panel.hpp"
#include "mochila_composite.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "\n" << std::string(75, '=') << "\n";
    std::cout << " G4_ProjetoJogo — DEMONSTRAÇÃO DO PADRÃO GOF COMPOSITE (C++)\n";
    std::cout << " SubEquipe 02: João Victor da Silva Batista de Farias (@beyondmagic)\n";
    std::cout << " Integração Arquitetural: Composite (Apresentação UI) + Facade (Alquimia)\n";
    std::cout << std::string(75, '=') << "\n\n";

    // =========================================================================
    // PARTE 1: Construção e Operação do BattleHUD (Composite de Batalha)
    // =========================================================================
    std::cout << "--- [PARTE 1] Interface de Batalha (BattleHUD no padrão Composite) ---\n";
    std::cout << "[PASSO 1] Construindo a árvore de UI da Batalha (Composite)...\n";
    auto hud = std::make_shared<G4UI::BattleHUD>("Alquimista Edward", "Gólem de Enxofre");
    std::cout << ">> Árvore BattleHUD instanciada com sucesso!\n\n";

    std::cout << "[PASSO 2] Renderizando a HUD chamando hud->render():\n";
    std::cout << std::string(75, '-') << "\n";
    std::cout << hud->render();
    std::cout << std::string(75, '-') << "\n\n";

    std::cout << "[PASSO 3] Enviando comandos de combate para o Composite Raiz:\n";
    std::cout << ">> Testando comando 'cmd_attack':\n";
    hud->handleInput("cmd_attack");
    std::cout << ">> Testando comando 'cmd_mix' no menu de batalha:\n";
    hud->handleInput("cmd_mix");
    std::cout << "\n";

    // =========================================================================
    // PARTE 2: Construção da Mochila e Alquimia (MochilaComposite)
    // =========================================================================
    std::cout << "--- [PARTE 2] Refatoração da Mochila (MochilaComposite + Facade) ---\n";
    std::cout << "[PASSO 4] Construindo a Mochila como árvore Composite acoplada à AlquimiaFacade...\n";
    auto facade = std::make_shared<G4UI::MockAlquimiaFacade>();
    auto mochila = std::make_shared<G4UI::MochilaComposite>(facade);
    std::cout << ">> MochilaComposite instanciada com sucesso!\n\n";

    std::cout << "[PASSO 5] Renderizando a Mochila completa chamando mochila->render():\n";
    std::cout << std::string(75, '-') << "\n";
    std::cout << mochila->render();
    std::cout << std::string(75, '-') << "\n\n";

    std::cout << "[PASSO 6] Interagindo com os botões da Mochila via despacho Composite:\n";
    std::cout << ">> Selecionando 'salvia_de_fogo':\n";
    mochila->handleInput("cmd_slot_salvia_de_fogo");

    std::cout << ">> Selecionando 'solucao_condutora':\n";
    mochila->handleInput("cmd_slot_solucao_condutora");

    std::cout << ">> Disparando comando de mistura ('cmd_mix') na árvore Composite:\n";
    mochila->handleInput("cmd_mix");
    std::cout << "\n";

    std::cout << "[PASSO 7] Renderizando a Mochila após a reação de alquimia:\n";
    std::cout << std::string(75, '-') << "\n";
    std::cout << mochila->render();
    std::cout << std::string(75, '-') << "\n\n";

    // =========================================================================
    // PARTE 3: Reúso Transversal do Componente (BancadaPanel no BattleHUD)
    // =========================================================================
    std::cout << "--- [PARTE 3] Reúso de Componente: BancadaPanel acoplada no BattleHUD ---\n";
    std::cout << "[PASSO 8] Demonstrando a modularidade: o mesmo BancadaPanel da Alquimia é plugado no BattleHUD:\n";
    auto bancadaCombate = std::make_shared<G4UI::BancadaPanel>(G4UI::Vector2(700.0f, 400.0f), G4UI::Vector2(500.0f, 250.0f));
    bancadaCombate->setOnMixCallback([facade](const std::string& a, const std::string& b) {
        std::cout << "   >> [Alquimia em Batalha!] " << facade->combinar(a, b) << "\n";
    });
    bancadaCombate->colocarNaBancada("salvia_de_fogo");
    bancadaCombate->colocarNaBancada("veneno_roxo");

    hud->add(bancadaCombate);
    std::cout << ">> BancadaPanel acoplada dinamicamente à raiz do BattleHUD!\n";

    std::cout << ">> Acionando síntese em combate via hud->handleInput('cmd_mix'):\n";
    hud->handleInput("cmd_mix");
    std::cout << "\n";

    std::cout << std::string(75, '=') << "\n";
    std::cout << " DEMONSTRAÇÃO CONCLUÍDA COM SUCESSO!\n";
    std::cout << " A arquitetura Composite provou a capacidade de unificar telas e orquestrar a Facade.\n";
    std::cout << std::string(75, '=') << "\n\n";

    return 0;
}
