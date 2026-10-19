#pragma once

#include "dominio.h"
#include "estado_combate.h"

#include <iostream>
#include <string>

namespace G4Combate {

// Context (GoF): guarda o estado atual e repassa a ele os pedidos do turno.
// O combate não testa de quem é a vez nem se a luta acabou: quem sabe o que
// fazer com cada pedido é o estado. Na Entrega 02, essa decisão ficava no
// atributo vezDoJogador, que o padrão eliminou.
class Combate {
public:
    Combate(Jogador &jogador, Inimigo &inimigo, std::ostream &saida = std::cout);

    // Pedidos do cliente: cada um é repassado ao estado atual.
    void iniciar();
    void executarTurnoJogador(const Combinacao &combinacao);
    void executarTurnoInimigo();
    bool verificarFim() const;

    // Usadas pelos estados (e, as consultas, também pelo menu).
    void mudarEstado(const EstadoCombate &novo);
    void avancarTurno();
    int getTurnoAtual() const;
    std::string getNomeEstado() const;
    Jogador &getJogador();
    Inimigo &getInimigo();
    void narrar(const std::string &mensagem);

private:
    Jogador &jogador;
    Inimigo &inimigo;
    std::ostream &saida;
    const EstadoCombate *estado;
    int turnoAtual = 1;
};

} // namespace G4Combate
