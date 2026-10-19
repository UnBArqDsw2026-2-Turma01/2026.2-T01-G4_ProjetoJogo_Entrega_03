#pragma once

#include "estado_combate.h"

namespace G4Combate {

// ConcreteState: vez do jogador. É o único estado em que a mistura acontece.
class TurnoJogador : public EstadoCombate {
public:
    static const TurnoJogador &instancia();

    std::string getNome() const override;
    bool ehFinal() const override;
    void executarTurnoJogador(Combate &combate, const Combinacao &combinacao) const override;
    void executarTurnoInimigo(Combate &combate) const override;

private:
    TurnoJogador() = default;
};

} // namespace G4Combate
