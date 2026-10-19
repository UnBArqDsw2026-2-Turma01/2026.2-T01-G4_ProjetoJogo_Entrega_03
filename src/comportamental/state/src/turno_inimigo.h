#pragma once

#include "estado_combate.h"

namespace G4Combate {

// ConcreteState: vez do inimigo. Recusa ações do jogador até o inimigo agir.
class TurnoInimigo : public EstadoCombate {
public:
    static const TurnoInimigo &instancia();

    std::string getNome() const override;
    bool ehFinal() const override;
    void executarTurnoJogador(Combate &combate, const Combinacao &combinacao) const override;
    void executarTurnoInimigo(Combate &combate) const override;

private:
    TurnoInimigo() = default;
};

} // namespace G4Combate
