#pragma once
#include "controlador_movimento.hpp"

namespace bridge {

// Regra própria do jogador: a velocidade depende do atributo VELOCIDADE,
// que a cadeia de decoradores altera (ex.: Pena Leve, +2).
// BASE e GANHO são valores iniciais: calibre-os com o jogo.
class ControladorJogador : public ControladorMovimento {
public:
    using ControladorMovimento::ControladorMovimento;

    static constexpr double BASE = 120.0;  // px/s sem bônus
    static constexpr double GANHO = 4.0;   // px/s por ponto de atributo

    void aplicarAtributoVelocidade(int valorAtributo) {
        velocidade = BASE + valorAtributo * GANHO;
    }
};

}