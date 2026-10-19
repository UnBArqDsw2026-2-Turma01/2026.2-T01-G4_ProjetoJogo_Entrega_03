#pragma once

#include "partida.hpp"

// Clientes do Singleton. Nenhum deles recebe a partida por parâmetro nem a
// guarda como atributo: cada um a obtém por Partida::instancia() no momento em
// que precisa. São partes do jogo que não se conhecem entre si.

class Exploracao {
public:
    void entrarEm(const std::string& area);
};

class BancadaDeAlquimia {
public:
    void criarPocao(const std::string& pocao);
};

class Combate {
public:
    void derrotar(const std::string& inimigo);
};

class MenuDePausa {
public:
    Salvamento salvar();
    Salvamento salvarESair();
};
