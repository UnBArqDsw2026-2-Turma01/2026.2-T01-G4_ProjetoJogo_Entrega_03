#pragma once

#include "combate.h"
#include "dominio.h"

#include <iostream>
#include <optional>

namespace G4Combate {

// Client: lê as escolhas do jogador no terminal e faz os pedidos ao Combate.
// O menu nunca pergunta de quem é a vez: a cada rodada ele pede o turno do
// jogador e o do inimigo, e o estado atual decide o que cada pedido faz.
class MenuTerminal {
public:
    MenuTerminal(std::istream &entrada, std::ostream &saida);

    void jogar(Combate &combate);

private:
    void mostrarSituacao(Combate &combate);
    std::optional<Combinacao> escolherCombinacao(const Jogador &jogador);
    std::optional<Combinacao> misturar(const Jogador &jogador);
    std::optional<Combinacao> escolherDoDiario(const Jogador &jogador);
    // Devolve -1 quando a entrada acaba.
    int lerOpcao(int minimo, int maximo);

    std::istream &entrada;
    std::ostream &saida;
};

} // namespace G4Combate
