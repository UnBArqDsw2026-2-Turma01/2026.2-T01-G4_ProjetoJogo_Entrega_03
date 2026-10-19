#include "../src/combate.h"
#include "../src/dominio.h"
#include "../src/menu_terminal.h"
#include "../src/turno_jogador.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace G4Combate;

namespace {

int verificacoes = 0;
int falhas = 0;

void verificar(bool condicao, const std::string &descricao) {
    ++verificacoes;
    if (condicao) {
        std::cout << "[OK]    " << descricao << "\n";
    } else {
        ++falhas;
        std::cout << "[FALHA] " << descricao << "\n";
    }
}

const Elemento H("H", "Hidrogênio", 1);
const Elemento C("C", "Carbono", 6);
const Elemento O("O", "Oxigênio", 8);
const Elemento S("S", "Enxofre", 16);

Jogador novoJogador(int vida = 40) {
    Jogador jogador("Aventureiro", vida, 5, 2);
    for (const Elemento &elemento : { H, C, O, S }) {
        jogador.getInventario().adicionar(elemento);
    }
    return jogador;
}

Inimigo novoGolem() {
    return Inimigo("Gólem de Enxofre", 50, 9, 3, TipoEfeito::GELO);
}

bool contem(const std::ostringstream &saida, const std::string &trecho) {
    return saida.str().find(trecho) != std::string::npos;
}

void testarFormulas() {
    verificar(Combinacao({ H, H, O }).getFormula() == "H2O", "H + H + O forma H2O");
    verificar(Combinacao({ O, H, H }).getFormula() == "H2O", "A ordem da mistura não muda a fórmula");
    verificar(Combinacao({ C, H, H, H, H }).getFormula() == "CH4", "Com carbono, C e H vêm primeiro (CH4)");
    verificar(Combinacao({ O, C, O }).getFormula() == "CO2", "O + C + O forma CO2");
    verificar(Combinacao({ S, H, H }).getFormula() == "H2S", "S + H + H forma H2S");
    verificar(Combinacao({ H, H, O }).ehValida(), "H2O é uma combinação válida");
    verificar(!Combinacao({ C, S }).ehValida(), "CS não é uma combinação válida");
    const Efeito gelo = Combinacao({ H, H, O }).gerarEfeito();
    verificar(gelo.getTipo() == TipoEfeito::GELO && gelo.getDanoBase() == 12, "H2O gera GELO com dano base 12");
}

void testarDominio() {
    const Jogador jogador = novoJogador();
    bool recusou = false;
    try {
        jogador.misturarElementos({ H, Elemento("N", "Nitrogênio", 7) });
    } catch (const std::invalid_argument &) {
        recusou = true;
    }
    verificar(recusou, "Não mistura elemento que não está no inventário (RF09)");

    recusou = false;
    try {
        jogador.misturarElementos({ H });
    } catch (const std::invalid_argument &) {
        recusou = true;
    }
    verificar(recusou, "Uma combinação precisa de pelo menos dois elementos");

    const Inimigo golem = novoGolem();
    verificar(golem.calcularMultiplicador(Efeito(TipoEfeito::GELO, 10)) == 2.0f, "A fraqueza do inimigo dobra o dano (RF06)");
    verificar(golem.calcularMultiplicador(Efeito(TipoEfeito::FOGO, 10)) == 1.0f, "Um efeito comum não altera o dano");
}

void testarEstadosCompartilhados() {
    verificar(&TurnoJogador::instancia() == &TurnoJogador::instancia(), "Cada estado concreto tem uma única instância");
}

void testarCombateAteAVitoria() {
    Jogador jogador = novoJogador();
    Inimigo golem = novoGolem();
    std::ostringstream saida;
    Combate combate(jogador, golem, saida);
    combate.iniciar();
    verificar(combate.getNomeEstado() == "TurnoJogador" && !combate.verificarFim(), "O combate começa no turno do jogador");

    combate.executarTurnoInimigo();
    verificar(jogador.getVidaAtual() == 40 && combate.getNomeEstado() == "TurnoJogador",
            "TurnoJogador recusa o pedido de turno do inimigo");

    combate.executarTurnoJogador(jogador.misturarElementos({ H, H, O }));
    verificar(golem.getVidaAtual() == 26, "H2O causa 24 de dano no Gólem, que é fraco contra GELO");
    verificar(jogador.getDiario().conhece(Combinacao({ H, H, O })), "A combinação nova vai para o diário (RF08)");
    verificar(combate.getNomeEstado() == "TurnoInimigo", "Depois da ação do jogador, o estado passa a TurnoInimigo");
    verificar(contem(saida, "[estado] TurnoJogador -> TurnoInimigo"), "A troca de estado aparece na saída");

    combate.executarTurnoJogador(jogador.misturarElementos({ H, H, O }));
    verificar(golem.getVidaAtual() == 26 && combate.getNomeEstado() == "TurnoInimigo",
            "TurnoInimigo recusa uma nova ação do jogador");

    combate.executarTurnoInimigo();
    verificar(jogador.getVidaAtual() == 33, "O Gólem causa 7 de dano (ataque 9 menos defesa 2)");
    verificar(combate.getTurnoAtual() == 2 && combate.getNomeEstado() == "TurnoJogador",
            "O turno avança e a vez volta ao jogador");

    combate.executarTurnoJogador(jogador.misturarElementos({ C, S }));
    verificar(jogador.getVidaAtual() == 25, "A mistura inválida explode e causa 8 de dano no jogador");
    verificar(jogador.getDiario().listarCombinacoes().size() == 1, "A mistura inválida não entra no diário");
    verificar(golem.getVidaAtual() == 26 && combate.getNomeEstado() == "TurnoInimigo",
            "Depois da explosão, a vez passa ao inimigo");

    combate.executarTurnoInimigo();
    combate.executarTurnoJogador(jogador.getDiario().listarCombinacoes().front());
    verificar(golem.getVidaAtual() == 2, "A combinação do diário é usada direto (RF08)");
    verificar(jogador.getDiario().listarCombinacoes().size() == 1, "Usar uma combinação conhecida não a duplica no diário");

    combate.executarTurnoInimigo();
    combate.executarTurnoJogador(jogador.misturarElementos({ C, H, H, H, H }));
    verificar(!golem.estaVivo() && combate.getNomeEstado() == "Vitoria", "Derrotar o inimigo leva ao estado Vitoria");
    verificar(combate.verificarFim(), "Vitoria é um estado final");

    const int vidaDoJogador = jogador.getVidaAtual();
    combate.executarTurnoInimigo();
    combate.executarTurnoJogador(jogador.misturarElementos({ H, H, O }));
    verificar(jogador.getVidaAtual() == vidaDoJogador && combate.getNomeEstado() == "Vitoria",
            "Vitoria recusa novos pedidos");
}

void testarDerrotaPeloInimigo() {
    Jogador jogador = novoJogador(5);
    Inimigo golem = novoGolem();
    std::ostringstream saida;
    Combate combate(jogador, golem, saida);
    combate.iniciar();
    combate.executarTurnoJogador(jogador.misturarElementos({ H, H, O }));
    combate.executarTurnoInimigo();
    verificar(!jogador.estaVivo() && combate.getNomeEstado() == "Derrota", "Morrer no ataque do inimigo leva ao estado Derrota");
    verificar(combate.verificarFim(), "Derrota é um estado final");

    combate.executarTurnoJogador(jogador.misturarElementos({ H, H, O }));
    verificar(golem.getVidaAtual() == 26, "Derrota recusa novos pedidos do jogador");
}

void testarDerrotaPelaExplosao() {
    Jogador jogador = novoJogador(6);
    Inimigo golem = novoGolem();
    std::ostringstream saida;
    Combate combate(jogador, golem, saida);
    combate.iniciar();
    combate.executarTurnoJogador(jogador.misturarElementos({ O, S }));
    verificar(combate.getNomeEstado() == "Derrota", "Morrer na explosão encerra o combate na hora, sem turno do inimigo");
}

void testarMenu() {
    Jogador jogador = novoJogador();
    Inimigo golem = novoGolem();
    std::istringstream entrada(
            "abc\n9\n1\n1 1 3\n" // entradas inválidas, depois a mistura H2O
            "2\n1\n"             // H2O pelo diário
            "1\n2 1 1 1 1\n");   // mistura CH4
    std::ostringstream saida;
    Combate combate(jogador, golem, saida);
    MenuTerminal menu(entrada, saida);
    menu.jogar(combate);
    verificar(combate.getNomeEstado() == "Vitoria", "O menu joga uma partida completa até a vitória");
    verificar(contem(saida, "Opção inválida"), "O menu pede de novo quando a opção é inválida");
}

void testarMenuSemEntrada() {
    Jogador jogador = novoJogador();
    Inimigo golem = novoGolem();
    std::istringstream entrada("1\n");
    std::ostringstream saida;
    Combate combate(jogador, golem, saida);
    MenuTerminal menu(entrada, saida);
    menu.jogar(combate);
    verificar(contem(saida, "Entrada encerrada"), "O menu termina quando a entrada acaba");
}

} // namespace

int main() {
    testarFormulas();
    testarDominio();
    testarEstadosCompartilhados();
    testarCombateAteAVitoria();
    testarDerrotaPeloInimigo();
    testarDerrotaPelaExplosao();
    testarMenu();
    testarMenuSemEntrada();

    std::cout << "\nRESULTADO: " << verificacoes << " verificações, " << falhas << " falha(s)\n";
    return falhas == 0 ? 0 : 1;
}
