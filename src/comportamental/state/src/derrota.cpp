#include "derrota.h"

#include "combate.h"

namespace G4Combate {

const Derrota &Derrota::instancia() {
    static const Derrota unico{};
    return unico;
}

std::string Derrota::getNome() const { return "Derrota"; }
bool Derrota::ehFinal() const { return true; }

void Derrota::executarTurnoJogador(Combate &combate, const Combinacao &) const {
    combate.narrar("Aviso (" + getNome() + "): o jogador foi derrotado e não pode mais agir.");
}

void Derrota::executarTurnoInimigo(Combate &combate) const {
    combate.narrar("Aviso (" + getNome() + "): o combate já terminou com a derrota do jogador.");
}

} // namespace G4Combate
