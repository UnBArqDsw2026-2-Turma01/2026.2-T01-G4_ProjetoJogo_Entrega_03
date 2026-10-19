#include "clientes.hpp"

void Exploracao::entrarEm(const std::string& area) {
    Partida::instancia().mudarArea(area);
}

void BancadaDeAlquimia::criarPocao(const std::string& pocao) {
    Partida::instancia().registrarPocao(pocao);
}

void Combate::derrotar(const std::string& inimigo) {
    Partida::instancia().registrarVitoria(inimigo);
}

Salvamento MenuDePausa::salvar() {
    return Partida::instancia().salvar("Menu de pausa");
}

Salvamento MenuDePausa::salvarESair() {
    Partida& partida = Partida::instancia();
    Salvamento salvamento = partida.salvar("Menu de pausa (salvar e sair)");
    partida.sair();
    return salvamento;
}
