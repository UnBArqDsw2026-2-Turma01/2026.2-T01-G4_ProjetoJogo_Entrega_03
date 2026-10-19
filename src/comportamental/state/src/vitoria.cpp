#include "vitoria.h"

#include "combate.h"

namespace G4Combate {

const Vitoria &Vitoria::instancia() {
    static const Vitoria unico{};
    return unico;
}

std::string Vitoria::getNome() const { return "Vitoria"; }
bool Vitoria::ehFinal() const { return true; }

void Vitoria::executarTurnoJogador(Combate &combate, const Combinacao &) const {
    combate.narrar("Aviso (" + getNome() + "): o combate já terminou com a vitória do jogador.");
}

void Vitoria::executarTurnoInimigo(Combate &combate) const {
    combate.narrar("Aviso (" + getNome() + "): o inimigo foi derrotado e não age mais.");
}

} // namespace G4Combate
