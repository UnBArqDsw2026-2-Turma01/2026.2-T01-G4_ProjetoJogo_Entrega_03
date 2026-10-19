#include "turno_inimigo.h"

#include "combate.h"
#include "derrota.h"
#include "turno_jogador.h"

#include <string>

namespace G4Combate {

const TurnoInimigo &TurnoInimigo::instancia() {
    static const TurnoInimigo unico{};
    return unico;
}

std::string TurnoInimigo::getNome() const { return "TurnoInimigo"; }
bool TurnoInimigo::ehFinal() const { return false; }

void TurnoInimigo::executarTurnoJogador(Combate &combate, const Combinacao &) const {
    combate.narrar("Aviso (" + getNome() + "): é a vez do inimigo; aguarde.");
}

void TurnoInimigo::executarTurnoInimigo(Combate &combate) const {
    Jogador &jogador = combate.getJogador();
    Inimigo &inimigo = combate.getInimigo();
    const int vidaAntes = jogador.getVidaAtual();
    inimigo.atacar(jogador);
    combate.narrar(inimigo.getNome() + " ataca! " + jogador.getNome() + " sofre "
            + std::to_string(vidaAntes - jogador.getVidaAtual()) + " de dano. Vida: " + textoDeVida(jogador) + ".");

    if (jogador.estaVivo()) {
        combate.avancarTurno();
        combate.mudarEstado(TurnoJogador::instancia());
    } else {
        combate.mudarEstado(Derrota::instancia());
    }
}

} // namespace G4Combate
