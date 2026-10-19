#pragma once

#include <string>

namespace G4Combate {

class Combate;
class Combinacao;

// State (GoF): interface comum dos estados do combate. O Combate repassa cada
// pedido ao estado atual, e cada estado concreto decide o que fazer e qual é o
// próximo estado. Os estados não guardam dados (tudo fica no Combate), por isso
// as operações são const e cada estado concreto tem uma única instância.
class EstadoCombate {
public:
    virtual ~EstadoCombate() = default;
    EstadoCombate(const EstadoCombate &) = delete;
    EstadoCombate &operator=(const EstadoCombate &) = delete;

    virtual std::string getNome() const = 0;
    virtual bool ehFinal() const = 0;
    virtual void executarTurnoJogador(Combate &combate, const Combinacao &combinacao) const = 0;
    virtual void executarTurnoInimigo(Combate &combate) const = 0;

protected:
    EstadoCombate() = default;
};

} // namespace G4Combate
