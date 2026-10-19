#include "combate.h"
#include "dominio.h"
#include "menu_terminal.h"

#include <iostream>
#include <string>

using namespace G4Combate;

namespace {

const Elemento HIDROGENIO("H", "Hidrogênio", 1);
const Elemento CARBONO("C", "Carbono", 6);
const Elemento OXIGENIO("O", "Oxigênio", 8);
const Elemento ENXOFRE("S", "Enxofre", 16);

Jogador criarJogador(int vida) {
    Jogador jogador("Aventureiro", vida, 5, 2);
    for (const Elemento &elemento : { HIDROGENIO, CARBONO, OXIGENIO, ENXOFRE }) {
        jogador.getInventario().adicionar(elemento);
    }
    return jogador;
}

Inimigo criarGolem() {
    return Inimigo("Gólem de Enxofre", 50, 9, 3, TipoEfeito::GELO);
}

void passo(Combate &combate, const std::string &descricao) {
    std::cout << "\n> " << descricao << "  [turno " << combate.getTurnoAtual()
              << " | estado atual: " << combate.getNomeEstado() << "]\n";
}

void mostrarFim(const Combate &combate) {
    std::cout << "verificarFim(): " << (combate.verificarFim() ? "true" : "false") << "\n";
}

// Roteiro fixo que passa por todos os estados e mostra pedidos feitos fora de hora.
void demonstrar() {
    std::cout << "=== Demonstração do padrão State no combate por turnos ===\n"
              << "\n##### Combate 1: do primeiro turno até a vitória #####\n";
    Jogador jogador = criarJogador(40);
    Inimigo golem = criarGolem();
    Combate combate(jogador, golem);
    combate.iniciar();

    passo(combate, "1. O cliente pede o turno do inimigo antes da hora");
    combate.executarTurnoInimigo();

    passo(combate, "2. O jogador mistura H + H + O");
    combate.executarTurnoJogador(jogador.misturarElementos({ HIDROGENIO, HIDROGENIO, OXIGENIO }));

    passo(combate, "3. O jogador tenta agir de novo na vez do inimigo");
    combate.executarTurnoJogador(jogador.misturarElementos({ HIDROGENIO, HIDROGENIO, OXIGENIO }));

    passo(combate, "4. Turno do inimigo");
    combate.executarTurnoInimigo();

    passo(combate, "5. O jogador mistura C + S, combinação que não existe");
    combate.executarTurnoJogador(jogador.misturarElementos({ CARBONO, ENXOFRE }));

    passo(combate, "6. Turno do inimigo");
    combate.executarTurnoInimigo();

    passo(combate, "7. O jogador usa H2O, que já está no diário");
    combate.executarTurnoJogador(jogador.getDiario().listarCombinacoes().front());

    passo(combate, "8. Turno do inimigo");
    combate.executarTurnoInimigo();

    passo(combate, "9. O jogador mistura C + H + H + H + H");
    combate.executarTurnoJogador(
            jogador.misturarElementos({ CARBONO, HIDROGENIO, HIDROGENIO, HIDROGENIO, HIDROGENIO }));

    passo(combate, "10. O cliente pede o turno do inimigo depois do fim");
    combate.executarTurnoInimigo();
    mostrarFim(combate);

    std::cout << "\n##### Combate 2: derrota pela explosão #####\n";
    Jogador ferido = criarJogador(6);
    Inimigo outroGolem = criarGolem();
    Combate combate2(ferido, outroGolem);
    combate2.iniciar();

    passo(combate2, "1. O jogador, com pouca vida, mistura O + S, combinação que não existe");
    combate2.executarTurnoJogador(ferido.misturarElementos({ OXIGENIO, ENXOFRE }));

    passo(combate2, "2. O cliente pede o turno do inimigo depois do fim");
    combate2.executarTurnoInimigo();
    mostrarFim(combate2);
}

} // namespace

int main(int argc, char *argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--demo") {
        demonstrar();
        return 0;
    }

    std::cout << "=== G4_ProjetoJogo: combate por turnos com o padrão State ===\n";
    Jogador jogador = criarJogador(40);
    Inimigo golem = criarGolem();
    Combate combate(jogador, golem);
    MenuTerminal menu(std::cin, std::cout);
    menu.jogar(combate);
    return 0;
}
