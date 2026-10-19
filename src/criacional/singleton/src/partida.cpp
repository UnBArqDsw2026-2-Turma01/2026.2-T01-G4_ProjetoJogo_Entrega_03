#include "partida.hpp"

#include <stdexcept>

namespace {
void exigirNome(const std::string& valor, const char* campo) {
    if (valor.empty()) throw std::invalid_argument(std::string("Informe ") + campo + ".");
}
}

std::string nomeDoEstado(EstadoDaPartida estado) {
    switch (estado) {
        case EstadoDaPartida::NaoIniciada: return "não iniciada";
        case EstadoDaPartida::EmAndamento: return "em andamento";
        case EstadoDaPartida::Encerrada: return "encerrada";
    }
    return "desconhecido";
}

Partida& Partida::instancia() {
    // Variável estática local: criada na primeira chamada e destruída ao fim do
    // programa. Desde o C++11, essa inicialização é segura entre threads.
    static Partida unica;
    return unica;
}

void Partida::iniciar(const std::string& jogador, const std::string& areaInicial) {
    exigirNome(jogador, "o nome do jogador");
    exigirNome(areaInicial, "a área inicial");
    estado_ = EstadoDaPartida::EmAndamento;
    jogador_ = jogador;
    area_ = areaInicial;
    pocoesCriadas_ = 0;
    inimigosDerrotados_ = 0;
    salvamentos_.clear();
}

void Partida::mudarArea(const std::string& area) {
    exigirEmAndamento();
    exigirNome(area, "a área");
    area_ = area;
}

void Partida::registrarPocao(const std::string& pocao) {
    exigirEmAndamento();
    exigirNome(pocao, "a poção");
    ++pocoesCriadas_;
}

void Partida::registrarVitoria(const std::string& inimigo) {
    exigirEmAndamento();
    exigirNome(inimigo, "o inimigo");
    ++inimigosDerrotados_;
}

Salvamento Partida::salvar(const std::string& origem) {
    exigirEmAndamento();
    exigirNome(origem, "a origem do salvamento");
    salvamentos_.push_back({jogador_, area_, pocoesCriadas_, inimigosDerrotados_, origem});
    return salvamentos_.back();
}

void Partida::sair() {
    exigirEmAndamento();
    estado_ = EstadoDaPartida::Encerrada;
}

void Partida::exigirEmAndamento() const {
    if (estado_ != EstadoDaPartida::EmAndamento)
        throw std::logic_error("A partida está " + nomeDoEstado(estado_) + ".");
}

EstadoDaPartida Partida::estado() const { return estado_; }
const std::string& Partida::jogador() const { return jogador_; }
const std::string& Partida::area() const { return area_; }
int Partida::pocoesCriadas() const { return pocoesCriadas_; }
int Partida::inimigosDerrotados() const { return inimigosDerrotados_; }
const std::vector<Salvamento>& Partida::salvamentos() const { return salvamentos_; }
