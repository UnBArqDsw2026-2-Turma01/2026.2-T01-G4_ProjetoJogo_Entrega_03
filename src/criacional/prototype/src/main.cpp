#include "demonstracao.hpp"
#include <iostream>
#include <sstream>

int main(int argc, char* argv[]) {
    Demonstracao demo;
    if (argc == 2 && std::string(argv[1]) == "--roteiro") {
        for (const auto& [acao, argumento] : std::vector<std::pair<Comando, int>>{
                 {Comando::Criar, 0}, {Comando::Criar, 0}, {Comando::Selecionar, 0},
                 {Comando::Dano, 0}, {Comando::Fortalecer, 0}, {Comando::ClonarCopia, 0},
                 {Comando::Dano, 0}, {Comando::Criar, 0}}) {
            demo.executar(acao, argumento);
            std::cout << demo.json() << '\n';
        }
        return 0;
    }
    std::cout << "Prototype: criação de inimigos\n"
              << "1 modelo (0 cogumelo, 1 gólem, 2 morcego) | 2 criar | 3 selecionar (0..3)\n"
              << "4 dano | 5 fortalecer ataque | 6 clonar selecionado | 7 reiniciar | 0 sair\n";
    std::string linha;
    while (std::cout << "> " && std::getline(std::cin, linha)) {
        std::istringstream entrada(linha);
        int acao = 0, argumento = 0;
        if (!(entrada >> acao)) { std::cout << "Digite um comando numérico.\n"; continue; }
        if (acao == 0) break;
        if ((acao == 1 || acao == 3) && !(entrada >> argumento)) {
            std::cout << "Informe o índice após o comando.\n"; continue;
        }
        try {
            demo.executar(static_cast<Comando>(acao), argumento);
            demo.imprimir(std::cout);
        } catch (const std::exception& erro) { std::cout << erro.what() << '\n'; }
    }
}
