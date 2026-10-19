#include "clientes.hpp"

#include <functional>
#include <iostream>
#include <sstream>

// Cliente de demonstração: chama os clientes do jogo e mostra, depois de cada
// chamada, o endereço devolvido por Partida::instancia() e o progresso da partida.
// Com --demo, executa uma sequência fixa; com --roteiro, imprime a mesma
// sequência em JSON, incluindo o texto do terminal. A visualização do
// relatório reproduz exatamente essa saída, sem refazer a lógica em JavaScript.

namespace {

std::string endereco() {
    std::ostringstream texto;
    texto << static_cast<const void*>(&Partida::instancia());
    return texto.str();
}

std::string json(const std::string& valor) {
    std::string saida = "\"";
    for (char c : valor) {
        if (c == '\n') { saida += "\\n"; continue; }
        if (c == '"' || c == '\\') saida += '\\';
        saida += c;
    }
    return saida + '"';
}

std::string resumo(const Partida& p) {
    std::ostringstream texto;
    texto << p.jogador() << " | " << p.area() << " | poções " << p.pocoesCriadas()
          << " | vitórias " << p.inimigosDerrotados() << " | " << nomeDoEstado(p.estado())
          << " | salvamentos " << p.salvamentos().size();
    return texto.str();
}

std::string jsonDoPasso(int passo, const std::string& cliente, const std::string& chamada,
                        const std::string& erro, const std::string& saida) {
    const Partida& p = Partida::instancia();
    std::ostringstream texto;
    texto << "{\"passo\":" << passo << ",\"cliente\":" << json(cliente)
          << ",\"chamada\":" << json(chamada) << ",\"endereco\":" << json(endereco())
          << ",\"erro\":" << json(erro) << ",\"saida\":" << json(saida)
          << ",\"jogador\":" << json(p.jogador())
          << ",\"area\":" << json(p.area()) << ",\"pocoes\":" << p.pocoesCriadas()
          << ",\"vitorias\":" << p.inimigosDerrotados()
          << ",\"estado\":" << json(nomeDoEstado(p.estado())) << ",\"salvamentos\":[";
    for (std::size_t i = 0; i < p.salvamentos().size(); ++i) {
        const Salvamento& s = p.salvamentos()[i];
        texto << (i ? "," : "") << "{\"area\":" << json(s.area) << ",\"pocoes\":" << s.pocoesCriadas
              << ",\"vitorias\":" << s.inimigosDerrotados << ",\"origem\":" << json(s.origem) << "}";
    }
    texto << "]}";
    return texto.str();
}

// Executa uma chamada de cliente e registra o resultado no formato escolhido.
void executar(bool emJson, int passo, const std::string& cliente, const std::string& chamada,
              const std::function<void()>& acao) {
    std::string erro;
    try { acao(); } catch (const std::exception& e) { erro = e.what(); }
    std::ostringstream saida;
    saida << "[" << passo << "] " << cliente << "." << chamada << '\n'
          << "    Partida::instancia() = " << endereco() << '\n';
    if (!erro.empty()) saida << "    Recusado: " << erro << '\n';
    saida << "    " << resumo(Partida::instancia()) << '\n';
    if (emJson) std::cout << jsonDoPasso(passo, cliente, chamada, erro, saida.str()) << '\n';
    else std::cout << saida.str() << '\n';
}

void roteiro(bool emJson) {
    Exploracao exploracao;
    BancadaDeAlquimia bancada;
    Combate combate;
    MenuDePausa menu;
    int passo = 0;
    executar(emJson, ++passo, "Jogo", "iniciar(\"Alquimista\", \"Floresta\")",
             [] { Partida::instancia().iniciar("Alquimista", "Floresta"); });
    executar(emJson, ++passo, "Exploracao", "entrarEm(\"Caverna\")",
             [&] { exploracao.entrarEm("Caverna"); });
    executar(emJson, ++passo, "BancadaDeAlquimia", "criarPocao(\"Poção Flamejante\")",
             [&] { bancada.criarPocao("Poção Flamejante"); });
    executar(emJson, ++passo, "Combate", "derrotar(\"Cogumelo Andante\")",
             [&] { combate.derrotar("Cogumelo Andante"); });
    executar(emJson, ++passo, "MenuDePausa", "salvar()", [&] { menu.salvar(); });
    executar(emJson, ++passo, "Combate", "derrotar(\"Gólem de Enxofre\")",
             [&] { combate.derrotar("Gólem de Enxofre"); });
    executar(emJson, ++passo, "MenuDePausa", "salvarESair()", [&] { menu.salvarESair(); });
    executar(emJson, ++passo, "Exploracao", "entrarEm(\"Floresta\")",
             [&] { exploracao.entrarEm("Floresta"); });
}

std::string textoOu(std::istringstream& entrada, const std::string& padrao) {
    std::string texto;
    std::getline(entrada >> std::ws, texto);
    return texto.empty() ? padrao : texto;
}

void menuInterativo() {
    Exploracao exploracao;
    BancadaDeAlquimia bancada;
    Combate combate;
    MenuDePausa menu;
    int passo = 0;
    std::cout << "Singleton: a partida única do G4_ProjetoJogo\n"
              << "1 [área]     Exploracao.entrarEm         (padrão: Caverna)\n"
              << "2 [poção]    BancadaDeAlquimia.criarPocao (padrão: Poção Flamejante)\n"
              << "3 [inimigo]  Combate.derrotar             (padrão: Cogumelo Andante)\n"
              << "4            MenuDePausa.salvar\n"
              << "5            MenuDePausa.salvarESair\n"
              << "6 [nome]     novo jogo                    (padrão: Alquimista)\n"
              << "0            fechar o programa\n\n";
    executar(false, ++passo, "Jogo", "iniciar(\"Alquimista\", \"Floresta\")",
             [] { Partida::instancia().iniciar("Alquimista", "Floresta"); });
    std::string linha;
    while (std::cout << "> " && std::getline(std::cin, linha)) {
        std::istringstream entrada(linha);
        int opcao = -1;
        if (!(entrada >> opcao)) { std::cout << "Digite o número de uma opção.\n"; continue; }
        if (opcao == 0) break;
        if (opcao == 1) {
            std::string area = textoOu(entrada, "Caverna");
            executar(false, ++passo, "Exploracao", "entrarEm(\"" + area + "\")",
                     [&] { exploracao.entrarEm(area); });
        } else if (opcao == 2) {
            std::string pocao = textoOu(entrada, "Poção Flamejante");
            executar(false, ++passo, "BancadaDeAlquimia", "criarPocao(\"" + pocao + "\")",
                     [&] { bancada.criarPocao(pocao); });
        } else if (opcao == 3) {
            std::string inimigo = textoOu(entrada, "Cogumelo Andante");
            executar(false, ++passo, "Combate", "derrotar(\"" + inimigo + "\")",
                     [&] { combate.derrotar(inimigo); });
        } else if (opcao == 4) {
            executar(false, ++passo, "MenuDePausa", "salvar()", [&] { menu.salvar(); });
        } else if (opcao == 5) {
            executar(false, ++passo, "MenuDePausa", "salvarESair()", [&] { menu.salvarESair(); });
        } else if (opcao == 6) {
            std::string nome = textoOu(entrada, "Alquimista");
            executar(false, ++passo, "Jogo", "iniciar(\"" + nome + "\", \"Floresta\")",
                     [&] { Partida::instancia().iniciar(nome, "Floresta"); });
        } else {
            std::cout << "Opção desconhecida.\n";
        }
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string modo = argc == 2 ? argv[1] : "";
    if (modo == "--roteiro") roteiro(true);
    else if (modo == "--demo") roteiro(false);
    else menuInterativo();
}
