#include "menu_terminal.h"

#include <sstream>
#include <string>
#include <vector>

namespace G4Combate {

MenuTerminal::MenuTerminal(std::istream &p_entrada, std::ostream &p_saida) :
        entrada(p_entrada), saida(p_saida) {}

void MenuTerminal::jogar(Combate &combate) {
    combate.iniciar();
    while (!combate.verificarFim()) {
        mostrarSituacao(combate);
        const std::optional<Combinacao> escolha = escolherCombinacao(combate.getJogador());
        if (!escolha) {
            saida << "\nEntrada encerrada. Até a próxima!\n";
            return;
        }
        combate.executarTurnoJogador(*escolha);
        combate.executarTurnoInimigo();
    }
    saida << "\nFim do combate: " << combate.getNomeEstado() << ".\n";
}

void MenuTerminal::mostrarSituacao(Combate &combate) {
    const Jogador &jogador = combate.getJogador();
    const Inimigo &inimigo = combate.getInimigo();
    saida << "\n--- Turno " << combate.getTurnoAtual() << " | estado atual: " << combate.getNomeEstado() << " ---\n"
          << jogador.getNome() << ": " << textoDeVida(jogador) << " de vida\n"
          << inimigo.getNome() << ": " << textoDeVida(inimigo) << " de vida (fraco contra "
          << nomeDoTipo(inimigo.getFraqueza()) << ")\n";
}

std::optional<Combinacao> MenuTerminal::escolherCombinacao(const Jogador &jogador) {
    while (true) {
        saida << "O que fazer?\n"
              << "  1) Misturar elementos\n"
              << "  2) Usar uma combinação do diário\n"
              << "Opção: ";
        const int opcao = lerOpcao(1, 2);
        if (opcao < 0) {
            return std::nullopt;
        }
        if (opcao == 1) {
            return misturar(jogador);
        }
        if (jogador.getDiario().listarCombinacoes().empty()) {
            saida << "O diário ainda está vazio: descubra uma combinação misturando elementos.\n";
            continue;
        }
        return escolherDoDiario(jogador);
    }
}

std::optional<Combinacao> MenuTerminal::misturar(const Jogador &jogador) {
    const std::vector<Elemento> &disponiveis = jogador.getInventario().listarElementos();
    saida << "Elementos no inventário:\n";
    for (size_t i = 0; i < disponiveis.size(); ++i) {
        saida << "  " << i + 1 << ") " << disponiveis[i].getSimbolo() << "  " << disponiveis[i].getNome() << "\n";
    }
    saida << "Digite os números dos elementos, separados por espaço (ex.: 1 1 3 forma H2O): ";

    std::string linha;
    while (std::getline(entrada, linha)) {
        std::istringstream campos(linha);
        std::vector<Elemento> escolhidos;
        bool valido = true;
        int numero = 0;
        while (campos >> numero) {
            if (numero < 1 || numero > static_cast<int>(disponiveis.size())) {
                valido = false;
                break;
            }
            escolhidos.push_back(disponiveis[numero - 1]);
        }
        if (valido && campos.eof() && escolhidos.size() >= 2) {
            return jogador.misturarElementos(escolhidos);
        }
        saida << "Escolha pelo menos dois números da lista, separados por espaço: ";
    }
    return std::nullopt;
}

std::optional<Combinacao> MenuTerminal::escolherDoDiario(const Jogador &jogador) {
    const std::vector<Combinacao> &registradas = jogador.getDiario().listarCombinacoes();
    saida << "Combinações no diário:\n";
    for (size_t i = 0; i < registradas.size(); ++i) {
        saida << "  " << i + 1 << ") " << registradas[i].getFormula() << "\n";
    }
    saida << "Escolha: ";
    const int opcao = lerOpcao(1, static_cast<int>(registradas.size()));
    if (opcao < 0) {
        return std::nullopt;
    }
    return registradas[opcao - 1];
}

int MenuTerminal::lerOpcao(int minimo, int maximo) {
    std::string linha;
    while (std::getline(entrada, linha)) {
        std::istringstream campos(linha);
        int valor = 0;
        std::string sobra;
        if (campos >> valor && !(campos >> sobra) && valor >= minimo && valor <= maximo) {
            return valor;
        }
        saida << "Opção inválida. Digite um número de " << minimo << " a " << maximo << ": ";
    }
    return -1;
}

} // namespace G4Combate
