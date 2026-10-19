#pragma once

#include "estado_combate.h"

namespace G4Combate {

// ConcreteState final: o inimigo foi derrotado. Todo pedido vira só um aviso.
class Vitoria : public EstadoCombate {
public:
    static const Vitoria &instancia();

    std::string getNome() const override;
    bool ehFinal() const override;
    void executarTurnoJogador(Combate &combate, const Combinacao &combinacao) const override;
    void executarTurnoInimigo(Combate &combate) const override;

private:
    Vitoria() = default;
};

} // namespace G4Combate
