#include "combate.h"

#include "turno_jogador.h"

namespace G4Combate {

Combate::Combate(Jogador &p_jogador, Inimigo &p_inimigo, std::ostream &p_saida) :
        jogador(p_jogador), inimigo(p_inimigo), saida(p_saida), estado(&TurnoJogador::instancia()) {}

void Combate::iniciar() {
    turnoAtual = 1;
    estado = &TurnoJogador::instancia();
    narrar("Combate iniciado: " + jogador.getNome() + " x " + inimigo.getNome() + ".");
    narrar("[estado] inicial: " + estado->getNome());
}

// Delegação: nenhum if sobre de quem é a vez.
void Combate::executarTurnoJogador(const Combinacao &combinacao) {
    estado->executarTurnoJogador(*this, combinacao);
}

void Combate::executarTurnoInimigo() {
    estado->executarTurnoInimigo(*this);
}

bool Combate::verificarFim() const {
    return estado->ehFinal();
}

void Combate::mudarEstado(const EstadoCombate &novo) {
    narrar("[estado] " + estado->getNome() + " -> " + novo.getNome());
    estado = &novo;
}

void Combate::avancarTurno() { ++turnoAtual; }
int Combate::getTurnoAtual() const { return turnoAtual; }
std::string Combate::getNomeEstado() const { return estado->getNome(); }
Jogador &Combate::getJogador() { return jogador; }
Inimigo &Combate::getInimigo() { return inimigo; }

void Combate::narrar(const std::string &mensagem) {
    saida << mensagem << '\n';
}

} // namespace G4Combate
