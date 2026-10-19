#include "turno_jogador.h"

#include "combate.h"
#include "derrota.h"
#include "turno_inimigo.h"
#include "vitoria.h"

#include <cmath>
#include <string>

namespace G4Combate {

namespace {
// Dano que uma mistura inválida causa no próprio jogador ao explodir.
constexpr int PENALIDADE_EXPLOSAO = 8;
} // namespace

const TurnoJogador &TurnoJogador::instancia() {
    static const TurnoJogador unico{};
    return unico;
}

std::string TurnoJogador::getNome() const { return "TurnoJogador"; }
bool TurnoJogador::ehFinal() const { return false; }

void TurnoJogador::executarTurnoJogador(Combate &combate, const Combinacao &combinacao) const {
    Jogador &jogador = combate.getJogador();
    Inimigo &inimigo = combate.getInimigo();
    DiarioAventureiro &diario = jogador.getDiario();
    combate.narrar(jogador.getNome() + " usa a combinação " + combinacao.getFormula() + ".");

    // Só combinações válidas entram no diário: se ela já está lá, não precisa validar de novo (RF08).
    if (diario.conhece(combinacao)) {
        combate.narrar(combinacao.getFormula() + " já está no diário: é usada direto, sem nova validação.");
    } else if (combinacao.ehValida()) {
        diario.registrarCombinacao(combinacao);
        combate.narrar("Nova combinação registrada no Diário do Aventureiro: " + combinacao.getFormula() + ".");
    } else {
        jogador.receberDano(PENALIDADE_EXPLOSAO);
        combate.narrar("A mistura é instável e explode! " + jogador.getNome() + " sofre "
                + std::to_string(PENALIDADE_EXPLOSAO) + " de dano. Vida: " + textoDeVida(jogador) + ".");
        // A morte pela explosão encerra o combate na hora, sem passar pelo turno do inimigo.
        if (jogador.estaVivo()) {
            combate.mudarEstado(TurnoInimigo::instancia());
        } else {
            combate.mudarEstado(Derrota::instancia());
        }
        return;
    }

    const Efeito efeito = combinacao.gerarEfeito();
    const float multiplicador = inimigo.calcularMultiplicador(efeito);
    const int dano = static_cast<int>(std::lround(efeito.getDanoBase() * multiplicador));
    inimigo.receberDano(dano);
    combate.narrar("Efeito " + nomeDoTipo(efeito.getTipo()) + ": " + inimigo.getNome() + " sofre "
            + std::to_string(dano) + " de dano" + (multiplicador > 1.0f ? " (fraqueza!)" : "")
            + ". Vida: " + textoDeVida(inimigo) + ".");

    if (inimigo.estaVivo()) {
        combate.mudarEstado(TurnoInimigo::instancia());
    } else {
        combate.mudarEstado(Vitoria::instancia());
    }
}

void TurnoJogador::executarTurnoInimigo(Combate &combate) const {
    combate.narrar("Aviso (" + getNome() + "): ainda é a vez do jogador; o inimigo espera.");
}

} // namespace G4Combate
