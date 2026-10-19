#pragma once
#include "bridge/fonte_de_entrada.hpp"

namespace jogo {

// Uma IA simples devolve uma direção a cada quadro, por exemplo,
// andar em direção ao jogador quando ele estiver perto.
// [Implementar: patrulha ou perseguição.]
class EntradaIA : public bridge::FonteDeEntrada {
public:
    bridge::Direcao lerDirecao() const override {
        return {0.0, 0.0};
    }
};
}