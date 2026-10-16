// G4_ProjetoJogo - SubEquipe 02 (Entrega 03: GoF Estrutural)
// Padrão: Composite
// Autor: João Victor da Silva Batista de Farias (@beyondmagic)
// Arquivo: composite_demo.cpp
//
// Demonstração executável do padrão Composite em C++ para o G4_ProjetoJogo.

#include "ui_component.hpp"
#include "ui_composite.hpp"
#include "ui_label.hpp"
#include "ui_button.hpp"
#include "ui_progress_bar.hpp"
#include "ui_panel.hpp"
#include "battle_hud.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "\n" << std::string(75, '=') << "\n";
    std::cout << " G4_ProjetoJogo — DEMONSTRAÇÃO DO PADRÃO GOF COMPOSITE (C++)\n";
    std::cout << " SubEquipe 02: João Victor da Silva Batista de Farias (@beyondmagic)\n";
    std::cout << std::string(75, '=') << "\n\n";

    // 1. Construção da Árvore de UI usando BattleHUD (Composite Raiz)
    std::cout << "[PASSO 1] Construindo a árvore de UI da Batalha (Composite)...\n";
    auto hud = std::make_shared<G4UI::BattleHUD>("Alquimista Edward", "Gólem de Enxofre");
    std::cout << ">> Árvore instanciada com sucesso!\n\n";

    // 2. Renderização Polimórfica Uniforme da Árvore Completa
    std::cout << "[PASSO 2] Renderizando a HUD completa chamando hud->render():\n";
    std::cout << std::string(75, '-') << "\n";
    std::cout << hud->render();
    std::cout << std::string(75, '-') << "\n\n";

    // 3. Propagação Recursiva de Eventos de Entrada (handleInput)
    std::cout << "[PASSO 3] Enviando comandos de entrada para o Composite Raiz:\n";
    std::cout << ">> Testando comando 'cmd_mix' (Misturar Elementos):\n";
    bool handledMix = hud->handleInput("cmd_mix");
    std::cout << "   Resultado: " << (handledMix ? "Consumido pela árvore" : "Não tratado") << "\n\n";

    std::cout << ">> Testando comando 'cmd_attack' (Atacar):\n";
    bool handledAttack = hud->handleInput("cmd_attack");
    std::cout << "   Resultado: " << (handledAttack ? "Consumido pela árvore" : "Não tratado") << "\n\n";

    std::cout << ">> Testando comando desconhecido 'cmd_dançar':\n";
    bool handledUnknown = hud->handleInput("cmd_dançar");
    std::cout << "   Resultado: " << (handledUnknown ? "Consumido pela árvore" : "Não tratado (esperado)") << "\n\n";

    // 4. Modificação Dinâmica da Estrutura: Adicionando Subárvore de Alquimia
    std::cout << "[PASSO 4] Adicionando dinamicamente um novo Composite (Bancada Rápida de Campo):\n";
    auto bench = std::make_shared<G4UI::UIPanel>("AlchemyQuickBench", "Bancada Rápida de Campo", G4UI::Vector2(700.0f, 400.0f), G4UI::Vector2(400.0f, 200.0f));
    bench->add(std::make_shared<G4UI::UILabel>("LblSlot1", "Slot A: [H] Hidrogênio x2"));
    bench->add(std::make_shared<G4UI::UILabel>("LblSlot2", "Slot B: [O] Oxigênio x1"));
    bench->add(std::make_shared<G4UI::UIButton>("BtnConfirmMix", "Sintetizar [H2O - Poção de Água Pura]", "cmd_synthesize"));
    hud->add(bench);
    std::cout << ">> Subárvore 'AlchemyQuickBench' acoplada como filho do Composite raiz!\n\n";

    // 5. Ocultando o Menu de Comandos (ação de fim de turno) e Renderizando Novamente
    std::cout << "[PASSO 5] Ocultando o Menu de Comandos (ação de fim de turno):\n";
    hud->setTurnActive(false);
    std::cout << ">> hud->setTurnActive(false) executado.\n";
    std::cout << ">> Renderizando novamente para evidenciar que o Composite 'ActionMenuPanel' e todos os seus filhos foram omitidos recursivamente:\n";
    std::cout << std::string(75, '-') << "\n";
    std::cout << hud->render();
    std::cout << std::string(75, '-') << "\n\n";

    // 6. Testando acionamento da nova janela de alquimia
    std::cout << "[PASSO 6] Disparando comando na janela recém-adicionada ('cmd_synthesize'):\n";
    bool handledSynth = hud->handleInput("cmd_synthesize");
    std::cout << "   Resultado: " << (handledSynth ? "Consumido com sucesso pela nova subárvore" : "Não tratado") << "\n\n";

    std::cout << std::string(75, '=') << "\n";
    std::cout << " DEMONSTRAÇÃO CONCLUÍDA COM SUCESSO! O padrão Composite opera conforme o GoF.\n";
    std::cout << std::string(75, '=') << "\n\n";

    return 0;
}

