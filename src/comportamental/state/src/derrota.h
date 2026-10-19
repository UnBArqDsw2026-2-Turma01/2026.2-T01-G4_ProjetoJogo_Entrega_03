#pragma once

#include "estado_combate.h"

namespace G4Combate {

// ConcreteState final: o jogador foi derrotado. Todo pedido vira só um aviso.
class Derrota : public EstadoCombate {
public:
    static const Derrota &instancia();

    std::string getNome() const override;
    bool ehFinal() const override;
    void executarTurnoJogador(Combate &combate, const Combinacao &combinacao) const override;
    void executarTurnoInimigo(Combate &combate) const override;

private:
    Derrota() = default;
};

} // namespace G4Combate
